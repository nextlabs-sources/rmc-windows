

#include "stdafx.h"

#include <Shlobj.h>
#include <Knownfolders.h>

#include <nx\common\stringex.h>
#include <nx\winutil\disk.h>
#include <nx\winutil\path.h>

using namespace NX;
using namespace NX::win;


namespace {

	class VolumeNameFinder : public LogicDriveFinder
	{
	public:
		VolumeNameFinder(const std::wstring& volume) : _volumeName(volume) {}
	protected:
		virtual bool Match(const DriveInfo& info)
		{
			return (0 == _wcsicmp(_volumeName.c_str(), info.GetNtName().c_str()));
		}
	private:
		std::wstring _volumeName;
	};

	std::wstring FileHandleToDosPath(HANDLE h)
	{
		std::wstring s = FilePath::FileHandleToNtPath(h);
		if (!s.empty()) {
			s = FilePath::NtPathToDosPath(s);
		}
		return s;
	}

}

FilePath::FilePath()
{
}

FilePath::FilePath(const std::wstring& path, bool normalize) : _fullPath(normalize ? Normalize(path) : path)
{
}

FilePath::FilePath(HANDLE h) : _fullPath(FileHandleToDosPath(h))
{
}

FilePath::FilePath(const FilePath& rhs) : _fullPath(rhs._fullPath)
{
}

FilePath::~FilePath()
{
}

FilePath& FilePath::operator = (const FilePath& rhs)
{
	if (this != &rhs) {
		_fullPath = rhs._fullPath;
	}
	return *this;
}

void FilePath::Clear()
{
	_fullPath.clear();
}

bool FilePath::IsRemoteFile() const
{
	return (IsGlobalPath(_fullPath) || IsGlobalUncPath(_fullPath));
}

std::wstring FilePath::GetParentDir() const
{
	auto pos = _fullPath.rfind(L'\\');
	return (pos == std::wstring::npos) ? std::wstring() : _fullPath.substr(0, pos);
}

std::wstring FilePath::GetFileName() const
{
	auto pos = _fullPath.rfind(L'\\');
	return (pos == std::wstring::npos) ? _fullPath : _fullPath.substr(pos + 1);
}

std::wstring FilePath::GetFileExtension() const
{
	const std::wstring& name = GetFileName();
	auto pos = name.rfind(L'.');
	return (pos == std::wstring::npos) ? std::wstring() : name.substr(pos);
}

bool FilePath::IsDosDriveName(const std::wstring& path)
{
	return (path.length() == 2
			&& NX::isalpha<wchar_t>(path[0])
			&& NX::icompare<wchar_t>(path[1], L':'));
}

bool FilePath::IsDosDriveRoot(const std::wstring& path)
{
	return (path.length() == 3
			&& NX::isalpha<wchar_t>(path[0])
			&& NX::icompare<wchar_t>(path[1], L':')
			&& NX::icompare<wchar_t>(path[2], L'\\'));
}

bool FilePath::IsGlobalPath(const std::wstring& path)
{
	return (path.length() > 4
			&& NX::icompare<wchar_t>(path[0], L'\\')
			&& NX::icompare<wchar_t>(path[2], L'?')
			&& NX::icompare<wchar_t>(path[3], L'\\')
			&& (NX::icompare<wchar_t>(path[1], L'\\') || NX::icompare<wchar_t>(path[1], L'?')));
}

bool FilePath::IsGlobalDosPath(const std::wstring& path)
{
	// "\??\C:\Windows\Temp\foo.doc"
	return (path.length() >= 7
			&& IsGlobalPath(path)
			&& NX::icompare<wchar_t>(path[5], L':')
			&& NX::icompare<wchar_t>(path[6], L'\\')
			&& !NX::isalpha<wchar_t>(path[4]));
}

bool FilePath::IsGlobalUncPath(const std::wstring& path)
{
	// "\??\UNC\MyServer\Temp\foo.doc"
	return (path.length() > 9
			&& IsGlobalPath(path)
			&& NX::icompare<wchar_t>(path[4], L'U')
			&& NX::icompare<wchar_t>(path[5], L'N')
			&& NX::icompare<wchar_t>(path[6], L'C')
			&& NX::icompare<wchar_t>(path[7], L'\\')
			&& !NX::icompare<wchar_t>(path[8], L'\\'));
}

