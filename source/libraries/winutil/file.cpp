

#include "stdafx.h"

#include <nx\common\stringex.h>
#include <nx\winutil\file.h>

using namespace NX;
using namespace NX::win;


FileInfo::FileInfo()
	: _product_version({0, 0}),
	_file_version({ 0, 0 }),
	_file_time({ 0, 0 }),
	_file_flags(0),
	_file_os(0),
	_file_type(0),
	_file_subtype(0)
{
}

FileInfo::FileInfo(const FileInfo& rhs)
	: _company_name(rhs._company_name),
	_product_name(rhs._product_name),
	_product_version_string(rhs._product_version_string),
	_file_name(rhs._file_name),
	_file_description(rhs._file_description),
	_file_version_string(rhs._file_version_string),
	_product_version({ rhs._product_version.LowPart, rhs._product_version.HighPart }),
	_file_version({ rhs._file_version.LowPart, rhs._file_version.HighPart }),
	_file_time({ rhs._file_time.dwLowDateTime, rhs._file_time.dwHighDateTime }),
	_file_flags(rhs._file_flags),
	_file_os(rhs._file_os),
	_file_type(rhs._file_type),
	_file_subtype(rhs._file_subtype)
{
}

FileInfo::~FileInfo()
{
}

FileInfo& FileInfo::operator = (const FileInfo& rhs)
{
	if (this != &rhs) {
		_company_name = rhs._company_name;
		_product_name = rhs._product_name;
		_product_version_string = rhs._product_version_string;
		_file_name = rhs._file_name;
		_file_description = rhs._file_description;
		_file_version_string = rhs._file_version_string;
		_product_version = { rhs._product_version.LowPart, rhs._product_version.HighPart };
		_file_version = { rhs._file_version.LowPart, rhs._file_version.HighPart };
		_file_time = { rhs._file_time.dwLowDateTime, rhs._file_time.dwHighDateTime };
		_file_flags = rhs._file_flags;
		_file_os = rhs._file_os;
		_file_type = rhs._file_type;
		_file_subtype = rhs._file_subtype;
	}
	return *this;
}

void FileInfo::Clear()
{
	_company_name.clear();
	_product_name.clear();
	_product_version_string.clear();
	_file_name.clear();
	_file_description.clear();
	_file_version_string.clear();
	_product_version = { 0, 0 };
	_file_version = { 0, 0 };
	_file_time = { 0, 0 };
	_file_flags = 0;
	_file_os = 0;
	_file_type = 0;
	_file_subtype = 0;
}

static std::wstring LoadFileString(void* data, const std::wstring& name, unsigned short language, unsigned short codepage)
{
	unsigned int cch = 0;
	wchar_t* str = NULL;
	const std::wstring resource_name = NX::FormatString(L"\\StringFileInfo\\%04x%04x\\%s", language, codepage, name.c_str());
	if (!VerQueryValueW(data, resource_name.c_str(), (LPVOID*)&str, &cch)) {
		return std::wstring();
	}
	if (0 == cch || NULL == str) {
		return std::wstring();
	}
	std::wstring s = str;
	return s;
}

FileInfo FileInfo::LoadFileInfo(const std::wstring& file)
{
	typedef struct LANGANDCODEPAGE {
		WORD wLanguage;
		WORD wCodePage;
	} LANGANDCODEPAGE, *LPLANGANDCODEPAGE;

	FileInfo fileinfo;

	try {

		const unsigned long version_info_size = GetFileVersionInfoSizeW(file.c_str(), NULL);
		if (0 == version_info_size) {
			throw std::exception("fail to get version size");
		}

		std::vector<unsigned char> info;
		info.resize(version_info_size, 0);
		if (!GetFileVersionInfoW(file.c_str(), 0, version_info_size, info.data())) {
			throw std::exception("fail to get version data");
		}

		VS_FIXEDFILEINFO* fixed_info = NULL;
		unsigned int data_size = 0;

		if (!VerQueryValueW(info.data(), L"\\", (LPVOID*)&fixed_info, &data_size)) {
			throw std::exception("fail to get fixed file info");
		}
		fileinfo._product_version.HighPart = fixed_info->dwProductVersionMS;
		fileinfo._product_version.LowPart = fixed_info->dwProductVersionLS;
		fileinfo._file_version.HighPart = fixed_info->dwFileVersionMS;
		fileinfo._file_version.LowPart = fixed_info->dwFileVersionLS;
		fileinfo._file_time.dwHighDateTime = fixed_info->dwFileDateMS;
		fileinfo._file_time.dwLowDateTime = fixed_info->dwFileDateLS;
		fileinfo._file_flags = fixed_info->dwFileFlags;
		fileinfo._file_os = fixed_info->dwFileOS;
		fileinfo._file_type = fixed_info->dwFileType;
		fileinfo._file_subtype = fixed_info->dwFileSubtype;

		// get language & codepage
		LPLANGANDCODEPAGE Translate;
		if (!VerQueryValueW(info.data(), L"\\VarFileInfo\\Translation", (LPVOID*)&Translate, &data_size)) {
			throw std::exception("fail to get translate info");
		}
		if (NULL == Translate || data_size < sizeof(LANGANDCODEPAGE)) {
			throw std::exception("empty translate info");
		}

		// now get all the string information in default language
		//    -> CompanyName
		fileinfo._company_name = LoadFileString(info.data(), L"CompanyName", Translate[0].wLanguage, Translate[0].wCodePage);
		//    -> ProductName
		fileinfo._product_name = LoadFileString(info.data(), L"ProductName", Translate[0].wLanguage, Translate[0].wCodePage);
		//    -> OriginalFilename
		fileinfo._file_name = LoadFileString(info.data(), L"OriginalFilename", Translate[0].wLanguage, Translate[0].wCodePage);
		//    -> FileDescription
		fileinfo._file_description = LoadFileString(info.data(), L"FileDescription", Translate[0].wLanguage, Translate[0].wCodePage);
		//    -> ProductVersion
		fileinfo._product_version_string = LoadFileString(info.data(), L"ProductVersion", Translate[0].wLanguage, Translate[0].wCodePage);
		//    -> FileVersion
		fileinfo._file_version_string = LoadFileString(info.data(), L"FileVersion", Translate[0].wLanguage, Translate[0].wCodePage);
	}
	catch (const std::exception& e) {
		UNREFERENCED_PARAMETER(e);
		fileinfo.Clear();
	}

	return fileinfo;
}

