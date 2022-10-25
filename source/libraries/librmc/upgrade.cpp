

#include "stdafx.h"

#include <Urlmon.h>

#include <nx\common\stringex.h>
#include <nx\common\rapidjson.h>
#include <nx\crypt\sha.h>
#include <nx\crypt\cert.h>
#include <nx\winutil\path.h>
#include <nx\winutil\file.h>
#include <nx\winutil\cred.h>
#include <nx\winutil\zip.h>
#include <nx\rmc\upgrade.h>

using namespace NX;
using namespace NX::rapidjson;


RmUpgradeInfo::RmUpgradeInfo()
{
}

RmUpgradeInfo::RmUpgradeInfo(const std::wstring& version, const std::wstring& downloadUrl, const std::wstring& checksum, const std::wstring& downloadedFilePath)
	: _version(version), _downloadUrl(downloadUrl), _checksum(checksum), _downloadedFile(downloadedFilePath)
{
}

RmUpgradeInfo::RmUpgradeInfo(const RmUpgradeInfo& rhs)
	: _version(rhs._version), _downloadUrl(rhs._downloadUrl), _checksum(rhs._checksum), _downloadedFile(rhs._downloadedFile)
{
}

RmUpgradeInfo::~RmUpgradeInfo()
{
}

RmUpgradeInfo& RmUpgradeInfo::operator = (const RmUpgradeInfo& rhs)
{
	if (this != &rhs) {
		_version = rhs._version;
		_downloadUrl = rhs._downloadUrl;
		_checksum = rhs._checksum;
		_downloadedFile = rhs._downloadedFile;
	}
	return *this;
}

void RmUpgradeInfo::Clear()
{
	_version.clear();
	_downloadUrl.clear();
	_checksum.clear();
	_downloadedFile.clear();
}

std::string RmUpgradeInfo::Serialize() const
{
	std::string s;

	std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
	if (NULL == root)
		return s;

	root->set(L"version", JsonValue::CreateString(_version));
	root->set(L"downloadUrl", JsonValue::CreateString(_downloadUrl));
	root->set(L"checksum", JsonValue::CreateString(_checksum));
	root->set(L"downloadedFilePath", JsonValue::CreateString(_downloadedFile));

	JsonStringWriter<char> writer;
	s = writer.Write(root.get());

	return s;
}

Result RmUpgradeInfo::Load(const std::string& s)
{
	JsonDocument doc;
	if (!doc.LoadJsonString<char>(s))
		return RESULT(ERROR_INVALID_DATA);

	PCJSONVALUE root = doc.GetRoot();
	if(!IsValidJsonObject(root))
		return RESULT(ERROR_INVALID_DATA);
	
	PCJSONVALUE pv = root->AsObject()->at(L"version");
	if(!IsValidJsonString(pv))
		return RESULT(ERROR_INVALID_DATA);
	_version = pv->AsString()->GetString();

	pv = root->AsObject()->at(L"downloadUrl");
	if (!IsValidJsonString(pv))
		return RESULT(ERROR_INVALID_DATA);
	_downloadUrl = pv->AsString()->GetString();

	pv = root->AsObject()->at(L"checksum");
	if (!IsValidJsonString(pv))
		return RESULT(ERROR_INVALID_DATA);
	_checksum = pv->AsString()->GetString();

	pv = root->AsObject()->at(L"downloadedFilePath");
	if (IsValidJsonString(pv)) {
		_downloadedFile = pv->AsString()->GetString();
	}

	return RESULT(0);
}

ULONG RmUpgradeInfo::VersionToNumber() const
{
	ULONG uv = 0;
	int major = 0;
	int minor = 0;
	int build = 0;

	if (_version.empty())
		return 0U;
	swscanf_s(_version.c_str(), L"%d.%d.%d", &major, &minor, &build);
	uv |= (((ULONG)major) << 24);
	uv |= (((ULONG)minor) << 16);
	uv |= (((ULONG)build) & 0x0000FFFF);
	return uv;
}

