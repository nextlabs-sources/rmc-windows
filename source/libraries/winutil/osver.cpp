

#include "stdafx.h"

#include <nx\common\string.h>
#include <nx\winutil\osver.h>
#include <nx\winutil\registry.h>

#include <VersionHelpers.h>

using namespace NX;
using namespace NX::win;


const OsVersion& NX::win::GetOsVersion()
{
	static const OsVersion osver(OsVersion::QueryOsVersion());
	return osver;
}


OsVersion::OsVersion()
	: _isServer(false),
	_cpuArch(0),
	_buildNumber(0),
	_platformId(PLATFORM_WINDOWS)
{
}

OsVersion::OsVersion(const OsVersion& rhs)
	: _isServer(rhs._isServer),
	_cpuArch(rhs._cpuArch),
	_buildNumber(rhs._buildNumber),
	_platformId(rhs._platformId),
	_name(rhs._name),
	_edition(rhs._edition),
	_winDir(rhs._winDir),
	_verStr(rhs._verStr)
{
}

OsVersion::~OsVersion()
{
}

OsVersion& OsVersion::operator = (const OsVersion& rhs)
{
	if (this != &rhs) {
		_isServer = rhs._isServer;
		_cpuArch = rhs._cpuArch;
		_buildNumber = rhs._buildNumber;
		_platformId = rhs._platformId;
		_name = rhs._name;
		_edition = rhs._edition;
		_winDir = rhs._winDir;
		_verStr = rhs._verStr;
	}
	return *this;
}

static unsigned long GetProcessorArchitecture()
{
	SYSTEM_INFO         sysinf;
	memset(&sysinf, 0, sizeof(SYSTEM_INFO));
	GetNativeSystemInfo(&sysinf);
	return sysinf.wProcessorArchitecture;
}

static unsigned long VersionToPlatformId(const std::wstring& sVer, bool isServer)
{
	if (sVer == L"5.0") {
		return PLATFORM_WIN2000;
	}
	else if (sVer == L"5.1") {
		return PLATFORM_WINXP;
	}
	else if (sVer == L"5.2") {
		if (isServer) {
			return (0 == GetSystemMetrics(SM_SERVERR2)) ? PLATFORM_WINDOWS_SERVER_2003 : PLATFORM_WINDOWS_SERVER_2003_R2;
		}
		else {
			return PLATFORM_WINXP;
		}
	}
	else if (sVer == L"6.0") {
		return isServer ? PLATFORM_WINDOWS_SERVER_2008 : PLATFORM_WINVISTA;
	}
	else if (sVer == L"6.1") {
		return isServer ? PLATFORM_WINDOWS_SERVER_2008_R2 : PLATFORM_WIN7;
	}
	else if (sVer == L"6.2") {
		return isServer ? PLATFORM_WINDOWS_SERVER_2012 : PLATFORM_WIN8;
	}
	else if (sVer == L"6.3") {
		if (IsWindows10OrGreater()) {
			return isServer ? PLATFORM_WINDOWS_SERVER_2016 : PLATFORM_WIN10;
		}
		else {
			return isServer ? PLATFORM_WINDOWS_SERVER_2012_R2 : PLATFORM_WIN8_1;
		}
	}
	else {
		return PLATFORM_WINDOWS;
	}
}

