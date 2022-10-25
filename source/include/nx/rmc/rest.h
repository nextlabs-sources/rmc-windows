

#ifndef __LIBRMC_REST_H__
#define __LIBRMC_REST_H__


#include <nx\common\result.h>
#include <nx\rest\http_client.h>
#include <nx\nxl\nxlfile.h>
#include <nx\common\rapidjson.h>

#include <string>
#include <iostream>
#include <streambuf>
#include <unordered_map>


namespace NX {

    // Forward class
    class RmTenantPreference;
    class RmUser;
    class RmMyDriveUsage;
    class RmRepository;
    class RmRepoFile;
    class RmUpgradeInfo;
    class RmFileActivityRecord;
    class RmHeartbeatObject;
    class RmRepoMarkedFile;
    class RmMyVaultFile;
    class RmMyVaultFileMetadata;
    class RmMyVaultUploadResult;
    class RmProject;
    class RmProjectFile;
    class RmProjectFileMetadata;
    class RmProjectInvitation;
    class RmProjectMember;
	class RmSharedWithMeFile;

	typedef enum MYVAULTRECORDTYPE {
		MyVaultAllFile = 0,
		MyVaultActiveTransaction,
		MyVaultShared,
		MyVaultProtected
	} MYVAULTRECORDTYPE;


    class RmRestCommonParams
    {
    public:
        RmRestCommonParams();
        RmRestCommonParams(const std::wstring clientId);
        RmRestCommonParams(const RmRestCommonParams& rhs);
        ~RmRestCommonParams();

        RmRestCommonParams& operator = (const RmRestCommonParams& rhs);

        inline const std::wstring& GetTenant() const { return _tenant; }
        inline const std::wstring& GetClientId() const { return _clientId; }
        inline const std::wstring& GetPlatformId() const { return _platformId; }
        inline const std::wstring& GetDeviceId() const { return _deviceId; }
        inline const std::wstring& GetDeviceType() const { return _deviceType; }
        inline const std::wstring& GetUserId() const { return _userId; }
        inline const std::wstring& GetUserName() const { return _userName; }
        inline const std::wstring& GetTicket() const { return _ticket; }

        inline void SetTenant(const std::wstring& v) { _tenant = v; }
        inline void SetClientId(const std::wstring& v) { _clientId = v; }
        inline void SetPlatformId(const std::wstring& v) { _platformId = v; }
        inline void SetDeviceId(const std::wstring& v) { _deviceId = v; }
        inline void SetUserId(const std::wstring& v) { _userId = v; }
        inline void SetUserName(const std::wstring& v) { _userName = v; }
        inline void SetTicket(const std::wstring& v) { _ticket = v; }

    private:
        std::wstring    _tenant;        // tenant
        std::wstring    _clientId;      // client_id
        std::wstring    _platformId;    // platformId
        std::wstring    _deviceId;      // deviceId
        std::wstring    _deviceType;    // deviceType
        std::wstring    _userId;        // userId
        std::wstring    _userName;      // userName
        std::wstring    _ticket;        // ticket
    };


    class RmRestStatus
    {
    public:
        RmRestStatus();
        RmRestStatus(const RmRestStatus& rhs);
        ~RmRestStatus();

        inline USHORT GetStatus() const { return _status; }
        inline const std::wstring& GetPhrase() const { return _phrase; }
		
		operator bool() const { return (_status >= 200 && _status < 300); }

        RmRestStatus& operator = (const RmRestStatus& rhs);

    private:
        USHORT  _status;
        std::wstring _phrase;
        friend class RmRestClient;
    };


	class RmFileShareResult
	{
	public:
		RmFileShareResult();
		RmFileShareResult(const RmFileShareResult& rhs);
		virtual ~RmFileShareResult();

		RmFileShareResult& operator = (const RmFileShareResult& rhs);

		void Clear();

		inline bool Empty() const { return (_duid.empty() || _path.empty()); }
		inline const std::wstring& GetName() const { return _name; }
		inline const std::wstring& GetDuid() const { return _duid; }
		inline const std::wstring& GetPath() const { return _path; }
		inline const std::wstring& GetTransactionId() const { return _transactionId; }

	private:
		std::wstring    _name;
		std::wstring    _duid;
		std::wstring    _path;
		std::wstring    _transactionId;
		friend class RmRestClient;
		friend class RmSession;
	};

	class RmSharedWithMeFile
	{
	public:
		RmSharedWithMeFile();
		RmSharedWithMeFile(const RmSharedWithMeFile& rhs);
		virtual ~RmSharedWithMeFile();

		RmSharedWithMeFile& operator = (const RmSharedWithMeFile& rhs);

		void Clear();

		inline bool Empty() const { return _duid.empty(); }
		inline const std::wstring& GetDuid() const { return _duid; }
		inline const std::wstring& GetName() const { return _name; }
		inline const std::wstring& GetSharedBy() const { return _sharedBy; }
		inline const std::wstring& GetSharedLink() const { return _sharedLink; }
		inline __int64 GetSharedDate() const { return _sharedDate; }
		inline __int64 GetSize() const { return _size; }

	private:
		std::wstring _duid;
		std::wstring _name;
		std::wstring _sharedBy;
		std::wstring _sharedLink;
		__int64	_sharedDate;
		__int64 _size;
		friend class RmRestClient;
	};


	class RmMyVaultUploadResult
	{
	public:
		RmMyVaultUploadResult();
		RmMyVaultUploadResult(const RmMyVaultUploadResult& rhs);
		virtual ~RmMyVaultUploadResult();

		RmMyVaultUploadResult& operator = (const RmMyVaultUploadResult& rhs);

		void Clear();
		
		inline bool Empty() const { return (_duid.empty() || _path.empty()); }
		inline const std::wstring& GetName() const { return _name; }
		inline const std::wstring& GetDuid() const { return _duid; }
		inline const std::wstring& GetPath() const { return _path; }
		inline const std::wstring& GetDisplayPath() const { return _displayPath; }
		inline __int64 GetLastModifiedTime() const { return _lastModifiedTime; }
		inline __int64 GetSize() const { return _size; }

	private:
		std::wstring    _name;
		std::wstring    _duid;
		std::wstring    _path;
		std::wstring    _displayPath;
		__int64         _lastModifiedTime;
		__int64         _size;
        friend class RmRestClient;
	};

	class RmUploadBuffer : public std::streambuf
	{
	public:
		explicit RmUploadBuffer(const std::string& apiInput, const std::wstring& file);
		virtual ~RmUploadBuffer();

		virtual void clear();

		inline size_t GetTotalDataLength() const { return _end; }
		inline size_t GetRemainDataLength() const { return (_end - _current); }

	public:
		typedef typename traits_type::int_type int_type;
		typedef typename traits_type::pos_type pos_type;
		typedef typename traits_type::off_type off_type;

	protected:
		std::string InitHeader(const std::string& apiInput, const std::wstring& file);
		size_t InitSize(const std::wstring& file);

	private:
		//virtual int_type overflow(int_type _Meta = traits_type::eof());
		virtual int_type pbackfail(int_type _Meta = traits_type::eof());
		virtual int_type underflow();
		virtual int_type uflow();
		//virtual pos_type seekoff(off_type _Off, std::ios_base::seekdir _Way);
		//virtual pos_type seekpos(pos_type _Pos);
		//virtual RmUploadBuffer*setbuf(char*_Buffer, std::streamsize _Count);
		//virtual int sync();
		//virtual void imbue(const std::locale& _Loc);
		virtual std::streamsize showmanyc();
		virtual std::streamsize xsgetn(char* s, std::streamsize n);
		//virtual std::streamsize xsputn(const char* s, std::streamsize n);