RmUpgrade::RmUpgrade()
{
	PrepareDirs();
}

RmUpgrade::RmUpgrade(const RmUpgradeInfo& existingInfo) : _info(existingInfo)
{
}

RmUpgrade::~RmUpgrade()
{
}

Result RmUpgrade::DownloadNewVersion()
{
	const std::wstring& url = _info.GetDownloadUrl();
	const std::wstring& checksum = _info.GetChecksum();

	auto pos = url.rfind(L'/');
	assert(pos != std::wstring::npos);
	if (pos == std::wstring::npos)
		return RESULT(ERROR_INVALID_DATA);

	Result res = RESULT(0);
	const std::wstring downloadedFile(GetUpgradeDir() + L"\\" + url.substr(pos + 1));
	do {
		const std::wstring& upgradeDir = GetUpgradeDir();

		static const std::wstring dirNextLabsData(NX::win::GetSystemDirs().GetProgramDataDir() + L"\\NextLabs");
		if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(dirNextLabsData.c_str()))
			::CreateDirectoryW(dirNextLabsData.c_str(), NULL);
		if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(upgradeDir.c_str()))
			::CreateDirectoryW(upgradeDir.c_str(), NULL);

		if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(upgradeDir.c_str())) {
			res = RESULT(ERROR_PATH_NOT_FOUND);
			break;
		}

		// Delete existing file
		::DeleteFileW(downloadedFile.c_str());

		HRESULT hr = ::URLDownloadToFileW(NULL, url.c_str(), downloadedFile.c_str(), 0, NULL);
		if (FAILED(hr) || INVALID_FILE_ATTRIBUTES == GetFileAttributesW(downloadedFile.c_str())) {
			res = RESULT(GetLastError());
			break;
		}

		_info._downloadedFile = downloadedFile;

	} while (false);
	if (!res) {
		if (!downloadedFile.empty()) {
			::DeleteFileW(downloadedFile.c_str());
		}
		return res;
	}

	return RESULT(0);
}

Result RmUpgrade::InstallNewVersion()
{
	static const std::wstring NextLabsSubject(L"NextLabs Inc.");
	static const std::wstring NextLabsFileName(L"SkyDRM");

	if (_info.GetDownloadedFile().empty())
		return RESULT(ERROR_NOT_FOUND);
	if(INVALID_FILE_ATTRIBUTES == GetFileAttributesW(_info.GetDownloadedFile().c_str()))
		return RESULT(ERROR_NOT_FOUND);

	// Now verify installer
	Result res = VerifySha1Checksum(_info.GetDownloadedFile(), _info.GetChecksum());
	if (!res)
		return res;

	res = VerifyPublisher(_info.GetDownloadedFile(), NextLabsSubject);
	if (!res)
		return res;

	if (NX::iend_with<wchar_t>(_info.GetDownloadedFile(), L".cab")) {
		// It is a cab/zip file
		std::wstring unpackDir;
		res = UnpackCabFile(_info.GetDownloadedFile(), unpackDir);
		if (!res)
			return res;

		std::vector<std::wstring> msiFiles;
		res = NX::win::Directory::FindFiles(unpackDir, msiFiles, false, [](const WIN32_FIND_DATAW* fdw) -> bool {
			const wchar_t* p = wcsrchr(fdw->cFileName, L'.');
			if (p && 0 == _wcsicmp(L".msi", p)) {
				const std::wstring namePart(fdw->cFileName, p);
				return (0 == _wcsicmp(NextLabsFileName.c_str(), namePart.c_str()));
			}
			return false;
		});
		if (msiFiles.empty())
			return RESULT(ERROR_FILE_NOT_FOUND);
		const std::wstring& msiFile = msiFiles[0];
		res = VerifyPublisher(msiFile, NextLabsSubject);
		if (!res)
			return res;
		std::wstring logFile(GetUpgradeDir() + L"\\install." + _info.GetVersion() + L".log");
		res = InstallMSI(msiFile, false, true, logFile);
		if (!res)
			return res;
	}
	else if (NX::iend_with<wchar_t>(_info.GetDownloadedFile(), L".msi")) {
		// It is a msi file
		std::wstring logFile(GetUpgradeDir() + L"\\install." + _info.GetVersion() + L".log");
		res = InstallMSI(_info.GetDownloadedFile(), false, true, logFile);
		if (!res)
			return res;
	}
	else if (NX::iend_with<wchar_t>(_info.GetDownloadedFile(), L".exe")) {
		// It is a exe file
		res = InstallEXE(_info.GetDownloadedFile());
		if (!res)
			return res;
	}
	else {
		// It is type that we don't support
		return RESULT(ERROR_BAD_FILE_TYPE);
	}

	return RESULT(0);
}