bool FilePath::IsUncPath(const std::wstring& path)
{
	// "\\MyServer\Temp\foo.doc"
	return (path.length() > 4
			&& NX::icompare<wchar_t>(path[0], L'\\')
			&& NX::icompare<wchar_t>(path[1], L'\\')
			&& !NX::icompare<wchar_t>(path[2], L'\\'));
}

bool FilePath::IsDosPath(const std::wstring& path)
{
	return (path.length() >= 3
			&& NX::icompare<wchar_t>(path[1], L':')
			&& NX::icompare<wchar_t>(path[2], L'\\')
			&& !NX::isalpha<wchar_t>(path[0]));
}

bool FilePath::IsNtPath(const std::wstring& path)
{
	static const std::wstring prefix(L"\\Device\\");
	return NX::ibegin_with<wchar_t>(path, prefix);
}

bool FilePath::IsRemoteNtPath(const std::wstring& path)
{
	static const std::wstring prefix(L"\\Device\\Mup\\");
	return NX::ibegin_with<wchar_t>(path, prefix);
}

bool FilePath::IsFileName(const std::wstring& path)
{
	return (std::wstring::npos == path.find(L'\\'));
}

std::wstring FilePath::NtPathToDosPath(const std::wstring& path)
{
	if (!IsNtPath(path)) {
		return std::wstring();
	}

	if (IsRemoteNtPath(path)) {
		// "\Device\Mup\MyServer\Temp\foo.doc"
		return std::wstring(L"\\" + path.substr(11));
	}

	// Normal Nt path
	// e.g. "\Device\Harddisk8\Windows\Temp\foo.doc"
	auto pos = path.find(L'\\', 8);
	if (pos == std::wstring::npos) {
		// Something is wrong
		return std::wstring();
	}

	VolumeNameFinder finder(path.substr(0, pos));
	const DriveInfo& drive = finder.Find();
	if(drive.Empty() || !drive.IsValid())
		return std::wstring();

	std::wstring dosPath(drive.GetDosName());
	dosPath += path.substr(pos);
	return dosPath;
}

std::wstring FilePath::GetCurrentDir()
{
	std::wstring s;
	if (0 == GetCurrentDirectoryW(MAX_PATH, NX::wstring_buffer(s, MAX_PATH))) {
		s.clear();
	}
	return s;
}


std::wstring FilePath::ToShortFilePath(const std::wstring& path)
{
	std::wstring s;
	if (0 == GetShortPathNameW(path.c_str(), NX::wstring_buffer(s, MAX_PATH), MAX_PATH)) {
		s = path;
	}
	return s;
}

std::wstring FilePath::ToLongFilePath(const std::wstring& path)
{
	std::wstring s;
	if (0 == GetLongPathNameW(path.c_str(), NX::wstring_buffer(s, MAX_PATH), MAX_PATH)) {
		s = path;
	}
	return s;
}

std::wstring FilePath::DosPathToNtPath(const std::wstring& path)
{
	if (IsGlobalDosPath(path)) {
		// "\??\C:\Windows\Temp\foo.doc"
		const DriveInfo drive(path[4]);
		return drive.IsValid() ? std::wstring(drive.GetNtName() + path.substr(4)) : std::wstring();
	}
	else if (IsGlobalUncPath(path)) {
		// "\??\UNC\MyServer\Temp\foo.doc"
		return std::wstring(L"\\Device\\Mup" + path.substr(7));
	}
	else if (IsUncPath(path)) {
		// "\\MyServer\Temp\foo.doc"
		return std::wstring(L"\\Device\\Mup" + path.substr(1));
	}
	else if (IsDosPath(path)) {
		// "C:\Windows\Temp\foo.doc"
		const DriveInfo drive(path[0]);
		return drive.IsValid() ? std::wstring(drive.GetNtName() + path.substr(2)) : std::wstring();
	}
	else if (IsNtPath(path)) {
		return path;
	}
	else {
		// Unrecognized path
		return std::wstring();
	}
}

std::wstring FilePath::FileHandleToNtPath(HANDLE h)
{
	std::wstring path;
	if (0 == GetFinalPathNameByHandleW(h, NX::wstring_buffer(path, 1024), 1024, VOLUME_NAME_NT)) {
		// Failed
		path.clear();
	}
	return path;
}