bool FileFindMatchAll(const WIN32_FIND_DATAW* fdw)
{
	return true;
}

Result NX::win::Directory::DeleteFiles(const std::wstring& directory, std::vector<std::wstring>& failedList, bool recursively, FILEFINDMATCHFUNC Match)
{
	HANDLE h = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAW wfd = { 0 };

	if (NULL == Match)
		Match = FileFindMatchAll;

	if (directory.empty())
		return RESULT(ERROR_INVALID_PARAMETER);

	std::wstring searchName(directory);
	searchName.append((searchName[searchName.length() - 1] != L'\\') ? L"\\*" : L"*");

	h = ::FindFirstFileW(searchName.c_str(), &wfd);
	if (INVALID_HANDLE_VALUE == h)
		return RESULT(GetLastError());

	do {

		if (0 == _wcsicmp(wfd.cFileName, L".") || 0 == _wcsicmp(wfd.cFileName, L".."))
			continue;

		std::wstring filePath(directory);
		if (L'\\' != filePath[filePath.length() - 1])
			filePath.append(L"\\");
		filePath.append(wfd.cFileName);

		if (0 == (FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes)) {
			// Normal file
			if (Match(&wfd)) {
				if (!DeleteFileW(filePath.c_str()))
					failedList.push_back(filePath);
			}
		}
		else {
			// Directory
			if (recursively) {
				DeleteFiles(filePath, failedList, recursively, Match);
			}
			if (Match(&wfd)) {
				if (!RemoveDirectoryW(filePath.c_str()))
					failedList.push_back(filePath);
			}
		}

	} while (::FindNextFileW(h, &wfd));
	FindClose(h); h = INVALID_HANDLE_VALUE;

	assert(h == INVALID_HANDLE_VALUE);
	return RESULT(0);

}

Result NX::win::Directory::FindFiles(const std::wstring& directory, std::vector<std::wstring>& files, bool recursively, FILEFINDMATCHFUNC Match)
{
	HANDLE h = INVALID_HANDLE_VALUE;
	WIN32_FIND_DATAW wfd = { 0 };

	if (NULL == Match)
		Match = FileFindMatchAll;

	if (directory.empty())
		return RESULT(ERROR_INVALID_PARAMETER);

	std::wstring searchName(directory);
	searchName.append((searchName[searchName.length() - 1] != L'\\') ? L"\\*" : L"*");

	h = ::FindFirstFileW(searchName.c_str(), &wfd);
	if (INVALID_HANDLE_VALUE == h)
		return RESULT(GetLastError());

	do {

		if (0 == _wcsicmp(wfd.cFileName, L".") || 0 == _wcsicmp(wfd.cFileName, L".."))
			continue;

		std::wstring filePath(directory);
		if (L'\\' != filePath[filePath.length() - 1])
			filePath.append(L"\\");
		filePath.append(wfd.cFileName);

		if (0 == (FILE_ATTRIBUTE_DIRECTORY & wfd.dwFileAttributes)) {
			// Normal file
			if (Match(&wfd)) {
				files.push_back(filePath);
			}
		}
		else {
			// Directory
			if (recursively) {
				FindFiles(filePath, files, recursively, Match);
			}
		}

	} while (::FindNextFileW(h, &wfd));
	FindClose(h); h = INVALID_HANDLE_VALUE;

	assert(h == INVALID_HANDLE_VALUE);
	return RESULT(0);
}

