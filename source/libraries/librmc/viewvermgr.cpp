

#include "stdafx.h"

#include <nx\crypt\aes.h>
#include <nx\winutil\process.h>
#include <nx\rmc\viewermgr.h>



using namespace NX;
using namespace NX::rapidjson;


std::string RmViewerData::ToBase64(const UCHAR* key, ULONG size) const
{
	const std::string& s = serialize();

	// No encryption
	if (NULL == key || 0 == size) {
		return NX::conv::Base64Encode((const UCHAR*)s.c_str(), (ULONG)s.length());
	}

	// Need encryption
	std::vector<UCHAR> buf;
	ULONG bufSize = ROUND_TO_SIZE(s.length() + 1, 16);
	buf.resize(bufSize, 0);
	memcpy(buf.data(), s.c_str(), s.length());

	NX::crypt::AesKey sessionKey;
	Result res = sessionKey.Import(key, size);
	if (!res)
		return std::string();

	res = NX::crypt::AesEncrypt(sessionKey, buf.data(), bufSize, buf.data(), &bufSize, NULL);
	if (!res)
		return std::string();

	return NX::conv::Base64Encode(buf);
}

Result RmViewerData::FromBase64(const std::string& sbase64, const UCHAR* key, ULONG size)
{
	std::vector<UCHAR> buf = NX::conv::Base64Decode(sbase64);
	if (0 == buf.size() || !IS_ALIGNED(buf.size(), 16))
		return RESULT(ERROR_INVALID_DATA);

	std::string jsondata;

	if (NULL != key && 0 != size) {
		// Need encryption
		NX::crypt::AesKey sessionKey;
		Result res = sessionKey.Import(key, size);
		if (!res)
			return res;

		ULONG bufSize = (ULONG)buf.size();
		res = NX::crypt::AesDecrypt(sessionKey, buf.data(), bufSize, buf.data(), &bufSize, NULL);
		if (!res)
			return res;
		jsondata = std::string((const char*)buf.data());
	}
	else {
		if (0 == buf[buf.size() - 1])
			jsondata = std::string((const char*)buf.data());
		else
			jsondata = std::string(buf.begin(), buf.end());
	}

	return deserialize(jsondata);
}

RmRemoteViewerData::RmRemoteViewerData() : RmViewerData(RemoteViewer)
{
}

RmRemoteViewerData::RmRemoteViewerData(const std::vector<std::wstring>& cookies, const std::wstring& url)
	: RmViewerData(RemoteViewer), _viewerCookies(cookies), _viewerUrl(url)
{
}

RmRemoteViewerData::~RmRemoteViewerData()
{
}

std::string RmRemoteViewerData::serialize() const
{
	std::string s;

	do {

		std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
		if (root == NULL)
			break;

		JsonValue* pv = JsonValue::CreateNumber((int)_viewerType);
		if (!IsValidJsonNumber(pv)) break;
		root->set(L"viewerType", pv);

		JsonArray* cookies = JsonValue::CreateArray();
		if (!IsValidJsonArray(cookies)) break;
		root->set(L"viewerCookies", cookies);
		std::for_each(_viewerCookies.begin(), _viewerCookies.end(), [&](const std::wstring& cookie) {
			if (cookie.empty())
				return;
			JsonValue* item = JsonValue::CreateString(cookie);
			if (NULL != item)
				cookies->push_back(item);
		});

		pv = JsonValue::CreateString(_viewerUrl);
		if (!IsValidJsonString(pv)) break;
		root->set(L"viewerUrl", pv);

		JsonStringWriter<char> writer;
		s = writer.Write(root.get());

	} while (FALSE);

	return s;
}

