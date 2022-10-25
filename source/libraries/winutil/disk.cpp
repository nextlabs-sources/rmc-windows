

#include "stdafx.h"

#include <nx\common\string.h>
#include <nx\winutil\disk.h>

using namespace NX;
using namespace NX::win;



DriveSpace DriveSpace::QueryDriveSpace(const wchar_t letter)
{
	const WCHAR wzDrive[4] = { letter, L':', L'\\', 0 };
	ULARGE_INTEGER li_total = { 0, 0 };
	ULARGE_INTEGER li_total_free = { 0, 0 };
	ULARGE_INTEGER li_available_free = { 0, 0 };
	UINT old_state = SetErrorMode(SEM_FAILCRITICALERRORS);
	GetDiskFreeSpaceExW(wzDrive, &li_available_free, &li_total, &li_total_free);
	SetErrorMode(old_state);
	return DriveSpace(li_total.QuadPart, li_total_free.QuadPart, li_available_free.QuadPart);
}

DriveInfo::DriveInfo() : _letter(0), _type(DRIVE_UNKNOWN), _typeName(L"Unknown")
{
}

DriveInfo::DriveInfo(const wchar_t letter) : _letter(letter), _type(QueryDriveType(letter)), _typeName(DriveTypeToName(_type))
{
	if (IsValid()) {
		GetNames();
	}
	else {
		Clear();
	}
}

DriveInfo::DriveInfo(const DriveInfo& rhs)
	: _letter(rhs._letter), _type(rhs._type), _typeName(rhs._typeName), _dosName(rhs._dosName), _ntName(rhs._ntName)
{
}

DriveInfo::~DriveInfo()
{
}

DriveInfo& DriveInfo::operator = (const DriveInfo& rhs)
{
	if (this != &rhs) {
		_letter = rhs._letter;
		_type = rhs._type;
		_typeName = rhs._typeName;
		_dosName = rhs._dosName;
		_ntName = rhs._ntName;
	}
	return *this;
}

void DriveInfo::Clear()
{
	_letter = 0;
	_type = DRIVE_UNKNOWN;
	_dosName.clear();
	_ntName.clear();
	_typeName = L"Unknown";
}

/*
#define DRIVE_UNKNOWN     0
#define DRIVE_NO_ROOT_DIR 1
#define DRIVE_REMOVABLE   2
#define DRIVE_FIXED       3
#define DRIVE_REMOTE      4
#define DRIVE_CDROM       5
#define DRIVE_RAMDISK     6
*/
bool DriveInfo::IsValid() const
{
	return (_type > DRIVE_NO_ROOT_DIR);
}

bool DriveInfo::IsRemovable() const
{
	return (_type == DRIVE_REMOVABLE);
}

bool DriveInfo::IsFixed() const
{
	return (_type == DRIVE_FIXED);
}

bool DriveInfo::IsRemote() const
{
	return (_type == DRIVE_REMOTE);
}

bool DriveInfo::IsCdrom() const
{
	return (_type == DRIVE_CDROM);
}

bool DriveInfo::IsRamdisk() const
{
	return (_type == DRIVE_RAMDISK);
}

DriveSpace DriveInfo::GetSpace() const
{
	return DriveSpace::QueryDriveSpace(_letter);
}

unsigned int DriveInfo::QueryDriveType(const wchar_t driveLetter)
{
	const wchar_t drivePath[4] = { driveLetter, L':', L'\\', 0 };
	return GetDriveTypeW(drivePath);
}

std::wstring DriveInfo::DriveTypeToName(unsigned int type)
{
	switch (type)
	{
	case DRIVE_FIXED:
		return std::wstring(L"Fixed Drive");
	case DRIVE_REMOVABLE:
		return std::wstring(L"Removable Drive");
	case DRIVE_REMOTE:
		return std::wstring(L"Remote Drive");
	case DRIVE_CDROM:
		return std::wstring(L"CD/DVD");
	case DRIVE_RAMDISK:
		return std::wstring(L"Ram Disk");
	case DRIVE_NO_ROOT_DIR:
	case DRIVE_UNKNOWN:
	default:
		break;
	}

	return std::wstring(L"Unknown");
}

void DriveInfo::GetNames()
{
	if (!IsValid())
		return;
	
	const wchar_t drivePath[4] = { _letter, L':', L'\\', 0 };

	if (IsRemote()) {
		UNIVERSAL_NAME_INFOW* puni = NULL;
		std::vector<unsigned char> buf;
		buf.resize(2);
		unsigned long bufsize = 2;
		//Call WNetGetUniversalName using UNIVERSAL_NAME_INFO_LEVEL option
		if (WNetGetUniversalNameW(drivePath, UNIVERSAL_NAME_INFO_LEVEL, (LPVOID)buf.data(), &bufsize) == ERROR_MORE_DATA) {
			// now we have the size required to hold the UNC path
			buf.resize(bufsize + 1, 0);
			puni = (UNIVERSAL_NAME_INFOW*)buf.data();
			if (WNetGetUniversalNameW(drivePath, UNIVERSAL_NAME_INFO_LEVEL, (LPVOID)puni, &bufsize) == NO_ERROR) {
				_dosName = puni->lpUniversalName;
				_ntName = L"\\Device\\Mup";
				_ntName += _dosName.substr(1);
			}
		}
	}
	else {
		_dosName = std::wstring(drivePath, drivePath + 2);
		if (!QueryDosDeviceW(_dosName.c_str(), NX::wstring_buffer(_ntName, 1024), 1024)) {
			_ntName.clear();
		}
	}
}

std::vector<DriveInfo> NX::win::GetLogicDrives()
{
	std::vector<DriveInfo> drives;
	wchar_t driveNames[512] = { 0 };
	memset(driveNames, 0, sizeof(driveNames));
	if (GetLogicalDriveStringsW(511, driveNames)) {
		const wchar_t* p = driveNames;
		while (0 != *p) {
			const wchar_t driveLetter = (wchar_t)::toupper(*p);
			p += (wcslen(p) + 1);
			DriveInfo drive(driveLetter);
			if(drive.IsValid())
				drives.push_back(drive);
		}
	}
	return drives;
}

DriveInfo LogicDriveFinder::Find()
{
	wchar_t driveNames[512] = { 0 };
	memset(driveNames, 0, sizeof(driveNames));
	if (GetLogicalDriveStringsW(511, driveNames)) {
		const wchar_t* p = driveNames;
		while (0 != *p) {
			const wchar_t driveLetter = (wchar_t)::toupper(*p);
			p += (wcslen(p) + 1);
			DriveInfo drive(driveLetter);
			if (drive.IsValid() && Match(drive))
				return drive;
		}
	}
	// Not found
	return DriveInfo();
}