
#include "stdafx.h"

#include <nx\rmc\session.h>

using namespace NX;


Result RmSession::SharePointOnlineGetFileInfo(const RmRepository& repo, const std::wstring& path, RmRepoFile& file, std::wstring& serverRelUrl)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    std::wstring parentFolder;
    const std::wstring& fileName = NX::remove_tail<wchar_t, L'/'>(path, parentFolder);
    if (parentFolder.empty())
        parentFolder.append(L"/");

    RmRestStatus status;
    std::vector<RmRepoFile> files;
    std::vector<std::wstring> serverRelUrls;
    Result res = SharePointOnlineListFiles(repo, parentFolder, NULL, files, &serverRelUrls);
    if (!res)
        return res;

    size_t i;
    for (i = 0; i < files.size(); i++) {
        if (0 == _wcsicmp(files[i].GetId().c_str(), path.c_str())) {
            file = files[i];
            serverRelUrl = serverRelUrls[i];
            break;
        }
    }

    if (i == files.size()) {
        return RESULT(ERROR_NOT_FOUND);
    }

    return RESULT(0);
}

Result RmSession::SharePointOnlineListFiles(const RmRepository& repo, const std::wstring& path, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files, std::vector<std::wstring> *serverRelUrls)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRestStatus status;
    Result res;

    if (path == L"/") {
        res = _restClient.SharePointOnlineListCollections(repo.GetAccountName(), repo.GetAccessToken(), status, files, serverRelUrls);
        if (!res)
            return res;
        if (!status)
            return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
    } else {
        std::vector<RmRepoFile> filesInAllLevels;
        std::vector<__int64> idsInAllLevels;
        std::vector<std::wstring> sourceUrlsInAllLevels;
        std::vector<std::wstring> serverRelUrlsInAllLevels;
        const std::wstring& collection = path.substr(1, path.find('/', 1) - 1);
        res = _restClient.SharePointOnlineListOrSearchFilesInCollection(repo.GetAccountName(), cancelFlag, repo.GetAccessToken(), collection, std::wstring(), status, filesInAllLevels, idsInAllLevels, sourceUrlsInAllLevels, &serverRelUrlsInAllLevels);

        if (!res)
            return res;
        if (!status)
            return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

        std::wstring pathWithSlash = path;
        if (path.back() != L'/') {
            pathWithSlash += L'/';
        }

        for (size_t i = 0; i < filesInAllLevels.size(); i++) {
            const std::wstring& filePath = filesInAllLevels[i].GetId();
            // Include only the files/folders at exactly this folder level, not higher or lower.
            if (filePath.compare(0, pathWithSlash.length(), pathWithSlash) == 0 && filePath.find(L'/', pathWithSlash.length()) == std::wstring::npos) {
                files.push_back(filesInAllLevels[i]);
                if (serverRelUrls) {
                    serverRelUrls->push_back(serverRelUrlsInAllLevels[i]);
                }
            }
        }
    }

    return RESULT(0);
}

Result RmSession::SharePointOnlineSearchFiles(const RmRepository& repo, const std::wstring& keywords, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    std::vector<RmRepoFile> collections;
    RmRestStatus status;
    Result res;

    res = _restClient.SharePointOnlineListCollections(repo.GetAccountName(), repo.GetAccessToken(), status, collections, nullptr);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    for (const auto& collection : collections) {
        std::vector<__int64> ids;
        std::vector<std::wstring> sourceUrls;

        res = _restClient.SharePointOnlineListOrSearchFilesInCollection(repo.GetAccountName(), cancelFlag, repo.GetAccessToken(), collection.GetId().substr(1), keywords, status, files, ids, sourceUrls, nullptr);

        if (!res)
            return res;
        if (!status)
            return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
    }

    return RESULT(0);
}

Result RmSession::SharePointOnlineCreateFolder(const RmRepository& repo, const std::wstring& parentDir, const std::wstring& folderName, RmRepoFile& folder)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRepoFile repoParentFolderAsFile;
    std::wstring repoParentFolderServerRelUrl;
    Result res = SharePointOnlineGetFileInfo(repo, parentDir, repoParentFolderAsFile, repoParentFolderServerRelUrl);
    if (!res)
        return res;

    RmRestStatus status;
    std::wstring repoParentFolderUniqId;
    res = _restClient.SharePointOnlineGetFolderUniqId(repo.GetAccountName(), repo.GetAccessToken(), repoParentFolderServerRelUrl, status, repoParentFolderUniqId);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    res = _restClient.SharePointOnlineCreateFolder(repo.GetAccountName(), repo.GetAccessToken(), repoParentFolderAsFile, repoParentFolderServerRelUrl, folderName, status, folder);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}