std::wstring FilePath::Normalize(const std::wstring& path)
{
	std::wstring s;

	if (0 == GetFullPathNameW(path.c_str(), MAX_PATH, NX::wstring_buffer(s, MAX_PATH), NULL)) {
		s = path;
	}

	s = ToLongFilePath(s);

	if (IsGlobalUncPath(s)) {
		// "\??\UNC\MyServer\Temp\foo.doc"
		return std::wstring(L"\\" + s.substr(7));
	}

	if (IsGlobalDosPath(s)) {
		// "\??\C:\Windows\Temp\foo.doc"
		return s.substr(4);
	}

	if (IsDosPath(s) || IsUncPath(s)) {
		// "C:\Windows\Temp\foo.doc"
		// "\\MyServer\Temp\foo.doc"
		return s;
	}

	return NtPathToDosPath(s);
}


ModulePath::ModulePath() : FilePath()
{
}

ModulePath::ModulePath(HMODULE h) : FilePath(HandleToPath(h), false)
{
}

ModulePath::~ModulePath()
{
}

std::wstring ModulePath::HandleToPath(HMODULE h)
{
	std::wstring s;
	if (0 == GetModuleFileNameW(h, NX::wstring_buffer(s, MAX_PATH), MAX_PATH)) {
		s.clear();
	}
	return s;
}


//
//  Known Directories
//

const SystemDirs& NX::win::GetSystemDirs()
{
	static const SystemDirs gKnownSystemDirs;
	return gKnownSystemDirs;
}
const PublicUserDirs& NX::win::GetPublicUserDirs()
{
	static const PublicUserDirs	gKnownPublicUserDirs;
	return gKnownPublicUserDirs;
}
const UserDirs& NX::win::GetCurrentUserDirs()
{
	static const UserDirs gKnownCurrentUserDirs(NULL);
	return gKnownCurrentUserDirs;
}


std::wstring KnownDirs::GetKnownDirectory(const GUID& rfid, _In_opt_ HANDLE hToken, bool createIfNotExits)
{
	std::wstring s;
	PWSTR pszPath = NULL;
	HRESULT hr = SHGetKnownFolderPath((REFKNOWNFOLDERID)rfid, createIfNotExits ? KF_FLAG_DEFAULT_PATH : (KF_FLAG_DEFAULT_PATH | KF_FLAG_CREATE), hToken, &pszPath);
	if (SUCCEEDED(hr) && NULL != pszPath) {
		s = pszPath;
		CoTaskMemFree(pszPath);
		pszPath = NULL;
	}
	return s;
}


SystemDirs::SystemDirs()
{
	Initialize();
}

SystemDirs::SystemDirs(const SystemDirs& rhs)
	: _system(rhs._system), _windows(rhs._windows), _wintemp(rhs._wintemp), _fonts(rhs._fonts),
	_users(rhs._users), _programData(rhs._programData), _programFiles(rhs._programFiles), _programFilesX86(rhs._programFilesX86),
	_programCommonFiles(rhs._programCommonFiles), _programCommonFilesX86(rhs._programCommonFilesX86)
{
}

SystemDirs::~SystemDirs()
{
}

SystemDirs& SystemDirs::operator = (const SystemDirs& rhs)
{
	if (this != &rhs) {
		_system  = rhs._system;
		_windows = rhs._windows;
		_wintemp = rhs._wintemp;
		_fonts = rhs._fonts;
		_users = rhs._users;
		_programData = rhs._programData;
		_programFiles = rhs._programFiles;
		_programFilesX86 = rhs._programFilesX86;
		_programCommonFiles = rhs._programCommonFiles;
		_programCommonFilesX86 = rhs._programCommonFilesX86;
	}
	return *this;
}

