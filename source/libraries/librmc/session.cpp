
#include "stdafx.h"

#include <nx\rmc\securefile.h>
#include <nx\rmc\upgrade.h>
#include <nx\rmc\heartbeat.h>
#include <nx\rmc\watermark.h>

#include <nx\common\stringex.h>
#include <nx\common\rapidjson.h>
#include <nx\winutil\osver.h>
#include <nx\winutil\registry.h>
#include <nx\rest\uri.h>

#include <nx\rmc\session.h>

using namespace NX;
using namespace NX::rapidjson;


RmSession::RmSession() : _actLog(this)
{
}

RmSession::RmSession(const RmRouter& router) : _router(router), _actLog(this)
{
}

RmSession::~RmSession()
{
	Cleanup();
}

RmSession* RmSession::Create(Result& error)
{
    RmSession* ps = new RmSession();
    if (!ps) {
        error = RESULT(GetLastError());
        return nullptr;
    }

	Result res = ps->Initialize();
	if (!res) {
		error = res;
		delete ps;
		ps = nullptr;
		return nullptr;
	}

    return ps;
}

RmSession* RmSession::Create(const RmRouter& router, Result& error)
{
    RmSession* ps = new RmSession(router);
    if (!ps) {
        error = RESULT(GetLastError());
        return nullptr;
    }

	Result res = ps->Initialize();
	if (!res) {
		error = res;
		delete ps;
		ps = nullptr;
		return nullptr;
	}

    return ps;
}

Result RmSession::Initialize()
{
	_product.Load();
	_dirs.SetAppRoot(_product.GetInstallLocation());
	SetIeFeatures();

    Result res = PrepareClientKey();
    if (!res)
        return res;

    res = _restClient.Init(_clientId.GetClientId());
    if (!res)
        return res;

	res = _viewerMgr.Init(GetDirs().GetAppBinDir());
	if (!res)
		return res;

	(VOID)LoadExistingCredential();
    return RESULT(0);
}

void RmSession::Cleanup()
{
	// Save all the tokens to disk
	_viewerMgr.Cleanup();
	SaveAllMembershipTokens();
	_user.Clear();
}

void RmSession::SetIeFeatures()
{
	DWORD dwCode = 0;
	NX::win::RegUserKey iekey;
	Result res = iekey.Create(L"SOFTWARE\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION");
	if (res) {
		res = iekey.ReadValue(L"nxrmc.exe", (unsigned int *)&dwCode);
		if (!res || 11000 > dwCode)
			iekey.WriteValue(L"nxrmc.exe", (UINT)11000);
		res = iekey.ReadValue(L"nxrviewer.exe", (unsigned int *)&dwCode);
		if (!res || 11000 > dwCode)
			iekey.WriteValue(L"nxrviewer.exe", (UINT)11000);

		iekey.Close();
	}

	res = iekey.Create(L"SOFTWARE\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_GPU_RENDERING");
	if (res) {
		res = iekey.ReadValue(L"nxrmc.exe", (unsigned int *)&dwCode);
		if (!res || 1 != dwCode)
			iekey.WriteValue(L"nxrmc.exe", (UINT)1);
		res = iekey.ReadValue(L"nxrviewer.exe", (unsigned int *)&dwCode);
		if (!res || 1 != dwCode)
			iekey.WriteValue(L"nxrviewer.exe", (UINT)1);
		iekey.Close();
	}

	// The IE Control use legacy mode by default, we need to allow it use latest mode
	res = iekey.Create(L"SOFTWARE\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_NINPUT_LEGACYMODE");
	if (res) {
		res = iekey.ReadValue(L"nxrviewer.exe", (unsigned int *)&dwCode);
		if (!res || 0 != dwCode)
			iekey.WriteValue(L"nxrviewer.exe", (UINT)0);
		iekey.Close();
	}
}

RmUpgradeInfo RmSession::CheckForUpgrade()
{
	RmUpgradeInfo remoteInfo;
	RmUpgradeInfo localInfo;
	RmRestStatus  status;
	const std::wstring& cachedFilePath = NX::RmUpgrade::GetNewVersionInfoFilePath();
	Result res = _restClient.CheckSoftwareUpdate(_rmserver.GetUrl(), _rmserver.GetTenantName(), _product.GetVersion(), status, remoteInfo);
	if (!res || !status) {
		remoteInfo.Clear();
	}
	else {
		DeleteFileW(cachedFilePath.c_str());
	}

	std::string s;
	res = SecureFileRead(cachedFilePath, s);
	if (res) {
		res = localInfo.Load(s);
		if (!res) {
			localInfo.Clear();
		}
		else {
			if (_product.VersionToNumber() >= localInfo.VersionToNumber() || status.GetStatus() == NX::REST::http::status_codes::NotModified.id) {
				if (!localInfo.GetDownloadedFile().empty())
					DeleteFileW(localInfo.GetDownloadedFile().c_str());
				localInfo.Clear();
			}
		}
		DeleteFileW(cachedFilePath.c_str());
	}

	if (!remoteInfo.Empty()) {
		// We get a valid remote upgrade info
		// So we need to update local info accordingly (if they are not identical)
		if (remoteInfo.VersionToNumber() != localInfo.VersionToNumber()) {
			const std::string& s = remoteInfo.Serialize();
			if (!s.empty())
				(void)SecureFileWrite(NX::RmUpgrade::GetNewVersionInfoFilePath(), s);
		}
		else {
			// Local info also valid, then we use local info
			// Because the file may have been downloaded already
			remoteInfo = localInfo;
		}
		return remoteInfo;
	}

	return localInfo;
}