Result RmUpgrade::VerifySha1Checksum(const std::wstring& file, const std::wstring& expectedChecksum)
{
	HANDLE h = INVALID_HANDLE_VALUE;

	h = ::CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == h)
		return RESULT(GetLastError());

	Result res = RESULT(0);

	do {

		DWORD dwSize = ::GetFileSize(h, NULL);
		if (INVALID_FILE_SIZE == dwSize) {
			res = RESULT(GetLastError());
			break;
		}

		if (0 == dwSize) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		DWORD dwRead = 0;
		std::vector<unsigned char> buf;
		buf.resize(dwSize, 0);

		if (!::ReadFile(h, buf.data(), dwSize, &dwRead, NULL)) {
			res = RESULT(GetLastError());
			break;
		}
		if (dwSize != dwRead) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		UCHAR newChecksum[20] = { 0 };

		res = NX::crypt::CreateSha1(buf.data(), (ULONG)buf.size(), newChecksum);
		if (!res)
			break;

		const std::wstring& s = NX::bintohs<wchar_t>(newChecksum, 20);
		if (0 != _wcsicmp(s.c_str(), expectedChecksum.c_str())) {
			res = RESULT(ERROR_DATA_CHECKSUM_ERROR);
			break;
		}

	} while (FALSE);
	if (INVALID_HANDLE_VALUE != h) {
		CloseHandle(h);
		h = INVALID_HANDLE_VALUE;
	}
	if (!res)
		return res;

	return RESULT(0);
}

Result RmUpgrade::VerifyPublisher(const std::wstring& file, const std::wstring& publisher)
{
	NX::crypt::CertContext cc;
	Result res = cc.CreateFromEmbeddedSignature(file);
	if (!res)
		return RESULT(ERROR_NOT_FOUND);

	const NX::crypt::CertInfo& info = cc.GetCertInfo();
	if (0 != _wcsicmp(publisher.c_str(), info.GetSubject().c_str()))
		return RESULT(ERROR_EVT_INVALID_PUBLISHER_NAME);

	return RESULT(0);
}

Result RmUpgrade::UnpackCabFile(const std::wstring& file, std::wstring& dir)
{
	std::wstring::size_type pos = file.rfind(L'\\');
	const std::wstring fileName((pos == std::wstring::npos) ? file : file.substr(pos + 1));

	dir = GetUpgradeDir();
	dir.append(L"\\");
	pos = fileName.rfind(L'.');
	dir.append((std::wstring::npos == pos) ? fileName : fileName.substr(0, pos));

	//const std::vector<NX::win::ExplicitAccess> eas({
	//	NX::win::ExplicitAccess(NX::win::SidObject::LocalUsersSid, GENERIC_READ | GENERIC_WRITE, TRUSTEE_IS_WELL_KNOWN_GROUP, INHERIT_ONLY)
	//});
	//NX::win::SecurityAttribute sa(eas);

	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(dir.c_str())) {
		if (!::CreateDirectoryW(dir.c_str(), NULL)) {
			return RESULT(GetLastError());
		}
		Result r2 = win::GrantFileAccess(dir, NX::win::ExplicitAccess(NX::win::SidObject::LocalUsersSid, GENERIC_READ | GENERIC_WRITE, TRUSTEE_IS_WELL_KNOWN_GROUP, INHERIT_ONLY));
		if (!r2) {
			;
		}
	}
	else {
		std::vector<std::wstring> failedList;
		NX::win::Directory::DeleteFiles(dir, failedList, true, [](const WIN32_FIND_DATAW* fdw) -> bool {return true; });
	}

	Result res = NX::win::ZipUnpack(file, dir);
	if (!res)
		return res;

	return RESULT(0);
}

