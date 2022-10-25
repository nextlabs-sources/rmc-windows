
#include "stdafx.h"

#include <nx\rmc\session.h>
#include <nx\rmc\securefile.h>

#include <nx\common\stringex.h>
#include <nx\common\rapidjson.h>
#include <nx\common\time.h>
#include <nx\rest\uri.h>

using namespace NX;


Result RmSession::MyVaultGetFirstPageFiles(RmFinder& finder, _In_opt_ bool* cancelFlag, std::vector<RmMyVaultFile>& files)
{
	RmRestStatus status;
	__int64 totalRecords = 0;

	files.clear();

	finder.ResetPageId();
	Result res = _restClient.MyVaultListFiles(_rmserver.GetUrl(), cancelFlag,
		finder.GetPageId(),
		finder.GetPageSize(),
		finder.GetSortType(),
		finder.GetSearchKeyword(),
		finder.GetFilter(),
		status,
		&totalRecords,
		files);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	finder.SetTotalRecords(totalRecords);
	return RESULT(0);
}

Result RmSession::MyVaultGetNextPageFiles(RmFinder& finder, _In_opt_ bool* cancelFlag, std::vector<RmMyVaultFile>& files)
{
	files.clear();

	if (finder.NoMoreData())
		return RESULT(ERROR_NO_MORE_ITEMS);

	RmRestStatus status;
	__int64 totalRecords = 0;
	Result res = _restClient.MyVaultListFiles(_rmserver.GetUrl(), cancelFlag,
		finder.NextPage(),
		finder.GetPageSize(),
		finder.GetSortType(),
		finder.GetSearchKeyword(),
		finder.GetFilter(),
		status,
		NULL,
		files);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	return RESULT(0);
}

Result RmSession::MyVaultGetFileInfo(const std::wstring& duid, const std::wstring& path, RmMyVaultFileMetadata& metadata)
{
	RmRestStatus status;
	Result res = _restClient.MyVaultGetFileMetadata(_rmserver.GetUrl(),
		duid,
		path,
		status,
		metadata);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	return RESULT(0);
}

