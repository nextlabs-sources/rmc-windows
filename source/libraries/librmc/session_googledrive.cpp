
#include "stdafx.h"

#include <nx\rmc\session.h>
#include <nx\common\time.h>

using namespace NX;


Result RmSession::GoogleDriveGetFileInfo(const RmRepository& repo, const std::wstring& fileId, RmRepoFile& file)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRestStatus status;
    std::vector<std::pair<RmRepoFile, std::wstring>> filesAndGdIds;
    Result res = _restClient.GoogleDriveListFiles(NULL, repo.GetAccessToken(), std::wstring(), status, filesAndGdIds);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    for (const auto& fileAndGdId : filesAndGdIds) {
        if (fileId == fileAndGdId.first.GetId()) {
            file = fileAndGdId.first;
            return RESULT(0);
        }
    }

    return RESULT(ERROR_NOT_FOUND);
}

Result RmSession::GoogleDriveListFiles(const RmRepository& repo, const std::wstring& folderId, const std::wstring& keywords, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRestStatus status;
    std::vector<std::pair<RmRepoFile, std::wstring>> filesAndGdIds;
    Result res = _restClient.GoogleDriveListFiles(cancelFlag, repo.GetAccessToken(), keywords, status, filesAndGdIds);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    std::wstring folderIdWithSlash = folderId;
    if (folderId.back() != L'/') {
        folderIdWithSlash += L'/';
    }

    for (const auto& fileAndGdId : filesAndGdIds) {
        if (keywords.empty()) {
            // This is a list operation.  The result so far contains all files
            // plus all folders.
            //
            // We should return only those files/folders in the result that
            // are at exactly this folder level, not higher or lower.
            const std::wstring& fileId = fileAndGdId.first.GetId();
            if (fileId.compare(0, folderIdWithSlash.length(), folderIdWithSlash) == 0 && fileId.find(L'/', folderIdWithSlash.length()) == std::wstring::npos) {
                files.push_back(fileAndGdId.first);
            }
        } else {
            // This is a search operation.  The result so far contains all
            // files whose names contain the keyword, plus all folders.
            //
            // We should return all files in the result, plus only those
            // folders in the result whose names contain the keyword.
            if (!fileAndGdId.first.IsFolder() || NX::duplower<wchar_t>(fileAndGdId.first.GetName()).find(NX::duplower<wchar_t>(keywords)) != std::wstring::npos) {
                files.push_back(fileAndGdId.first);
            }
        }
    }

    return RESULT(0);
}

Result RmSession::GoogleDriveCreateFolder(const RmRepository& repo, const std::wstring& parentFolderId, const std::wstring& folderName, RmRepoFile& folder)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRestStatus status;
    Result res;

    std::vector<std::pair<RmRepoFile, std::wstring>> filesAndGdIds;
    const std::pair<RmRepoFile, std::wstring> *parentFolderFileAndGdId = nullptr;

    if (parentFolderId != L"/") {
        Result res = _restClient.GoogleDriveListFiles(NULL, repo.GetAccessToken(), std::wstring(), status, filesAndGdIds);
        if (!res)
            return res;
        if (!status)
            return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

        for (const auto& fileAndGdId : filesAndGdIds) {
            if (parentFolderId == fileAndGdId.first.GetId()) {
                parentFolderFileAndGdId = &fileAndGdId;
                break;
            }
        }
        if (!parentFolderFileAndGdId) {
            return RESULT(ERROR_NOT_FOUND);
        }
    }

    res = _restClient.GoogleDriveCreateFolder(repo.GetAccessToken(), *parentFolderFileAndGdId, folderName, status, folder);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}

Result RmSession::GoogleDriveDeleteFolder(const RmRepository& repo, const std::wstring& folderId)
{
    return GoogleDriveDeleteFile(repo, folderId);
}

Result RmSession::GoogleDriveDeleteFile(const RmRepository& repo, const std::wstring& fileId)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRestStatus status;
    std::vector<std::pair<RmRepoFile, std::wstring>> filesAndGdIds;
    Result res = _restClient.GoogleDriveListFiles(NULL, repo.GetAccessToken(), std::wstring(), status, filesAndGdIds);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    const std::wstring *fileGdId = nullptr;
    for (const auto& fileAndGdId : filesAndGdIds) {
        if (fileId == fileAndGdId.first.GetId()) {
            fileGdId = &fileAndGdId.second;
            break;
        }
    }

    if (!fileGdId) {
        return RESULT(ERROR_NOT_FOUND);
    }

    res = _restClient.GoogleDriveDeleteFile(repo.GetAccessToken(), *fileGdId, status);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}