Result RmSession::DoHeartbeat(_Out_ std::vector<std::wstring>& changedItems)
{
	std::vector<std::pair<std::wstring, std::wstring>> history;
	std::vector<RmHeartbeatObject> newObjects;
	RmRestStatus status;
	Result res = ReadHeartbeatHistory(history);
	if (!res)
		history.clear();
	res = _restClient.HeartBeat(_rmserver.GetUrl(),
		history,
		status,
		newObjects);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	std::for_each(newObjects.begin(), newObjects.end(), [&](const RmHeartbeatObject& hb) {

		Result res = RESULT(0);

		if (0 == _wcsicmp(hb.GetName().c_str(), L"watermarkConfig")) {
			res = GenerateWatermarkImage(hb.GetData());
		}
		else if (0 == _wcsicmp(hb.GetName().c_str(), L"policyBundle")) {
			res = GeneratePolicyBundle(hb.GetData());
		}
		else if (0 == _wcsicmp(hb.GetName().c_str(), L"classifyConfig")) {
			; // Nothing
		}
		else if (0 == _wcsicmp(hb.GetName().c_str(), L"clientConfig")) {
			; // Nothing
		}
		else {
			; // Nothing
		}

		if (!res)
			return;

		// This item is changed
		changedItems.push_back(hb.GetName());

		// Update history data
		auto pos = std::find_if(history.begin(), history.end(), [&](const std::pair<std::wstring, std::wstring>& item) -> bool {
			return (0 == _wcsicmp(item.first.c_str(), hb.GetName().c_str()));
		});

		if (pos != history.end()) {
			(*pos).second = hb.GetSerial();
		}
	});

	if (!changedItems.empty()) {
		WriteHeartbeatHistory(history);
	}

	return RESULT(0);
}

Result RmSession::ReadHeartbeatHistory(std::vector<std::pair<std::wstring, std::wstring>>& items)
{
	const std::wstring file(GetCurrentUserHome() + L"\\heartbeat.history");
	std::string s;
	Result res = SecureFileRead(file, s);
	if (!res)
		return res;

	if (s.empty())
		return RESULT(0);

	JsonDocument doc;
	if (!doc.LoadJsonString<char>(s)) {
		::DeleteFileW(file.c_str());
		return RESULT(ERROR_INVALID_DATA);
	}

	PCJSONVALUE root = doc.GetRoot();
	if (IsValidJsonObject(root)) {
		std::for_each(root->AsObject()->cbegin(), root->AsObject()->cend(), [&](const JsonObject::value_type& item) {
			if(IsValidJsonString(item.second))
				items.push_back(std::pair<std::wstring, std::wstring>(item.first, item.second->AsString()->GetString()));
		});
	}

	return RESULT(0);
}

Result RmSession::WriteHeartbeatHistory(const std::vector<std::pair<std::wstring, std::wstring>>& items)
{
	std::string s;

	if (!items.empty()) {
		std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
		if (root == nullptr)
			return RESULT(ERROR_NOT_ENOUGH_MEMORY);
		std::for_each(items.begin(), items.end(), [&](const std::pair<std::wstring, std::wstring>& item) {
			PJSONVALUE pv = JsonValue::CreateString(item.second);
			if (IsValidJsonString(pv))
				root->set(item.first, pv);
		});
		JsonStringWriter<char> writer;
		s = writer.Write(root.get());
	}

	if(s.empty())
		s = "{}";

	const std::wstring file(GetCurrentUserHome() + L"\\heartbeat.history");
	Result res = SecureFileWrite(file, s);
	if (!res)
		return res;

	return RESULT(0);
}

Result RmSession::GenerateWatermarkImage(const std::wstring& content)
{
	RmWatermarkConfig config;
	Result res = config.deserialize(content);
	if (!res)
		return res;

	const std::string& s = config.serialize();
	const std::wstring file(GetCurrentUserHome() + L"\\watermark.dat");
	res = SecureFileWrite(file, s);
	if (!res)
		return res;

	return RESULT(0);
}

