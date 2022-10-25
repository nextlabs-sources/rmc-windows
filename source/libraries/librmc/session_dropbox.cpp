
#include "stdafx.h"

#include <nx\rmc\session.h>

using namespace NX;


Result RmSession::DropboxGetFileInfo(const RmRepository& repo, const std::wstring& path, RmRepoFile& file)
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
    Result res = DropboxListFiles(repo, parentFolder, NULL, files);
    if (!res)
        return res;

    auto pos = std::find_if(files.begin(), files.end(), [&](const RmRepoFile& f)->bool {
        return (0 == _wcsicmp(f.GetName().c_str(), fileName.c_str()));
    });

    if (pos == files.end()) {
        return RESULT(ERROR_NOT_FOUND);
    }

    file = *pos;
    return RESULT(0);
}

Result RmSession::DropboxListFiles(const RmRepository& repo, const std::wstring& path, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRestStatus status;
    Result res = _restClient.DropboxListFiles(cancelFlag, repo.GetAccessToken(), path, status, files);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}

Result RmSession::DropboxSearchFiles(const RmRepository& repo, const std::wstring& path, const std::wstring& keywords, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRestStatus status;
    Result res = _restClient.DropboxSearchFiles(cancelFlag, repo.GetAccessToken(), path, keywords, status, files);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}

Result RmSession::DropboxCreateFolder(const RmRepository& repo, const std::wstring& parentDir, const std::wstring& folderName, RmRepoFile& folder)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRestStatus status;
    Result res = _restClient.DropboxCreateFolder(repo.GetAccessToken(), parentDir, folderName, status, folder);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}

Result RmSession::DropboxDeleteFolder(const RmRepository& repo, const std::wstring& folderPath)
{
    return DropboxDeleteFile(repo, folderPath);
}

Result RmSession::DropboxDeleteFile(const RmRepository& repo, const std::wstring& repoFilePath)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRestStatus status;
    Result res = _restClient.DropboxDeleteFile(repo.GetAccessToken(), repoFilePath, status);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}

Result RmSession::DropboxUploadFile(const RmRepository& repo, const std::wstring& file, const std::wstring& repoPath, RmRepoFile& repoFile, _In_opt_ bool* cancelFlag)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    // If the file already exists, we can still upload since Dropbox allows
    // uploading a file whose name and content are identical to an existing
    // file.  However, we need to disallow this since Web Client doesn't
    // support this.
    RmRepoFile existingRepoFile;
    Result res = DropboxGetFileInfo(repo, repoPath, existingRepoFile);
    if (res) {
        return RESULT(ERROR_FILE_EXISTS);
    } else if (res.GetCode() != ERROR_NOT_FOUND) {
        return res;
    }

    RmRestStatus status;
    res = _restClient.DropboxUploadFile(cancelFlag, repo.GetAccessToken(), file, repoPath, status, repoFile);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}

Result RmSession::DropboxDownloadFile(const RmRepository& repo, const std::wstring& repoFilePath, const std::wstring& localFilePath, _In_opt_ bool* cancelFlag, const ULONG requestedLength)
{
    if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
        return RESULT(ERROR_EXPIRED_HANDLE);
    }

    RmRestStatus status;
    Result res = _restClient.DropboxDownloadFile(cancelFlag, requestedLength, repo.GetAccessToken(), repoFilePath, localFilePath, status);
    if (!res)
        return res;
    if (!status)
        return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

    return RESULT(0);
}