		// copy ctor and assignment not implemented;
		// copying not allowed
		RmUploadBuffer(const RmUploadBuffer&) = delete;
		RmUploadBuffer& operator= (const RmUploadBuffer&) = delete;

	private:
		HANDLE				_h;
		const std::string	_header;
		size_t				_current;
		const size_t		_end;
		bool				_feof;
	};

	class RmSimpleUploadBuffer : public std::streambuf
	{
	public:
		explicit RmSimpleUploadBuffer(const std::wstring& file);
		virtual ~RmSimpleUploadBuffer();

		virtual void clear();

		inline size_t GetTotalDataLength() const { return _end; }
		inline size_t GetRemainDataLength() const { return (_end - _current); }

	public:
		typedef typename traits_type::int_type int_type;
		typedef typename traits_type::pos_type pos_type;
		typedef typename traits_type::off_type off_type;

	protected:
		size_t InitSize(const std::wstring& file);

	private:
		//virtual int_type overflow(int_type _Meta = traits_type::eof());
		virtual int_type pbackfail(int_type _Meta = traits_type::eof());
		virtual int_type underflow();
		virtual int_type uflow();
		//virtual pos_type seekoff(off_type _Off, std::ios_base::seekdir _Way);
		//virtual pos_type seekpos(pos_type _Pos);
		//virtual RmSimpleUploadBuffer *setbuf(char*_Buffer, std::streamsize _Count);
		//virtual int sync();
		//virtual void imbue(const std::locale& _Loc);
		virtual std::streamsize showmanyc();
		virtual std::streamsize xsgetn(char* s, std::streamsize n);
		//virtual std::streamsize xsputn(const char* s, std::streamsize n);

		// copy ctor and assignment not implemented;
		// copying not allowed
		RmSimpleUploadBuffer(const RmSimpleUploadBuffer&) = delete;
		RmSimpleUploadBuffer& operator= (const RmSimpleUploadBuffer&) = delete;

	private:
		HANDLE				_h;
		size_t				_current;
		const size_t		_end;
		bool				_feof;
	};

	class RmUploadRequest : public NX::REST::http::Request
	{
	public:
		RmUploadRequest(const RmRestCommonParams& commonParams, const std::wstring& url, const std::string& apiInput, const std::wstring& file, bool* canceled);
		virtual ~RmUploadRequest();

		virtual ULONG GetRemainBodyLength()
		{
			return (ULONG)_buf.GetRemainDataLength();
		}
		virtual std::istream& GetBodyStream() { return _is; }

		virtual  bool IsUpload() const { return true; }
		
	protected:
		std::istream	_is;
		RmUploadBuffer	_buf;
	};

    class RmRestClient : public REST::http::Client
    {
    public:
        RmRestClient();
        virtual ~RmRestClient();

        Result Init(const std::wstring& clientId);
        void Clear();

        NX::REST::http::Connection* Connect(const std::wstring& server_url, Result& res);

        // Router
        Result RouterQuery(const std::wstring& router_url, const std::wstring& tenant_name, RmRestStatus& status, std::wstring& rms_url);

        // Login Service
        Result LoginWithNativeAccount(const std::wstring& rms_url, const std::wstring& name, const std::wstring& password, RmRestStatus& status, RmUser& user);
        Result LoginWithGoogleAccount(const std::wstring& token, RmRestStatus& status);
        Result LoginWithFacebookAccount(const std::wstring& token, RmRestStatus& status);

        // Tenant Service
        Result TenantQueryPreference(const std::wstring& rms_url,
            RmRestStatus& status,
            RmTenantPreference& preference);
        Result TenantQueryPreference(NX::REST::http::Connection* conn,
            RmRestStatus& status,
            RmTenantPreference& preference);

        // User Service
        Result UserGetCaptcha(const std::wstring& rms_url,
            RmRestStatus& status,
			std::vector<UCHAR>& captcha,
			std::wstring& nonce);
        Result UserGetCaptcha(NX::REST::http::Connection* conn,
            RmRestStatus& status,
			std::vector<UCHAR>& captcha,
			std::wstring& nonce);
        Result UserQueryBasicProfile(const std::wstring& rms_url,
            RmRestStatus& status,
            RmUser& profile);
        Result UserQueryBasicProfile(NX::REST::http::Connection* conn,
            RmRestStatus& status,
            RmUser& profile);
        Result UserQueryProfile(const std::wstring& rms_url,
            RmRestStatus& status,
            RmUser& profile);
        Result UserQueryProfile(NX::REST::http::Connection* conn,
            RmRestStatus& status,
            RmUser& profile);
        Result UserUpdateProfile(const std::wstring& rms_url,
			const std::wstring& displayName,
			const std::wstring& securityMode,
			const std::wstring& profilePicture,
            RmRestStatus& status);
        Result UserUpdateProfile(NX::REST::http::Connection* conn,
			const std::wstring& displayName,
			const std::wstring& securityMode,
			const std::wstring& profilePicture,
            RmRestStatus& status);
        Result UserResetPassword(const std::wstring& rms_url,
            const std::wstring& email,
            const std::wstring& nonce,
            const std::wstring& captcha,
            RmRestStatus& status);
        Result UserResetPassword(NX::REST::http::Connection* conn,
            const std::wstring& email,
            const std::wstring& nonce,
            const std::wstring& captcha,
            RmRestStatus& status);
        Result UserChangePassword(const std::wstring& rms_url,
            const std::wstring& oldPassword,
            const std::wstring& newPassword,
            RmRestStatus& status);
        Result UserChangePassword(NX::REST::http::Connection* conn,
			const std::wstring& oldPassword,
			const std::wstring& newPassword,
            RmRestStatus& status);

        // Membership Service
        Result MembershipGetCertificates(const std::wstring& rms_url,
			const std::wstring& membership,
            RmRestStatus& status,
            std::vector<std::wstring>& certificates);
        Result MembershipGetCertificates(NX::REST::http::Connection* conn,
			const std::wstring& membership,
            RmRestStatus& status,
            std::vector<std::wstring>& certificates);

        // File Token Service
        Result TokenGenerate(const std::wstring& rms_url,
            const std::wstring& membership,
            const std::wstring& agreement,
            int count,
            RmRestStatus& status,
            std::vector<NXL::FileToken>& tokens);
        Result TokenGenerate(NX::REST::http::Connection* conn,
            const std::wstring& membership,
            const std::wstring& agreement,
            int count,
            RmRestStatus& status,
            std::vector<NXL::FileToken>& tokens);
        Result TokenRetrieve(const std::wstring& rms_url,
            const std::wstring& membership,
            const std::wstring& agreement,
            const std::wstring& duid,
			unsigned int ml,
            RmRestStatus& status,
            NXL::FileToken& token);
        Result TokenRetrieve(NX::REST::http::Connection* conn,
            const std::wstring& membership,
            const std::wstring& agreement,
            const std::wstring& duid,
			unsigned int ml,
            RmRestStatus& status,
            NXL::FileToken& token);