Result RmSession::MyVaultDeleteFile(const std::wstring& duid, const std::wstring& path)
{
	RmRestStatus status;
	Result res = _restClient.MyVaultDeleteFile(_rmserver.GetUrl(),
		duid,
		path,
		status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	return RESULT(0);
}

Result RmSession::MyVaultDownloadFile(const std::wstring& source, const std::wstring& target, bool failIfExists, _In_opt_ bool* cancelFlag, const ULONG requestedLength)
{
	if (failIfExists && INVALID_FILE_ATTRIBUTES != GetFileAttributesW(target.c_str())) {
		return RESULT(ERROR_FILE_EXISTS);
	}

	RmRestStatus status;
	Result res = _restClient.MyVaultDownloadFile(_rmserver.GetUrl(), cancelFlag, requestedLength, source, target, 0, 0, false, status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::MyVaultDownloadFileForViewer(const std::wstring& source, const std::wstring& target, _In_opt_ bool* cancelFlag)
{
	RmRestStatus status;
	Result res = _restClient.MyVaultDownloadFile(_rmserver.GetUrl(), cancelFlag, 0, source, target, 0, 0, true, status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::MyVaultAddFileFromRepository(const std::wstring& source,
	const std::wstring& repoId,
	const std::wstring& repoName,
	const std::wstring& repoType,
	const std::wstring& repoFilePath,
	const std::wstring& repoFileDisplayPath,
	RmMyVaultUploadResult& newFile)
{
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(source.c_str()))
		return RESULT(ERROR_FILE_NOT_FOUND);

	RmRestStatus status;
	Result res = _restClient.MyVaultUploadFile(_rmserver.GetUrl(),
		NULL,
		source,
		repoFilePath,
		repoFileDisplayPath,
		repoId,
		repoName,
		repoType,
		status,
		newFile);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::MyVaultAddFileFromLocal(const std::wstring& source, const std::wstring& displayName, RmMyVaultUploadResult& newFile, _In_opt_ bool* cancelFlag)
{
	if (INVALID_FILE_ATTRIBUTES == GetFileAttributesW(source.c_str()))
		return RESULT(ERROR_FILE_NOT_FOUND);

	RmRestStatus status;
	Result res = _restClient.MyVaultUploadFile(_rmserver.GetUrl(), cancelFlag, source, L"", displayName, L"", L"Local", L"Local", status, newFile);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::MyVaultGetFolderMetadata(
	__int64 lastModifiedTime,
	_In_opt_ bool* cancelFlag,
	std::vector<RmRepoFile>& files)
{
	RmRestStatus status;
	Result res = _restClient.MyDriveGetFolderMetadata(_rmserver.GetUrl(), cancelFlag, L"/nxl_myvault_nxl/", lastModifiedTime, status, files);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::MyVaultRevokeShare(const std::wstring& duid)
{
	RmRestStatus status;
	Result res = _restClient.ShareRevoke(_rmserver.GetUrl(), duid, status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::MyVaultUpdateShareRecipients(const std::wstring& duid,
	const std::vector<std::wstring>& recipientsToAdd,
	const std::vector<std::wstring>& recipientsToRemove,
	std::vector<std::wstring>& recipientsAdded,
	std::vector<std::wstring>& recipientsRemoved
	)
{
	RmRestStatus status;
	Result res = _restClient.ShareChangeRecipients(_rmserver.GetUrl(),
		duid,
		recipientsToAdd,
		recipientsToRemove,
		status,
		recipientsAdded,
		recipientsRemoved);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::MyVaultShareFile(const std::wstring& membershipId,
	const RmRepository& mydrive,
	const std::wstring& fileId,
	const std::wstring& fileName,
	const std::wstring& filePath,
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
	return MyVaultShareFile(membershipId, mydrive, fileId, fileName, filePath, permissions, metadata, tags, recipients, unixExpireTime, sr);
}

Result RmSession::MyVaultShareFile(const std::wstring& membershipId,
	const RmRepository& mydrive,
	const std::wstring& fileId,
	const std::wstring& fileName,
	const std::wstring& filePath,
	ULONGLONG permissions,
	const NXL::FileMeta& metadata,
	const NXL::FileTags& tags,
	std::vector<std::wstring>& recipients,
	_In_ __int64 unixExpireTime,
	RmFileShareResult& sr)
{
	RmRestStatus status;
	Result res = _restClient.ShareRepositoryFile(_rmserver.GetUrl(),
		membershipId,
		recipients,
		permissions,
		unixExpireTime,
		mydrive.GetId(),
		fileName,
		filePath,
		fileId,
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

std::wstring RmSession::MyVaultBuildFileName(const std::wstring& fileName)
{
	static const std::wstring timestampFmt(L"-YYYY-MM-DD-HH-MM-SS");

	std::wstring tmpFileName(fileName);
	bool isOrigNXL = false;

	if (NX::iend_with<wchar_t>(tmpFileName, L".nxl")) {
		isOrigNXL = true;
		tmpFileName = tmpFileName.substr(0, tmpFileName.length() - 4);
	}
	
	std::wstring::size_type pos = tmpFileName.rfind(L'.');
	const std::wstring origFileExt((pos == std::wstring::npos) ? L"" : tmpFileName.substr(pos));
	tmpFileName = tmpFileName.substr(0, tmpFileName.length() - origFileExt.length());

	// check if file has time-stamp: "-2017-03-23-10-05-49"
	if (tmpFileName.length() > timestampFmt.length()) {
		const wchar_t* p = tmpFileName.c_str() + (tmpFileName.length() - timestampFmt.length());
		if (p[0] == L'-'
			&& p[5] == L'-'
			&& p[8] == L'-'
			&& p[11] == L'-'
			&& p[14] == L'-'
			&& p[17] == L'-'
			&& NX::isdigit<wchar_t>(p[1]) && NX::isdigit<wchar_t>(p[2]) && NX::isdigit<wchar_t>(p[3]) && NX::isdigit<wchar_t>(p[4])	// Year
			&& NX::isdigit<wchar_t>(p[6]) && NX::isdigit<wchar_t>(p[7])		// Month
			&& NX::isdigit<wchar_t>(p[9]) && NX::isdigit<wchar_t>(p[10])	// Day
			&& NX::isdigit<wchar_t>(p[12]) && NX::isdigit<wchar_t>(p[13])	// Hour
			&& NX::isdigit<wchar_t>(p[15]) && NX::isdigit<wchar_t>(p[16])	// Minute
			&& NX::isdigit<wchar_t>(p[18]) && NX::isdigit<wchar_t>(p[19])	// Second
			)
		{
			// Already has a time-stamp
			tmpFileName = tmpFileName.substr(0, tmpFileName.length() - timestampFmt.length());
		}
	}

	SYSTEMTIME st = { 0 };
	GetSystemTime(&st);
	tmpFileName.append(NX::FormatString(L"-%04d-%02d-%02d-%02d-%02d-%02d", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond));
	tmpFileName.append(origFileExt);
	tmpFileName.append(L".nxl");

	return tmpFileName;
}