Result NX::win::ReadSmallFile(const std::wstring& file, std::vector<UCHAR>& content)
{
	HANDLE h = INVALID_HANDLE_VALUE;

	h = ::CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == h)
		return RESULT(GetLastError());

	Result res = RESULT(0);
	do {

		DWORD dwSize = GetFileSize(h, NULL);
		if (INVALID_FILE_SIZE == dwSize) {
			res = RESULT(GetLastError());
			break;
		}

		if (0 == dwSize)
			break;

		DWORD bytesRead = 0;
		content.resize(dwSize, 0);
		if(!::ReadFile(h, content.data(), dwSize, &bytesRead, NULL)) {
			res = RESULT(GetLastError());
			break;
		}

	} while (false);
	CloseHandle(h); h = INVALID_HANDLE_VALUE;
	if (!res)
		return res;

	return RESULT(0);
}

Result NX::win::WriteSmallFile(const std::wstring& file, const std::vector<UCHAR>& content, bool overwrite, PSECURITY_ATTRIBUTES psa)
{
	HANDLE h = INVALID_HANDLE_VALUE;

	h = ::CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, psa, overwrite ? CREATE_ALWAYS : CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == h)
		return RESULT(GetLastError());

	Result res = RESULT(0);
	do {

		DWORD dwSize = GetFileSize(h, NULL);
		if (INVALID_FILE_SIZE == dwSize) {
			res = RESULT(GetLastError());
			break;
		}

		if (0 == dwSize)
			break;

		DWORD bytesWritten = 0;
		if(!::WriteFile(h, content.data(), dwSize, &bytesWritten, NULL)) {
			res = RESULT(GetLastError());
			break;
		}

	} while (false);
	CloseHandle(h); h = INVALID_HANDLE_VALUE;
	if (!res) {
		::DeleteFileW(file.c_str());
		return res;
	}

	return RESULT(0);
}


Result NX::win::GrantFileAccess(const std::wstring& file, const ExplicitAccess& ea)
{
	return NX::win::GrantFileAccess(file, &ea, 1);
}

Result NX::win::GrantFileAccess(const std::wstring& file, const ExplicitAccess* eas, unsigned long ea_count)
{
	Result res = RESULT(0);
	DWORD dwRes = ERROR_SUCCESS;
	PACL pOldDACL = NULL, pNewDACL = NULL;
	PSECURITY_DESCRIPTOR pSD = NULL;

	do {

		dwRes = GetNamedSecurityInfoW(file.c_str(),
			SE_FILE_OBJECT,
			DACL_SECURITY_INFORMATION,
			NULL,
			NULL,
			&pOldDACL,
			NULL,
			&pSD);
		if (ERROR_SUCCESS != dwRes) {
			res = RESULT(dwRes);
			break;
		}

		// Create a new ACL that merges the new ACE
		// into the existing DACL.
		dwRes = SetEntriesInAclW(ea_count, (PEXPLICIT_ACCESS)eas, pOldDACL, &pNewDACL);
		if (ERROR_SUCCESS != dwRes) {
			res = RESULT(dwRes);
			break;
		}

		// Attach the new ACL as the object's DACL.
		dwRes = SetNamedSecurityInfoW((LPWSTR)file.c_str(),
			SE_FILE_OBJECT,
			DACL_SECURITY_INFORMATION,
			NULL,
			NULL,
			pNewDACL,
			NULL);
		if (ERROR_SUCCESS != dwRes) {
			res = RESULT(dwRes);
			break;
		}

	} while (FALSE);

	if (pSD != NULL) {
		LocalFree((HLOCAL)pSD);
		pSD = NULL;
	}
	if (pNewDACL != NULL) {
		LocalFree((HLOCAL)pNewDACL);
		pNewDACL = NULL;
	}

	return res;
}

Result NX::win::GetFileTime(const std::wstring& file, _Out_opt_ LPFILETIME lpCreationTime, _Out_opt_ LPFILETIME lpLastAccessTime, _Out_opt_ LPFILETIME lpLastModifiedTime)
{
	HANDLE h = ::CreateFileW(file.c_str(), GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == h)
		return RESULT(GetLastError());

	Result res = ::GetFileTime(h, lpCreationTime, lpLastAccessTime, lpLastModifiedTime) ? RESULT(GetLastError()) : RESULT(0);
	CloseHandle(h);
	h = INVALID_HANDLE_VALUE;

	return res;
}

Result NX::win::SetFileTime(const std::wstring& file, _In_opt_ const FILETIME* lpCreationTime, _In_opt_ const FILETIME* lpLastAccessTime, _In_opt_ const FILETIME* lpLastModifiedTime)
{
	HANDLE h = ::CreateFileW(file.c_str(), GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == h)
		return RESULT(GetLastError());

	Result res = ::SetFileTime(h, lpCreationTime, lpLastAccessTime, lpLastModifiedTime) ? RESULT(GetLastError()) : RESULT(0);
	CloseHandle(h);
	h = INVALID_HANDLE_VALUE;

	return res;
}