
#include "stdafx.h"

#include <nx\rmc\session.h>
#include <nx\rmc\securefile.h>

#include <nx\common\stringex.h>
#include <nx\common\rapidjson.h>
#include <nx\rest\uri.h>

using namespace NX;


Result RmSession::MyDriveGetUsage(RmMyDriveUsage& usage)
{
	if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
		return RESULT(ERROR_EXPIRED_HANDLE);
	}

	RmRestStatus status;
	Result res = _restClient.MyDriveGetUsage(_rmserver.GetUrl(), status, usage);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::MyDriveGetFileInfo(const std::wstring& path, RmRepoFile& file)
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
	Result res = MyDriveListFiles(parentFolder, NULL, files);
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

Result RmSession::MyDriveListFiles(const std::wstring& parentDir, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files)
{
	files.clear();

	if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
		return RESULT(ERROR_EXPIRED_HANDLE);
	}

	RmRestStatus status;
	std::wstring parentDirPathId(parentDir);
	if (!NX::iend_with<wchar_t>(parentDirPathId, L"/"))
		parentDirPathId.append(L"/");
	Result res = _restClient.MyDriveListFiles(_rmserver.GetUrl(), cancelFlag, parentDirPathId, status, files);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::MyDriveSearchFiles(const std::wstring& parentDir, _In_opt_ bool* cancelFlag, const std::wstring& keywords, bool recursively, std::vector<RmRepoFile>& files)
{
	files.clear();

	if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
		return RESULT(ERROR_EXPIRED_HANDLE);
	}

	RmRestStatus status;
	Result res = _restClient.MyDriveSearchFiles(_rmserver.GetUrl(), cancelFlag, parentDir, keywords, recursively, status, files);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::MyDriveCreateFolder(const std::wstring& parentDir, const std::wstring& folderName, RmRepoFile& folder)
{
	if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
		return RESULT(ERROR_EXPIRED_HANDLE);
	}

	RmRestStatus status;
	Result res = _restClient.MyDriveCreateFolder(_rmserver.GetUrl(), parentDir, folderName, status, folder);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::MyDriveDeleteFile(const std::wstring& path, _Out_opt_ __int64* pUsage)
{
	if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
		return RESULT(ERROR_EXPIRED_HANDLE);
	}

	RmRestStatus status;
	__int64 newUsage = 0;
	Result res = _restClient.MyDriveDeleteFile(_rmserver.GetUrl(), path, status, newUsage);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	if (pUsage) {
		*pUsage = newUsage;
	}
	return RESULT(0);
}

Result RmSession::MyDriveDownloadFile(const std::wstring& source, const std::wstring& target, _In_opt_ bool* cancelFlag, const ULONG requestedLength)
{
	if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
		return RESULT(ERROR_EXPIRED_HANDLE);
	}

	RmRestStatus status;
	RmRepoFile source_file;
	Result res = MyDriveGetFileInfo(source, source_file);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	res = MyDriveDownloadFile(source_file, target, cancelFlag, requestedLength);
	if (!res)
		return res;

	return RESULT(0);
}

Result RmSession::MyDriveDownloadFile(const RmRepoFile& source, const std::wstring& target, _In_opt_ bool* cancelFlag, const ULONG requestedLength)
{
	if (source.IsFolder())
		return RESULT(ERROR_DIRECTORY_NOT_SUPPORTED);

	Result res = _restClient.MyDriveDownloadFile(_rmserver.GetUrl(), cancelFlag, source, target, requestedLength);
	if (!res)
		return res;

	return RESULT(0);
}

Result RmSession::MyDriveUploadFile(const std::wstring& source, const std::wstring& target, RmRepoFile& newFile, _In_opt_ bool* cancelFlag, _Out_opt_ RmMyDriveUsage* newUsage)
{
	RmRestStatus status;
	Result res = _restClient.MyDriveUploadFile(_rmserver.GetUrl(), cancelFlag, source, target, status, newFile, newUsage);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::MyDriveGetFolderMetadata(
	const std::wstring& folderId,
	__int64 lastModifiedTime,
	_In_opt_ bool* cancelFlag,
	std::vector<RmRepoFile>& files)
{
	RmRestStatus status;
	Result res = _restClient.MyDriveGetFolderMetadata(_rmserver.GetUrl(), cancelFlag, folderId, lastModifiedTime, status, files);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}