Result RmRemoteViewerData::deserialize(const std::string& s)
{
	JsonDocument doc;
	if (!doc.LoadJsonString(s))
		return RESULT(ERROR_INVALID_DATA);

	Result res = RESULT(0);
	do {

		PCJSONVALUE root = doc.GetRoot();
		if (!IsValidJsonObject(root)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		PCJSONVALUE pv = root->AsObject()->at(L"viewerType");
		if (!IsValidJsonNumber(pv)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}
		_viewerType = (VIEWERTYPE)pv->AsNumber()->ToInt();
		
		pv = root->AsObject()->at(L"viewerCookies");
		if (IsValidJsonArray(pv)) {
			const int count = (int)pv->AsArray()->size();
			for (int i = 0; i < count; i++) {
				PCJSONVALUE item =pv->AsArray()->at(i);
				if (IsValidJsonString(item)) {
					const std::wstring& sCookie = item->AsString()->GetString();
					if (!sCookie.empty())
						_viewerCookies.push_back(sCookie);
				}
			}
		}
		else if (IsValidJsonString(pv)) {
			const std::wstring& sCookie = pv->AsString()->GetString();
			if (!sCookie.empty())
				_viewerCookies.push_back(sCookie);
		}
		else {
			; // No Cookie
		}

		pv = root->AsObject()->at(L"viewerUrl");
		if (IsValidJsonString(pv)) {
			_viewerUrl = pv->AsString()->GetString();
		}

	} while (FALSE);
	if (!res)
		return res;

	return RESULT(0);
}


RmLocalViewerData::RmLocalViewerData()
	: RmViewerData(LocalViewer), _lastModifiedTime(0), _fileSize(0), _offline(false), _favorite(false), _local(false), _tempFileEncrypted(false)
{
}

RmLocalViewerData::~RmLocalViewerData()
{
}

std::string RmLocalViewerData::serialize() const
{
	std::string s;

	do {

		std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
		if (root == NULL)
			break;

		JsonValue* pv = JsonValue::CreateNumber((int)_viewerType);
		if (!IsValidJsonNumber(pv)) break;
		root->set(L"viewerType", pv);

		pv = JsonValue::CreateString(_tempFilePath);
		if (!IsValidJsonString(pv)) break;
		root->set(L"tempFilePath", pv);

		pv = JsonValue::CreateString(_filePath);
		if (!IsValidJsonString(pv)) break;
		root->set(L"filePath", pv);

		if (!_fileId.empty()) {
			pv = JsonValue::CreateString(_fileId);
			if (!IsValidJsonString(pv)) break;
			root->set(L"fileId", pv);
		}

		if (!_fileUid.empty()) {
			pv = JsonValue::CreateString(_fileUid);
			if (!IsValidJsonString(pv)) break;
			root->set(L"fileUid", pv);
		}

		pv = JsonValue::CreateString(_originalFileName);
		if (!IsValidJsonString(pv)) break;
		root->set(L"originalFileName", pv);

		pv = JsonValue::CreateNumber(_lastModifiedTime);
		if (!IsValidJsonNumber(pv)) break;
		root->set(L"lastModifiedTime", pv);

		pv = JsonValue::CreateNumber(_fileSize);
		if (!IsValidJsonNumber(pv)) break;
		root->set(L"fileSize", pv);

		pv = JsonValue::CreateBool(_offline);
		if (!IsValidJsonBool(pv)) break;
		root->set(L"offline", pv);

		pv = JsonValue::CreateBool(_favorite);
		if (!IsValidJsonBool(pv)) break;
		root->set(L"favorite", pv);

		pv = JsonValue::CreateBool(_local);
		if (!IsValidJsonBool(pv)) break;
		root->set(L"local", pv);

		pv = JsonValue::CreateBool(_tempFileEncrypted);
		if (!IsValidJsonBool(pv)) break;
		root->set(L"tempFileEncrypted", pv);

		JsonStringWriter<char> writer;
		s = writer.Write(root.get());

	} while (FALSE);

	return s;
}

Result RmLocalViewerData::deserialize(const std::string& s)
{
	JsonDocument doc;
	if (!doc.LoadJsonString(s))
		return RESULT(ERROR_INVALID_DATA);

	Result res = RESULT(0);
	do {

		PCJSONVALUE root = doc.GetRoot();
		if (!IsValidJsonObject(root)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		PCJSONVALUE pv = root->AsObject()->at(L"viewerType");
		if (!IsValidJsonNumber(pv)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}
		_viewerType = (VIEWERTYPE)pv->AsNumber()->ToInt();
		
		pv = root->AsObject()->at(L"tempFilePath");
		if (IsValidJsonString(pv)) {
			_tempFilePath = pv->AsString()->GetString();
		}
		
		pv = root->AsObject()->at(L"filePath");
		if (IsValidJsonString(pv)) {
			_filePath = pv->AsString()->GetString();
		}
		
		pv = root->AsObject()->at(L"fileId");
		if (IsValidJsonString(pv)) {
			_fileId = pv->AsString()->GetString();
		}
		
		pv = root->AsObject()->at(L"fileUid");
		if (IsValidJsonString(pv)) {
			_fileUid = pv->AsString()->GetString();
		}
		
		pv = root->AsObject()->at(L"originalFileName");
		if (IsValidJsonString(pv)) {
			_originalFileName = pv->AsString()->GetString();
		}
		
		pv = root->AsObject()->at(L"lastModifiedTime");
		if (IsValidJsonNumber(pv)) {
			_lastModifiedTime = pv->AsNumber()->ToInt64();
		}
		
		pv = root->AsObject()->at(L"fileSize");
		if (IsValidJsonNumber(pv)) {
			_fileSize = pv->AsNumber()->ToInt64();
		}
		
		pv = root->AsObject()->at(L"offline");
		if (IsValidJsonBool(pv)) {
			_offline = pv->AsBool()->GetBool();
		}
		
		pv = root->AsObject()->at(L"favorite");
		if (IsValidJsonBool(pv)) {
			_favorite = pv->AsBool()->GetBool();
		}
		
		pv = root->AsObject()->at(L"local");
		if (IsValidJsonBool(pv)) {
			_local = pv->AsBool()->GetBool();
		}
		
		pv = root->AsObject()->at(L"tempFileEncrypted");
		if (IsValidJsonBool(pv)) {
			_tempFileEncrypted = pv->AsBool()->GetBool();
		}

	} while (FALSE);
	if (!res)
		return res;

	return RESULT(0);
}



RmViewerManager::RmViewerManager()
{
}

RmViewerManager::~RmViewerManager()
{
}

Result RmViewerManager::Init(const std::wstring& binDir)
{
	_binDir = binDir;

	_remoteViewer = std::shared_ptr<RmRemoteViewer>(new RmRemoteViewer(std::wstring(_binDir + L"\\nxrviewer.exe")));

	LoadViewers(std::wstring(_binDir + L"\\viewers"));

	_h = ::CreateJobObjectW(NULL, L"NxRmViewerJob");
	if (NULL == _h)
		return RESULT(GetLastError());

	JOBOBJECT_BASIC_LIMIT_INFORMATION basicLimit = { 0 };
	memset(&basicLimit, 0, sizeof(basicLimit));
	basicLimit.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE | JOB_OBJECT_LIMIT_ACTIVE_PROCESS;
	basicLimit.ActiveProcessLimit = 64;
	SetInformationJobObject(_h, JobObjectBasicLimitInformation, &basicLimit, (DWORD)sizeof(basicLimit));
	return RESULT(0);
}

void RmViewerManager::Cleanup()
{
	if (NULL != _h) {
		CloseHandle(_h);
		_h = NULL;
	}
}


void RmViewerManager::LoadViewers(const std::wstring& viewersDir)
{
	WIN32_FIND_DATAW wfd = { 0 };

	const std::wstring sRoot(viewersDir + (viewersDir[viewersDir.length()-1] == L'\\' ? L"" : L"\\"));
	const std::wstring sFilter(sRoot + L"*");
	HANDLE hFind = ::FindFirstFileW(sFilter.c_str(), &wfd);
	if (INVALID_HANDLE_VALUE == hFind)
		return;

	do {

		if (FILE_ATTRIBUTE_DIRECTORY != (wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			continue;
		if(0 == _wcsicmp(wfd.cFileName, L".") || 0 == _wcsicmp(wfd.cFileName, L".."))
			continue;

		RmViewer* pViewer = RmViewer::Load(std::wstring(sRoot + wfd.cFileName));
		if (pViewer != NULL) {
			_viewers.push_back(std::shared_ptr<RmViewer>(pViewer));
			pViewer = NULL;
		}

	} while (::FindNextFileW(hFind, &wfd));

	FindClose(hFind);
	hFind = INVALID_HANDLE_VALUE;

	for (const std::shared_ptr<RmViewer>& p : _viewers) {
		const std::unordered_map<std::wstring, RmSupportFileType>& supportTypes = p->GetSupportTypes();
		std::for_each(supportTypes.cbegin(), supportTypes.cend(), [&] (const std::pair<std::wstring, RmSupportFileType>& item) {
			auto pos = _fileTypeToViewer.find(item.first);
			if (pos == _fileTypeToViewer.end()) {
				_fileTypeToViewer[item.first].first = item.second.GetMetric();
				_fileTypeToViewer[item.first].second = p;
			}
			else {
				if ((*pos).second.first < item.second.GetMetric()) {
					(*pos).second.first = item.second.GetMetric();
					(*pos).second.second = p;
				}
			}
		});
	}
}

void RmViewerManager::ClearViewers()
{
	_fileTypeToViewer.clear();
	_viewers.clear();
}

std::shared_ptr<RmViewer> RmViewerManager::GetLocalViewer(const std::wstring& fileOrExtension) const
{
	std::wstring extension(fileOrExtension);
	auto pos = fileOrExtension.rfind('.');
	if (pos != std::wstring::npos)
		extension = fileOrExtension.substr(pos + 1);
	NX::tolower<wchar_t>(extension);

	auto it = _fileTypeToViewer.find(extension);
	if (it == _fileTypeToViewer.end())
		return NULL; // _remoteViewer;

	return (*it).second.second;
}

Result RmViewerManager::AddViewerJob(HANDLE hProcess)
{
	if (!AssignProcessToJobObject(_h, hProcess))
		return RESULT(GetLastError());

	return RESULT(0);
}

std::vector<DWORD> RmViewerManager::GetJobProcesses()
{
	std::vector<DWORD> vProcList;

	if (NULL == _h)
		return vProcList;

	std::vector<UCHAR> buf;
	ULONG bufSize = sizeof(JOBOBJECT_BASIC_PROCESS_ID_LIST) + 2048;
	buf.resize(sizeof(JOBOBJECT_BASIC_PROCESS_ID_LIST) + 2048, 0);
	PJOBOBJECT_BASIC_PROCESS_ID_LIST pInfo = (PJOBOBJECT_BASIC_PROCESS_ID_LIST)buf.data();;


	if (!QueryInformationJobObject(_h, JobObjectBasicProcessIdList, buf.data(), bufSize, &bufSize))
		return vProcList;

	for (int i = 0; i < (int)pInfo->NumberOfProcessIdsInList; i++) {
		vProcList.push_back((DWORD)pInfo->ProcessIdList[i]);
	}

	return vProcList;
}


RmSupportFileType::RmSupportFileType() : _metric(0)
{
}

RmSupportFileType::RmSupportFileType(const RmSupportFileType& rhs)
	: _type(rhs._type), _descritpion(rhs._descritpion), _metric(rhs._metric)
{
}

RmSupportFileType::RmSupportFileType(const std::wstring& type, const std::wstring& descritpion, int metric)
	: _type(type), _descritpion(descritpion), _metric(metric)
{
}

RmSupportFileType& RmSupportFileType::operator = (const RmSupportFileType& rhs)
{
	if (this != &rhs) {
		_type = rhs._type;
		_descritpion = rhs._descritpion;
		_metric = rhs._metric;
	}
	return *this;
}

void RmSupportFileType::clear()
{
	_type.clear();
	_descritpion.clear();
	_metric = 0;
}


RmViewer::RmViewer()
{
}

RmViewer::RmViewer(const std::wstring& name,
	const std::wstring& type,
	const std::wstring& version,
	const std::wstring& interfaceVer,
	const std::wstring& path) : _name(name), _type(type), _version(version), _interface(interfaceVer), _path(path)
{
}

RmViewer::~RmViewer()
{
}

RmViewer* RmViewer::Load(const std::wstring& dir)
{
	RmViewer* pViewer = NULL;

	if (dir.empty())
		return NULL;

	// set config file path
	const std::wstring configFile(dir + (dir[dir.length()-1] == L'\\' ? L"config.json" : L"\\config.json"));

	try {

		do {

			NX::rapidjson::JsonDocument doc;
			if (!doc.LoadJsonFile<char>(configFile))
				break;

			PCJSONVALUE root = doc.GetRoot();
			if (!IsValidJsonObject(root))
				break;

			PCJSONVALUE plugin = root->AsObject()->at(L"ViewerPlugin");
			if (!IsValidJsonObject(plugin))
				break;

			std::wstring	name;
			std::wstring	type;
			std::wstring	version;
			std::wstring	interfaceVer;
			std::wstring	file;


			PCJSONVALUE pv = plugin->AsObject()->at(L"Name");
			if (!IsValidJsonString(pv))
				break;
			name = pv->AsString()->GetString();
			if (name.empty())
				break;

			pv = plugin->AsObject()->at(L"Version");
			if (!IsValidJsonString(pv))
				break;
			version = pv->AsString()->GetString();
			if (version.empty())
				break;

			pv = plugin->AsObject()->at(L"SupportInterface");
			if (!IsValidJsonString(pv))
				break;
			interfaceVer = pv->AsString()->GetString();

			pv = plugin->AsObject()->at(L"FileName");
			if (!IsValidJsonString(pv))
				break;
			file = pv->AsString()->GetString();
			if (file.empty())
				break;

			pv = plugin->AsObject()->at(L"Type");
			if (!IsValidJsonString(pv))
				break;
			type = pv->AsString()->GetString();
			if (type.empty())
				break;

			pViewer = new RmViewer(name, type, version, interfaceVer, std::wstring(dir + (dir[dir.length()-1] == L'\\' ? L"" : L"\\") + file));
			if (pViewer == NULL)
				break;

			// Now load support file types
			PCJSONVALUE pSupportTypes = plugin->AsObject()->at(L"SupportFileType");
			if (!IsValidJsonArray(pSupportTypes))
				break;

			std::for_each(pSupportTypes->AsArray()->cbegin(), pSupportTypes->AsArray()->cend(), [&](PCJSONVALUE item) {

				if (!IsValidJsonObject(item))
					return;

				std::wstring type;
				std::wstring descritpion;
				int metric = 0;

				PCJSONVALUE pv = item->AsObject()->at(L"Extension");
				if (!IsValidJsonString(pv))
					return;
				type = pv->AsString()->GetString();
				if (type.empty())
					return;
				NX::tolower<wchar_t>(type);

				pv = item->AsObject()->at(L"Description");
				if (!IsValidJsonString(pv))
					return;
				descritpion = pv->AsString()->GetString();

				pv = item->AsObject()->at(L"Metric");
				if (!IsValidJsonNumber(pv))
					return;
				metric = pv->AsNumber()->ToInt();

				pViewer->_supportTypes[type] = RmSupportFileType(type, descritpion, metric);
			});

		} while (FALSE);

	}
	catch (const std::exception& e) {
		UNREFERENCED_PARAMETER(e);
		if (pViewer) {
			delete pViewer;
			pViewer = NULL;
		}
	}

	return pViewer;
}

typedef struct _MAINWND_DATA {
	DWORD processId;
	HWND  hWnd;
} MAINWND_DATA, *LPMAINWND_DATA;

BOOL IsMainWindow(HWND hWnd)
{
	return ((GetWindow(hWnd, GW_OWNER) == (HWND)0) && IsWindowVisible(hWnd));
}

BOOL CALLBACK EnumWindowsCallback(HWND hWnd, LPARAM lParam)
{
	LPMAINWND_DATA data = (LPMAINWND_DATA)lParam;
	DWORD processId = 0;
	GetWindowThreadProcessId(hWnd, &processId);
	if (data->processId != processId || !IsMainWindow(hWnd)) {
		return TRUE;
	}
	data->hWnd = hWnd;
	return FALSE;
}

HWND FindMainWindow(DWORD processId)
{
	MAINWND_DATA data = { processId, NULL};
	EnumWindows(EnumWindowsCallback, (LPARAM)&data);
	return data.hWnd;
}


Result RmViewer::CreateInstance(const RmViewerData* data, const UCHAR* key, ULONG keySize, _Out_opt_ PHANDLE ph)
{
	HANDLE h = NULL;
	DWORD  pid = 0;
	Result res = RESULT(0);

	if (NULL != ph) {
		*ph = NULL;
	}

	do {

		// serialize viewer data
		const std::string& base64 = data->ToBase64(key, keySize);

		// Build environment block
		const std::vector<std::pair<std::wstring, std::wstring>> viewerEnvData({
			std::pair<std::wstring, std::wstring>(NXRM_ENV_TYPE_NAME, IsRemoteViewer() ? L"Remote" : L"Local"),
			std::pair<std::wstring, std::wstring>(NXRM_ENV_VARIABLE_NAME, std::wstring(base64.begin(), base64.end()))
		});

		NX::win::ProcessEnvironment env(viewerEnvData);
		const std::vector<wchar_t>& block = env.CreateBlock();
		if (block.empty()) {
			res = RESULT(GetLastError());
			break;
		}

		STARTUPINFOW sti = { 0 };
		PROCESS_INFORMATION pi = { 0 };
		memset(&sti, 0, sizeof(sti));
		sti.cb = (DWORD)sizeof(sti);
		memset(&pi, 0, sizeof(pi));

		std::wstring cmd(L"\"");
		cmd.append(_path);
		cmd.append(L"\"");

		if (!::CreateProcessW(NULL, (LPWSTR)cmd.c_str(), NULL, NULL, TRUE, CREATE_UNICODE_ENVIRONMENT | CREATE_SUSPENDED, (LPVOID)block.data(), NULL, &sti, &pi)) {
			res = RESULT(GetLastError());
			break;
		}

		h = pi.hProcess;
		pid = pi.dwProcessId;
		ResumeThread(pi.hThread);
		CloseHandle(pi.hThread);

	} while (FALSE);
	if (!res) {
		if (NULL != h) {
			CloseHandle(h);
			h = NULL;
		}
		return res;
	}

	if (NULL != ph) {
		*ph = h;
		h = NULL;
	}
	else {
		if (NULL != h) {
			CloseHandle(h);
			h = NULL;
		}
	}

	if (0 != pid) {
		for (int i = 0; i < 5; i++) {
			HWND hWnd = FindMainWindow(pid);
			if (NULL != hWnd) {
				BringWindowToTop(hWnd);
				//long dwExStyle = GetWindowLongW(hWnd, GWL_EXSTYLE);
				//dwExStyle &= ~WS_EX_TOPMOST;
				//SetWindowLongW(hWnd, GWL_EXSTYLE, dwExStyle);
				::SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);
				break;
			}
			Sleep(1000);
		}
	}

	return RESULT(0);
}