        // Sharing Service
        Result ShareRepositoryFile(const std::wstring& rms_url,
            const std::wstring& membership,
            const std::vector<std::wstring>& recipients,
            const ULONGLONG permission,
            const __int64 expireTime,
            const std::wstring& repoId,
            const std::wstring& fileName,
            const std::wstring& filePath,
            const std::wstring& filePathId,
            const NXL::FileMeta& fileMetadata,
            const NXL::FileTags& fileTags,
            RmRestStatus& status,
			RmFileShareResult& sr);
        Result ShareRepositoryFile(NX::REST::http::Connection* conn,
            const std::wstring& membership,
            const std::vector<std::wstring>& recipients,
            const ULONGLONG permission,
            const __int64 expireTime,
            const std::wstring& repoId,
            const std::wstring& fileName,
            const std::wstring& filePath,
            const std::wstring& filePathId,
			const NXL::FileMeta& fileMetadata,
			const NXL::FileTags& fileTags,
            RmRestStatus& status,
			RmFileShareResult& sr);
        Result ShareLocalFile(const std::wstring& rms_url,
            const std::wstring& membership,
            const std::vector<std::wstring>& recipients,
            const ULONGLONG permission,
            const __int64 expireTime,
            const std::wstring& filePath,
            const std::wstring& displayPath,
			const NXL::FileMeta& fileMetadata,
			const NXL::FileTags& fileTags,
            RmRestStatus& status,
			RmFileShareResult& sr,
			_In_opt_ bool* cancelFlag);
        Result ShareLocalFile(NX::REST::http::Connection* conn,
            const std::wstring& membership,
            const std::vector<std::wstring>& recipients,
            const ULONGLONG permission,
            const __int64 expireTime,
            const std::wstring& filePath,
			const std::wstring& displayPath,
			const NXL::FileMeta& fileMetadata,
			const NXL::FileTags& fileTags,
            RmRestStatus& status,
			RmFileShareResult& sr,
			_In_opt_ bool* cancelFlag);
        Result ShareChangeRecipients(const std::wstring& rms_url,
            const std::wstring& duid,
            const std::vector<std::wstring>& addRecipients,
            const std::vector<std::wstring>& removeRecipients,
            RmRestStatus& status,
            std::vector<std::wstring>& addedRecipients,
            std::vector<std::wstring>& removedRecipients);
        Result ShareChangeRecipients(NX::REST::http::Connection* conn,
            const std::wstring& duid,
            const std::vector<std::wstring>& addRecipients,
            const std::vector<std::wstring>& removeRecipients,
            RmRestStatus& status,
            std::vector<std::wstring>& addedRecipients,
            std::vector<std::wstring>& removedRecipients);
        Result ShareRevoke(const std::wstring& rms_url,
            const std::wstring& duid,
            RmRestStatus& status);
        Result ShareRevoke(NX::REST::http::Connection* conn,
            const std::wstring& duid,
            RmRestStatus& status);

		Result ListSharedWithMeFiles(const std::wstring& rms_url,
			_In_opt_ bool* cancelFlag,
			__int64 pageId,
			__int64 pageSize,
			const std::wstring& orderBy,
			const std::wstring& keyWords,
			_Out_opt_ __int64* totalRecords,
			RmRestStatus& status,
			std::vector<RmSharedWithMeFile>& files);
		Result ListSharedWithMeFiles(NX::REST::http::Connection* conn,
			_In_opt_ bool* cancelFlag,
			__int64 pageId,
			__int64 pageSize,
			const std::wstring& orderBy,
			const std::wstring& keyWords,
			_Out_opt_ __int64* totalRecords,
			RmRestStatus& status,
			std::vector<RmSharedWithMeFile>& files);

        // Log Service
        Result LogActivities(const std::wstring& rms_url,
            const std::vector<unsigned char>& logdata,
            RmRestStatus& status);
        Result LogActivities(NX::REST::http::Connection* conn,
            const std::vector<unsigned char>& logdata,
            RmRestStatus& status);
        Result FetchActivities(const std::wstring& rms_url,
            const std::wstring& duid,
            __int64 startPos,
            __int64 count,
            const std::wstring& searchField,
            const std::wstring& searchText,
            const std::wstring& orderByField,
            bool orderByReverse,
			RmRestStatus& status,
			std::wstring& fileName,
			std::wstring& fileDuid,
			std::vector<RmFileActivityRecord>& records);
        Result FetchActivities(NX::REST::http::Connection* conn,
			const std::wstring& duid,
			__int64 startPos,
			__int64 count,
			const std::wstring& searchField,
			const std::wstring& searchText,
			const std::wstring& orderByField,
			bool orderByReverse,
			RmRestStatus& status,
			std::wstring& fileName,
			std::wstring& fileDuid,
			std::vector<RmFileActivityRecord>& records);

        // Heart beat service
        Result HeartBeat(const std::wstring& rms_url,
			const std::vector<std::pair<std::wstring, std::wstring>>& objects,
			RmRestStatus& status,
			std::vector<RmHeartbeatObject>& returnedObjects);
        Result HeartBeat(NX::REST::http::Connection* conn,
			const std::vector<std::pair<std::wstring, std::wstring>>& objects,
            RmRestStatus& status,
            std::vector<RmHeartbeatObject>& returnedObjects);

        // Check Update Service
        Result CheckSoftwareUpdate(const std::wstring& rms_url,
            const std::wstring& tenant,
			const std::wstring& currentVersion,
            RmRestStatus& status,
			RmUpgradeInfo& info);
        Result CheckSoftwareUpdate(NX::REST::http::Connection* conn,
            const std::wstring& tenant,
			const std::wstring& currentVersion,
            RmRestStatus& status,
            RmUpgradeInfo& info);

        // Repository Service
        Result GetRepoAuthUrl(const std::wstring& rms_url,
            const std::wstring& repoName,
            const std::wstring& repoType,
            const std::wstring& siteUrl,
            RmRestStatus& status,
            std::wstring& url);
        Result GetRepoAuthUrl(NX::REST::http::Connection* conn,
            const std::wstring& repoName,
            const std::wstring& repoType,
            const std::wstring& siteUrl,
            RmRestStatus& status,
            std::wstring& url);
        Result GetRepositories(const std::wstring& rms_url,
            RmRestStatus& status,
            std::vector<RmRepository>& repos,
            bool& isFullCopy);
        Result GetRepositories(NX::REST::http::Connection* conn,
            RmRestStatus& status,
            std::vector<RmRepository>& repos,
            bool& isFullCopy);
        Result AddRepository(const std::wstring& rms_url,
            const std::wstring& repoName,
            const std::wstring& repoType,
            bool isRepoShared,
            const std::wstring& accountName,
            const std::wstring& accountId,
			const std::wstring& accountToken,
            RmRestStatus& status,
            std::wstring& repoId);
        Result AddRepository(NX::REST::http::Connection* conn,
            const std::wstring& repoName,
            const std::wstring& repoType,
            bool isRepoShared,
            const std::wstring& accountName,
            const std::wstring& accountId,
			const std::wstring& accountToken,
            RmRestStatus& status,
            std::wstring& repoId);
        Result UpdateRepository(const std::wstring& rms_url,
            const std::wstring& repoId,
            const std::wstring& repoName,
			const std::wstring& repoToken,
            RmRestStatus& status);
        Result UpdateRepository(NX::REST::http::Connection* conn,
            const std::wstring& repoId,
            const std::wstring& repoName,
			const std::wstring& repoToken,
            RmRestStatus& status);
        Result RemoveRepository(const std::wstring& rms_url,
            const std::wstring& repoId,
            RmRestStatus& status);
        Result RemoveRepository(NX::REST::http::Connection* conn,
            const std::wstring& repoId,
            RmRestStatus& status);
        Result GetAccessToken(const std::wstring& rms_url,
            const std::wstring& repoId,
            RmRestStatus& status,
            std::wstring& accessToken);
        Result GetAccessToken(NX::REST::http::Connection* conn,
            const std::wstring& repoId,
            RmRestStatus& status,
            std::wstring& accessToken);

    private:
        typedef struct {
            std::wstring    gdId;
            bool            isRoot;
        } GdParentInfo;

