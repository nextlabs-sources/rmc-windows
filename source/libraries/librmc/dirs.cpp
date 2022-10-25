
#include "stdafx.h"

#include <nx\rmc\dirs.h>

#include <nx\common\string.h>

#include <Shlobj.h>

using namespace NX;


RmDirs::RmDirs()
{
    Init();
}

RmDirs::RmDirs(const RmDirs& rhs)
    : _app_root(rhs._app_root),
    _data_root(rhs._data_root),
    _bin_dir(rhs._bin_dir)
{
}

RmDirs::~RmDirs()
{
}

RmDirs& RmDirs::operator = (const RmDirs& rhs)
{
    if (this != &rhs)
    {
        _app_root = rhs._app_root;
        _data_root = rhs._data_root;
        _bin_dir = rhs._bin_dir;
    }

    return *this;
}

void RmDirs::SetAppRoot(const std::wstring& dir)
{
	if (dir.empty())
		return;

	std::wstring sRoot(dir);
	if (sRoot[sRoot.length() - 1] == L'\\')
		sRoot = sRoot.substr(0, sRoot.length() - 1);

	DWORD dwAttributes = GetFileAttributesW(sRoot.c_str());
	if (INVALID_FILE_ATTRIBUTES == dwAttributes || (FILE_ATTRIBUTE_DIRECTORY != (FILE_ATTRIBUTE_DIRECTORY & dwAttributes)))
		return;

	_app_root = sRoot;
	_bin_dir = sRoot + L"\\bin";
}

void RmDirs::Init()
{
    std::wstring image_path;
    std::wstring image_name;
    const std::wstring bin_name(L"\\bin");

    // Application Directory is current directory or its upper directory (if current one is "bin")
    GetModuleFileNameW(NULL, wstring_buffer(image_path, MAX_PATH), MAX_PATH);
    _bin_dir = get_file_directory(image_path);
    if (iend_with(_bin_dir, bin_name)) {
        _app_root = _bin_dir.substr(0, _bin_dir.length() - bin_name.length());
    }
    else {
        _app_root = _bin_dir;
    }

    // Get user profile AppData dir
    PWSTR pszPath = NULL;
    HRESULT hr = SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DEFAULT, NULL, &pszPath);
    if (SUCCEEDED(hr)) {
        _data_root = pszPath;
        _data_root += L"\\NextLabs";
        ::CreateDirectoryW(_data_root.c_str(), NULL);
        _data_root += L"\\SkyDRM";
        ::CreateDirectoryW(_data_root.c_str(), NULL);
    }
    else {
        _data_root = _app_root;
    }
    if (NULL != pszPath) {
        CoTaskMemFree(pszPath);
        pszPath = NULL;
    }

    assert(NULL == pszPath);

    const std::wstring& temp_dir = GetDataTempDir();
    ::CreateDirectoryW(temp_dir.c_str(), NULL);
    const std::wstring& cache_dir = GetDataCacheDir();
    ::CreateDirectoryW(cache_dir.c_str(), NULL);
    const std::wstring& offline_dir = GetDataOfflineDir();
    ::CreateDirectoryW(offline_dir.c_str(), NULL);
}