Result RmUpgrade::InstallMSI(const std::wstring& msiFile, bool quiet, bool noRestart, const std::wstring& logFile)
{
	// Good, start installer now
	std::wstring msiexecCmd(NX::win::GetSystemDirs().GetSystemDir());
	msiexecCmd.append(L"\\msiexec.exe /i \"");
	msiexecCmd.append(msiFile);
	msiexecCmd.append(L"\"");
	if(quiet)
		msiexecCmd.append(L" /quiet");
	if(noRestart)
		msiexecCmd.append(L" /norestart");
	if (!logFile.empty()) {
		msiexecCmd.append(L" /L*V \"");
		msiexecCmd.append(logFile);
		msiexecCmd.append(L"\"");
	}

	NX::win::FilePath path(msiFile);

	// Create Process
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(si);
	if (!::CreateProcessW(NULL, (LPWSTR)msiexecCmd.c_str(), NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, path.GetParentDir().c_str(), &si, &pi)) {
		return RESULT(GetLastError());
	}
	ResumeThread(pi.hThread);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return RESULT(0);
}

Result RmUpgrade::InstallEXE(const std::wstring& exeFile)
{
	NX::win::FilePath path(exeFile);
	STARTUPINFOW si;
	PROCESS_INFORMATION pi;
	memset(&si, 0, sizeof(si));
	memset(&pi, 0, sizeof(pi));
	si.cb = sizeof(si);
	if (!::CreateProcessW(NULL, (LPWSTR)exeFile.c_str(), NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, path.GetParentDir().c_str(), &si, &pi)) {
		return RESULT(GetLastError());
	}
	ResumeThread(pi.hThread);
	CloseHandle(pi.hThread);
	CloseHandle(pi.hProcess);
	return RESULT(0);
}

Result RmUpgrade::PrepareDirs()
{
	const std::vector<NX::win::ExplicitAccess> eas({
		NX::win::ExplicitAccess(NX::win::SidObject::LocalUsersSid, SPECIFIC_RIGHTS_ALL | GENERIC_READ | GENERIC_WRITE, TRUSTEE_IS_WELL_KNOWN_GROUP, INHERIT_ONLY)
	});
	NX::win::SecurityAttribute sa(eas);

	std::wstring path(NX::win::GetSystemDirs().GetProgramDataDir() + L"\\NextLabs");
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(path.c_str())) {
		if (!::CreateDirectoryW(path.c_str(), &sa)) {
			return RESULT(GetLastError());
		}
	}

	path.append(L"\\Upgrade");
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(path.c_str())) {
		if (!::CreateDirectoryW(path.c_str(), &sa)) {
			return RESULT(GetLastError());
		}
	}

	return RESULT(0);
}


const std::wstring& RmUpgrade::GetUpgradeDir()
{
	static const std::wstring _upgradeDir(NX::win::GetSystemDirs().GetProgramDataDir() + L"\\NextLabs\\Upgrade");
	return _upgradeDir;
}

const std::wstring& RmUpgrade::GetNewVersionInfoFilePath()
{
	static const std::wstring _newVersionInfoFile(NX::win::GetSystemDirs().GetProgramDataDir() + L"\\NextLabs\\Upgrade\\NewVersion.inf");
	return _newVersionInfoFile;
}