        typedef struct {
            std::wstring                name;
            bool                        folder;
            __int64                     size;
            __int64                     lastModifiedTime;
            std::vector<GdParentInfo>   parents;
        } GdRepoFile;

        void RmRestClient::GoogleDriveListFilesProcessParents(const std::unordered_map<std::wstring, GdRepoFile>& gdIdFileMap,
            const std::wstring& descendantGdId,
            const GdRepoFile& descendantGdRepoFile,
            const std::wstring& childGdId,
            const std::wstring& childToDescendentId,
            const std::wstring& childToDescendentPath,
            std::vector<std::pair<RmRepoFile, std::wstring>>& filesAndGdIds);

    public:
        Result RmRestClient::GoogleDriveGetFileMimeType(
            _In_opt_ bool* cancelFlag,
            const std::wstring& accessToken,
            const std::wstring& fileGdId,
            RmRestStatus& status,
            std::wstring &mimeType);
        Result RmRestClient::GoogleDriveGetFileMimeType(NX::REST::http::Connection* conn,
            _In_opt_ bool* cancelFlag,
            const std::wstring& accessToken,
            const std::wstring& fileGdId,
            RmRestStatus& status,
            std::wstring &mimeType);
        Result GoogleDriveListFiles(_In_opt_ bool* cancelFlag, const std::wstring& accessToken,
            const std::wstring& keywords,
            RmRestStatus& status,
            std::vector<std::pair<RmRepoFile, std::wstring>>& filesAndGdIds);
        Result GoogleDriveListFiles(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
            const std::wstring& accessToken,
            const std::wstring& keywords,
            RmRestStatus& status,
            std::vector<std::pair<RmRepoFile, std::wstring>>& filesAndGdIds);
        Result GoogleDriveCreateFolder(const std::wstring& accessToken,
            const std::pair<RmRepoFile, std::wstring>& parentFolderFileAndGdId,
            const std::wstring& folderName,
            RmRestStatus& status,
            RmRepoFile& folder);
        Result GoogleDriveCreateFolder(NX::REST::http::Connection* conn,
            const std::wstring& accessToken,
            const std::pair<RmRepoFile, std::wstring>& parentFolderFileAndGdId,
            const std::wstring& folderName,
            RmRestStatus& status,
            RmRepoFile& folder);
        Result GoogleDriveDeleteFile(const std::wstring& accessToken,
            const std::wstring& fileGdId,
            RmRestStatus& status);
        Result GoogleDriveDeleteFile(NX::REST::http::Connection* conn,
            const std::wstring& accessToken,
            const std::wstring& fileGdId,
            RmRestStatus& status);
        Result GoogleDriveDownloadFile(_In_opt_ bool* cancelFlag,
			const ULONG requestedLength,
			const std::wstring& accessToken,
            const std::wstring& sourceFileGdId,
            const std::wstring& target,
            RmRestStatus& status);
        Result GoogleDriveDownloadFile(NX::REST::http::Connection* conn,
			_In_opt_ bool* cancelFlag,
			const ULONG requestedLength,
            const std::wstring& accessToken,
            const std::wstring& sourceFileGdId,
            const std::wstring& target,
            RmRestStatus& status);
        Result RmRestClient::GoogleDriveExportFile(_In_opt_ bool* cancelFlag,
            const ULONG requestedLength,
            const std::wstring& accessToken,
            const std::wstring& sourceFileGdId,
            const std::wstring& target,
            const std::wstring& targetMimeType,
            RmRestStatus& status);
        Result RmRestClient::GoogleDriveExportFile(NX::REST::http::Connection* conn,
            _In_opt_ bool* cancelFlag,
            const ULONG requestedLength,
            const std::wstring& accessToken,
            const std::wstring& sourceFileGdId,
            const std::wstring& target,
            const std::wstring& targetMimeType,
            RmRestStatus& status);
        Result GoogleDriveUploadFile(_In_opt_ bool* cancelFlag,
			const std::wstring& accessToken,
            const std::wstring& sourceFile,
            const std::pair<RmRepoFile, std::wstring>& targetFolderFileAndGdId,
            const std::wstring& targetFileName,
            RmRestStatus& status,
            RmRepoFile& newFile);
        Result GoogleDriveUploadFile(NX::REST::http::Connection* conn,
			_In_opt_ bool* cancelFlag,
            const std::wstring& accessToken,
            const std::wstring& sourceFile,
            const std::pair<RmRepoFile, std::wstring>& targetFolderFileAndGdId,
            const std::wstring& targetFileName,
            RmRestStatus& status,
            RmRepoFile& newFile);

    private:
        void OneDriveMetadataToRepoFile(NX::rapidjson::PCJSONOBJECT fileObject,
            const std::wstring parentFolderId,
            RmRepoFile& repoFile);

    public:
        Result OneDriveGetFileInfo(const std::wstring& accessToken,
            const std::wstring& fileId,
            RmRestStatus& status,
            RmRepoFile& file);
        Result OneDriveGetFileInfo(NX::REST::http::Connection* conn,
            const std::wstring& accessToken,
            const std::wstring& fileId,
            RmRestStatus& status,
            RmRepoFile& file);
        Result OneDriveListOrSearchFiles(_In_opt_ bool* cancelFlag, const std::wstring& accessToken,
            const std::wstring& folderId,
            const std::wstring& keywords,
            RmRestStatus& status,
            std::vector<RmRepoFile>& files);
        Result OneDriveListOrSearchFiles(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
            const std::wstring& accessToken,
            const std::wstring& folderId,
            const std::wstring& keywords,
            RmRestStatus& status,
            std::vector<RmRepoFile>& files);
        Result OneDriveCreateFolder(const std::wstring& accessToken,
            const std::wstring& parentFolderId,
            const std::wstring& folderName,
            RmRestStatus& status,
            RmRepoFile& folder);
        Result OneDriveCreateFolder(NX::REST::http::Connection* conn,
            const std::wstring& accessToken,
            const std::wstring& parentFolderId,
            const std::wstring& folderName,
            RmRestStatus& status,
            RmRepoFile& folder);
        Result OneDriveDelete(const std::wstring& accessToken,
            const std::wstring& itemId,
            RmRestStatus& status);
        Result OneDriveDelete(NX::REST::http::Connection* conn,
            const std::wstring& accessToken,
            const std::wstring& itemId,
            RmRestStatus& status);
        Result OneDriveDownloadFile(
			_In_opt_ bool* cancelFlag,
			const ULONG requestedLength,
			const std::wstring& accessToken,
            const std::wstring& fileId,
            const std::wstring& localFilePath,
            RmRestStatus& status);
        Result OneDriveDownloadFile(NX::REST::http::Connection* conn,
			_In_opt_ bool* cancelFlag,
			const ULONG requestedLength,
            const std::wstring& accessToken,
            const std::wstring& fileId,
            const std::wstring& localFilePath,
            RmRestStatus& status);
        Result OneDriveUploadFile(
			_In_opt_ bool* cancelFlag, 
			const std::wstring& accessToken,
            const std::wstring& file,
            const std::wstring& repoPath,
            RmRestStatus& status,
            RmRepoFile& newFile);
        Result OneDriveUploadFile(NX::REST::http::Connection* conn,
			_In_opt_ bool* cancelFlag,
            const std::wstring& accessToken,
            const std::wstring& file,
            const std::wstring& repoPath,
            RmRestStatus& status,
            RmRepoFile& newFile);

    private:
        void DropboxFileMetadataToRepoFile(NX::rapidjson::PCJSONOBJECT fileObject,
            RmRepoFile& repoFile);

