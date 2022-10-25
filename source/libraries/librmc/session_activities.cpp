
#include "stdafx.h"

#include <nx\rmc\session.h>
#include <nx\rmc\securefile.h>

#include <nx\common\stringex.h>
#include <nx\common\rapidjson.h>
#include <nx\winutil\host.h>
#include <nx\nxl\nxlrights.h>
#include <nx\rest\uri.h>

using namespace NX;


#define ACCESS_DENIED		0
#define ACCESS_ALLOWED		1

#define PERSONAL_ACCOUNT	0
#define PROJECT_ACCOUNT		1


Result RmSession::FetchAllActivityLogs(_Out_ std::wstring& fileName,
	_Out_ std::vector<RmFileActivityRecord>& records,
	const std::wstring& duid,
	ActivitiesOrderBy orderBy,
	bool descending)
{
	std::vector<RmFileActivityRecord> tmpRecords;
	std::wstring strFileName;
	__int64 start = 0;
	const __int64 pageSize = 100;

	Result res = FetchActivityLogsEx(fileName, tmpRecords, duid, start, pageSize, orderBy, descending);
	if (!res)
		return res;

	do {

		for (auto item : tmpRecords)
			records.push_back(item);

		if (tmpRecords.size() < pageSize)
			break;

		tmpRecords.clear();
		start += pageSize;

	} while (FetchActivityLogsEx(strFileName, tmpRecords, duid, start, 100, orderBy, descending));

	return RESULT(0);
}

Result RmSession::FetchActivityLogsEx(_Out_ std::wstring& fileName,
	_Out_ std::vector<RmFileActivityRecord>& records,
	const std::wstring& duid,
	__int64 start,
	__int64 count,
	ActivitiesOrderBy orderBy,
	bool descending)
{
	return SearchActivityLogs(fileName, records, duid, start, count, SearchNone, L"", orderBy, descending);
}

Result RmSession::SearchActivityLogs(_Out_ std::wstring& fileName,
	_Out_ std::vector<RmFileActivityRecord>& records,
	const std::wstring& duid,
	__int64 start,
	__int64 count,
	ActivitiesSearchField searchField,
	const std::wstring& keyWord,
	ActivitiesOrderBy orderBy,
	bool descending)
{
	if (duid.empty())
		return RESULT(ERROR_INVALID_PARAMETER);

	std::wstring strSearchField;
	std::wstring strOrderBy;
	std::wstring fileDuid;

	if (!keyWord.empty()) {
		switch (searchField)
		{
		case SearchEmail:
			strSearchField = L"email";
			break;
		case SearchOperation:
			strSearchField = L"operation";
			break;
		case SearchDeviceId:
			strSearchField = L"deviceId";
			break;
		default:
			break;
		}
	}

	switch (orderBy)
	{
	case ActivitiesOrderbyAccessResult:
		strOrderBy = L"accessResult";
		break;
	case ActivitiesOrderbyAccessTime:
	default:
		break;
	}
	
	RmRestStatus status;
	Result res = _restClient.FetchActivities(_rmserver.GetUrl(), duid, start, count, strSearchField, keyWord, strOrderBy, descending, status, fileName, fileDuid, records);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}


static std::wstring PrepareDeviceId()
{
	const std::wstring& id = NX::win::GetLocalHostInfo().GetFQDN().empty() ? NX::win::GetLocalHostInfo().GetHostName() : NX::win::GetLocalHostInfo().GetFQDN();
	std::wstring encodedId;
	std::for_each(id.begin(), id.end(), [&](const wchar_t c) {
		if (c > 31 && c < 127) {
			encodedId.append(&c, 1);
		}
		else {
			wchar_t wz[16] = { 0 };
			swprintf_s(wz, L"\\u%04X", c);
			encodedId.append(wz);
		}
	});
	return encodedId;
}