Result RmSession::GeneratePolicyBundle(const std::wstring& content)
{
	if (content.empty())
		return RESULT(ERROR_INVALID_PARAMETER);

	JsonDocument doc;
	if(!doc.LoadJsonString<wchar_t>(content))
		return RESULT(ERROR_INVALID_DATA);

	PCJSONVALUE root = doc.GetRoot();
	if(!IsValidJsonObject(root))
		return RESULT(ERROR_INVALID_DATA);

	PCJSONVALUE pv = root->AsObject()->at(L"version");
	if(!IsValidJsonString(pv))
		return RESULT(ERROR_INVALID_DATA);
	pv = root->AsObject()->at(L"issuer");
	if (!IsValidJsonString(pv))
		return RESULT(ERROR_INVALID_DATA);
	doc.Clear();

	const std::string& s = NX::conv::utf16toutf8(content);
	const std::wstring file(GetCurrentUserHome() + L"\\bundle.dat");

	Result res = SecureFileWrite(file, s);
	if (!res)
		return res;

	return RESULT(0);
}

Result RmSession::DownloadUpgradeInstaller(_Inout_ RmUpgradeInfo& info)
{
	if (!info.GetDownloadedFile().empty() && INVALID_FILE_ATTRIBUTES != GetFileAttributesW(info.GetDownloadedFile().c_str()))
		return RESULT(0);

	RmUpgrade upgrader(info);
	Result res = upgrader.DownloadNewVersion();
	if (res) {
		info = upgrader.GetNewVersionInfo();
		const std::string& s = info.Serialize();
		if(!s.empty())
			(void)SecureFileWrite(NX::RmUpgrade::GetNewVersionInfoFilePath(), s);
	}

	return res;
}

Result RmSession::PrepareClientKey()
{
	// First, try to load client key from current machine's store
	Result res = _clientId.Load(true);
	if (!res) {
		// If there is no global client key, try to load it from current user's cert store
		res = _clientId.Load(false);
		if (!res) {
			return res;
		}
	}
	return RESULT(0);
}

Result RmSession::SecureFileRead(const std::wstring& file, std::string& s)
{
	if (_clientId.GetClientKey().empty())
		return RESULT(ERROR_NOT_READY);
	RmSecureFile sf(file, _clientId.GetClientKey());
	return sf.Read(s);
}

Result RmSession::SecureFileRead(const std::wstring& file, std::vector<UCHAR>& data)
{
	if (_clientId.GetClientKey().empty())
		return RESULT(ERROR_NOT_READY);
	RmSecureFile sf(file, _clientId.GetClientKey());
	return sf.Read(data);
}

Result RmSession::SecureFileWrite(const std::wstring& file, const std::string& s)
{
	if (_clientId.GetClientKey().empty())
		return RESULT(ERROR_NOT_READY);
	RmSecureFile sf(file, _clientId.GetClientKey());
	return sf.Write((const UCHAR*)s.c_str(), (ULONG)s.length());
}

Result RmSession::SecureFileWrite(const std::wstring& file, const std::vector<UCHAR>& data)
{
	if (_clientId.GetClientKey().empty())
		return RESULT(ERROR_NOT_READY);
	RmSecureFile sf(file, _clientId.GetClientKey());
	return sf.Write(data);
}

Result RmSession::SecureFileWrite(const std::wstring& file, const UCHAR* data, ULONG size)
{
	if (_clientId.GetClientKey().empty())
		return RESULT(ERROR_NOT_READY);
	RmSecureFile sf(file, _clientId.GetClientKey());
	return sf.Write(data, size);
}

Result RmSession::ListSharedWithMeFiles(__int64 pageId, __int64 pageSize, const std::wstring& orderBy, const std::wstring& keyWord, std::vector<RmSharedWithMeFile>& files)
{
	RmRestStatus status;

	files.clear();

	Result res = _restClient.ListSharedWithMeFiles(_rmserver.GetUrl(), NULL,
		pageId,
		pageSize,
		orderBy,
		keyWord,
		NULL,
		status,
		files);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	return RESULT(0);
}

Result RmSession::GetFirstPageSharedWithMeFiles(RmFinder& finder, bool* cancelFlag, std::vector<RmSharedWithMeFile>& files)
{
	RmRestStatus status;
	__int64 totalRecords = 0;

	files.clear();

	finder.ResetPageId();
	Result res = _restClient.ListSharedWithMeFiles(_rmserver.GetUrl(), cancelFlag,
		finder.GetPageId(),
		finder.GetPageSize(),
		finder.GetSortType(),
		finder.GetSearchKeyword(),
		&totalRecords,
		status,
		files);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	finder.SetTotalRecords(totalRecords);
	return RESULT(0);
}

Result RmSession::GetNextPageSharedWithMeFiles(RmFinder& finder, bool* cancelFlag, std::vector<RmSharedWithMeFile>& files)
{
	files.clear();

	if (finder.NoMoreData())
		return RESULT(ERROR_NO_MORE_ITEMS);

	RmRestStatus status;
	Result res = _restClient.ListSharedWithMeFiles(_rmserver.GetUrl(), cancelFlag,
		finder.GetPageId(),
		finder.GetPageSize(),
		finder.GetSortType(),
		finder.GetSearchKeyword(),
		NULL,
		status,
		files);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	return RESULT(0);
}