    public:
        Result DropboxListFiles(_In_opt_ bool* cancelFlag, const std::wstring& accessToken,
            const std::wstring& path,
            RmRestStatus& status,
            std::vector<RmRepoFile>& files);
        Result DropboxListFiles(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
            const std::wstring& accessToken,
            const std::wstring& path,
            RmRestStatus& status,
            std::vector<RmRepoFile>& files);
        Result DropboxSearchFiles(_In_opt_ bool* cancelFlag, const std::wstring& accessToken,
            const std::wstring& path,
            const std::wstring& keywords,
            RmRestStatus& status,
            std::vector<RmRepoFile>& files);
        Result DropboxSearchFiles(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
            const std::wstring& accessToken,
            const std::wstring& path,
            const std::wstring& keywords,
            RmRestStatus& status,
            std::vector<RmRepoFile>& files);
        Result DropboxCreateFolder(const std::wstring& accessToken,
            const std::wstring& parentDir,
            const std::wstring& folderName,
            RmRestStatus& status,
            RmRepoFile& folder);
        Result DropboxCreateFolder(NX::REST::http::Connection* conn,
            const std::wstring& accessToken,
            const std::wstring& parentDir,
            const std::wstring& folderName,
            RmRestStatus& status,
            RmRepoFile& folder);
        Result DropboxDeleteFile(const std::wstring& accessToken,
            const std::wstring& repoPath,
            RmRestStatus& status);
        Result DropboxDeleteFile(NX::REST::http::Connection* conn,
            const std::wstring& accessToken,
            const std::wstring& repoPath,
            RmRestStatus& status);
        Result DropboxDownloadFile(
			_In_opt_ bool* cancelFlag, 
			const ULONG requestedLength,
			const std::wstring& accessToken,
            const std::wstring& source,
            const std::wstring& target,
            RmRestStatus& status);
        Result DropboxDownloadFile(NX::REST::http::Connection* conn,
			_In_opt_ bool* cancelFlag,
			const ULONG requestedLength,
            const std::wstring& accessToken,
            const std::wstring& source,
            const std::wstring& target,
            RmRestStatus& status);
        Result DropboxUploadFile(
			_In_opt_ bool* cancelFlag, 
			const std::wstring& accessToken,
            const std::wstring& file,
            const std::wstring& repoPath,
            RmRestStatus& status,
            RmRepoFile& repoFile);
        Result DropboxUploadFile(NX::REST::http::Connection* conn,
			_In_opt_ bool* cancelFlag,
            const std::wstring& accessToken,
            const std::wstring& file,
            const std::wstring& repoPath,
            RmRestStatus& status,
            RmRepoFile& newFile);

    public:
        Result SharePointOnlineListCollections(const std::wstring& siteUrl,
            const std::wstring& accessToken,
            RmRestStatus& status,
            std::vector<RmRepoFile>& files,
            std::vector<std::wstring> *serverRelUrls);
        Result SharePointOnlineListCollections(NX::REST::http::Connection* conn,
            const std::wstring& accessToken,
            RmRestStatus& status,
            std::vector<RmRepoFile>& files,
            std::vector<std::wstring> *serverRelUrls);
        Result SharePointOnlineListOrSearchFilesInCollection(const std::wstring& siteUrl, _In_opt_ bool* cancelFlag,
            const std::wstring& accessToken,
            const std::wstring& collection,
            const std::wstring& keywords,
            RmRestStatus& status,
            std::vector<RmRepoFile>& files,
            std::vector<__int64>& ids,
            std::vector<std::wstring>& sourceUrls,
            std::vector<std::wstring> *serverRelUrls);
        Result SharePointOnlineListOrSearchFilesInCollection(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
            const std::wstring& accessToken,
            const std::wstring& collection,
            const std::wstring& keywords,
            RmRestStatus& status,
            std::vector<RmRepoFile>& files,
            std::vector<__int64>& ids,
            std::vector<std::wstring>& sourceUrls,
            std::vector<std::wstring> *serverRelUrls);
        Result SharePointOnlineCreateFolder(const std::wstring& siteUrl,
            const std::wstring& accessToken,
            const RmRepoFile& repoParentDir,
            const std::wstring& parentDirServerRelUrl,
            const std::wstring& folderName,
            RmRestStatus& status,
            RmRepoFile& folder);
        Result SharePointOnlineCreateFolder(NX::REST::http::Connection* conn,
            const std::wstring& accessToken,
            const RmRepoFile& repoParentDir,
            const std::wstring& parentDirServerRelUrl,
            const std::wstring& folderName,
            RmRestStatus& status,
            RmRepoFile& folder);
        Result SharePointOnlineDeleteFile(const std::wstring& siteUrl,
            const std::wstring& accessToken,
            const std::wstring& collection,
            __int64 fileId,
            RmRestStatus& status);
        Result SharePointOnlineDeleteFile(NX::REST::http::Connection* conn,
            const std::wstring& accessToken,
            const std::wstring& collection,
            __int64 fileId,
            RmRestStatus& status);
        Result SharePointOnlineDownloadFile(const std::wstring& siteUrl,
			_In_opt_ bool* cancelFlag,
			const ULONG requestedLength,
            const std::wstring& accessToken,
            const std::wstring& sourceFileSourceUrl,
            const std::wstring& target,
            RmRestStatus& status);
        Result SharePointOnlineDownloadFile(NX::REST::http::Connection* conn,
			_In_opt_ bool* cancelFlag,
			const ULONG requestedLength,
            const std::wstring& accessToken,
            const std::wstring& sourceFileSourceUrl,
            const std::wstring& target,
            RmRestStatus& status);
        Result SharePointOnlineGetFolderUniqId(const std::wstring& siteUrl,
            const std::wstring& accessToken,
            const std::wstring& serverRelUrl,
            RmRestStatus& status,
            std::wstring& uniqId);
        Result SharePointOnlineGetFolderUniqId(NX::REST::http::Connection* conn,
            const std::wstring& accessToken,
            const std::wstring& serverRelUrl,
            RmRestStatus& status,
            std::wstring& uniqId);
        Result SharePointOnlineUploadFile(const std::wstring& siteUrl,
			_In_opt_ bool* cancelFlag,
            const std::wstring& accessToken,
            const std::wstring& file,
            const RmRepoFile& repoParentFolder,
            const std::wstring& repoParentFolderUniqId,
            const std::wstring& repoFileName,
            RmRestStatus& status,
            RmRepoFile& repoFile);
        Result SharePointOnlineUploadFile(NX::REST::http::Connection* conn,
			_In_opt_ bool* cancelFlag,
            const std::wstring& accessToken,
            const std::wstring& file,
            const RmRepoFile& repoParentFolder,
            const std::wstring& repoParentFolderUniqId,
            const std::wstring& repoFileName,
            RmRestStatus& status,
            RmRepoFile& newFile);

        // Repository Favorite File Service
        Result RepoGetAllFavoriteFiles(const std::wstring& rms_url,
            RmRestStatus& status,
            std::vector<std::pair<std::wstring, std::vector<RmRepoMarkedFile>>>& favoriteFiles);
        Result RepoGetAllFavoriteFiles(NX::REST::http::Connection* conn,
            RmRestStatus& status,
			std::vector<std::pair<std::wstring, std::vector<RmRepoMarkedFile>>>& favoriteFiles);
        Result RepoMarkFavoriteFiles(const std::wstring& rms_url,
            const std::wstring& repoId,
			const std::wstring& parentFolderId,
            const std::vector<RmRepoMarkedFile>& files,
            RmRestStatus& status);
        Result RepoMarkFavoriteFiles(NX::REST::http::Connection* conn,
            const std::wstring& repoId,
			const std::wstring& parentFolderId,
            const std::vector<RmRepoMarkedFile>& files,
            RmRestStatus& status);
        Result RepoUnmarkFavoriteFiles(const std::wstring& rms_url,
            const std::wstring& repoId,
            const std::vector<RmRepoMarkedFile>& files,
            RmRestStatus& status);
        Result RepoUnmarkFavoriteFiles(NX::REST::http::Connection* conn,
            const std::wstring& repoId,
            const std::vector<RmRepoMarkedFile>& files,
            RmRestStatus& status);