OsVersion OsVersion::QueryOsVersion()
{
	OsVersion		ver;
	std::wstring	sTemp;

	ver._isServer = IsWindowsServer();
	ver._cpuArch  = GetProcessorArchitecture();


	RegMachineKey rkey;
	Result res = rkey.Open(L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", true);
	if (!res)
		return ver;

	res = rkey.ReadValue(L"ProductName", ver._name);
	res = rkey.ReadValue(L"EditionID", ver._edition);
	res = rkey.ReadValue(L"CurrentVersion", ver._verStr);
	res = rkey.ReadValue(L"CurrentBuildNumber", sTemp);
	if (res) {
		ver._buildNumber = static_cast<unsigned long>((int)NX::stoi64<wchar_t>(sTemp));
	}

	ver._platformId = VersionToPlatformId(ver._verStr, ver._isServer);
	GetWindowsDirectoryW(NX::wstring_buffer(ver._winDir, MAX_PATH), MAX_PATH);

	return ver;
}


bool OsVersion::IsXpOrGreater() { return IsWindowsXPOrGreater(); }
bool OsVersion::IsXpSp1OrGreater() { return IsWindowsXPSP1OrGreater(); }
bool OsVersion::IsXpSp2OrGreater() { return IsWindowsXPSP2OrGreater(); }
bool OsVersion::IsXpSp3OrGreater() { return IsWindowsXPSP3OrGreater(); }
bool OsVersion::IsVistaOrGreater() { return IsWindowsVistaOrGreater(); }
bool OsVersion::IsVistaSp1OrGreater() { return IsWindowsVistaSP1OrGreater(); }
bool OsVersion::IsVistaSp2OrGreater() { return IsWindowsVistaSP2OrGreater(); }
bool OsVersion::IsWin7OrGreater() { return IsWindows7OrGreater(); }
bool OsVersion::IsWin7Sp1OrGreater() { return IsWindows7SP1OrGreater(); }
bool OsVersion::IsWin8OrGreater() { return IsWindows8OrGreater(); }
bool OsVersion::IsWin81OrGreater() { return IsWindows8Point1OrGreater(); }
bool OsVersion::IsWin10OrGreater() { return IsWindows10OrGreater(); }

bool OsVersion::IsWinsrv03OrGreater()
{
	return (IsWindowsServer() && IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WS03), LOBYTE(_WIN32_WINNT_WS03), 0));
}

bool OsVersion::IsWinsrv03_sp1OrGreater()
{
	return (IsWindowsServer() && IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WS03), LOBYTE(_WIN32_WINNT_WS03), 1));
}

bool OsVersion::IsWinsrv03_sp2OrGreater()
{
	return (IsWindowsServer() && IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WS03), LOBYTE(_WIN32_WINNT_WS03), 2));
}

bool OsVersion::IsWinsrv03_sp3OrGreater()
{
	return (IsWindowsServer() && IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WS03), LOBYTE(_WIN32_WINNT_WS03), 3));
}

bool OsVersion::IsWinsrv03r2OrGreater()
{
	return (IsWindowsServer() && 0 != GetSystemMetrics(SM_SERVERR2) && IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WS03), LOBYTE(_WIN32_WINNT_WS03), 0));
}

bool OsVersion::IsWinsrv03r2_sp1OrGreater()
{
	return (IsWindowsServer() && 0 != GetSystemMetrics(SM_SERVERR2) && IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WS03), LOBYTE(_WIN32_WINNT_WS03), 1));
}

bool OsVersion::IsWinsrv03r2_sp2OrGreater()
{
	return (IsWindowsServer() && 0 != GetSystemMetrics(SM_SERVERR2) && IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WS03), LOBYTE(_WIN32_WINNT_WS03), 2));
}

bool OsVersion::IsWinsrv03r2_sp3OrGreater()
{
	return (IsWindowsServer() && 0 != GetSystemMetrics(SM_SERVERR2) && IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WS03), LOBYTE(_WIN32_WINNT_WS03), 3));
}

bool OsVersion::IsWinsrv08OrGreater()
{
	return (IsWindowsServer() && IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WS08), LOBYTE(_WIN32_WINNT_WS08), 0));
}

bool OsVersion::IsWinsrv08_sp1OrGreater()
{
	return (IsWindowsServer() && IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WS08), LOBYTE(_WIN32_WINNT_WS08), 1));
}

bool OsVersion::IsWinsrv08_sp2OrGreater()
{
	return (IsWindowsServer() && IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WS08), LOBYTE(_WIN32_WINNT_WS08), 2));
}

