

#include"stdafx.h"


#include <nx\common\result.h>
#include <nx\common\string.h>
#include <nx\common\time.h>

#include <nx\rmc\activity.h>

using namespace NX;


RmActivityRecord::RmActivityRecord()
	: _operation(0), _deviceType(0), _accessResult(0), _accountType(0), _accessTime(0)
{
}

RmActivityRecord::RmActivityRecord(const std::wstring& duid,
								   const std::wstring& ownerId,
								   const std::wstring& userId,
								   const std::wstring& deviceId,
								   const std::wstring& repositoryId,
								   const std::wstring& fileId,
								   const std::wstring& fileName,
								   const std::wstring& filePath,
								   const std::wstring& appName,
								   const std::wstring& appPath,
								   const std::wstring& appPublisher,
								   const std::wstring& activityData,
								   int operation,
								   int deviceType, //  platformId
								   int accessResult,
								   int accountType,
								   __int64 accessTime)
	: _duid(duid),
	  _ownerId(ownerId),
	  _userId(userId),
	  _deviceId(deviceId),
	  _repositoryId(repositoryId),
	  _fileId(fileId),
	  _fileName(fileName),
	  _filePath(filePath),
	  _appName(appName),
	  _appPath(appPath),
	  _appPublisher(appPublisher),
	  _activityData(activityData),
	  _operation(operation),
	  _deviceType(deviceType),
	  _accessResult(accessResult),
	  _accountType(accountType),
	  _accessTime(accessTime)
{
	if (0 == _accessTime) {
		NX::time::datetime dt = NX::time::datetime::current_time();
		_accessTime = dt.to_java_time();
	}
}

RmActivityRecord::RmActivityRecord(const RmActivityRecord& rhs)
	: _userId(rhs._userId),
	_duid(rhs._duid),
	_ownerId(rhs._ownerId),
	_deviceId(rhs._deviceId),
	_repositoryId(rhs._repositoryId),
	_fileId(rhs._fileId),
	_fileName(rhs._fileName),
	_filePath(rhs._filePath),
	_appName(rhs._appName),
	_appPath(rhs._appPath),
	_appPublisher(rhs._appPublisher),
	_activityData(rhs._activityData),
	_operation(rhs._operation),
	_deviceType(rhs._deviceType),
	_accessResult(rhs._accessResult),
	_accountType(rhs._accountType),
	_accessTime(rhs._accessTime)
{
}

RmActivityRecord::~RmActivityRecord()
{
}

RmActivityRecord& RmActivityRecord::operator = (const RmActivityRecord& rhs)
{
	if (this != &rhs) {
		_userId = rhs._userId;
		_duid = rhs._duid;
		_ownerId = rhs._ownerId;
		_deviceId = rhs._deviceId;
		_repositoryId = rhs._repositoryId;
		_fileId = rhs._fileId;
		_fileName = rhs._fileName;
		_filePath = rhs._filePath;
		_appName = rhs._appName;
		_appPath = rhs._appPath;
		_appPublisher = rhs._appPublisher;
		_activityData = rhs._activityData;
		_operation = rhs._operation;
		_deviceType = rhs._deviceType;
		_accessResult = rhs._accessResult;
		_accountType = rhs._accountType;
		_accessTime = rhs._accessTime;
	}
	return *this;
}