        // MyDrive Service
        Result MyDriveGetUsage(const std::wstring& rms_url,
            RmRestStatus& status,
			RmMyDriveUsage& usage);
        Result MyDriveGetUsage(NX::REST::http::Connection* conn,
            RmRestStatus& status,
			RmMyDriveUsage& usage);
        Result MyDriveListFiles(const std::wstring& rms_url, _In_opt_ bool* cancelFlag,
            const std::wstring& folderId,
            RmRestStatus& status,
            std::vector<RmRepoFile>& files);
        Result MyDriveListFiles(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
            const std::wstring& folderId,
            RmRestStatus& status,
            std::vector<RmRepoFile>& files);
        Result MyDriveSearchFiles(const std::wstring& rms_url, _In_opt_ bool* cancelFlag,
			const std::wstring& parentDir,
            const std::wstring& keywords,
			bool recursively,
            RmRestStatus& status,
            std::vector<RmRepoFile>& files);
        Result MyDriveSearchFiles(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
			const std::wstring& parentDir,
            const std::wstring& keywords,
			bool recursively,
            RmRestStatus& status,
            std::vector<RmRepoFile>& files);
        Result MyDriveCreateFolder(const std::wstring& rms_url,
			const std::wstring& parentDir,
			const std::wstring& folderName,
			RmRestStatus& status,
			RmRepoFile& folder);
        Result MyDriveCreateFolder(NX::REST::http::Connection* conn,
			const std::wstring& parentDir,
			const std::wstring& folderName,
			RmRestStatus& status,
			RmRepoFile& folder);
        Result MyDriveDownloadFile(const std::wstring& rms_url,
			_In_opt_ bool* cancelFlag,
            const RmRepoFile& source,
            const std::wstring& target_path,
			const ULONG requestedLength);
        Result MyDriveDownloadFile(NX::REST::http::Connection* conn,
			_In_opt_ bool* cancelFlag,
            const RmRepoFile& source,
            const std::wstring& target_path,
			const ULONG requestedLength);
        Result MyDriveUploadFile(const std::wstring& rms_url,
			_In_opt_ bool* cancelFlag,
            const std::wstring& source_path,
            const std::wstring& target_path,
            RmRestStatus& status,
			RmRepoFile& newFile,
			_Out_opt_ RmMyDriveUsage* newUsage);
        Result MyDriveUploadFile(NX::REST::http::Connection* conn,
			_In_opt_ bool* cancelFlag,
            const std::wstring& source_path,
            const std::wstring& target_path,
			RmRestStatus& status,
			RmRepoFile& newFile,
			_Out_opt_ RmMyDriveUsage* newUsage);
        Result MyDriveDeleteFile(const std::wstring& rms_url,
            const std::wstring& path,
            RmRestStatus& status,
            __int64& usage);
        Result MyDriveDeleteFile(NX::REST::http::Connection* conn,
            const std::wstring& path,
            RmRestStatus& status,
            __int64& usage);
        Result MyDriveCreatePublicShareUrl(const std::wstring& rms_url,
            const std::wstring& path,
            RmRestStatus& status,
            std::wstring& public_url);
        Result MyDriveCreatePublicShareUrl(NX::REST::http::Connection* conn,
            const std::wstring& path,
            RmRestStatus& status,
            std::wstring& public_url);
        Result MyDriveCopyFile(const std::wstring& rms_url,
            const std::wstring& source_path,
            const std::wstring& target_path,
            RmRestStatus& status,
            __int64* usage);
        Result MyDriveCopyFile(NX::REST::http::Connection* conn,
            const std::wstring& source_path,
            const std::wstring& target_path,
            RmRestStatus& status,
            __int64* usage);
        Result MyDriveMoveFile(const std::wstring& rms_url,
            const std::wstring& source_path,
            const std::wstring& target_path,
            RmRestStatus& status);
        Result MyDriveMoveFile(NX::REST::http::Connection* conn,
            const std::wstring& source_path,
            const std::wstring& target_path,
            RmRestStatus& status);
        Result MyDriveGetFolderMetadata(const std::wstring& rms_url, _In_opt_ bool* cancelFlag,
            const std::wstring& folderId,
			__int64 lastModifiedTime,
            RmRestStatus& status,
			std::vector<RmRepoFile>& files);
        Result MyDriveGetFolderMetadata(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
			const std::wstring& folderId,
			__int64 lastModifiedTime,
            RmRestStatus& status,
			std::vector<RmRepoFile>& files);

        // MyVault Service
        Result MyVaultDeleteFile(const std::wstring& rms_url,
			const std::wstring& duid,
            const std::wstring& filePath,
            RmRestStatus& status);
        Result MyVaultDeleteFile(NX::REST::http::Connection* conn,
			const std::wstring& duid,
            const std::wstring& filePath,
            RmRestStatus& status);
        Result MyVaultListFiles(const std::wstring& rms_url, _In_opt_ bool* cancelFlag,
            __int64 pageId,
            __int64 pageSize,
            const std::wstring& orderBy,
            const std::wstring& searchKeyWords,
            const std::wstring& searchFilter,
            RmRestStatus& status,
			_Out_opt_ __int64* total,
			std::vector<RmMyVaultFile>& files);
        Result MyVaultListFiles(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
            __int64 pageId,
            __int64 pageSize,
            const std::wstring& orderBy,
            const std::wstring& searchKeyWords,
            const std::wstring& searchFilter,
            RmRestStatus& status,
			_Out_opt_ __int64* total,
			std::vector<RmMyVaultFile>& files);
        Result MyVaultGetFileMetadata(const std::wstring& rms_url,
			const std::wstring& duid,
            const std::wstring& filePath,
            RmRestStatus& status,
			RmMyVaultFileMetadata& metadata);
        Result MyVaultGetFileMetadata(NX::REST::http::Connection* conn,
			const std::wstring& duid,
            const std::wstring& filePath,
            RmRestStatus& status,
			RmMyVaultFileMetadata& metadata);
        Result MyVaultDownloadFile(const std::wstring& rms_url,
			_In_opt_ bool* cancelFlag,
			const ULONG requestedLength,
            const std::wstring& source_path,
            const std::wstring& target_path,
            __int64 start_pos,
            __int64 download_size,
            bool forViewerOnly,
            RmRestStatus& status);
        Result MyVaultDownloadFile(NX::REST::http::Connection* conn,
			_In_opt_ bool* cancelFlag,
			const ULONG requestedLength,
            const std::wstring& source_path,
            const std::wstring& target_path,
            __int64 start_pos,
            __int64 download_size,
            bool forViewerOnly,
            RmRestStatus& status);
        Result MyVaultUploadFile(const std::wstring& rms_url,
			_In_opt_ bool* cancelFlag,
			const std::wstring& file,
            const std::wstring& repoFilePath,
            const std::wstring& repoFilePathDisplay,
            const std::wstring& repoId,
            const std::wstring& repoName,
            const std::wstring& repoType,
            RmRestStatus& status,
			RmMyVaultUploadResult& result);
        Result MyVaultUploadFile(NX::REST::http::Connection* conn,
			_In_opt_ bool* cancelFlag,
			const std::wstring& file,
			const std::wstring& repoFilePath,
			const std::wstring& repoFilePathDisplay,
			const std::wstring& repoId,
			const std::wstring& repoName,
			const std::wstring& repoType,
            RmRestStatus& status,
			RmMyVaultUploadResult& result);

