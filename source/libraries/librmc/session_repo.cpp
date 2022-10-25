
#include "stdafx.h"

#include <nx\rmc\session.h>
#include <nx\rmc\securefile.h>

#include <nx\common\stringex.h>
#include <nx\common\rapidjson.h>
#include <nx\common\time.h>
#include <nx\nxl\nxlrights.h>
#include <nx\rest\uri.h>

using namespace NX;



#include <nx\rmc\rest.h>
#include <nx\rmc\session.h>
#include <nx\rmc\securefile.h>

#include <nx\common\stringex.h>
#include <nx\common\rapidjson.h>
#include <nx\crypt\md5.h>
#include <nx\crypt\sha.h>
#include <nx\rest\uri.h>

using namespace NX;
using namespace NX::rapidjson;



Result RmSession::RepoGetReposListFromRemote(std::vector<RmRepository>& repos)
{
	if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
		return RESULT(ERROR_EXPIRED_HANDLE);
	}

	RmRestStatus status;
	bool isFullCopy;
	Result res = _restClient.GetRepositories(_rmserver.GetUrl(), status, repos, isFullCopy);
	if (!res)
		return res;

	RepoSaveReposListToCache(repos);
	return RESULT(0);
}

Result RmSession::RepoGetReposListFromCache(std::vector<RmRepository>& repos)
{
	const std::wstring repoListCacheFile(GetCurrentUserHome() + L"\\repos.list");
	std::string s;
	Result res = SecureFileRead(repoListCacheFile, s);
	if (!res)
		return res;
	if (s.empty())
		return RESULT(ERROR_NOT_FOUND);

	do {

		JsonDocument doc;
		if (!doc.LoadJsonString<char>(s)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		PCJSONVALUE root = doc.GetRoot();
		if(!IsValidJsonObject(root)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		PCJSONVALUE repoArray = root->AsObject()->at(L"repos");
		if(!IsValidJsonArray(repoArray)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		std::for_each(repoArray->AsArray()->cbegin(), repoArray->AsArray()->cend(), [&](const JsonArray::value_type& item) {

			RmRepository repo;
			PCJSONVALUE pv = item->AsObject()->at(L"type");
			if (!IsValidJsonString(pv))
				return;
			repo._type = RmRepository::StringToType(pv->AsString()->GetString());
			if(repo._type == RmRepository::UNKNOWNREPO)
				return;

			pv = item->AsObject()->at(L"id");
			if (!IsValidJsonString(pv))
				return;
			repo._id = pv->AsString()->GetString();

			pv = item->AsObject()->at(L"name");
			if (!IsValidJsonString(pv))
				return;
			repo._name = pv->AsString()->GetString();

			pv = item->AsObject()->at(L"accountName");
			if (!IsValidJsonString(pv)) {
				repo._accountName = pv->AsString()->GetString();
			}

			pv = item->AsObject()->at(L"accountId");
			if (!IsValidJsonString(pv)) {
				repo._accountId = pv->AsString()->GetString();
			}

			pv = item->AsObject()->at(L"token");
			if (!IsValidJsonString(pv)) {
				repo._token = pv->AsString()->GetString();
			}

			pv = item->AsObject()->at(L"creationTime");
			if (!IsValidJsonNumber(pv)) {
				repo._creationTime = pv->AsNumber()->ToInt64();
			}

			pv = item->AsObject()->at(L"isShared");
			if (!IsValidJsonBool(pv)) {
				repo._shared = pv->AsBool()->GetBool();
			}

			repos.push_back(repo);
		});

	} while (FALSE);

	if (!res) {
		::DeleteFileW(repoListCacheFile.c_str());
		return res;
	}

	return RESULT(0);
}

Result RmSession::RepoSaveReposListToCache(const std::vector<RmRepository>& repos)
{
	const std::wstring repoListCacheFile(GetCurrentUserHome() + L"\\repos.list");

	std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
	if(root == nullptr)
		return RESULT(ERROR_NOT_ENOUGH_MEMORY);

	JsonArray* repoArray = JsonValue::CreateArray();
	if(root == nullptr)
		return RESULT(ERROR_NOT_ENOUGH_MEMORY);
	root->set(L"repos", repoArray);

	std::for_each(repos.begin(), repos.end(), [&](const RmRepository& repo) {
		if (repo.Empty())
			return;
		JsonObject* repoObj = JsonValue::CreateObject();
		if (NULL == repoObj)
			return;
		repoArray->push_back(repoObj);
		repoObj->set(L"type", JsonValue::CreateString(repo.GetTypeName()));
		repoObj->set(L"id", JsonValue::CreateString(repo.GetId()));
		repoObj->set(L"name", JsonValue::CreateString(repo.GetName()));
		repoObj->set(L"accountName", JsonValue::CreateString(repo.GetAccountName()));
		repoObj->set(L"accountId", JsonValue::CreateString(repo.GetAccountId()));
		repoObj->set(L"token", JsonValue::CreateString(repo.GetToken()));
		repoObj->set(L"creationTime", JsonValue::CreateNumber(repo.GetCreationTime()));
		repoObj->set(L"isShared", JsonValue::CreateBool(repo.IsShared()));
	});

	rapidjson::JsonStringWriter<char> writer;
	const std::string& s = writer.Write(root.get());
	root.reset();
	if (s.empty()) {
		return RESULT(ERROR_INVALID_DATA);
	}
	
	Result res = SecureFileWrite(repoListCacheFile, s);
	if (!res)
		return res;

	return RESULT(0);
}

Result RmSession::RepoGetFavoriteFilesFromRemote(std::vector<std::pair<std::wstring, std::vector<RmRepoFile>>>& favoriteFiles)
{
	RmRestStatus status;
	std::vector<RmRepository> repos;
	std::vector<std::pair<std::wstring, std::vector<RmRepoMarkedFile>>> repoFiles;

	Result res = RepoGetReposListFromRemote(repos);
	if (!res)
		return res;

	res = _restClient.RepoGetAllFavoriteFiles(_rmserver.GetUrl(), status, repoFiles);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	
	std::for_each(repoFiles.begin(), repoFiles.end(), [&](const std::pair<std::wstring, std::vector<RmRepoMarkedFile>>& item) {
		
		const std::wstring repoId = item.first;
		const std::vector<RmRepoMarkedFile>& markedFiles = item.second;

		if (!markedFiles.empty()) {

			auto repoPos = std::find_if(repos.begin(), repos.end(), [&](const RmRepository& repo) -> bool {
				return (0 == _wcsicmp(repo.GetId().c_str(), repoId.c_str()));
			});
			if (repoPos == repos.end()) {
				// repo doesn't exist
				return;
			}

			std::vector<RmRepoFile> files;
			std::for_each(markedFiles.begin(), markedFiles.end(), [&](const RmRepoMarkedFile& markedFile) {
				RmRepoFile file;
				res = RepoGetFileInfo(*repoPos, markedFile.GetFileId(), file);
				if (res) {
					files.push_back(file);
				}
			});
			if (!files.empty())
				favoriteFiles.push_back(std::pair<std::wstring, std::vector<RmRepoFile>>(repoId, files));
		}

	});

	RepoSaveFavoriteFilesToCache(favoriteFiles);
	return RESULT(0);
}

Result RmSession::RepoGetFavoriteFilesFromCache(std::vector<std::pair<std::wstring, std::vector<RmRepoFile>>>& favoriteFiles)
{
	const std::wstring cacheFileName(GetCurrentUserHome() + L"\\favorites.list");
	std::string s;
	Result res = SecureFileRead(cacheFileName, s);
	if (!res)
		return res;
	if (s.empty())
		return RESULT(ERROR_NOT_FOUND);

	do {

		JsonDocument doc;
		if (!doc.LoadJsonString<char>(s)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		PCJSONVALUE root = doc.GetRoot();
		if (!IsValidJsonObject(root)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		PCJSONVALUE pCategory = root->AsObject()->at(L"favorite");
		if (IsValidJsonObject(pCategory)) {

			// There is list of favorite files
			std::for_each(pCategory->AsObject()->cbegin(), pCategory->AsObject()->cend(), [&](const JsonObject::value_type& repo) {

				const std::wstring& strRepoId = repo.first;
				if (strRepoId.empty() || !IsValidJsonArray(repo.second))
					return;
				std::vector<RmRepoFile> repoFiles;
				std::for_each(repo.second->AsArray()->cbegin(), repo.second->AsArray()->cend(), [&](const JsonArray::value_type& item) {

					if (!IsValidJsonObject(item))
						return;

					RmRepoFile file;
					PCJSONVALUE pv = item->AsObject()->at(L"id");
					if (!IsValidJsonString(pv))
						return;
					file._id = pv->AsString()->GetString();

					pv = item->AsObject()->at(L"path");
					if (!IsValidJsonString(pv))
						return;
					file._path = pv->AsString()->GetString();

					pv = item->AsObject()->at(L"name");
					if (!IsValidJsonString(pv))
						return;
					file._name = pv->AsString()->GetString();

					pv = item->AsObject()->at(L"folder");
					if (!IsValidJsonBool(pv))
						return;
					file._folder = pv->AsBool()->GetBool();

					pv = item->AsObject()->at(L"size");
					if (!IsValidJsonNumber(pv))
						return;
					file._size = pv->AsNumber()->ToInt64();

					pv = item->AsObject()->at(L"time");
					if (!IsValidJsonNumber(pv))
						return;
					file._time = pv->AsNumber()->ToInt64();

					repoFiles.push_back(file);
				});
				if (!repoFiles.empty()) {
					favoriteFiles.push_back(std::pair<std::wstring, std::vector<RmRepoFile>>(strRepoId, repoFiles));
				}
			});
		}

	} while (FALSE);
	if (!res) {
		::DeleteFileW(cacheFileName.c_str());
		return res;
	}

	return RESULT(0);
}

Result RmSession::RepoSaveFavoriteFilesToCache(const std::vector<std::pair<std::wstring, std::vector<RmRepoFile>>>& favoriteFiles)
{
	const std::wstring cacheFileName(GetCurrentUserHome() + L"\\favorites.list");

	std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
	if (root == nullptr)
		return RESULT(ERROR_NOT_ENOUGH_MEMORY);

	JsonObject* pCategory = JsonValue::CreateObject();
	if (pCategory == nullptr)
		return RESULT(ERROR_NOT_ENOUGH_MEMORY);
	root->set(L"favorite", pCategory);
	std::for_each(favoriteFiles.begin(), favoriteFiles.end(), [&](const std::pair<std::wstring, std::vector<RmRepoFile>>& repo) {

		if (repo.first.empty() || repo.second.empty())
			return;
		JsonArray* pRepo = JsonValue::CreateArray();
		if (pRepo == nullptr)
			return;
		pCategory->set(repo.first, pRepo);
		std::for_each(repo.second.begin(), repo.second.end(), [&](const RmRepoFile& file) {

			if (file.Empty())
				return;

			JsonObject* pFile = JsonValue::CreateObject();
			if (pFile == nullptr)
				return;
			pRepo->push_back(pFile);

			PJSONVALUE pv = JsonValue::CreateString(file.GetName());
			if (pv != NULL)
				pFile->set(L"name", pv);
			pv = JsonValue::CreateString(file.GetId());
			if (pv != NULL)
				pFile->set(L"id", pv);
			pv = JsonValue::CreateString(file.GetPath());
			if (pv != NULL)
				pFile->set(L"path", pv);
			pv = JsonValue::CreateBool(file.IsFolder());
			if (pv != NULL)
				pFile->set(L"folder", pv);
			pv = JsonValue::CreateNumber(file.GetSize());
			if (pv != NULL)
				pFile->set(L"size", pv);
			pv = JsonValue::CreateNumber(file.GetLastModifiedTime());
			if (pv != NULL)
				pFile->set(L"time", pv);

		});
	});

	rapidjson::JsonStringWriter<char> writer;
	const std::string& s = writer.Write(root.get());
	root.reset();
	if (s.empty()) {
		return RESULT(ERROR_INVALID_DATA);
	}

	Result res = SecureFileWrite(cacheFileName, s);
	if (!res)
		return res;

	return RESULT(0);
}

Result RmSession::RepoSetFavoriteFile(const RmRepository& repo, const std::wstring& folderId, const RmRepoFile& file)
{
	std::vector<RmRepoFile> files;
	files.push_back(file);
	return RepoSetFavoriteFiles(repo, folderId, files);
}

Result RmSession::RepoSetFavoriteFiles(const RmRepository& repo, const std::wstring& folderId, const std::vector<RmRepoFile>& files)
{
	if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
		return RESULT(ERROR_EXPIRED_HANDLE);
	}

	RmRestStatus status;
	std::vector<RmRepoMarkedFile> markedFiles;
	std::for_each(files.begin(), files.end(), [&](const RmRepoFile& file) {
		markedFiles.push_back(RmRepoMarkedFile(file.GetId(), file.GetPath(), std::wstring()));
	});
	Result res = _restClient.RepoMarkFavoriteFiles(_rmserver.GetUrl(), repo.GetId(), folderId.empty() ? L"/" : folderId, markedFiles, status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::RepoUnsetFavoriteFile(const RmRepository& repo, const RmRepoFile& file)
{
	std::vector<RmRepoFile> files;
	files.push_back(file);
	return RepoUnsetFavoriteFiles(repo, files);
}

Result RmSession::RepoUnsetFavoriteFiles(const RmRepository& repo, const std::vector<RmRepoFile>& files)
{
	if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
		return RESULT(ERROR_EXPIRED_HANDLE);
	}

	RmRestStatus status;
	std::vector<RmRepoMarkedFile> markedFiles;
	std::for_each(files.begin(), files.end(), [&](const RmRepoFile& file) {
		markedFiles.push_back(RmRepoMarkedFile(file.GetId(), file.GetPath(), std::wstring()));
	});
	Result res = _restClient.RepoUnmarkFavoriteFiles(_rmserver.GetUrl(), repo.GetId(), markedFiles, status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);

}

Result RmSession::RepoGetOfflineFiles(std::vector<std::pair<std::wstring, std::vector<RmRepoFile>>>& offlineFiles)
{
	const std::wstring cacheFileName(GetCurrentUserHome() + L"\\offlines.list");
	std::string s;
	Result res = SecureFileRead(cacheFileName, s);
	if (!res) {
		if (res.GetCode() == ERROR_NOT_FOUND || res.GetCode() == ERROR_FILE_NOT_FOUND || res.GetCode() == ERROR_PATH_NOT_FOUND) {
			// File not exist, treat it as SUCCEED -- it means there is no offline files
			res = RESULT(0);
		}
		return res;
	}
	if (s.empty())
		return RESULT(0);

	do {

		JsonDocument doc;
		if (!doc.LoadJsonString<char>(s)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		PCJSONVALUE root = doc.GetRoot();
		if (!IsValidJsonObject(root)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		PCJSONVALUE pCategory = root->AsObject()->at(L"offline");
		if (IsValidJsonObject(pCategory)) {

			// There is list of favorite files
			std::for_each(pCategory->AsObject()->cbegin(), pCategory->AsObject()->cend(), [&](const JsonObject::value_type& repo) {

				const std::wstring& strRepoId = repo.first;
				if (strRepoId.empty() || !IsValidJsonArray(repo.second))
					return;
				std::vector<RmRepoFile> repoFiles;
				std::for_each(repo.second->AsArray()->cbegin(), repo.second->AsArray()->cend(), [&](const JsonArray::value_type& item) {

					if (!IsValidJsonObject(item))
						return;

					RmRepoFile file;
					PCJSONVALUE pv = item->AsObject()->at(L"id");
					if (!IsValidJsonString(pv))
						return;
					file._id = pv->AsString()->GetString();

					pv = item->AsObject()->at(L"path");
					if (!IsValidJsonString(pv))
						return;
					file._path = pv->AsString()->GetString();

					pv = item->AsObject()->at(L"name");
					if (!IsValidJsonString(pv))
						return;
					file._name = pv->AsString()->GetString();

					pv = item->AsObject()->at(L"folder");
					if (!IsValidJsonBool(pv))
						return;
					file._folder = pv->AsBool()->GetBool();

					pv = item->AsObject()->at(L"size");
					if (!IsValidJsonNumber(pv))
						return;
					file._size = pv->AsNumber()->ToInt64();

					pv = item->AsObject()->at(L"time");
					if (!IsValidJsonNumber(pv))
						return;
					file._time = pv->AsNumber()->ToInt64();

					repoFiles.push_back(file);
				});
				if (!repoFiles.empty()) {
					offlineFiles.push_back(std::pair<std::wstring, std::vector<RmRepoFile>>(strRepoId, repoFiles));
				}
			});
		}

	} while (FALSE);
	if (!res) {
		::DeleteFileW(cacheFileName.c_str());
		return res;
	}

	return RESULT(0);
}

Result RmSession::RepoSaveOfflineFiles(const std::vector<std::pair<std::wstring, std::vector<RmRepoFile>>>& offlineFiles)
{
	const std::wstring cacheFileName(GetCurrentUserHome() + L"\\offlines.list");

	std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
	if (root == nullptr)
		return RESULT(ERROR_NOT_ENOUGH_MEMORY);

	JsonObject* pCategory = JsonValue::CreateObject();
	if (pCategory == nullptr)
		return RESULT(ERROR_NOT_ENOUGH_MEMORY);
	root->set(L"offline", pCategory);
	std::for_each(offlineFiles.begin(), offlineFiles.end(), [&](const std::pair<std::wstring, std::vector<RmRepoFile>>& repo) {

		if (repo.first.empty() || repo.second.empty())
			return;
		JsonArray* pRepo = JsonValue::CreateArray();
		if (pRepo == nullptr)
			return;
		pCategory->set(repo.first, pRepo);
		std::for_each(repo.second.begin(), repo.second.end(), [&](const RmRepoFile& file) {

			if (file.Empty())
				return;

			JsonObject* pFile = JsonValue::CreateObject();
			if (pFile == nullptr)
				return;
			pRepo->push_back(pFile);

			PJSONVALUE pv = JsonValue::CreateString(file.GetName());
			if (pv != NULL)
				pFile->set(L"name", pv);
			pv = JsonValue::CreateString(file.GetId());
			if (pv != NULL)
				pFile->set(L"id", pv);
			pv = JsonValue::CreateString(file.GetPath());
			if (pv != NULL)
				pFile->set(L"path", pv);
			pv = JsonValue::CreateBool(file.IsFolder());
			if (pv != NULL)
				pFile->set(L"folder", pv);
			pv = JsonValue::CreateNumber(file.GetSize());
			if (pv != NULL)
				pFile->set(L"size", pv);
			pv = JsonValue::CreateNumber(file.GetLastModifiedTime());
			if (pv != NULL)
				pFile->set(L"time", pv);

		});
	});

	rapidjson::JsonStringWriter<char> writer;
	const std::string& s = writer.Write(root.get());
	root.reset();
	if (s.empty()) {
		return RESULT(ERROR_INVALID_DATA);
	}

	Result res = SecureFileWrite(cacheFileName, s);
	if (!res)
		return res;

	return RESULT(0);
}

Result RmSession::RepoGetAccessToken(RmRepository& repo)
{
    std::wstring token;
    RmRestStatus status;
    Result res = _restClient.GetAccessToken(_rmserver.GetUrl(), repo.GetId(), status, token);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    repo.SetAccessToken(token);
    return RESULT(0);
}

std::wstring RmSession::RepoGetTempFolderName(const std::wstring& repoId, const std::wstring& filePath)
{
	UCHAR checksum[16] = { 0 };
	std::wstring s(repoId + filePath);
	std::transform(s.begin(), s.end(), s.begin(), ::tolower);
	Result res = NX::crypt::CreateHmacMd5((const unsigned char*)s.c_str(), (ULONG)s.length(), GetClientId().GetClientKey().data(), (ULONG)GetClientId().GetClientKey().size(), checksum);
	if (!res)
		return std::wstring();

	return NX::bintohs<wchar_t>(checksum, 16);
}

std::wstring RmSession::RepoGetOfflineFilePath(const std::wstring& repoId, const std::wstring& filePath)
{
	const std::wstring& folderName = RepoGetTempFolderName(repoId, filePath);
	std::wstring s(GetCurrentUserOfflineDir() + L"\\" + folderName);
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(s.c_str())) {
		if (!::CreateDirectoryW(s.c_str(), NULL))
			return std::wstring();
	}
	auto pos = filePath.rfind(L'/');
	if(pos == std::wstring::npos)
		pos = filePath.rfind(L'\\');
	return std::wstring(GetCurrentUserOfflineDir() + L"\\" + folderName + L"\\" + ((pos == std::wstring::npos) ? filePath : filePath.substr(pos+1)));
}

std::wstring RmSession::RepoGetTempFilePath(const std::wstring& repoId, const std::wstring& filePath)
{
	const std::wstring& folderName = RepoGetTempFolderName(repoId, filePath);
	std::wstring s(GetCurrentUserOfflineDir() + L"\\" + folderName);
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(s.c_str())) {
		if (!::CreateDirectoryW(s.c_str(), NULL))
			return std::wstring();
	}
	auto pos = filePath.rfind(L'/');
	if(pos == std::wstring::npos)
		pos = filePath.rfind(L'\\');
	return std::wstring(GetCurrentUserTempDir() + L"\\" + folderName + L"\\" + ((pos == std::wstring::npos) ? filePath : filePath.substr(pos+1)));
}

Result RmSession::RepoAdd(const std::wstring& name,
	RmRepository::REPO_TYPE type,
	bool isShared,
	const std::wstring& accountName,
	const std::wstring& accountId,
	const std::wstring& accountToken,
	std::wstring& repoId)
{
	if (RmRepository::MYDRIVE == type || RmRepository::MYVAULT == type)
		return RESULT(ERROR_INVALID_PARAMETER);

	if(0 == _wcsicmp(name.c_str(), L"MyDrive") || 0 == _wcsicmp(name.c_str(), L"MyVault"))
		return RESULT(ERROR_INVALID_PARAMETER);

	RmRestStatus status;
	Result res = _restClient.AddRepository(_rmserver.GetUrl(),
		name,
		RmRepository::TypeToString(type),
		isShared,
		accountName,
		accountId,
		accountToken,
		status,
		repoId);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::RepoGetAuthUrl(const std::wstring& name, RmRepository::REPO_TYPE type, const std::wstring& siteUrl, std::wstring& url)
{
	if (RmRepository::MYDRIVE == type || RmRepository::MYVAULT == type)
		return RESULT(ERROR_INVALID_PARAMETER);

	if(0 == _wcsicmp(name.c_str(), L"MyDrive") || 0 == _wcsicmp(name.c_str(), L"MyVault"))
		return RESULT(ERROR_INVALID_PARAMETER);

	RmRestStatus status;
	std::wstring urlTail;
	Result res = _restClient.GetRepoAuthUrl(_rmserver.GetUrl(), name, RmRepository::TypeToString(type), siteUrl, status, urlTail);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	url = _rmserver.GetUrl() + urlTail;
	return RESULT(0);
}

Result RmSession::RepoRename(const RmRepository& repo, const std::wstring& name)
{
	if (RmRepository::MYDRIVE == repo.GetType() || RmRepository::MYVAULT == repo.GetType())
		return RESULT(ERROR_INVALID_PARAMETER);

	if(0 == _wcsicmp(name.c_str(), L"MyDrive") || 0 == _wcsicmp(name.c_str(), L"MyVault"))
		return RESULT(ERROR_INVALID_PARAMETER);

	if (name == repo.GetName())
		return RESULT(0);

	RmRestStatus status;
	Result res = _restClient.UpdateRepository(_rmserver.GetUrl(),
		repo.GetId(),
		name,
		repo.GetToken(),
		status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::RepoRemove(const RmRepository& repo)
{
	if (RmRepository::MYDRIVE == repo.GetType() || RmRepository::MYVAULT == repo.GetType())
		return RESULT(ERROR_ACCESS_DENIED);

	RmRestStatus status;
	Result res = _restClient.RemoveRepository(_rmserver.GetUrl(), repo.GetId(), status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::RepoGetQuota(const RmRepository& repo, __int64* pTotal, __int64* pUsed, __int64* pAvailable)
{
	Result res = RESULT(0);
	NX::RmMyDriveUsage usage;

	switch (repo.GetType())
	{
	case RmRepository::MYDRIVE:
		res = MyDriveGetUsage(usage);
		if (res) {
			if (pTotal) *pTotal = usage.GetQuota();
			if (pUsed) *pUsed = usage.GetUsedBytes();
			if (pAvailable) *pAvailable = usage.GetFreeBytes();
		}
		break;
	case RmRepository::GOOGLEDRIVE:
	case RmRepository::ONEDRIVE:
	case RmRepository::DROPBOX:
	case RmRepository::SHAREPOINTONLINE:
	case RmRepository::SHAREPOINTONPREM:
	case RmRepository::SHAREPOINTCROSSLAUNCH:
	case RmRepository::SHAREPOINTONLINE_CROSSLAUNCH:
	default:
		res = RESULT(ERROR_NOT_SUPPORTED);
		break;
	}
	
	return res;
}

Result RmSession::RepoCreateFileIdHash(const std::wstring& fileId, std::wstring& hash)
{
	UCHAR sha1[20] = { 0 };
	Result res = NX::crypt::CreateSha1((const unsigned char*)fileId.c_str(), (ULONG)(fileId.length() * 2), sha1);
	if (!res)
		return res;

	hash = NX::bintohs<wchar_t>(sha1, 20);
	return RESULT(0);
}

Result RmSession::RepoFindOfflineFile(const std::wstring& repoId, const std::wstring& fileId, _Out_ std::wstring& offlinePath, _Out_opt_ __int64* timestamp)
{
	const std::wstring& repoDir = GetCurrentUserRepoDir(repoId);
	std::wstring fileIdHash;

	Result res = RepoCreateFileIdHash(fileId, fileIdHash);
	if (!res)
		return res;

	const std::wstring offlinePathFilter(repoDir + L"\\offline-" + fileIdHash + L"-*");

	WIN32_FIND_DATAW fwd = { 0 };
	HANDLE hFind = FindFirstFileW(offlinePathFilter.c_str(), &fwd);
	if (INVALID_HANDLE_VALUE == hFind)
		return RESULT(GetLastError());

	FindClose(hFind);

	if (timestamp) {
		const wchar_t* pos = wcsrchr(fwd.cFileName, L'-');
		if (NULL == pos) {
			return RESULT(ERROR_NOT_FOUND);
		}
		*timestamp = NX::hstoi64<wchar_t>(pos + 1);
	}
	offlinePath = std::wstring(repoDir + L"\\" + fwd.cFileName);
	return RESULT(0);
}

Result RmSession::RepoCreateOfflineFilePath(const std::wstring& repoId, const std::wstring& fileId, _In_ __int64 timestamp, std::wstring& offlinePath)
{
	const std::wstring& repoDir = GetCurrentUserRepoDir(repoId);
	std::wstring fileIdHash;
		
	Result res = RepoCreateFileIdHash(fileId, fileIdHash);
	if (!res)
		return res;

	offlinePath = std::wstring(repoDir + L"\\offline-" + fileIdHash + L"-" + NX::i64tos<wchar_t>(timestamp));
	return RESULT(0);
}

Result RmSession::RepoDownloadOfflineFile(RmRepository& repo, const std::wstring& fileId, bool overwriteExisting, _Out_ std::wstring& offlinePath)
{
	std::wstring localFile;
	__int64 timestamp = 0;
	NX::RmRepoFile file;

	Result res = RepoGetFileInfo(repo, fileId, file);
	if (!res)
		return res;

	res = RepoFindOfflineFile(repo.GetId(), fileId, localFile, &timestamp);
	if (res) {
		timestamp = 0;
		localFile.clear();
	}

	if (!overwriteExisting && timestamp == file.GetLastModifiedTime()) {
		// Cache already exist
		offlinePath = localFile;
		return RESULT(0);
	}
	
	// Cache is old, delete old one and re-download
	if(!localFile.empty())
		DeleteFileW(localFile.c_str());
	localFile.clear();
	timestamp = 0;
	res = RepoCreateOfflineFilePath(repo.GetId(), fileId, file.GetLastModifiedTime(), localFile);
	if(!res)
		return res;

	res = RepoDownloadFile(repo, fileId, localFile, NULL, 0, false);
	if (!res) {
		DeleteFileW(localFile.c_str());
		return res;
	}

	offlinePath = localFile;
	return RESULT(0);
}

Result RmSession::RepoGetFolderFileListFromRemote(RmRepository& repo, const std::wstring& folderId, std::vector<RmRepoFile>& files)
{
	Result res = RepoListFiles(repo, folderId, NULL, files);
	if (!res)
		return res;

	RepoSaveFolderFileListFromCache(repo, folderId, files);
	return RESULT(0);
}

Result RmSession::RepoGetFolderFileListFromCache(const RmRepository& repo, const std::wstring& folderId, std::vector<RmRepoFile>& files, __int64* timestamp)
{
	std::wstring cacheFile;
	Result res = RepoBuildFolderCacheFilePath(repo, folderId, cacheFile);
	if (!res)
		return res;

	return RepoGetFolderFileListFromCache(cacheFile, files, timestamp);
}

Result RmSession::RepoGetFolderFileListFromCache(const std::wstring& cacheFile, std::vector<RmRepoFile>& files, _Out_opt_ __int64* timestamp)
{
	std::string s;
	Result res = SecureFileRead(cacheFile, s);
	if (!res) {
		if (ERROR_NOT_FOUND != res.GetCode() && ERROR_FILE_NOT_FOUND != res.GetCode())
			::DeleteFileW(cacheFile.c_str());
		return res;
	}

	JsonDocument doc;
	if (!doc.LoadJsonString<char>(s)) {
		::DeleteFileW(cacheFile.c_str());
		return RESULT(ERROR_INVALID_DATA);
	}

	PCJSONVALUE root = doc.GetRoot();
	if (!IsValidJsonObject(root)) {
		::DeleteFileW(cacheFile.c_str());
		return RESULT(ERROR_INVALID_DATA);
	}

	PCJSONVALUE pv = root->AsObject()->at(L"timeStamp");
	if (!IsValidJsonNumber(pv)) {
		::DeleteFileW(cacheFile.c_str());
		return RESULT(ERROR_INVALID_DATA);
	}
	if (timestamp)
		*timestamp = pv->AsNumber()->ToInt64();

	PCJSONVALUE pfa = root->AsObject()->at(L"files");
	if (!IsValidJsonArray(pfa)) {
		::DeleteFileW(cacheFile.c_str());
		return RESULT(ERROR_INVALID_DATA);
	}

	std::for_each(pfa->AsArray()->cbegin(), pfa->AsArray()->cend(), [&](const JsonArray::value_type& pfo) {

		RmRepoFile file;

		if (!IsValidJsonObject(pfo))
			return;

		pv = pfo->AsObject()->at(L"name");
		if (!IsValidJsonString(pv))
			return;
		file._name = pv->AsString()->GetString();

		pv = pfo->AsObject()->at(L"id");
		if (!IsValidJsonString(pv))
			return;
		file._id = pv->AsString()->GetString();

		pv = pfo->AsObject()->at(L"path");
		if (!IsValidJsonString(pv))
			return;
		file._path = pv->AsString()->GetString();

		pv = pfo->AsObject()->at(L"size");
		if (IsValidJsonNumber(pv)) {
			file._size = pv->AsNumber()->ToInt64();
		}

		pv = pfo->AsObject()->at(L"time");
		if (IsValidJsonNumber(pv)) {
			file._time = pv->AsNumber()->ToInt64();
		}

		pv = pfo->AsObject()->at(L"folder");
		if (IsValidJsonBool(pv)) {
			file._folder = pv->AsBool()->GetBool();
		}

		PCJSONVALUE pfps = pfo->AsObject()->at(L"properties");
		if (IsValidJsonArray(pfps)) {
			std::for_each(pfps->AsArray()->cbegin(), pfps->AsArray()->cend(), [&](const JsonArray::value_type& prop) {
				if (IsValidJsonString(prop)) {
					const std::wstring& sProp = prop->AsString()->GetString();
					auto pos = sProp.find(L'=');
					if (pos != std::wstring::npos) {
						const std::wstring& propKey = sProp.substr(0, pos);
						const std::wstring& propValue = sProp.substr(pos + 1);
						if (!propKey.empty() && !propValue.empty())
							file._props.push_back(std::pair<std::wstring, std::wstring>(propKey, propValue));
					}
				}
			});
		}

		files.push_back(file);
	});

	return RESULT(0);
}

Result RmSession::RepoBuildFolderCacheFilePath(const RmRepository& repo, const std::wstring& folderId, std::wstring& cacheFile)
{
	const std::wstring& repoDir = GetCurrentUserRepoDir(repo.GetId());
	if (repoDir.empty())
		return RESULT(ERROR_PATH_NOT_FOUND);

	std::wstring fileIdHash;
	Result res = RepoCreateFileIdHash(folderId, fileIdHash);
	if (!res)
		return res;

	cacheFile = std::wstring(repoDir + L"\\folder-" + fileIdHash + L".files");
	return RESULT(0);
}

Result RmSession::RepoSaveFolderFileListFromCache(const RmRepository& repo, const std::wstring& folderId, const std::vector<RmRepoFile>& files)
{
	std::wstring cacheFile;
	Result res = RepoBuildFolderCacheFilePath(repo, folderId, cacheFile);
	if (!res)
		return res;

	return RepoSaveFolderFileListFromCache(cacheFile, files);
}

Result RmSession::RepoSaveFolderFileListFromCache(const std::wstring& cacheFile, const std::vector<RmRepoFile>& files)
{
	Result res = RESULT(0);

	do {

		const NX::time::datetime& currentTime = NX::time::datetime::current_time();

		std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
		if (root == NULL) {
			res = RESULT(ERROR_NOT_ENOUGH_MEMORY);
			break;
		}

		PJSONVALUE pv = JsonValue::CreateNumber(currentTime.to_java_time());
		if (pv == NULL) {
			res = RESULT(ERROR_NOT_ENOUGH_MEMORY);
			break;
		}
		root->set(L"timeStamp", pv);

		PJSONARRAY pa = JsonValue::CreateArray();
		if (pa == NULL) {
			res = RESULT(ERROR_NOT_ENOUGH_MEMORY);
			break;
		}
		root->set(L"files", pa);

		std::for_each(files.begin(), files.end(), [&](const RmRepoFile& file) {

			if (file.Empty() || file.GetName().empty())
				return;

			PJSONOBJECT pfo = JsonValue::CreateObject();
			if (NULL == pfo)
				return;
			pa->push_back(pfo);

			pfo->set(L"name", JsonValue::CreateString(file.GetName()));
			pfo->set(L"id", JsonValue::CreateString(file.GetId()));
			pfo->set(L"path", JsonValue::CreateString(file.GetPath()));
			pfo->set(L"folder", JsonValue::CreateBool(file.IsFolder()));
			pfo->set(L"size", JsonValue::CreateNumber(file.GetSize()));
			pfo->set(L"time", JsonValue::CreateNumber(file.GetLastModifiedTime()));

			PJSONARRAY pfps = JsonValue::CreateArray();
			if (NULL != pfo) {
				pfo->set(L"properties", pfps);
				std::for_each(file.GetProperties().cbegin(), file.GetProperties().cend(), [&](const std::pair<std::wstring, std::wstring>& prop) {
					if (prop.first.empty() || prop.second.empty())
						return;
					const std::wstring item(prop.first + L"=" + prop.second);
					PJSONSTRING propStr = JsonValue::CreateString(item);
					if (NULL != propStr)
						pfps->push_back(propStr);
				});
			}

		});

		// serialize it
		JsonStringWriter<char> writer;
		const std::string& s = writer.Write(root.get());
		res = this->SecureFileWrite(cacheFile, s);

	} while (FALSE);
	if (!res) {
		::DeleteFileW(cacheFile.c_str());
		return res;
	}

	return RESULT(0);
}

void RmSession::RepoDeleteFolderCache(const RmRepository& repo, const std::wstring& folderId)
{
	std::wstring cacheFile;
	Result res = RepoBuildFolderCacheFilePath(repo, folderId, cacheFile);
	if (!res)
		return;


	std::vector<RmRepoFile> files;
	res = RepoGetFolderFileListFromCache(cacheFile, files, NULL);
	if (!res)
		return;

	DeleteFileW(cacheFile.c_str());

	std::for_each(files.begin(), files.end(), [&](const RmRepoFile& item) {
		if (item.IsFolder()) {
			RepoDeleteFolderCache(repo, item.GetId());
		}
	});
}

Result RmSession::RepoListFiles(RmRepository& repo, const std::wstring& folderId, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files)
{
	Result res = RESULT(0);

    if (repo.GetType() != RmRepository::MYDRIVE && repo.GetType() != RmRepository::MYVAULT) {
        if (repo.GetAccessToken() == L"") {
            res = RepoGetAccessToken(repo);
            if (!res)
                return res;
        }
    }

	switch (repo.GetType())
	{
	case RmRepository::MYDRIVE:
		res = MyDriveListFiles(folderId, cancelFlag, files);
		break;
	case RmRepository::GOOGLEDRIVE:
		res = GoogleDriveListFiles(repo, folderId, std::wstring(), cancelFlag, files);
		break;
	case RmRepository::ONEDRIVE:
		res = OneDriveListOrSearchFiles(repo, folderId, std::wstring(), cancelFlag, files);
		break;
	case RmRepository::DROPBOX:
		res = DropboxListFiles(repo, folderId, cancelFlag, files);
		break;
	case RmRepository::SHAREPOINTONLINE:
		res = SharePointOnlineListFiles(repo, folderId, cancelFlag, files, nullptr);
		break;
	case RmRepository::SHAREPOINTONPREM:
	case RmRepository::SHAREPOINTCROSSLAUNCH:
	case RmRepository::SHAREPOINTONLINE_CROSSLAUNCH:
	default:
		res = RESULT(ERROR_NOT_SUPPORTED);
		break;
	}
	
	return res;
}

Result RmSession::RepoSearchFiles(RmRepository& repo, const std::wstring& folderId, const std::wstring& filter, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files)
{
	Result res = RESULT(0);

	if (repo.GetType() != RmRepository::MYDRIVE && repo.GetType() != RmRepository::MYVAULT) {
		if (repo.GetAccessToken() == L"") {
			res = RepoGetAccessToken(repo);
			if (!res)
				return res;
		}
	}

	switch (repo.GetType())
	{
	case RmRepository::MYDRIVE:
		res = MyDriveSearchFiles(folderId, cancelFlag, filter, true, files);
		break;
	case RmRepository::GOOGLEDRIVE:
		res = GoogleDriveListFiles(repo, folderId, filter, cancelFlag, files);
		break;
	case RmRepository::ONEDRIVE:
		res = OneDriveListOrSearchFiles(repo, folderId, filter, cancelFlag, files);
		break;
	case RmRepository::DROPBOX:
		res = DropboxSearchFiles(repo, folderId, filter, cancelFlag, files);
		break;
	case RmRepository::SHAREPOINTONLINE:
		res = SharePointOnlineSearchFiles(repo, filter, cancelFlag, files);
		break;
	case RmRepository::SHAREPOINTONPREM:
	case RmRepository::SHAREPOINTCROSSLAUNCH:
	case RmRepository::SHAREPOINTONLINE_CROSSLAUNCH:
	default:
		res = RESULT(ERROR_NOT_SUPPORTED);
		break;
	}
	
	return res;
}

Result RmSession::RepoCreateFolder(RmRepository& repo, const std::wstring& parentFolderId, const std::wstring& folderName, RmRepoFile& folder)
{
	Result res = RESULT(0);

	if (repo.GetType() != RmRepository::MYDRIVE && repo.GetType() != RmRepository::MYVAULT) {
		if (repo.GetAccessToken() == L"") {
			res = RepoGetAccessToken(repo);
			if (!res)
				return res;
		}
	}

	switch (repo.GetType())
	{
	case RmRepository::MYDRIVE:
		res = MyDriveCreateFolder(parentFolderId, folderName, folder);
		break;
	case RmRepository::GOOGLEDRIVE:
		res = GoogleDriveCreateFolder(repo, parentFolderId, folderName, folder);
		break;
	case RmRepository::ONEDRIVE:
		res = OneDriveCreateFolder(repo, parentFolderId, folderName, folder);
		break;
	case RmRepository::DROPBOX:
		res = DropboxCreateFolder(repo, parentFolderId, folderName, folder);
		break;
	case RmRepository::SHAREPOINTONLINE:
		res = SharePointOnlineCreateFolder(repo, parentFolderId, folderName, folder);
		break;
	case RmRepository::SHAREPOINTONPREM:
	case RmRepository::SHAREPOINTCROSSLAUNCH:
	case RmRepository::SHAREPOINTONLINE_CROSSLAUNCH:
	default:
		res = RESULT(ERROR_NOT_SUPPORTED);
		break;
	}

	return res;
}

Result RmSession::RepoDeleteFolder(RmRepository& repo, const std::wstring& folderId)
{
	Result res = RESULT(0);

	if (repo.GetType() != RmRepository::MYDRIVE && repo.GetType() != RmRepository::MYVAULT) {
		if (repo.GetAccessToken() == L"") {
			res = RepoGetAccessToken(repo);
			if (!res)
				return res;
		}
	}

	switch (repo.GetType())
	{
	case RmRepository::MYDRIVE:
		res = MyDriveDeleteFile(folderId, NULL);
		break;
	case RmRepository::GOOGLEDRIVE:
		res = GoogleDriveDeleteFolder(repo, folderId);
		break;
	case RmRepository::ONEDRIVE:
		res = OneDriveDelete(repo, folderId);
		break;
	case RmRepository::DROPBOX:
		res = DropboxDeleteFolder(repo, folderId);
		break;
	case RmRepository::SHAREPOINTONLINE:
		res = SharePointOnlineDeleteFolder(repo, folderId);
		break;
	case RmRepository::SHAREPOINTONPREM:
	case RmRepository::SHAREPOINTCROSSLAUNCH:
	case RmRepository::SHAREPOINTONLINE_CROSSLAUNCH:
	default:
		res = RESULT(ERROR_NOT_SUPPORTED);
		break;
	}

	if (res) {
		RepoDeleteFolderCache(repo, folderId);
	}

	return res;
}

Result RmSession::RepoUploadFile(RmRepository& repo, const std::wstring& file, const std::wstring& repoPath, RmRepoFile& repoFile, _In_opt_ bool* cancelFlag)
{
	Result res = RESULT(0);

    if (repo.GetType() != RmRepository::MYDRIVE && repo.GetType() != RmRepository::MYVAULT) {
        if (repo.GetAccessToken() == L"") {
            res = RepoGetAccessToken(repo);
            if (!res)
                return res;
        }
    }

	switch (repo.GetType())
	{
	case RmRepository::MYDRIVE:
		res = MyDriveUploadFile(file, repoPath, repoFile, cancelFlag);
		break;
	case RmRepository::GOOGLEDRIVE:
		res = GoogleDriveUploadFile(repo, file, repoPath, repoFile, cancelFlag);
		break;
	case RmRepository::ONEDRIVE:
		res = OneDriveUploadFile(repo, file, repoPath, repoFile, cancelFlag);
		break;
	case RmRepository::DROPBOX:
		res = DropboxUploadFile(repo, file, repoPath, repoFile, cancelFlag);
		break;
	case RmRepository::SHAREPOINTONLINE:
		res = SharePointOnlineUploadFile(repo, file, repoPath, repoFile, cancelFlag);
		break;
	case RmRepository::SHAREPOINTONPREM:
	case RmRepository::SHAREPOINTCROSSLAUNCH:
	case RmRepository::SHAREPOINTONLINE_CROSSLAUNCH:
	default:
		res = RESULT(ERROR_NOT_SUPPORTED);
		break;
	}

	return res;
}

Result RmSession::RepoDownloadFile(RmRepository& repo, const std::wstring& fileId, std::wstring& localFilePath, _In_opt_ bool* cancelFlag, const ULONG requestedLength, bool logActivity)
{
	Result res = RESULT(0);

    if (repo.GetType() != RmRepository::MYDRIVE && repo.GetType() != RmRepository::MYVAULT) {
        if (repo.GetAccessToken() == L"") {
            res = RepoGetAccessToken(repo);
            if (!res)
                return res;
        }
    }

	// Make sure file exist
	NX::RmRepoFile file;
	res = RepoGetFileInfo(repo, fileId, file);
	if (!res)
		return res;

	switch (repo.GetType())
	{
	case RmRepository::MYDRIVE:
		res = MyDriveDownloadFile(file, localFilePath, cancelFlag, requestedLength);
		break;
	case RmRepository::GOOGLEDRIVE:
		res = GoogleDriveDownloadFile(repo, fileId, localFilePath, cancelFlag, requestedLength);
		break;
	case RmRepository::ONEDRIVE:
		res = OneDriveDownloadFile(repo, fileId, localFilePath, cancelFlag, requestedLength);
		break;
	case RmRepository::DROPBOX:
		res = DropboxDownloadFile(repo, fileId, localFilePath, cancelFlag, requestedLength);
		break;
	case RmRepository::SHAREPOINTONLINE:
		res = SharePointOnlineDownloadFile(repo, fileId, localFilePath, cancelFlag, requestedLength);
		break;
	case RmRepository::SHAREPOINTONPREM:
	case RmRepository::SHAREPOINTCROSSLAUNCH:
	case RmRepository::SHAREPOINTONLINE_CROSSLAUNCH:
	default:
		res = RESULT(ERROR_NOT_SUPPORTED);
		break;
	}

	if (logActivity && res && NX::iend_with<wchar_t>(file.GetName(), L".nxl")) {
		// Log DOWNLOAD activity for NXL file
		Result resOpen;
		std::shared_ptr<NX::NXL::File> fp(LocalFileOpen(localFilePath, true, resOpen));
		if (resOpen && fp != NULL) {
			(VOID)LogActivityDownload(true,
									  NX::bintohs<wchar_t>(fp->GetFileSecret().GetToken().GetId().data(), fp->GetFileSecret().GetToken().GetId().size()),
									  fp->GetFileSecret().GetOwnerIdW(),
									  repo.GetId(),
									  fileId,
									  file.GetName(),
									  file.GetPath());
		}
	}

	return res;
}

Result RmSession::RepoDeleteFile(RmRepository& repo, const std::wstring& fileId)
{
	Result res = RESULT(0);

	if (repo.GetType() != RmRepository::MYDRIVE && repo.GetType() != RmRepository::MYVAULT) {
		if (repo.GetAccessToken() == L"") {
			res = RepoGetAccessToken(repo);
			if (!res)
				return res;
		}
	}

	switch (repo.GetType())
	{
	case RmRepository::MYDRIVE:
		res = MyDriveDeleteFile(fileId, NULL);
		break;
	case RmRepository::GOOGLEDRIVE:
		res = GoogleDriveDeleteFile(repo, fileId);
		break;
	case RmRepository::ONEDRIVE:
		res = OneDriveDelete(repo, fileId);
		break;
	case RmRepository::DROPBOX:
		res = DropboxDeleteFile(repo, fileId);
		break;
	case RmRepository::SHAREPOINTONLINE:
		res = SharePointOnlineDeleteFile(repo, fileId);
		break;
	case RmRepository::SHAREPOINTONPREM:
	case RmRepository::SHAREPOINTCROSSLAUNCH:
	case RmRepository::SHAREPOINTONLINE_CROSSLAUNCH:
	default:
		res = RESULT(ERROR_NOT_SUPPORTED);
		break;
	}

	return res;
}

Result RmSession::RepoGetFileInfo(RmRepository& repo, const std::wstring& fileId, RmRepoFile& repoFile)
{
	Result res = RESULT(0);

    if (repo.GetType() != RmRepository::MYDRIVE && repo.GetType() != RmRepository::MYVAULT) {
        if (repo.GetAccessToken() == L"") {
            res = RepoGetAccessToken(repo);
            if (!res)
                return res;
        }
    }

	switch (repo.GetType())
	{
	case RmRepository::MYDRIVE:
		res = MyDriveGetFileInfo(fileId, repoFile);
		break;
	case RmRepository::GOOGLEDRIVE:
		res = GoogleDriveGetFileInfo(repo, fileId, repoFile);
		break;
	case RmRepository::ONEDRIVE:
		res = OneDriveGetFileInfo(repo, fileId, repoFile);
		break;
	case RmRepository::DROPBOX:
		res = DropboxGetFileInfo(repo, fileId, repoFile);
		break;
	case RmRepository::SHAREPOINTONLINE:
		res = SharePointOnlineGetFileInfo(repo, fileId, repoFile, std::wstring());
		break;
	case RmRepository::SHAREPOINTONPREM:
	case RmRepository::SHAREPOINTCROSSLAUNCH:
	case RmRepository::SHAREPOINTONLINE_CROSSLAUNCH:
	default:
		res = RESULT(ERROR_NOT_SUPPORTED);
		break;
	}

	return res;
}

Result RmSession::RepoGetNxlFileHeader(RmRepository& repo,
	const std::wstring& fileId,
	_In_opt_ bool* cancelFlag,
	_Out_opt_ NX::NXL::FileSecret* secret,
	_Out_opt_ NX::NXL::FileMeta* metadata,
	_Out_opt_ NX::NXL::FileAdHocPolicy* adhocPolicy,
	_Out_opt_ NX::NXL::FileTags* tags)
{
	RmRepoFile file;
	std::wstring fid = fileId;
	if (repo.GetType() == RmRepository::ONEDRIVE) {
		size_t nfind = fid.find_last_of(L'/');
		if (std::wstring::npos != nfind) {
			fid = fid.substr(nfind + 1);
		}
	}
	Result res = RepoGetFileInfo(repo, fid, file);
	if (!res)
		return res;

	if (!NX::iend_with<wchar_t>(file.GetName(), L".nxl"))
		return RESULT(ERROR_BAD_FILE_TYPE);

	if(file.GetSize() < 0x4000)
		return RESULT(ERROR_BAD_FILE_TYPE);

	// generate temp file for file header
	std::wstring strFullPath(repo.GetId() + L"\\header\\" + fid);
	UCHAR pathHash[20] = { 0 };
	NX::crypt::CreateSha1((const UCHAR*)strFullPath.c_str(), (ULONG)strFullPath.length() * 2, pathHash);
	std::wstring strTempFile(GetCurrentUserTempDir() + L"\\" + NX::bintohs<wchar_t>(pathHash, 20) + L".nxl");
	
	do {

		res = RepoDownloadFile(repo, fid, strTempFile, cancelFlag, 0x4000, false);
		if (!res)
			break;

		if (cancelFlag != NULL && *cancelFlag) {
			res = RESULT(ERROR_CANCELLED);
			break;
		}

		std::shared_ptr<NX::NXL::File> fp(LocalFileOpen(strTempFile, true, res));
		if (fp == NULL)
			break;

		res = RESULT(0);
		if (NULL != secret)
			*secret = fp->GetFileSecret();

		if (NULL != metadata)
			fp->ReadMetadata(*metadata);

		if (NULL != adhocPolicy)
			fp->ReadAdHocPolicy(*adhocPolicy);

		if (NULL != tags)
			fp->ReadTags(*tags);

	} while (FALSE);
	// force delete temp file
	::DeleteFileW(strTempFile.c_str());
	if (!res)
		return res;

	return RESULT(0);
}

Result RmSession::RepoProtectFile(const std::wstring& membershipId,
	RmRepository& repo,
	const RmRepoFile& file,
	ULONGLONG permissions,
	const NXL::FileMeta& metadata,
	const NXL::FileTags& tags,
	RmFileShareResult& sr,
	_In_opt_ bool* cancelFlag)
{
	RmRestStatus status;

	const std::wstring origFileName(file.GetName());
	if (NX::iend_with<wchar_t>(origFileName, L".nxl"))
		return RESULT(ERROR_FILE_ENCRYPTED);

	const std::wstring displayName(repo.GetName() + L":" + file.GetPath());
	
	const std::wstring& tmpFileDir(GetCurrentUserTempDir() + L"\\DIR" + RepoFileCreateCacheId(repo, file));
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(tmpFileDir.c_str())) {
		if (!CreateDirectoryW(tmpFileDir.c_str(), NULL))
			return RESULT(GetLastError());
	}

	std::vector<std::wstring> rights;
	std::vector<std::wstring> obs;

	if (0 != permissions) {
		if (0 != (permissions&BUILTIN_RIGHT_VIEW))
			rights.push_back(RIGHT_ACTION_VIEW);
		if (0 != (permissions&BUILTIN_RIGHT_PRINT))
			rights.push_back(RIGHT_ACTION_PRINT);
		if (0 != (permissions&BUILTIN_RIGHT_SHARE))
			rights.push_back(RIGHT_ACTION_SHARE);
		if (0 != (permissions&BUILTIN_RIGHT_DOWNLOAD))
			rights.push_back(RIGHT_ACTION_DOWNLOAD);

		if (0 != (permissions&BUILTIN_OBRIGHT_WATERMARK))
			obs.push_back(OBLIGATION_NAME_WATERMARK);
	}

	SYSTEMTIME st = { 0 };
	GetSystemTime(&st);
	const std::wstring wsTimestamp(NX::FormatString(L"-%04d-%02d-%02d-%02d-%02d-%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond));

	std::wstring tmpFilePath(tmpFileDir + L"\\" + origFileName);
	std::wstring encFilePath;
		
	Result res = RESULT(0);
	if (!res)
		return res;

	do {
		std::wstring fid = file.GetId();
		if (repo.GetType() == RmRepository::ONEDRIVE) {
			size_t nfind = fid.find_last_of(L'/');
			if (std::wstring::npos != nfind) {
				fid = fid.substr(nfind + 1);
			}
		}
		res = RepoDownloadFile(repo, fid, tmpFilePath, cancelFlag, 0, false);
		if (!res)
			break;

		RmUserMembership* membership = _user.Findmembership(membershipId);
		if (NULL == membership) {
			res = RESULT(ERROR_NOT_FOUND);
			break;
		}

		const NXL::FileToken& token = membership->GetFileToken();
		if (token.Empty() || token.GetKey().empty()) {
			res = RESULT(NTE_BAD_KEY);
			break;
		}

		NXL::FileSecret newSecret(std::string(membership->GetId().begin(), membership->GetId().end()), token, 0, membership->GetAgreement0(), membership->GetAgreement1(), 0);
		NXL::FileAdHocPolicy policy(membership->GetId(), rights, obs);

		std::wstring dummy;
		const std::wstring tmpFileName = NX::remove_tail<wchar_t, L'\\'>(tmpFilePath, dummy);
		const wchar_t* pExt = wcsrchr(tmpFileName.c_str(), L'.');
		const std::wstring fileExt(pExt?pExt:L"");
		encFilePath = fileExt.empty() ? (tmpFilePath + wsTimestamp + L".nxl") : (tmpFilePath.substr(0, tmpFilePath.length() - fileExt.length()) + wsTimestamp + fileExt + L".nxl");

		std::shared_ptr<NX::NXL::File> fp(NXL::File::CreateFrom(tmpFilePath, encFilePath, true, newSecret, NULL, &metadata, &policy, &tags, NXL_DEFAULT_MSG, res));
		if (fp == NULL) {
			break;
		}
		fp.reset();

		if (cancelFlag && *cancelFlag) {
			res = RESULT(ERROR_CANCELLED);
			break;
		}

		// File has been created, uploaded it
		RmMyVaultUploadResult newFile;
		RmRestStatus status;
		res = _restClient.MyVaultUploadFile(_rmserver.GetUrl(), cancelFlag, encFilePath, L"", file.GetPath(), L"", repo.GetName(), repo.GetTypeName(), status, newFile);
		if (!status)
			res = RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
		if (!res)
			break;

		// Succeeded
		sr._duid = NX::bintohs<wchar_t>(newSecret.GetToken().GetId().data(), newSecret.GetToken().GetId().size());
		sr._path = newFile.GetPath();
		sr._name = newFile.GetName();
		// Write log
		LogActivityProtect(NX::bintohs<wchar_t>(newSecret.GetToken().GetId().data(), newSecret.GetToken().GetId().size()), newSecret.GetOwnerIdW(), repo.GetId(), file.GetId(), file.GetName(), file.GetPath());

	} while (FALSE);

	// Always delete temp file/dir
	if (!encFilePath.empty())
		::DeleteFileW(encFilePath.c_str());
	::DeleteFileW(tmpFilePath.c_str());
	::RemoveDirectoryW(tmpFileDir.c_str());

	return res;
}

Result RmSession::RepoShareFile(const std::wstring& membershipId,
	const RmRepository& repo,
	const RmRepoFile& file,
	ULONGLONG permissions,
	const NXL::FileMeta& metadata,
	const NXL::FileTags& tags,
	std::vector<std::wstring>& recipients,
	_In_opt_ const SYSTEMTIME* expireTime,
	RmFileShareResult& sr)
{
	__int64 unixExpireTime = 0;
	if (expireTime) {
		NX::time::datetime dt(expireTime);
		unixExpireTime = dt.to_java_time();
	}
	return RepoShareFile(membershipId, repo, file, permissions, metadata, tags, recipients, unixExpireTime, sr);
}

Result RmSession::RepoShareFile(const std::wstring& membershipId,
	const RmRepository& repo,
	const RmRepoFile& file,
	ULONGLONG permissions,
	const NXL::FileMeta& metadata,
	const NXL::FileTags& tags,
	std::vector<std::wstring>& recipients,
	_In_ __int64 unixExpireTime,
	RmFileShareResult& sr)
{
	RmRestStatus status;
	std::wstring fid = file.GetId();
	if (repo.GetType() == RmRepository::ONEDRIVE) {
		size_t nfind = fid.find_last_of(L'/');
		if (std::wstring::npos != nfind) {
			fid = fid.substr(nfind + 1);
		}
	}
	Result res = _restClient.ShareRepositoryFile(_rmserver.GetUrl(),
		membershipId,
		recipients,
		permissions,
		unixExpireTime,
		repo.GetId(),
		file.GetName(),
		file.GetPath(),
		fid,
		metadata,
		tags,
		status,
		sr);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}