std::wstring RmActivityRecord::serialize() const
{
	// There are total 16 fields
	// Reference: https://bitbucket.org/nxtlbs-devops/rightsmanagement-wiki/wiki/RMS/RESTful%20API/Log%20REST%20API

	std::wstring csv(L"\"");
	// Field 0: DUID
	csv += GetDuid();
	// Field 1:  Owner Id
	csv += L"\",\""; csv += GetOwnerId();
	// Field 2:  User Id
	csv += L"\",\""; csv += GetUserId();
	// Field 3:  Operation
	csv += L"\",\""; csv += NX::itos<wchar_t>(GetOperation());
	// Field 4:  Device Id
	csv += L"\",\""; csv += GetDeviceId();
	// Field 5:  Device Type
	csv += L"\",\""; csv += NX::itos<wchar_t>(GetDeviceType());
	// Field 6:  Repository Id
	csv += L"\",\""; csv += GetRepositoryId();
	// Field 7:  File pathId
	csv += L"\",\""; csv += GetFileId();
	// Field 8:  File Name
	csv += L"\",\""; csv += GetFileName();
	// Field 9:  File Display Path
	csv += L"\",\""; csv += GetFilePath();
	// Field 10: Application Name
	csv += L"\",\""; csv += GetAppName();
	// Field 11: Application Path
	csv += L"\",\""; csv += GetAppPath();
	// Field 12: Application Publisher
	csv += L"\",\""; csv += GetAppPublisher();
	// Field 13: Access Result
	csv += L"\",\""; csv += NX::itos<wchar_t>(GetAccessResult());
	// Field 14: Access Time
	csv += L"\",\""; csv += NX::i64tos<wchar_t>(GetAccessTime());
	// Field 15: Activity Data
	csv += L"\",\""; csv += GetActivityData();
	// Field 16: Account Type
	csv += L"\",\""; csv += NX::itos<wchar_t>(GetAccountType());
	csv += L"\"";

	return csv;
}



RmFileActivityRecord::RmFileActivityRecord() : _accessTime(0)
{
}

RmFileActivityRecord::RmFileActivityRecord(const RmFileActivityRecord& rhs)
	: _email(rhs._email),
	_operation(rhs._operation),
	_deviceId(rhs._deviceId),
	_deviceType(rhs._deviceType),
	_accessResult(rhs._accessResult),
	_accountType(rhs._accountType),
	_accessTime(rhs._accessTime)
{
}

RmFileActivityRecord::~RmFileActivityRecord()
{
}

RmFileActivityRecord& RmFileActivityRecord::operator = (const RmFileActivityRecord& rhs)
{
	if (this != &rhs) {
		_email = rhs._email;
		_operation = rhs._operation;
		_deviceId = rhs._deviceId;
		_deviceType = rhs._deviceType;
		_accessResult = rhs._accessResult;
		_accountType = rhs._accountType;
		_accessTime = rhs._accessTime;
	}
	return *this;
}

RmFileActivityRecord RmFileActivityRecord::CreateFromJsonObject(const NX::rapidjson::JsonObject* object)
{
	RmFileActivityRecord record;

	if (object == NULL)
		return record;

	const NX::rapidjson::JsonValue* pv = object->at(L"email");
	if (pv && pv->IsString())
		record._email = pv->AsString()->GetString();
	pv = object->at(L"operation");
	if (pv && pv->IsString())
		record._operation = pv->AsString()->GetString();
	pv = object->at(L"deviceType");
	if (pv && pv->IsString())
		record._deviceType = pv->AsString()->GetString();
	pv = object->at(L"deviceId");
	if (pv && pv->IsString())
		record._deviceId = pv->AsString()->GetString();
	pv = object->at(L"accessResult");
	if (pv && pv->IsString())
		record._accessResult = pv->AsString()->GetString();
	pv = object->at(L"accountType");
	if (pv && pv->IsString())
		record._accountType = pv->AsString()->GetString();
	pv = object->at(L"accessTime");
	if (pv && pv->IsNumber())
		record._accessTime = pv->AsNumber()->ToInt64();

	return record;
}


RmActivityLog::RmActivityLog() : _h(INVALID_HANDLE_VALUE), _session(NULL)
{
	InitializeCriticalSection(&_lock);
}

RmActivityLog::RmActivityLog(RmSession* pSession) : _h(INVALID_HANDLE_VALUE), _session(pSession)
{
	InitializeCriticalSection(&_lock);
}

RmActivityLog::~RmActivityLog()
{
	Close(FALSE);
	DeleteCriticalSection(&_lock);
}