Result RmSession::LogActivityProtect(const std::wstring& duid,
	const std::wstring& owner,
	const std::wstring& repoId,
	const std::wstring& fileId,
	const std::wstring& fileName,
	const std::wstring& filePath)
{
	const std::string& s = CreateActivityRecord(duid, owner, ACTIVITY_OPERATION_ID_PROTECT, repoId, fileId, fileName, filePath, ACCESS_ALLOWED, L"");
	RmRestStatus status;

	Result res = _restClient.LogActivities(_rmserver.GetUrl(), std::vector<UCHAR>(s.begin(), s.end()), status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::LogActivityPrint(const std::wstring& duid,
	const std::wstring& owner,
	const std::wstring& repoId,
	const std::wstring& fileId,
	const std::wstring& fileName,
	const std::wstring& filePath)
{
	const std::string& s = CreateActivityRecord(duid, owner, ACTIVITY_OPERATION_ID_PRINT, repoId, fileId, fileName, filePath, ACCESS_ALLOWED, L"");
	RmRestStatus status;

	Result res = _restClient.LogActivities(_rmserver.GetUrl(), std::vector<UCHAR>(s.begin(), s.end()), status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::LogActivityShare(bool succeed,
	bool reshare,
	const std::wstring& duid,
	const std::wstring& owner,
	const std::wstring& repoId,
	const std::wstring& fileId,
	const std::wstring& fileName,
	const std::wstring& filePath)
{
	const std::string& s = CreateActivityRecord(duid, owner, reshare ? ACTIVITY_OPERATION_ID_RESHARE : ACTIVITY_OPERATION_ID_SHARE, repoId, fileId, fileName, filePath, succeed ? ACCESS_ALLOWED : ACCESS_DENIED, L"");
	RmRestStatus status;

	Result res = _restClient.LogActivities(_rmserver.GetUrl(), std::vector<UCHAR>(s.begin(), s.end()), status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::LogActivityDownload(bool succeed,
	const std::wstring& duid,
	const std::wstring& owner,
	const std::wstring& repoId,
	const std::wstring& fileId,
	const std::wstring& fileName,
	const std::wstring& filePath)
{
	const std::string& s = CreateActivityRecord(duid, owner, ACTIVITY_OPERATION_ID_DOWNLOAD, repoId, fileId, fileName, filePath, succeed ? ACCESS_ALLOWED : ACCESS_DENIED, L"");
	RmRestStatus status;

	Result res = _restClient.LogActivities(_rmserver.GetUrl(), std::vector<UCHAR>(s.begin(), s.end()), status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

Result RmSession::LogActivityView(bool succeed,
	const std::wstring& duid,
	const std::wstring& owner,
	const std::wstring& repoId,
	const std::wstring& fileId,
	const std::wstring& fileName,
	const std::wstring& filePath)
{
	const std::string& s = CreateActivityRecord(duid, owner, ACTIVITY_OPERATION_ID_VIEW, repoId, fileId, fileName, filePath, succeed ? ACCESS_ALLOWED : ACCESS_DENIED, L"");
	RmRestStatus status;

	Result res = _restClient.LogActivities(_rmserver.GetUrl(), std::vector<UCHAR>(s.begin(), s.end()), status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	return RESULT(0);
}

std::string RmSession::CreateActivityRecord(const std::wstring& duid,
											const std::wstring& owner,
											int operation,
											const std::wstring& repoId,
											const std::wstring& fileId,
											const std::wstring& fileName,
											const std::wstring& filePath,
											const int accessResult,
											const std::wstring& extraData,
											__int64 accessTime
											)
{
	static std::wstring strImage;
	if (strImage.empty()) {
		GetModuleFileNameW(NULL, wstring_buffer(strImage, MAX_PATH), MAX_PATH);
	}

	RmActivityRecord record(duid,
		owner,
		GetCurrentUser().GetId(),
		PrepareDeviceId(),
		repoId,
		fileId,
		fileName,
		filePath,
		GetProduct().GetName(),
		strImage,
		GetProduct().GetPublisher(),
		extraData,
		operation,
		0, //  platformId
		accessResult,
		0, //  accountType,
		accessTime);

	const std::wstring& ws = record.serialize();
	return NX::conv::utf16toutf8(ws);
}