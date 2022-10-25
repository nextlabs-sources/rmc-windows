
#include "stdafx.h"

#include <nx\rmc\session.h>

using namespace NX;


Result RmSession::OneDriveGetFileInfo(const RmRepository& repo, const std::wstring& fileId, RmRepoFile& file)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRestStatus status;
    Result res = _restClient.OneDriveGetFileInfo(repo.GetAccessToken(), fileId, status, file);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}

Result RmSession::OneDriveListOrSearchFiles(const RmRepository& repo, const std::wstring& folderId, const std::wstring& keywords, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRestStatus status;
    Result res = _restClient.OneDriveListOrSearchFiles(cancelFlag, repo.GetAccessToken(), folderId, keywords, status, files);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}

Result RmSession::OneDriveCreateFolder(const RmRepository& repo, const std::wstring& parentFolderId, const std::wstring& folderName, RmRepoFile& folder)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRestStatus status;
    Result res = _restClient.OneDriveCreateFolder(repo.GetAccessToken(), parentFolderId, folderName, status, folder);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}

Result RmSession::OneDriveDelete(const RmRepository& repo, const std::wstring& itemId)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRestStatus status;
    Result res = _restClient.OneDriveDelete(repo.GetAccessToken(), itemId, status);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}

Result RmSession::OneDriveUploadFile(const RmRepository& repo, const std::wstring& file, const std::wstring& repoPath, RmRepoFile& repoFile, _In_opt_ bool* cancelFlag)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    std::wstring parentFolderId;
    const std::wstring fileName = NX::remove_tail<wchar_t, L'/'>(repoPath, parentFolderId);
    if (parentFolderId.empty()) {
        parentFolderId = L"/";
    }

    // If the file already exists, we can still upload since OneDrive will
    // replace the existing file with the new file.  However, we need to
    // disallow this since Web Client doesn't support this.
    RmRestStatus status;
    std::vector<RmRepoFile> files;
    Result res = _restClient.OneDriveListOrSearchFiles(cancelFlag, repo.GetAccessToken(), parentFolderId, L"", status, files);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    for (const auto& file : files) {
        if (_wcsicmp(file.GetName().c_str(), fileName.c_str()) == 0) {
            return RESULT(ERROR_FILE_EXISTS);
        }
    }

    res = _restClient.OneDriveUploadFile(cancelFlag, repo.GetAccessToken(), file, repoPath, status, repoFile);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}

Result RmSession::OneDriveDownloadFile(const RmRepository& repo, const std::wstring& fileId, const std::wstring& localFilePath, _In_opt_ bool* cancelFlag, const ULONG requestedLength)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRestStatus status;
    Result res = _restClient.OneDriveDownloadFile(cancelFlag, requestedLength, repo.GetAccessToken(), fileId, localFilePath, status);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}