Result SystemDirs::Initialize(_In_opt_ HANDLE hToken, bool createIfNotExits)
{
	_system = GetKnownDirectory(FOLDERID_System, NULL, false);
	_windows = GetKnownDirectory(FOLDERID_Windows, NULL, false);
	_wintemp = _windows + L"\\Temp";
	_users = GetKnownDirectory(FOLDERID_UserProfiles, NULL, false);
	_fonts = GetKnownDirectory(FOLDERID_Fonts, NULL, false);
	_programData = GetKnownDirectory(FOLDERID_ProgramData, NULL, false);
	_programFiles = GetKnownDirectory(FOLDERID_ProgramFiles, NULL, false);
#ifndef _WIN64
	_programFilesX86 = _programFiles;
#else
	_programFilesX86 = GetKnownDirectory(FOLDERID_ProgramFilesX86, NULL, false);
#endif
	_programCommonFiles = GetKnownDirectory(FOLDERID_ProgramFilesCommon, NULL, false);
#ifndef _WIN64
	_programCommonFilesX86 = _programFiles;
#else
	_programCommonFilesX86 = GetKnownDirectory(FOLDERID_ProgramFilesCommonX86, NULL, false);
#endif
	return RESULT(0);
}



UserDirs::UserDirs()
{
}

UserDirs::UserDirs(_In_opt_ HANDLE hToken)
{
	Initialize(hToken, true);
}

UserDirs::UserDirs(const UserDirs& rhs)
	: PublicUserDirs(rhs), _inetCache(rhs._inetCache), _localAppData(rhs._localAppData), _localAppDataLow(rhs._localAppDataLow)
{
}

UserDirs::~UserDirs()
{
}

UserDirs& UserDirs::operator = (const UserDirs& rhs)
{
	if (this != &rhs) {
		PublicUserDirs::operator=(rhs);
	}
	return *this;
}

Result UserDirs::Initialize(_In_opt_ HANDLE hToken, bool createIfNotExits)
{
	_profile = GetKnownDirectory(FOLDERID_Profile, NULL, false);
	_desktop = GetKnownDirectory(FOLDERID_Desktop, NULL, false);
	_documents = GetKnownDirectory(FOLDERID_Documents, NULL, false);
	_downloads = GetKnownDirectory(FOLDERID_Downloads, NULL, false);
	_libraries = GetKnownDirectory(FOLDERID_Libraries, NULL, false);
	_music = GetKnownDirectory(FOLDERID_Music, NULL, false);
	_pictures = GetKnownDirectory(FOLDERID_Pictures, NULL, false);
	_videos = GetKnownDirectory(FOLDERID_PublicVideos, NULL, false);
	_inetCache = GetKnownDirectory(FOLDERID_InternetCache, NULL, false);
	_localAppData = GetKnownDirectory(FOLDERID_LocalAppData, NULL, false);
	_localAppDataLow = GetKnownDirectory(FOLDERID_LocalAppDataLow, NULL, false);
	return RESULT(0);
}



PublicUserDirs::PublicUserDirs()
{
}

PublicUserDirs::PublicUserDirs(const PublicUserDirs& rhs)
	: _profile(rhs._profile), _desktop(rhs._desktop), _documents(rhs._documents),
	_downloads(rhs._downloads), _libraries(rhs._libraries), _music(rhs._music),
	_pictures(rhs._pictures), _videos(rhs._videos)
{
}

PublicUserDirs::~PublicUserDirs()
{
}

PublicUserDirs& PublicUserDirs::operator = (const PublicUserDirs& rhs)
{
	if (this != &rhs) {
		_profile = rhs._profile;
		_desktop = rhs._desktop;
		_documents = rhs._documents;
		_downloads = rhs._downloads;
		_libraries = rhs._libraries;
		_music = rhs._music;
		_pictures = rhs._pictures;
		_videos = rhs._videos;
	}
	return *this;
}

Result PublicUserDirs::Initialize(_In_opt_ HANDLE hToken, bool createIfNotExits)
{
	_profile = GetKnownDirectory(FOLDERID_Public, NULL, false);
	_desktop = GetKnownDirectory(FOLDERID_PublicDesktop, NULL, false);
	_documents = GetKnownDirectory(FOLDERID_PublicDocuments, NULL, false);
	_downloads = GetKnownDirectory(FOLDERID_PublicDownloads, NULL, false);
	_libraries = GetKnownDirectory(FOLDERID_PublicLibraries, NULL, false);
	_music = GetKnownDirectory(FOLDERID_PublicMusic, NULL, false);
	_pictures = GetKnownDirectory(FOLDERID_PublicPictures, NULL, false);
	_videos = GetKnownDirectory(FOLDERID_PublicVideos, NULL, false);
	return RESULT(0);
}