bool OsVersion::IsWinsrv08_sp3OrGreater()
{
	return (IsWindowsServer() && IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WS08), LOBYTE(_WIN32_WINNT_WS08), 3));
}

bool OsVersion::IsWinsrv08r2OrGreater()
{
	return (IsWindowsServer() && IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 0));
}

bool OsVersion::IsWinsrv08r2_sp1OrGreater()
{
	return (IsWindowsServer() && IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN7), LOBYTE(_WIN32_WINNT_WIN7), 1));
}

bool OsVersion::IsWinsrv12OrGreater()
{
	return (IsWindowsServer() && IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WIN8), LOBYTE(_WIN32_WINNT_WIN8), 0));
}

bool OsVersion::IsWinsrv12r2OrGreater()
{
	return (IsWindowsServer() && IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINBLUE), LOBYTE(_WIN32_WINNT_WINBLUE), 0));
}

bool OsVersion::IsWinsrv16OrGreater()
{
	return (IsWindowsServer() && IsWindowsVersionOrGreater(HIBYTE(_WIN32_WINNT_WINTHRESHOLD), LOBYTE(_WIN32_WINNT_WINTHRESHOLD), 0));
}

IEVersion::IEVersion() : _major(0), _minor(0), _build(0)
{
	RegMachineKey iekey;
	Result res = iekey.Open(L"SOFTWARE\\Microsoft\\Internet Explorer", true);
	if (!res)
		return;

	std::wstring s;
	res = iekey.ReadValue(L"svcVersion", s);
	if (!res)
		return;

	const std::vector<std::wstring>& numbers = NX::split<wchar_t, L'.'>(s);
	if (numbers.empty())
		return;

	_major = std::stoi(numbers[0]);
	if(numbers.size() > 1) _minor = std::stoi(numbers[1]);
	if(numbers.size() > 2) _build = std::stoi(numbers[2]);
}

IEAppEmulation::IEEMULATECODE IEAppEmulation::ReadEmulationCode()
{
	RegUserKey iekey;
	Result res = iekey.Open(L"SOFTWARE\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION", true);
	if (!res)
		return IE7Default;

	int code = 0;
	res = iekey.ReadValue(_appName, (UINT*)&code);
	if (!res)
		return IE7Default;

	_emulateCode = ConvertCode(code);
	return _emulateCode;
}

Result IEAppEmulation::WriteEmulationCode(IEEMULATECODE code)
{
	RegUserKey iekey;
	Result res = iekey.Create(L"SOFTWARE\\Microsoft\\Internet Explorer\\Main\\FeatureControl\\FEATURE_BROWSER_EMULATION");
	if (!res)
		return res;

	res = iekey.WriteValue(_appName, (UINT)code);
	if (!res)
		return res;

	_emulateCode = code;
	return RESULT(0);
}

IEAppEmulation::IEEMULATECODE IEAppEmulation::ConvertCode(int code)
{
	IEEMULATECODE ecode = IE7Default;

	switch (code)
	{
	case IE11EdgeMode:
		ecode = IE11EdgeMode;
		break;
	case IE11Default:
		ecode = IE11Default;
		break;
	case IE10Standard:
		ecode = IE10Standard;
		break;
	case IE10Default:
		ecode = IE10Default;
		break;
	case IE9Standard:
		ecode = IE9Standard;
		break;
	case IE9Default:
		ecode = IE9Default;
		break;
	case IE8Standard:
		ecode = IE8Standard;
		break;
	case IE8Default:
		ecode = IE8Default;
		break;
	case IE7Default:
		ecode = IE7Default;
		break;
	default:
		if (code < 8000)
			ecode = IE7Default;
		else if (code < 9000)
			ecode = IE8Default;
		else if (code < 10000)
			ecode = IE9Default;
		else if (code < 11000)
			ecode = IE10Default;
		else
			ecode = IE11Default;
		break;
	}

	return ecode;
}