Result RmSession::SharePointOnlineDeleteFolder(const RmRepository& repo, const std::wstring& folderPath)
{
    return SharePointOnlineDeleteFile(repo, folderPath);
}

Result RmSession::SharePointOnlineDeleteFile(const RmRepository& repo, const std::wstring& repoFilePath)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    std::wstring pathUnderCollection;
    const std::wstring& collection = NX::remove_head<wchar_t, L'/'>(repoFilePath.substr(1), pathUnderCollection);

    if (pathUnderCollection.empty()) {
        // Cannot delete a collection.
        return RESULT(ERROR_ACCESS_DENIED);
    }

    RmRestStatus status;
    std::vector<RmRepoFile> files;
    std::vector<__int64> ids;
    std::vector<std::wstring> sourceUrls;
    Result res = _restClient.SharePointOnlineListOrSearchFilesInCollection(repo.GetAccountName(), NULL, repo.GetAccessToken(), collection, std::wstring(), status, files, ids, sourceUrls, nullptr);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    const __int64 *repoFileId = nullptr;

    for (size_t i = 0; i < files.size(); i++) {
        if (repoFilePath == files[i].GetId()) {
            repoFileId = &ids[i];
            break;
        }
    }

    if (!repoFileId) {
        return RESULT(ERROR_NOT_FOUND);
    }

    res = _restClient.SharePointOnlineDeleteFile(repo.GetAccountName(), repo.GetAccessToken(), collection, *repoFileId, status);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}

Result RmSession::SharePointOnlineUploadFile(const RmRepository& repo, const std::wstring& file, const std::wstring& repoPath, RmRepoFile& repoFile, _In_opt_ bool* cancelFlag)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    std::wstring repoParentFolder;
    const std::wstring& repoFileName = NX::remove_tail<wchar_t, L'/'>(repoPath, repoParentFolder);
    if (repoParentFolder.empty())
        repoParentFolder.append(L"/");

    RmRepoFile repoParentFolderAsFile;
    std::wstring repoParentFolderServerRelUrl;
    Result res = SharePointOnlineGetFileInfo(repo, repoParentFolder, repoParentFolderAsFile, repoParentFolderServerRelUrl);
    if (!res)
        return res;

    RmRestStatus status;
    std::wstring repoParentFolderUniqId;
    res = _restClient.SharePointOnlineGetFolderUniqId(repo.GetAccountName(), repo.GetAccessToken(), repoParentFolderServerRelUrl, status, repoParentFolderUniqId);
    if (!res)
        return res;

    res = _restClient.SharePointOnlineUploadFile(repo.GetAccountName(), cancelFlag, repo.GetAccessToken(), file, repoParentFolderAsFile, repoParentFolderUniqId, repoFileName, status, repoFile);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}

Result RmSession::SharePointOnlineDownloadFile(const RmRepository& repo, const std::wstring& repoFilePath, const std::wstring& localFilePath, _In_opt_ bool* cancelFlag, const ULONG requestedLength)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    std::wstring pathUnderCollection;
    const std::wstring& collection = NX::remove_head<wchar_t, L'/'>(repoFilePath.substr(1), pathUnderCollection);

    if (pathUnderCollection.empty()) {
        // Cannot download a collection.
        return RESULT(ERROR_NOT_SUPPORTED);
    }

    RmRestStatus status;
    std::vector<RmRepoFile> files;
    std::vector<__int64> ids;
    std::vector<std::wstring> sourceUrls;
    Result res = _restClient.SharePointOnlineListOrSearchFilesInCollection(repo.GetAccountName(), cancelFlag, repo.GetAccessToken(), collection, std::wstring(), status, files, ids, sourceUrls, nullptr);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    const std::wstring *repoFileSourceUrl = nullptr;

    for (size_t i = 0; i < files.size(); i++) {
        if (repoFilePath == files[i].GetId()) {
            repoFileSourceUrl = &sourceUrls[i];
            break;
        }
    }

    if (!repoFileSourceUrl) {
        return RESULT(ERROR_NOT_FOUND);
    }

    res = _restClient.SharePointOnlineDownloadFile(repo.GetAccountName(), cancelFlag, requestedLength, repo.GetAccessToken(), *repoFileSourceUrl, localFilePath, status);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}