Result RmSession::GoogleDriveUploadFile(const RmRepository& repo, const std::wstring& file, const std::wstring& repoPath, RmRepoFile& repoFile, _In_opt_ bool* cancelFlag)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRestStatus status;
    Result res;

    const size_t pos = repoPath.find_last_of('/');
    const std::wstring parentRepoPath = repoPath.substr(0, pos);
    const std::wstring repoFileName = repoPath.substr(pos + 1);
    std::vector<std::pair<RmRepoFile, std::wstring>> filesAndGdIds;
    const std::pair<RmRepoFile, std::wstring> *parentFileAndGdId = nullptr;

    res = _restClient.GoogleDriveListFiles(cancelFlag, repo.GetAccessToken(), std::wstring(), status, filesAndGdIds);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    // If the parent directory doesn't exist, we can't upload.
    if (!parentRepoPath.empty()) {
        for (const auto& fileAndGdId : filesAndGdIds) {
            if (fileAndGdId.first.IsFolder() && fileAndGdId.first.GetId() == parentRepoPath) {
                parentFileAndGdId = &fileAndGdId;
                break;
            }
        }
        if (!parentFileAndGdId) {
            return RESULT(ERROR_NOT_FOUND);
        }
    }

    // If the file already exists or if a folder has the same name as the
    // file, we can still upload since Google Drive can handle two items with
    // the same name.  However, we need to disallow this since Web Client
    // doesn't support this.
    const std::wstring newFilePath = (parentFileAndGdId == nullptr ? L"" : parentFileAndGdId->first.GetPath()) + L'/' + repoFileName;
    for (const auto& fileAndGdId : filesAndGdIds) {
        if (_wcsicmp(fileAndGdId.first.GetPath().c_str(), newFilePath.c_str()) == 0) {
            return RESULT(ERROR_FILE_EXISTS);
        }
    }

    res = _restClient.GoogleDriveUploadFile(cancelFlag, repo.GetAccessToken(), file, *parentFileAndGdId, repoFileName, status, repoFile);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}

// Since the result of About:get API says that Google Drive only allows the
// following source formats to be exported, we can only support exporting the
// following source formats.
//
// For each source format, we choose one of the supported destination formats
// to be our hard-coded destination format.
static const std::unordered_map<std::wstring, const std::pair<const std::wstring, const std::wstring>> exportTypeMap = {
    {
        L"application/vnd.google-apps.form",
        std::pair<const std::wstring, const std::wstring>{L"application/zip", L"zip"}
    },
    {
        L"application/vnd.google-apps.document",
        std::pair<const std::wstring, const std::wstring>{L"application/vnd.openxmlformats-officedocument.wordprocessingml.document", L"docx"}
    },
    {
        L"application/vnd.google-apps.drawing",
        std::pair<const std::wstring, const std::wstring>{L"image/png", L"png"}
    },
    {
        L"application/vnd.google-apps.spreadsheet",
        std::pair<const std::wstring, const std::wstring>{L"application/vnd.openxmlformats-officedocument.spreadsheetml.sheet", L"xlsx"}
    },
    {
        L"application/vnd.google-apps.script",
        std::pair<const std::wstring, const std::wstring>{L"application/vnd.google-apps.script+json", L"json"}
    },
    {
        L"application/vnd.google-apps.presentation",
        std::pair<const std::wstring, const std::wstring>{L"application/vnd.openxmlformats-officedocument.presentationml.presentation", L"pptx"}
    },
};

Result RmSession::GoogleDriveDownloadFile(const RmRepository& repo, const std::wstring& fileId, std::wstring& localFilePath, _In_opt_ bool* cancelFlag, const ULONG requestedLength)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRestStatus status;
    std::vector<std::pair<RmRepoFile, std::wstring>> filesAndGdIds;
    Result res = _restClient.GoogleDriveListFiles(cancelFlag, repo.GetAccessToken(), std::wstring(), status, filesAndGdIds);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    const RmRepoFile *repoFile = nullptr;
    const std::wstring *fileGdId;

    for (const auto& fileAndGdId : filesAndGdIds) {
        if (fileId == fileAndGdId.first.GetId()) {
            repoFile = &fileAndGdId.first;
            fileGdId = &fileAndGdId.second;
            break;
        }
    }

    if (!repoFile) {
        return RESULT(ERROR_NOT_FOUND);
    }

    // See if this file type needs to be exported instead of downloaded.
    std::wstring mimeType;
    res = _restClient.GoogleDriveGetFileMimeType(cancelFlag, repo.GetAccessToken(), *fileGdId, status, mimeType);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    const auto& it = exportTypeMap.find(mimeType);
    if (it == exportTypeMap.end()) {
        res = _restClient.GoogleDriveDownloadFile(cancelFlag, requestedLength, repo.GetAccessToken(), *fileGdId, localFilePath, status);
    } else {
        // Add the new file extension to the destination file path.
        localFilePath += L"." + it->second.second;
        res = _restClient.GoogleDriveExportFile(cancelFlag, requestedLength, repo.GetAccessToken(), *fileGdId, localFilePath, it->second.first, status);
    }

    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    FILETIME ft;
    NX::time::datetime::from_unix_time(repoFile->GetLastModifiedTime()).to_filetime(&ft);

    HANDLE hFile = ::CreateFile(localFilePath.c_str(), FILE_WRITE_ATTRIBUTES, FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
    if (hFile != INVALID_HANDLE_VALUE) {
        ::SetFileTime(hFile, NULL, NULL, &ft);
        ::CloseHandle(hFile);
    }

    return RESULT(0);
}
