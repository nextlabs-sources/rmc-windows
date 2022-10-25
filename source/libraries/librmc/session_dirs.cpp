
#include "stdafx.h"

#include <nx\rmc\session.h>
#include <nx\rmc\securefile.h>

#include <nx\common\stringex.h>
#include <nx\common\rapidjson.h>
#include <nx\rest\uri.h>
#include <nx\winutil\path.h>

using namespace NX;


std::wstring RmSession::GetNextLabsDataDir() const
{
	static const std::wstring dir(NX::win::GetSystemDirs().GetProgramDataDir() + L"\\NextLabs");
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(dir.c_str())) {
		if (!::CreateDirectoryW(dir.c_str(), NULL))
			return std::wstring();
	}
	return dir;
}

std::wstring RmSession::GeServerHome(bool createIfNotExist) const
{
	std::wstring dir(_dirs.GetDataRoot());
	if (dir.empty())
		return std::wstring();

	dir += L"\\";
	dir += _router.GetHost();
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(dir.c_str())) {
		if (createIfNotExist) {
			if (!::CreateDirectoryW(dir.c_str(), NULL))
				dir.clear();
		}
		else {
			dir.clear();
		}
	}

	return dir;
}

std::wstring RmSession::GetTenantHome(const std::wstring& tenantName, bool createIfNotExist) const
{
	std::wstring dir(GeServerHome(createIfNotExist));
	if (dir.empty())
		return std::wstring();

	dir += L"\\";
	dir += tenantName;
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(dir.c_str())) {
		if (createIfNotExist) {
			if (!::CreateDirectoryW(dir.c_str(), NULL))
				dir.clear();
		}
		else {
			dir.clear();
		}
	}

	return dir;
}

std::wstring RmSession::GetUserHome(const std::wstring& tenantName, const std::wstring& userId, bool createIfNotExist) const
{
	std::wstring dir(GetTenantHome(tenantName, createIfNotExist));
	if (dir.empty())
		return std::wstring();

	dir += L"\\";
	dir += userId;
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(dir.c_str())) {
		if (createIfNotExist) {
			if (!::CreateDirectoryW(dir.c_str(), NULL))
				dir.clear();
		}
		else {
			dir.clear();
		}
	}

	return dir;
}

std::wstring RmSession::GetUserCacheDir(const std::wstring& tenantName, const std::wstring& userId, bool createIfNotExist) const
{
	std::wstring dir(GetUserHome(tenantName, userId, createIfNotExist));
	if (dir.empty())
		return std::wstring();
	dir += L"\\Cache";
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(dir.c_str())) {
		if (createIfNotExist) {
			if (!::CreateDirectoryW(dir.c_str(), NULL))
				dir.clear();
		}
		else {
			dir.clear();
		}
	}
	return dir;
}

std::wstring RmSession::GetUserTempDir(const std::wstring& tenantName, const std::wstring& userId, bool createIfNotExist) const
{
	std::wstring dir(GetUserHome(tenantName, userId, createIfNotExist));
	if (dir.empty())
		return std::wstring();
	dir += L"\\Temp";
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(dir.c_str())) {
		if (createIfNotExist) {
			if (!::CreateDirectoryW(dir.c_str(), NULL))
				dir.clear();
		}
		else {
			dir.clear();
		}
	}
	return dir;
}

std::wstring RmSession::GetUserMembershipsDir(const std::wstring& tenantName, const std::wstring& userId, bool createIfNotExist) const
{
	std::wstring dir(GetUserHome(tenantName, userId, createIfNotExist));
	if (dir.empty())
		return std::wstring();
	dir += L"\\Memberships";
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(dir.c_str())) {
		if (createIfNotExist) {
			if (!::CreateDirectoryW(dir.c_str(), NULL))
				dir.clear();
		}
		else {
			dir.clear();
		}
	}
	return dir;
}

std::wstring RmSession::GetUserOfflineDir(const std::wstring& tenantName, const std::wstring& userId, bool createIfNotExist) const
{
	std::wstring dir(GetUserHome(tenantName, userId, createIfNotExist));
	if (dir.empty())
		return std::wstring();
	dir += L"\\Offline";
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(dir.c_str())) {
		if (createIfNotExist) {
			if (!::CreateDirectoryW(dir.c_str(), NULL))
				dir.clear();
		}
		else {
			dir.clear();
		}
	}
	return dir;
}

std::wstring RmSession::GetUserReposDir(const std::wstring& tenantName, const std::wstring& userId, bool createIfNotExist) const
{
	std::wstring dir(GetUserHome(tenantName, userId, createIfNotExist));
	if (dir.empty())
		return std::wstring();
	dir += L"\\Repos";
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(dir.c_str())) {
		if (createIfNotExist) {
			if (!::CreateDirectoryW(dir.c_str(), NULL))
				dir.clear();
		}
		else {
			dir.clear();
		}
	}
	return dir;
}

std::wstring RmSession::GetUserRepoDir(const std::wstring& tenantName, const std::wstring& userId, const std::wstring& repoId, bool createIfNotExist) const
{
	std::wstring dir(GetUserReposDir(tenantName, userId, createIfNotExist));
	if (dir.empty())
		return std::wstring();
	dir += L"\\";
	dir += repoId;
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(dir.c_str())) {
		if (createIfNotExist) {
			if (!::CreateDirectoryW(dir.c_str(), NULL))
				dir.clear();
		}
		else {
			dir.clear();
		}
	}
	return dir;
}

std::wstring RmSession::GetCurrentUserHome() const
{
	if (GetCurrentTenantName().empty() || GetCurrentUser().Empty())
		return std::wstring();
	return GetUserHome(GetCurrentTenantName(), GetCurrentUser().GetId(), true);
}

std::wstring RmSession::GetCurrentUserCacheDir() const
{
	if (GetCurrentTenantName().empty() || GetCurrentUser().Empty())
		return std::wstring();
	return GetUserCacheDir(GetCurrentTenantName(), GetCurrentUser().GetId(), true);
}

std::wstring RmSession::GetCurrentUserTempDir() const
{
	if (GetCurrentTenantName().empty() || GetCurrentUser().Empty())
		return std::wstring();
	return GetUserTempDir(GetCurrentTenantName(), GetCurrentUser().GetId(), true);
}

std::wstring RmSession::GetCurrentUserMembershipsDir() const
{
	if (GetCurrentTenantName().empty() || GetCurrentUser().Empty())
		return std::wstring();
	return GetUserMembershipsDir(GetCurrentTenantName(), GetCurrentUser().GetId(), true);
}

std::wstring RmSession::GetCurrentUserOfflineDir() const
{
	if (GetCurrentTenantName().empty() || GetCurrentUser().Empty())
		return std::wstring();
	return GetUserOfflineDir(GetCurrentTenantName(), GetCurrentUser().GetId(), true);
}

std::wstring RmSession::GetCurrentUserReposDir() const
{
	if (GetCurrentTenantName().empty() || GetCurrentUser().Empty())
		return std::wstring();
	return GetUserReposDir(GetCurrentTenantName(), GetCurrentUser().GetId(), true);
}

std::wstring RmSession::GetCurrentUserRepoDir(const std::wstring& repoId) const
{
	if (GetCurrentTenantName().empty() || GetCurrentUser().Empty())
		return std::wstring();
	return GetUserRepoDir(GetCurrentTenantName(), GetCurrentUser().GetId(), repoId, true);
}