Result RmActivityLog::Open(const std::wstring& cacheFile)
{
	if (INVALID_HANDLE_VALUE != _h)
		return RESULT(ERROR_ACCESS_DENIED);

	_h = CreateFileW(cacheFile.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == _h)
		return RESULT(GetLastError());

	// Succeed
	SetFilePointer(_h, 0, NULL, FILE_END);
	return RESULT(0);
}

void RmActivityLog::Close(BOOL cleanBeforeClose)
{
	::EnterCriticalSection(&_lock);
	if (INVALID_HANDLE_VALUE != _h) {
		if (cleanBeforeClose) {
			(VOID)PostActivities();
		}
		CloseHandle(_h);
		_h = INVALID_HANDLE_VALUE;
	}
	::LeaveCriticalSection(&_lock);
}

Result RmActivityLog::ReadAllRecords(std::string& records)
{
	DWORD dwBytesRead = 0;
	Result res = RESULT(0);

	::EnterCriticalSection(&_lock);
	do {

		if (_h == INVALID_HANDLE_VALUE) {
			res = RESULT(ERROR_INVALID_HANDLE);
			break;
		}

		DWORD dwSize = GetFileSize(_h, NULL);
		if (INVALID_FILE_SIZE == dwSize) {
			res = RESULT(GetLastError());
			break;
		}
		if (0 == dwSize) {
			res = RESULT(ERROR_NO_MORE_ITEMS);
			break;
		}

		std::vector<char> buf;
		buf.resize(dwSize, 0);
		if (!::ReadFile(_h, buf.data(), dwSize, &dwBytesRead, NULL)) {
			res = RESULT(GetLastError());
			break;
		}
		if (dwBytesRead < dwSize)
			buf.resize(dwBytesRead);

		records = std::string(buf.begin(), buf.end());

	} while (FALSE);
	::LeaveCriticalSection(&_lock);
	if (!res)
		return res;

	return RESULT(0);
}

Result RmActivityLog::WriteData(const std::string& record)
{
	DWORD dwBytesWritten = 0;
	static const char cLE = '\n';
	Result res = RESULT(0);

	::EnterCriticalSection(&_lock);
	do {

		if (_h == INVALID_HANDLE_VALUE) {
			res = RESULT(ERROR_INVALID_HANDLE);
			break;
		}

		if (record.empty()) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		if (!::WriteFile(_h, record.c_str(), (DWORD)record.length(), &dwBytesWritten, NULL)) {
			res = RESULT(GetLastError());
			break;
		}

		if (record[record.length() - 1] != cLE)
			::WriteFile(_h, &cLE, 1, &dwBytesWritten, NULL);

	} while (FALSE);
	::LeaveCriticalSection(&_lock);
	if (!res)
		return res;

	return RESULT(0);
}

Result RmActivityLog::QueryActivities(const std::wstring& duid, std::vector<RmFileActivityRecord>& records)
{
	return RESULT(ERROR_NOT_SUPPORTED);
}

Result RmActivityLog::PostActivities()
{
	return RESULT(ERROR_NOT_SUPPORTED);
}

Result RmActivityLog::LogProtect()
{
	return RESULT(ERROR_NOT_SUPPORTED);
}

Result RmActivityLog::LogShare()
{
	return RESULT(ERROR_NOT_SUPPORTED);
}

Result RmActivityLog::LogChangeRecipients()
{
	return RESULT(ERROR_NOT_SUPPORTED);
}

Result RmActivityLog::LogView()
{
	return RESULT(ERROR_NOT_SUPPORTED);
}

Result RmActivityLog::LogPrint()
{
	return RESULT(ERROR_NOT_SUPPORTED);
}

Result RmActivityLog::LogDownload()
{
	return RESULT(ERROR_NOT_SUPPORTED);
}

Result RmActivityLog::LogEdit()
{
	return RESULT(ERROR_NOT_SUPPORTED);
}

Result RmActivityLog::LogDecrypt()
{
	return RESULT(ERROR_NOT_SUPPORTED);
}