        // Project Service
        Result ProjectListProjects(const std::wstring& rms_url, bool* cancelFlag,
            bool ownedByMe,
			bool ownedByOther,
            RmRestStatus& status,
			std::vector<RmProject>& projects);
        Result ProjectListProjects(NX::REST::http::Connection* conn, bool* cancelFlag,
            bool ownedByMe,
			bool ownedByOther,
            RmRestStatus& status,
			std::vector<RmProject>& projects);
		Result ProjectListProjectsEx(const std::wstring& rms_url, bool* cancelFlag,
			__int64 pageId,
			__int64 pageSize,
			const std::wstring& orderBy,
			const std::wstring& searchValue,
			bool ownedByMe,
			bool ownedByOther,
			RmRestStatus& status,
			_Out_opt_ __int64* total,
			std::vector<RmProject>& projects);
		Result ProjectListProjectsEx(NX::REST::http::Connection* conn, bool* cancelFlag,
			__int64 pageId,
			__int64 pageSize,
			const std::wstring& orderBy,
			const std::wstring& searchValue,
			bool ownedByMe,
			bool ownedByOther,
			RmRestStatus& status,
			_Out_opt_ __int64* total,
			std::vector<RmProject>& projects);
        Result ProjectCreateProject(const std::wstring& rms_url,
            const std::wstring& projectName,
            const std::wstring& projectDescription,
			const std::wstring& inviteMsg,
            const std::vector<std::wstring>& inviteMembers,
            RmRestStatus& status,
            std::wstring& projectId);
        Result ProjectCreateProject(NX::REST::http::Connection* conn,
            const std::wstring& projectName,
            const std::wstring& projectDescription,
			const std::wstring& inviteMsg,
            const std::vector<std::wstring>& inviteMembers,
            RmRestStatus& status,
            std::wstring& projectId);
        Result ProjectGetProjectMetadata(const std::wstring& rms_url,
            const std::wstring& projectId,
            RmRestStatus& status,
			RmProject& project);
        Result ProjectGetProjectMetadata(NX::REST::http::Connection* conn,
            const std::wstring& projectId,
            RmRestStatus& status,
			RmProject& project);
        Result ProjectInvitePeople(const std::wstring& rms_url,
            const std::wstring& projectId,
            const std::vector<std::wstring>& inviteMembers,
            RmRestStatus& status,
			std::vector<std::wstring>& alreadyInvited,
			std::vector<std::wstring>& nowInvited,
			std::vector<std::wstring>& alreadyMembers);
        Result ProjectInvitePeople(NX::REST::http::Connection* conn,
            const std::wstring& projectId,
            const std::vector<std::wstring>& inviteMembers,
            RmRestStatus& status,
			std::vector<std::wstring>& alreadyInvited,
			std::vector<std::wstring>& nowInvited,
			std::vector<std::wstring>& alreadyMembers);
		Result ProjectAcceptInvitation(const std::wstring& rms_url,
			const std::wstring& invitationId,
			const std::wstring& invitationCode,
			RmRestStatus& status,
			std::wstring& projectId);
		Result ProjectAcceptInvitation(NX::REST::http::Connection* conn,
			const std::wstring& invitationId,
			const std::wstring& invitationCode,
			RmRestStatus& status,
			std::wstring& projectId);
		Result ProjectDeclineInvitation(const std::wstring& rms_url,
			const std::wstring& invitationId,
			const std::wstring& invitationCode,
			const std::wstring& reason,
			RmRestStatus& status);
		Result ProjectDeclineInvitation(NX::REST::http::Connection* conn,
			const std::wstring& invitationId,
			const std::wstring& invitationCode,
			const std::wstring& reason,
			RmRestStatus& status);
		Result ProjectSendInvitationReminder(const std::wstring& rms_url,
			const std::wstring& invitationId,
			RmRestStatus& status);
		Result ProjectSendInvitationReminder(NX::REST::http::Connection* conn,
			const std::wstring& invitationId,
			RmRestStatus& status);
		Result ProjectRevokeInvitation(const std::wstring& rms_url,
			const std::wstring& invitationId,
			RmRestStatus& status);
		Result ProjectRevokeInvitation(NX::REST::http::Connection* conn,
			const std::wstring& invitationId,
			RmRestStatus& status);
        Result ProjectListPendingInvitations(const std::wstring& rms_url,
            const std::wstring& projectId,
            __int64 pageId,
            __int64 pageSize,
            const std::wstring& orderBy,
            const std::wstring& searchField,
            const std::wstring& searchValue,
            RmRestStatus& status,
			_Out_opt_ __int64* total,
			std::vector<RmProjectInvitation>& invitations);
        Result ProjectListPendingInvitations(NX::REST::http::Connection* conn,
            const std::wstring& projectId,
            __int64 pageId,
            __int64 pageSize,
            const std::wstring& orderBy,
            const std::wstring& searchField,
            const std::wstring& searchValue,
            RmRestStatus& status,
			_Out_opt_ __int64* total,
			std::vector<RmProjectInvitation>& invitations);
		Result ProjectListUserPendingInvitations(const std::wstring& rms_url, _In_opt_ bool* cancelFlag,
			RmRestStatus& status,
			std::vector<RmProjectInvitation>& invitations);
		Result ProjectListUserPendingInvitations(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
			RmRestStatus& status,
			std::vector<RmProjectInvitation>& invitations);
		Result ProjectListFiles(const std::wstring& rms_url, _In_opt_ bool* cancelFlag,
			const std::wstring& projectId,
			__int64 pageId,
			__int64 pageSize,
			const std::wstring& orderBy,
			const std::wstring& parentPath,
			const std::wstring& keywords,
			RmRestStatus& status,
			_Out_opt_ int* totalFiles,
			_Out_opt_ __int64* usage,
			_Out_opt_ __int64* quota,
			_Out_opt_ __int64* lastUpdatedTime,
			std::vector<RmProjectFile>& files);
		Result ProjectListFiles(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
			const std::wstring& projectId,
			__int64 pageId,
			__int64 pageSize,
			const std::wstring& orderBy,
			const std::wstring& parentPath,
			const std::wstring& keywords,
			RmRestStatus& status,
			_Out_opt_ int* totalFiles,
			_Out_opt_ __int64* usage,
			_Out_opt_ __int64* quota,
			_Out_opt_ __int64* lastUpdatedTime,
			std::vector<RmProjectFile>& files);
        Result ProjectCreateFolder(const std::wstring& rms_url,
            const std::wstring& projectId,
			const std::wstring& parentDir,
			const std::wstring& folderName,
			bool autoRename,
			RmRestStatus& status,
			RmRepoFile& folder);
        Result ProjectCreateFolder(NX::REST::http::Connection* conn,
            const std::wstring& projectId,
			const std::wstring& parentDir,
			const std::wstring& folderName,
			bool autoRename,
			RmRestStatus& status,
			RmRepoFile& folder);
        Result ProjectGetFolderMetadata(const std::wstring& rms_url, _In_opt_ bool* cancelFlag,
            const std::wstring& projectId,
			const std::wstring& folderId,
			__int64 pageId,
			__int64 pageSize,
			const std::wstring& orderBy,
			__int64 lastModifiedTime,
			RmRestStatus& status,
			_Out_opt_ __int64* usage,
			_Out_opt_ __int64* quota,
			_Out_opt_ __int64* totalRecords,
			std::vector<RmProjectFile>& files);
        Result ProjectGetFolderMetadata(NX::REST::http::Connection* conn, _In_opt_ bool* cancelFlag,
            const std::wstring& projectId,
			const std::wstring& folderId,
			__int64 pageId,
			__int64 pageSize,
			const std::wstring& orderBy,
			__int64 lastModifiedTime,
			RmRestStatus& status,
			_Out_opt_ __int64* usage,
			_Out_opt_ __int64* quota,
			_Out_opt_ __int64* totalRecords,
			std::vector<RmProjectFile>& files);
        Result ProjectDeleteFile(const std::wstring& rms_url,
            const std::wstring& projectId,
            const std::wstring& filePath,
            RmRestStatus& status);
        Result ProjectDeleteFile(NX::REST::http::Connection* conn,
            const std::wstring& projectId,
            const std::wstring& filePath,
            RmRestStatus& status);
        Result ProjectUploadFile(const std::wstring& rms_url,
			_In_opt_ bool* cancelFlag,
            const std::wstring& projectId,
			const std::wstring& destDirPathId,
			const std::wstring& destFileName,
			const std::wstring& srcFilePath,
            const std::wstring& fileRights,
            const NXL::FileTags& fileTags,
            RmRestStatus& status,
			RmProjectFile& newFile);
        Result ProjectUploadFile(NX::REST::http::Connection* conn,
			_In_opt_ bool* cancelFlag,
            const std::wstring& projectId,
			const std::wstring& destDirPathId,
			const std::wstring& destFileName,
			const std::wstring& srcFilePath,
            const std::wstring& fileRights,
            const NXL::FileTags& fileTags,
            RmRestStatus& status,
			RmProjectFile& newFile);
		Result ProjectDownloadFile(const std::wstring& rms_url,
			_In_opt_ bool* cancelFlag,
			const std::wstring& projectId,
			const std::wstring& filePath,
			const std::wstring& downloadPath,
			bool forViewer,
			RmRestStatus& status,
			_Out_ std::wstring& preferredFileName);
        Result ProjectDownloadFile(NX::REST::http::Connection* conn,
			_In_opt_ bool* cancelFlag,
            const std::wstring& projectId,
            const std::wstring& filePath,
            const std::wstring& downloadPath,
            bool forViewer,
            RmRestStatus& status,
		    _Out_ std::wstring& preferredFileName);
        Result ProjectGetFileMetadata(const std::wstring& rms_url,
            const std::wstring& projectId,
            const std::wstring& filePath,
            RmRestStatus& status,
			RmProjectFileMetadata& metadata);
        Result ProjectGetFileMetadata(NX::REST::http::Connection* conn,
            const std::wstring& projectId,
            const std::wstring& filePath,
            RmRestStatus& status,
			RmProjectFileMetadata& metadata);
        Result ProjectListMembers(const std::wstring& rms_url,
            const std::wstring& projectId,
            __int64 pageId,
            __int64 pageSize,
            const std::wstring& orderBy,
            const std::wstring& searchField,
            const std::wstring& searchValue,
            bool returnPicture,
            RmRestStatus& status,
			_Out_opt_ int* totalMembers,
            std::vector<RmProjectMember>& members);
        Result ProjectListMembers(NX::REST::http::Connection* conn,
            const std::wstring& projectId,
            __int64 pageId,
            __int64 pageSize,
            const std::wstring& orderBy,
            const std::wstring& searchField,
            const std::wstring& searchValue,
            bool returnPicture,
            RmRestStatus& status,
			_Out_opt_ int* totalMembers,
			std::vector<RmProjectMember>& members);
        Result ProjectRemoveMember(const std::wstring& rms_url,
            const std::wstring& projectId,
            const std::wstring& memberId,
            RmRestStatus& status);
        Result ProjectRemoveMember(NX::REST::http::Connection* conn,
            const std::wstring& projectId,
            const std::wstring& memberId,
            RmRestStatus& status);
        Result ProjectGetMemberMetadata(const std::wstring& rms_url,
            const std::wstring& projectId,
            const std::wstring& memberId,
            RmRestStatus& status,
			RmProjectMember& member);
        Result ProjectGetMemberMetadata(NX::REST::http::Connection* conn,
            const std::wstring& projectId,
            const std::wstring& memberId,
            RmRestStatus& status,
			RmProjectMember& member);
        Result RemoteViewerNew(const std::wstring& rms_url,
            const std::wstring& filePath,
            const std::wstring& fileName,
            RmRestStatus& status,
			std::vector<std::wstring>& cookies,
			std::wstring& url,
			_In_opt_ bool* cancelFlag);
        Result RemoteViewerNew(NX::REST::http::Connection* conn,
			const std::wstring& filePath,
			const std::wstring& fileName,
            RmRestStatus& status,
			std::vector<std::wstring>& cookies,
			std::wstring& url,
			_In_opt_ bool* cancelFlag);
		Result RemoteViewerForRepoFile(const std::wstring& rms_url,
			_In_opt_ bool* cancelFlag,
			const std::wstring& repoId,
			const std::wstring& repoName,
			const std::wstring& repoType,
			const std::wstring& pathId,
			const std::wstring& pathDisplay,
			const std::wstring& email,
			__int64 lastModifiedDate,
			RmRestStatus& status,
			std::vector<std::wstring>& cookies,
			std::wstring& url);
		Result RemoteViewerForRepoFile(NX::REST::http::Connection* conn,
			_In_opt_ bool* cancelFlag,
			const std::wstring& repoId,
			const std::wstring& repoName,
			const std::wstring& repoType,
			const std::wstring& pathId,
			const std::wstring& pathDisplay,
			const std::wstring& email,
			__int64 lastModifiedDate,
			RmRestStatus& status,
			std::vector<std::wstring>& cookies,
			std::wstring& url);
		Result RemoteViewerForProjectFile(const std::wstring& rms_url,
			_In_opt_ bool* cancelFlag,
			const std::wstring& projectId,
			const std::wstring& pathId,
			const std::wstring& pathDisplay,
			const std::wstring& email,
			__int64 lastModifiedDate,
			RmRestStatus& status,
			std::vector<std::wstring>& cookies,
			std::wstring& url);
		Result RemoteViewerForProjectFile(NX::REST::http::Connection* conn,
			_In_opt_ bool* cancelFlag,
			const std::wstring& projectId,
			const std::wstring& pathId,
			const std::wstring& pathDisplay,
			const std::wstring& email,
			__int64 lastModifiedDate,
			RmRestStatus& status,
			std::vector<std::wstring>& cookies,
			std::wstring& url);
        
		inline const RmRestCommonParams& GetCommonParams() const { return _commonParams; }
		inline void SetCredential(const std::wstring& tenant, const std::wstring& userId, const std::wstring& userName, const std::wstring& ticket)
		{
			_commonParams.SetTenant(tenant);
			_commonParams.SetUserId(userId);
			_commonParams.SetUserName(userName);
			_commonParams.SetTicket(ticket);
		}

		inline void ClearCredential()
		{
			_commonParams.SetTenant(L"");
			_commonParams.SetUserId(L"");
			_commonParams.SetUserName(L"");
			_commonParams.SetTicket(L"");
		}

		inline bool ValidateCredential()
		{
			return (!_commonParams.GetTenant().empty() && !_commonParams.GetUserId().empty() && !_commonParams.GetUserName().empty() && !_commonParams.GetTicket().empty());
		}

    private:
        // Not copyable
        RmRestClient(const RmRestClient& rhs) {}
        RmRestClient& operator = (const RmRestClient& rhs) { return *this; }

	private:
		RmRestCommonParams	_commonParams;
    };
}


#endif