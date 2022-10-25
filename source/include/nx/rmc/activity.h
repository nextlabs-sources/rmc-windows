

#ifndef __NXRMC_ACTIVITY_H__
#define __NXRMC_ACTIVITY_H__


#include <string>
#include <nx\common\rapidjson.h>

namespace NX {

	class RmActivityRecord
	{
	public:
		RmActivityRecord();
		RmActivityRecord(const std::wstring& duid,
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
						 __int64 accessTime = 0);
		RmActivityRecord(const RmActivityRecord& rhs);
		~RmActivityRecord();

		RmActivityRecord& operator = (const RmActivityRecord& rhs);

		std::wstring serialize() const;

		inline const std::wstring& GetUserId() const { return _userId; }
		inline const std::wstring& GetDuid() const { return _duid; }
		inline const std::wstring& GetOwnerId() const { return _ownerId; }
		inline const std::wstring& GetDeviceId() const { return _deviceId; }
		inline const std::wstring& GetRepositoryId() const { return _repositoryId; }
		inline const std::wstring& GetFileId() const { return _fileId; }
		inline const std::wstring& GetFileName() const { return _fileName; }
		inline const std::wstring& GetFilePath() const { return _filePath; }
		inline const std::wstring& GetAppName() const { return _appName; }
		inline const std::wstring& GetAppPath() const { return _appPath; }
		inline const std::wstring& GetAppPublisher() const { return _appPublisher; }
		inline const std::wstring& GetActivityData() const { return _activityData; }
		inline int GetOperation() const { return _operation; }
		inline int GetDeviceType() const { return _deviceType; }
		inline int GetAccessResult() const { return _accessResult; }
		inline int GetAccountType() const { return _accountType; }
		inline __int64 GetAccessTime() const { return _accessTime; }

	private:
		std::wstring	_duid;
		std::wstring	_ownerId;
		std::wstring	_userId;
		std::wstring	_deviceId;
		std::wstring	_repositoryId;
		std::wstring	_fileId;
		std::wstring	_fileName;
		std::wstring	_filePath;
		std::wstring	_appName;
		std::wstring	_appPath;
		std::wstring	_appPublisher;
		std::wstring	_activityData;
		int				_operation;
		int				_deviceType; //  platformId
		int				_accessResult;
		int				_accountType;
		__int64			_accessTime;

		friend class RmSession;
	};

	typedef enum ActivitiesOrderBy {
		ActivitiesOrderbyAccessTime = 0,
		ActivitiesOrderbyAccessResult = 1
	} ActivitiesOrderBy;

	typedef enum ActivitiesSearchField {
		SearchNone = 0,
		SearchEmail = 1,
		SearchOperation = 2,
		SearchDeviceId = 4
	} ActivitiesSearchField;

	class RmFileActivityRecord
	{
	public:
		RmFileActivityRecord();
		RmFileActivityRecord(const RmFileActivityRecord& rhs);
		~RmFileActivityRecord();

		RmFileActivityRecord& operator = (const RmFileActivityRecord& rhs);

		static RmFileActivityRecord CreateFromJsonObject(const NX::rapidjson::JsonObject* object);

		inline bool Empty() const { return (_email.empty() || _operation.empty()); }
		inline const std::wstring& GetEmail() const { return _email; }
		inline const std::wstring& GetOperation() const { return _operation; }
		inline const std::wstring& GetDeviceId() const { return _deviceId; }
		inline const std::wstring& GetDeviceType() const { return _deviceType; }
		inline const std::wstring& GetAccessResult() const { return _accessResult; }
		inline const std::wstring& GetAccountType() const { return _accountType; }
		inline __int64 GetAccessTime() const { return _accessTime; }

	private:
		std::wstring	_email;
		std::wstring	_operation;
		std::wstring	_deviceType;
		std::wstring	_deviceId;
		std::wstring	_accessResult;
		std::wstring	_accountType;
		__int64			_accessTime;
	};

	class RmActivityLog
	{
	public:
		RmActivityLog();
		RmActivityLog(RmSession* pSession);
		~RmActivityLog();

		Result Open(const std::wstring& cacheFile);
		void Close(BOOL cleanBeforeClose);

		inline bool IsOpened() const { return (INVALID_HANDLE_VALUE != _h); }

		Result QueryActivities(const std::wstring& duid, std::vector<RmFileActivityRecord>& records);
		Result PostActivities();

		Result LogProtect();
		Result LogShare();
		Result LogChangeRecipients();
		Result LogView();
		Result LogPrint();
		Result LogDownload();
		Result LogEdit();
		Result LogDecrypt();

	protected:
		Result ReadAllRecords(std::string& records);
		Result WriteData(const std::string& record);

	private:
		HANDLE	_h;
		CRITICAL_SECTION _lock;
		RmSession* _session;
	};

}


#endif
