

#ifndef __LIBRMC_SESSION_H__
#define __LIBRMC_SESSION_H__

#include <nx\rmc\dirs.h>
#include <nx\rmc\router.h>
#include <nx\rmc\rmserver.h>
#include <nx\rmc\tenant.h>
#include <nx\rmc\user.h>
#include <nx\rmc\rest.h>
#include <nx\rmc\keym.h>
#include <nx\rmc\repo.h>
#include <nx\rmc\search.h>
#include <nx\rmc\project.h>
#include <nx\rmc\viewermgr.h>
#include <nx\rmc\product.h>
#include <nx\rmc\activity.h>

#include <nx\common\result.h>
#include <nx\crypt\rsa.h>
#include <nx\nxl\nxlfile.h>

#include <string>
#include <list>

namespace NX {

    class RmSession
    {
    public:
        ~RmSession();

        static RmSession* Create(Result& error);
        static RmSession* Create(const RmRouter& router, Result& error);
        
		inline const RmProduct& GetProduct() const { return _product; }
		inline const RmClientId& GetClientId() const { return _clientId; }
        inline const RmDirs& GetDirs() const { return _dirs; }
        inline const RmRouter& GetCurrentRouter() const { return _router; }
		inline void SetRouter(const RmRouter& router) {_router = router; }
        inline const RmServer& GetCurrentRMS() const { return _rmserver; }
        inline const std::wstring& GetCurrentTenantName() const { return _rmserver.GetTenantName(); }
        inline const RmUser& GetCurrentUser() const { return _user; }
		inline const RmRestCommonParams& GetRestCommonParams() const { return _restClient.GetCommonParams(); }

		inline bool ValidateCredential() const { return (!_user.Empty() && !_user.GetTicket().Empty() && !_user.GetTicket().Expired()); }
        
		//
		// Common Session Routines
		//
    public:
		void Cleanup();
		RmUpgradeInfo CheckForUpgrade();
		Result DownloadUpgradeInstaller(_Inout_ RmUpgradeInfo& info);
		Result DoHeartbeat(_Out_ std::vector<std::wstring>& changedItems);
		Result SecureFileRead(const std::wstring& file, std::string& s);
		Result SecureFileRead(const std::wstring& file, std::vector<UCHAR>& data);
		Result SecureFileWrite(const std::wstring& file, const std::string& s);
		Result SecureFileWrite(const std::wstring& file, const std::vector<UCHAR>& data);
		Result SecureFileWrite(const std::wstring& file, const UCHAR* data, ULONG size);
	protected:
        Result Initialize();
		void SetIeFeatures();
		Result PrepareClientKey();
		Result ReadHeartbeatHistory(std::vector<std::pair<std::wstring, std::wstring>>& items);
		Result WriteHeartbeatHistory(const std::vector<std::pair<std::wstring, std::wstring>>& items);
		Result GenerateWatermarkImage(const std::wstring& content);
		Result GeneratePolicyBundle(const std::wstring& content);


		//
		// Session Routines for NXL Viewer
		//
	public:
		std::vector<DWORD> GetActiveViewerProcesses();
		Result ViewLocalFile(const std::wstring& path, const std::wstring& displayName, _In_opt_ bool* cancelFlag);
		Result ViewRepoFile(NX::RmRepository& repo, const std::wstring& pathId, _In_opt_ bool* cancelFlag);
		Result ViewMyVaultFile(NX::RmRepository& mydrive, const std::wstring& pathId, _In_opt_ bool* cancelFlag);
		Result ViewProjectFile(NX::RmProject& project, const std::wstring& pathId);
        Result ViewProjectFile(const std::wstring& projectID, const std::wstring& pathId, _In_opt_ bool* cancelFlag);
	protected:
		Result RunRemoteViewer(const std::wstring& path, const std::wstring& fileName, _In_opt_ bool* cancelFlag);
		Result CreateRemoteViewerProcess(PHANDLE ph, const std::wstring& path, const std::wstring& pathDisplay, _In_opt_ bool* cancelFlag);
		Result CreateRemoteViewerProcessEx(PHANDLE ph, const RmRemoteViewerData& viewerData);

		//
		//  Session Directory Routines
		//
	protected:
		std::wstring GeServerHome(bool createIfNotExist = false) const;
		std::wstring GetTenantHome(const std::wstring& tenantName, bool createIfNotExist = false) const;
		std::wstring GetUserHome(const std::wstring& tenantName, const std::wstring& userId, bool createIfNotExist = false) const;
		std::wstring GetUserCacheDir(const std::wstring& tenantName, const std::wstring& userId, bool createIfNotExist = false) const;
		std::wstring GetUserTempDir(const std::wstring& tenantName, const std::wstring& userId, bool createIfNotExist = false) const;
		std::wstring GetUserMembershipsDir(const std::wstring& tenantName, const std::wstring& userId, bool createIfNotExist = false) const;
		std::wstring GetUserOfflineDir(const std::wstring& tenantName, const std::wstring& userId, bool createIfNotExist = false) const;
		std::wstring GetUserReposDir(const std::wstring& tenantName, const std::wstring& userId, bool createIfNotExist = false) const;
		std::wstring GetUserRepoDir(const std::wstring& tenantName, const std::wstring& userId, const std::wstring& repoId, bool createIfNotExist = false) const;
		std::wstring GetCurrentUserHome() const;
		std::wstring GetCurrentUserCacheDir() const;
		std::wstring GetCurrentUserTempDir() const;
		std::wstring GetCurrentUserMembershipsDir() const;
		std::wstring GetCurrentUserOfflineDir() const;
		std::wstring GetCurrentUserReposDir() const;
		std::wstring GetCurrentUserRepoDir(const std::wstring& repoId) const;
		std::wstring GetNextLabsDataDir() const;

		//
		// Session Credential Routines
		//
    public:
		Result InitRMS(const std::wstring& tenantName);
		Result PrepareLogin(const std::wstring& tenantName);
        Result Login(const std::wstring& userName, const std::wstring& password);
        Result LoginFromResult(const std::string& jsonResult);
        Result Logout();
		Result ChangePassword(const std::wstring& oldPassword, const std::wstring& newPassword);
		Result PrepareResetPassword(std::wstring& nonce, std::vector<UCHAR>& captcha);
		Result ResetPassword(const std::wstring& email, const std::wstring& nonce, const std::wstring& captcha);
		Result UpdateProfileDisplayName(const std::wstring& displayName);
		Result UpdateProfile();
	protected:
		Result FinalizeLogin();
		Result LoadExistingCredential();
		Result QueryLoginHistory(std::wstring& tenantName, std::wstring& rmsUrl, std::wstring& userId);
		Result QueryLoginHistoryEx(RmRouter& router, std::wstring& tenantName, std::wstring& rmsUrl, std::wstring& userId);
		Result CacheLoginHistory();
		Result RemoveCachedLoginHistory();
		Result QueryCachedUserProfile(const std::wstring& tenantName, const std::wstring& userId, RmUser& user);
		Result CacheUserProfile(const std::wstring& tenantName, const RmUser& user);
		Result QueryCachedUserToken(const std::wstring& tenantName, const std::wstring& userId, RmUserTicket& userTicket);
		Result RemoveCachedUserToken(const std::wstring& tenantName, const std::wstring& userId);
		Result CacheUserToken(const std::wstring& tenantName, const RmUser& user);
		std::list<std::wstring> FindAllCachedMemberships();
		Result LoadCachedMembership(const std::wstring& membershipIdHash, RmUserMembership& membership);
		Result LoadCachedMembershipProfile(const std::wstring& file, RmUserMembership& membership);
		Result LoadCachedMembershipAgreement(const std::wstring& file, std::vector<UCHAR>& agreement);
		Result LoadCachedmembershipTokens(const std::wstring& file, RmUserMembership& membership);
		Result SaveMembershipTokens(const std::wstring& file, const RmUserMembership& membership);
		void SaveAllMembershipTokens();
		Result QueryMembershipAgreements(RmUserMembership& membership);
		Result SaveMembership(const RmUserMembership& membership, bool profileOnly = false);

		Result EnsureEnoughFileToken(RmUserMembership& membership);
		Result PopEncryptToken(const std::wstring& membershipId, NXL::FileToken& token);
		
		//
		// Session Routines for Shared Files
		//
    public:
		Result ListSharedWithMeFiles(__int64 pageId, __int64 pageSize, const std::wstring& orderBy, const std::wstring& keyWord, std::vector<RmSharedWithMeFile>& files);

		Result GetFirstPageSharedWithMeFiles(RmFinder& finder, bool* cancelFlag, std::vector<RmSharedWithMeFile>& files);
		Result GetNextPageSharedWithMeFiles(RmFinder& finder, bool* cancelFlag, std::vector<RmSharedWithMeFile>& files);


		//
		// Session Routines for Local Files
		//
    public:
        NX::NXL::File* LocalFileOpen(const std::wstring& path, bool readOnly, Result& result);
		Result LocalFileProtect(const std::wstring& source,
			const std::wstring& target,
			const std::wstring& membershipId,
			const NX::NXL::FileMeta& metadata,
			const NX::NXL::FileTags& tags,
			const std::vector<std::wstring>& rights,
			const std::vector<std::wstring>& obs,
			_In_opt_ bool* cancelFlag,
			_Out_ std::wstring& newFileId,
			_Out_ std::wstring& newFilePath,
			_Out_opt_ NX::NXL::FileSecret* secret);
		Result LocalFileProtect(const std::wstring& source,
			const std::wstring& target,
			const std::wstring& membershipId,
			const NX::NXL::FileMeta& metadata,
			const NX::NXL::FileTags& tags,
			const ULONGLONG permissions,
			_In_opt_ bool* cancelFlag,
			_Out_ std::wstring& newFileId,
			_Out_ std::wstring& newFilePath,
			_Out_opt_ NX::NXL::FileSecret* secret);
		Result LocalFileUnprotect(const std::wstring& source, const std::wstring& target);
		Result LocalFileShare(const std::wstring& source,
			const std::wstring& membershipId,
			const NX::NXL::FileMeta& metadata,
			const NX::NXL::FileTags& tags,
			const std::vector<std::wstring>& recipients,
			const std::vector<std::wstring>& rights,
			const std::vector<std::wstring>& obs,
			_In_opt_ const SYSTEMTIME* expireTime,
			RmFileShareResult& shareResult, _In_opt_ bool* cancelFlag);
		Result LocalFileShare(const std::wstring& source,
			const std::wstring& membershipId,
			const NX::NXL::FileMeta& metadata,
			const NX::NXL::FileTags& tags,
			const std::vector<std::wstring>& recipients,
			const std::vector<std::wstring>& rights,
			const std::vector<std::wstring>& obs,
			_In_ __int64 unixExpireTime,
			RmFileShareResult& shareResult, _In_opt_ bool* cancelFlag);
	protected:
		Result ReadLocalFileTokenFromServer(const std::wstring& ownerId, const std::wstring& agreement, _Inout_ NX::NXL::FileToken& token);
		Result ReadCachedLocalFileToken(const std::wstring& path, const std::wstring& ownerId, const std::wstring& duid, _Inout_ NX::NXL::FileToken& token);
		Result WriteCachedLocalFileToken(const std::wstring& path, const std::wstring& ownerId, const std::wstring& duid, const NX::NXL::FileToken& token);


		//
		// Session Routines for Activity Log
		//
	public:
		Result FetchAllActivityLogs(_Out_ std::wstring& fileName,
			_Out_ std::vector<RmFileActivityRecord>& records,
			const std::wstring& duid,
			ActivitiesOrderBy orderBy = ActivitiesOrderbyAccessTime,
			bool descending = true);

		Result FetchActivityLogsEx(_Out_ std::wstring& fileName,
			_Out_ std::vector<RmFileActivityRecord>& records,
			const std::wstring& duid,
			__int64 start,
			__int64 count,
			ActivitiesOrderBy orderBy = ActivitiesOrderbyAccessTime,
			bool descending = true);

		Result SearchActivityLogs(_Out_ std::wstring& fileName,
			_Out_ std::vector<RmFileActivityRecord>& records,
			const std::wstring& duid,
			__int64 start,
			__int64 count,
			ActivitiesSearchField searchField,
			const std::wstring& keyWord,
			ActivitiesOrderBy orderBy = ActivitiesOrderbyAccessTime,
			bool descending = true);

		Result LogActivityProtect(const std::wstring& duid,
								  const std::wstring& owner,
								  const std::wstring& repoId,
								  const std::wstring& fileId,
								  const std::wstring& fileName,
								  const std::wstring& filePath);

		Result LogActivityPrint(const std::wstring& duid,
								const std::wstring& owner,
								const std::wstring& repoId,
								const std::wstring& fileId,
								const std::wstring& fileName,
								const std::wstring& filePath);

		Result LogActivityShare(bool succeed,
								bool reshare,
								const std::wstring& duid,
								const std::wstring& owner,
								const std::wstring& repoId,
								const std::wstring& fileId,
								const std::wstring& fileName,
								const std::wstring& filePath);

		Result LogActivityDownload(bool succeed,
								const std::wstring& duid,
								const std::wstring& owner,
								const std::wstring& repoId,
								const std::wstring& fileId,
								const std::wstring& fileName,
								const std::wstring& filePath);

		Result LogActivityView(bool succeed,
							const std::wstring& duid,
							const std::wstring& owner,
							const std::wstring& repoId,
							const std::wstring& fileId,
							const std::wstring& fileName,
							const std::wstring& filePath);


	protected:
		std::string CreateActivityRecord( const std::wstring& duid,
										  const std::wstring& owner,
										  int operation,
										  const std::wstring& repoId,
										  const std::wstring& fileId,
										  const std::wstring& fileName,
										  const std::wstring& filePath,
										  const int accessResult,
										  const std::wstring& extraData,
										  __int64 accessTime = 0
										  );


		//
		// Session Routines for Repository
		//
    public:
		Result RepoGetReposListFromRemote(std::vector<RmRepository>& repos);
		Result RepoGetReposListFromCache(std::vector<RmRepository>& repos);
		Result RepoSaveReposListToCache(const std::vector<RmRepository>& repos);

		Result RepoAdd(const std::wstring& name,
			RmRepository::REPO_TYPE type,
			bool isShared,
			const std::wstring& accountName,
			const std::wstring& accountId,
			const std::wstring& accountToken,
			std::wstring& repoId);
		Result RepoGetAuthUrl(const std::wstring& name, RmRepository::REPO_TYPE type, const std::wstring& siteUrl, std::wstring& url);
		Result RepoRename(const RmRepository& repo, const std::wstring& name);
		Result RepoRemove(const RmRepository& repo);
		Result RepoGetQuota(const RmRepository& repo, __int64* pTotal, __int64* pUsed, __int64* pAvailable);
		Result RepoListFiles(RmRepository& repo, const std::wstring& folderId, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files);
		Result RepoSearchFiles(RmRepository& repo, const std::wstring& folderId, const std::wstring& keywords, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files);
		Result RepoCreateFolder(RmRepository& repo, const std::wstring& parentFolderId, const std::wstring& folderName, RmRepoFile& folder);
		Result RepoDeleteFolder(RmRepository& repo, const std::wstring& folderId);
		Result RepoUploadFile(RmRepository& repo, const std::wstring& file, const std::wstring& repoPath, RmRepoFile& repoFile, _In_opt_ bool* cancelFlag);
		Result RepoDownloadFile(RmRepository& repo, const std::wstring& fileId, std::wstring& localFilePath, _In_opt_ bool* cancelFlag, const ULONG requestedLength = 0, bool logActivity = true);
		Result RepoDeleteFile(RmRepository& repo, const std::wstring& fileId);
		Result RepoGetFileInfo(RmRepository& repo, const std::wstring& fileId, RmRepoFile& repoFile);

		Result RepoGetNxlFileHeader(RmRepository& repo,
			const std::wstring& fileId,
			bool* cancelFlag,
			_Out_opt_ NX::NXL::FileSecret* secret,
			_Out_opt_ NX::NXL::FileMeta* metadata,
			_Out_opt_ NX::NXL::FileAdHocPolicy* adhocPolicy,
			_Out_opt_ NX::NXL::FileTags* tags);

		Result RepoGetFolderFileListFromRemote(RmRepository& repo, const std::wstring& folderId, std::vector<RmRepoFile>& files);
		Result RepoGetFolderFileListFromCache(const RmRepository& repo, const std::wstring& folderId, std::vector<RmRepoFile>& files, _Out_opt_ __int64* timestamp = NULL);
		Result RepoSaveFolderFileListFromCache(const RmRepository& repo, const std::wstring& folderId, const std::vector<RmRepoFile>& files);
		void   RepoDeleteFolderCache(const RmRepository& repo, const std::wstring& folderId);
		
		Result RepoGetFavoriteFilesFromRemote(std::vector<std::pair<std::wstring, std::vector<RmRepoFile>>>& files);
		Result RepoGetFavoriteFilesFromCache(std::vector<std::pair<std::wstring, std::vector<RmRepoFile>>>& files);
		Result RepoSaveFavoriteFilesToCache(const std::vector<std::pair<std::wstring, std::vector<RmRepoFile>>>& files);
		Result RepoSetFavoriteFile(const RmRepository& repo, const std::wstring& folderId, const RmRepoFile& file);
		Result RepoSetFavoriteFiles(const RmRepository& repo, const std::wstring& folderId, const std::vector<RmRepoFile>& files);
		Result RepoUnsetFavoriteFile(const RmRepository& repo, const RmRepoFile& file);
		Result RepoUnsetFavoriteFiles(const RmRepository& repo, const std::vector<RmRepoFile>& files);


		Result RepoGetOfflineFiles(std::vector<std::pair<std::wstring, std::vector<RmRepoFile>>>& files);
		Result RepoSaveOfflineFiles(const std::vector<std::pair<std::wstring, std::vector<RmRepoFile>>>& files);
		Result RepoFindOfflineFile(const std::wstring& repoId, const std::wstring& fileId, _Out_ std::wstring& offlinePath, _Out_opt_ __int64* timestamp);
		Result RepoCreateOfflineFilePath(const std::wstring& repoId, const std::wstring& fileId, _In_ __int64 timestamp, std::wstring& offlinePath);
		Result RepoDownloadOfflineFile(RmRepository& repo, const std::wstring& fileId, bool overwriteExisting, _Out_ std::wstring& offlinePath);
		
		Result RepoProtectFile(const std::wstring& membershipId,
			RmRepository& repo,
			const RmRepoFile& file,
			ULONGLONG permissions,
			const NXL::FileMeta& metadata,
			const NXL::FileTags& tags,
			RmFileShareResult& sr,
			_In_opt_ bool* cancelFlag);

		Result RepoShareFile(const std::wstring& membershipId, 
			const RmRepository& repo,
			const RmRepoFile& file,
			ULONGLONG permissions,
			const NXL::FileMeta& metadata,
			const NXL::FileTags& tags,
			std::vector<std::wstring>& recipients,
			_In_opt_ const SYSTEMTIME* expireTime,
			RmFileShareResult& sr);
		Result RepoShareFile(const std::wstring& membershipId, 
			const RmRepository& repo,
			const RmRepoFile& file,
			ULONGLONG permissions,
			const NXL::FileMeta& metadata,
			const NXL::FileTags& tags,
			std::vector<std::wstring>& recipients,
			_In_ __int64 unixExpireTime,
			RmFileShareResult& sr);

	protected:
		Result RepoCreateFileIdHash(const std::wstring& fileId, std::wstring& hash);
		Result RepoBuildFolderCacheFilePath(const RmRepository& repo, const std::wstring& folderId, std::wstring& cacheFile);
		Result RepoGetFolderFileListFromCache(const std::wstring& cacheFile, std::vector<RmRepoFile>& files, _Out_opt_ __int64* timestamp);
		Result RepoSaveFolderFileListFromCache(const std::wstring& cacheFile, const std::vector<RmRepoFile>& files);

	protected:
        Result RepoGetAccessToken(RmRepository& repo);
		std::wstring RepoGetTempFolderName(const std::wstring& repoId, const std::wstring& filePath);
		std::wstring RepoGetOfflineFilePath(const std::wstring& repoId, const std::wstring& filePath);
		std::wstring RepoGetTempFilePath(const std::wstring& repoId, const std::wstring& filePath);
		std::wstring RepoFileCreateCacheId(const NX::RmRepository& repo, const RmRepoFile& file);

		//
		//  Session Routines for Google Drive
		//
    protected:
        Result GoogleDriveGetFileInfo(const RmRepository& repo, const std::wstring& fileId, RmRepoFile& file);
        Result GoogleDriveListFiles(const RmRepository& repo, const std::wstring& folderId, const std::wstring& keywords, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files);
        Result GoogleDriveCreateFolder(const RmRepository& repo, const std::wstring& parentFolderId, const std::wstring& folderName, RmRepoFile& folder);
        Result GoogleDriveDeleteFolder(const RmRepository& repo, const std::wstring& folderId);
        Result GoogleDriveDeleteFile(const RmRepository& repo, const std::wstring& fileId);
        Result GoogleDriveUploadFile(const RmRepository& repo, const std::wstring& file, const std::wstring& repoPath, RmRepoFile& repoFile, _In_opt_ bool* cancelFlag);
        Result GoogleDriveDownloadFile(const RmRepository& repo, const std::wstring& fileId, std::wstring& localFilePath, _In_opt_ bool* cancelFlag, const ULONG requestedLength = 0);

		//
		//  Session Routines for OneDrive
		//
    protected:
        Result OneDriveGetFileInfo(const RmRepository& repo, const std::wstring& fileId, RmRepoFile& file);
        Result OneDriveListOrSearchFiles(const RmRepository& repo, const std::wstring& folderId, const std::wstring& keywords, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files);
        Result OneDriveCreateFolder(const RmRepository& repo, const std::wstring& parentFolderId, const std::wstring& folderName, RmRepoFile& folder);
        Result OneDriveDelete(const RmRepository& repo, const std::wstring& itemId);
        Result OneDriveUploadFile(const RmRepository& repo, const std::wstring& file, const std::wstring& repoPath, RmRepoFile& repoFile, _In_opt_ bool* cancelFlag);
        Result OneDriveDownloadFile(const RmRepository& repo, const std::wstring& fileId, const std::wstring& localFilePath, _In_opt_ bool* cancelFlag, const ULONG requestedLength = 0);

		//
		//  Session Routines for Dropbox
		//
    protected:
        Result DropboxGetFileInfo(const RmRepository& repo, const std::wstring& path, RmRepoFile& file);
        Result DropboxListFiles(const RmRepository& repo, const std::wstring& path, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files);
        Result DropboxSearchFiles(const RmRepository& repo, const std::wstring& path, const std::wstring& keywords, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files);
        Result DropboxCreateFolder(const RmRepository& repo, const std::wstring& parentDir, const std::wstring& folderName, RmRepoFile& folder);
        Result DropboxDeleteFolder(const RmRepository& repo, const std::wstring& folderPath);
        Result DropboxDeleteFile(const RmRepository& repo, const std::wstring& repoFilePath);
        Result DropboxUploadFile(const RmRepository& repo, const std::wstring& file, const std::wstring& repoPath, RmRepoFile& repoFile, _In_opt_ bool* cancelFlag);
        Result DropboxDownloadFile(const RmRepository& repo, const std::wstring& repoFilePath, const std::wstring& localFilePath, _In_opt_ bool* cancelFlag, const ULONG requestedLength = 0);

		//
		//  Session Routines for SharePoint
		//
    protected:
        Result SharePointOnlineGetFileInfo(const RmRepository& repo, const std::wstring& path, RmRepoFile& file, std::wstring& serverRelUrl);
        Result SharePointOnlineListFiles(const RmRepository& repo, const std::wstring& path, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files, std::vector<std::wstring> *serverRelUrls);
        Result SharePointOnlineSearchFiles(const RmRepository& repo, const std::wstring& keywords, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files);
        Result SharePointOnlineCreateFolder(const RmRepository& repo, const std::wstring& parentDir, const std::wstring& folderName, RmRepoFile& folder);
        Result SharePointOnlineDeleteFolder(const RmRepository& repo, const std::wstring& folderPath);
        Result SharePointOnlineDeleteFile(const RmRepository& repo, const std::wstring& repoFilePath);
        Result SharePointOnlineUploadFile(const RmRepository& repo, const std::wstring& file, const std::wstring& repoPath, RmRepoFile& repoFile, _In_opt_ bool* cancelFlag);
        Result SharePointOnlineDownloadFile(const RmRepository& repo, const std::wstring& repoFilePath, const std::wstring& localFilePath, _In_opt_ bool* cancelFlag, const ULONG requestedLength = 0);

		//
		// Session Routines for MyDrive
		//
    public:
        Result MyDriveGetUsage(RmMyDriveUsage& usage);
        Result MyDriveGetFileInfo(const std::wstring& path, RmRepoFile& file);
        Result MyDriveListFiles(const std::wstring& parentDir, _In_opt_ bool* cancelFlag, std::vector<RmRepoFile>& files);
        Result MyDriveSearchFiles(const std::wstring& parentDir,  _In_opt_ bool* cancelFlag, const std::wstring& keywords, bool recursively, std::vector<RmRepoFile>& files);
        Result MyDriveCreateFolder(const std::wstring& parentDir, const std::wstring& folderName, RmRepoFile& folder);
        Result MyDriveDeleteFile(const std::wstring& path, _Out_opt_ __int64* pUsage);
        Result MyDriveDownloadFile(const std::wstring& source, const std::wstring& target, _In_opt_ bool* cancelFlag, const ULONG requestedLength);
        Result MyDriveDownloadFile(const RmRepoFile& source, const std::wstring& target, _In_opt_ bool* cancelFlag, const ULONG requestedLength);
        Result MyDriveUploadFile(const std::wstring& source, const std::wstring& target, RmRepoFile& newFile, _In_opt_ bool* cancelFlag, _Out_opt_ RmMyDriveUsage* newUsage = NULL);
		Result MyDriveGetFolderMetadata(
			const std::wstring& folderId,
			__int64 lastModifiedTime,
			_In_opt_ bool* cancelFlag,
			std::vector<RmRepoFile>& files);

		//
		// Session Routines for MyVault
		//
    public:
        Result MyVaultGetFirstPageFiles(RmFinder& finder, bool* cancelFlag, std::vector<RmMyVaultFile>& files);
        Result MyVaultGetNextPageFiles(RmFinder& finder, bool* cancelFlag, std::vector<RmMyVaultFile>& files);
        Result MyVaultGetFileInfo(const std::wstring& duid, const std::wstring& path, RmMyVaultFileMetadata& metadata);
        Result MyVaultDeleteFile(const std::wstring& duid, const std::wstring& path);
        Result MyVaultDownloadFile(const std::wstring& source, const std::wstring& target, bool failIfExists, _In_opt_ bool* cancelFlag, const ULONG requestedLength = 0);
        Result MyVaultDownloadFileForViewer(const std::wstring& source, const std::wstring& target, _In_opt_ bool* cancelFlag);
        Result MyVaultAddFileFromRepository(const std::wstring& source,
			const std::wstring& repoId,
			const std::wstring& repoName,
			const std::wstring& repoType,
			const std::wstring& repoFilePath,
			const std::wstring& repoFileDisplayPath,
			RmMyVaultUploadResult& newFile);
        Result MyVaultAddFileFromLocal(const std::wstring& source, const std::wstring& displayName, RmMyVaultUploadResult& newFile, _In_opt_ bool* cancelFlag);
		Result MyVaultGetFolderMetadata(
			__int64 lastModifiedTime,
			_In_opt_ bool* cancelFlag,
			std::vector<RmRepoFile>& files);

		Result MyVaultRevokeShare(const std::wstring& duid);
		Result MyVaultUpdateShareRecipients(const std::wstring& duid,
			const std::vector<std::wstring>& addRecipients,
			const std::vector<std::wstring>& removeRecipients,
			std::vector<std::wstring>& recipientsAdded,
			std::vector<std::wstring>& recipientsRemoved
			);

		Result MyVaultShareFile(const std::wstring& membershipId,
			const RmRepository& mydrive,
			const std::wstring& fileId,
			const std::wstring& fileName,
			const std::wstring& filePath,
			ULONGLONG permissions,
			const NXL::FileMeta& metadata,
			const NXL::FileTags& tags,
			std::vector<std::wstring>& recipients,
			_In_opt_ const SYSTEMTIME* expireTime,
			RmFileShareResult& sr);
		Result MyVaultShareFile(const std::wstring& membershipId,
			const RmRepository& mydrive,
			const std::wstring& fileId,
			const std::wstring& fileName,
			const std::wstring& filePath,
			ULONGLONG permissions,
			const NXL::FileMeta& metadata,
			const NXL::FileTags& tags,
			std::vector<std::wstring>& recipients,
			_In_ __int64 unixExpireTime,
			RmFileShareResult& sr);

	protected:
		std::wstring MyVaultBuildFileName(const std::wstring& fileName);

		//
		// Session Routines for Project
		//
    public:
		typedef enum PROJECT_OWNER_TYPE {
			OwnedByMe = 1,
			OwnedByOthers = 2,
			OwnedByMeAndOthers = 3
		} PROJECT_OWNER_TYPE;
        Result ProjectListProjects(std::vector<RmProject>& projects, bool* cancelFlag, PROJECT_OWNER_TYPE type = OwnedByMeAndOthers);

		Result ProjectListFirstPageProjects(RmFinder& finder, std::vector<RmProject>& projects, bool* cancelFlag, PROJECT_OWNER_TYPE type = OwnedByMeAndOthers);
		Result ProjectListNextPageProjects(RmFinder& finder, std::vector<RmProject>& projects, bool* cancelFlag, PROJECT_OWNER_TYPE type = OwnedByMeAndOthers);

        Result ProjectCreate(const std::wstring& name, const std::wstring& description, const std::wstring& inviteMsg, const std::vector<std::wstring>& invitees, std::wstring& newProjectId);
        Result ProjectGetInfo(const std::wstring& projectId, RmProject& project);
        Result ProjectInvite(const std::wstring& projectId, const std::vector<std::wstring>& invitees,
			std::vector<std::wstring>& alreadyInvited, std::vector<std::wstring>& nowInvited, std::vector<std::wstring>& alreadyMembers);
        Result ProjectAcceptInvitation(const std::wstring& id, const std::wstring& code, std::wstring& projectId);
        Result ProjectDeclineInvitation(const std::wstring& id, const std::wstring& code, const std::wstring& reason);
        Result ProjectSendInvitationReminder(const std::wstring& id);
        Result ProjectRevokeInvitation(const std::wstring& id);
        Result ProjectGetFirstPagePendingInvitations(RmFinder& finder, const std::wstring& projectId, std::vector<RmProjectInvitation>& invitations);
        Result ProjectGetNextPagePendingInvitations(RmFinder& finder, const std::wstring& projectId, std::vector<RmProjectInvitation>& invitations);
        Result ProjectGetUserPendingInvitations(std::vector<RmProjectInvitation>& invitations, bool* cancelFlag);
        Result ProjectGetFirstPageFiles(RmFinder& finder, bool* cancelFlag,
			const std::wstring& projectId,
			const std::wstring& parentFolder,
			_Out_opt_ __int64* usage,
			_Out_opt_ __int64* quota,
			_Out_opt_ __int64* lastUpdatedTime,
			std::vector<RmProjectFile>& files);
        Result ProjectGetNextPageFiles(RmFinder& finder, bool* cancelFlag, const std::wstring& projectId, const std::wstring& parentFolder, std::vector<RmProjectFile>& files);
        Result ProjectFindFiles(const std::wstring& projectId, const std::wstring& parentFolder, const std::wstring& keyword, _In_opt_ bool* cancelFlag, std::vector<RmProjectFile>& files);
        Result ProjectCreateFolder(const std::wstring& projectId, const std::wstring& parentDir, const std::wstring& folderName, bool autoRename, RmRepoFile& folder);
        Result ProjectDeleteFile(const std::wstring& projectId, const std::wstring& filePath);

		Result ProjectGetFolderFirstPageMetadata(RmFinder& finder, bool* cancelFlag,
			const std::wstring& projectId,
			const std::wstring& folderId,
			__int64 lastModifiedTime,
			_Out_opt_ __int64* usage,
			_Out_opt_ __int64* quota,
			std::vector<RmProjectFile>& files);
		Result ProjectGetFolderNextPageMetadata(RmFinder& finder,
			bool* cancelFlag,
			const std::wstring& projectId,
			const std::wstring& folderId,
			__int64 lastModifiedTime,
			std::vector<RmProjectFile>& files);

        Result ProjectUploadFile(const std::wstring& projectId,
			const std::wstring& destDirPathId,
			const std::wstring& destFileName,
			const std::wstring& srcFilePath,
			const std::vector<std::wstring>& rights,
			const NXL::FileTags& tags,
			RmProjectFile& newFile,
			_In_opt_ bool* cancelFlag);
        Result ProjectDownloadFile(const std::wstring& projectId,
			const std::wstring& source,
			const std::wstring& targetFolder,
			bool failIfExists,
			_Out_ std::wstring& outFilePath,
			_In_opt_ bool* cancelFlag);
        Result ProjectDownloadFileForViewer(const std::wstring& projectId,
			const std::wstring& pathId,
			std::wstring& tmpFilePath,
			_In_opt_ bool* cancelFlag);
        Result ProjectGetFileMetadata(const std::wstring& projectId, const std::wstring& source, RmProjectFileMetadata& metadata);
		Result ProjectGetFirstPageMembers(RmFinder& finder, const std::wstring& projectId, bool returnPicture, std::vector<RmProjectMember>& members);
		Result ProjectGetNextPageMembers(RmFinder& finder, const std::wstring& projectId, bool returnPicture, std::vector<RmProjectMember>& members);
        Result ProjectRemoveMember(const std::wstring& projectId, const std::wstring& memberId);
        Result ProjectGetMemberInfo(const std::wstring& projectId, const std::wstring& memberId, RmProjectMember& member);
	protected:
        Result ProjectInterDownloadFile(const std::wstring& projectId,
			const std::wstring& pathId,
			const std::wstring& target,
			bool failIfExists,
			bool forViewer,
			_Out_ std::wstring& preferredFileName,
			_In_opt_ bool* cancelFlag);
		Result ProjectCreateTempFile(const std::wstring& projectId, const std::wstring& pathId, std::wstring& tmpFilePath);


    protected:
        RmSession();
        RmSession(const RmRouter& router);


    private:
		RmProduct		_product;
        RmRouter        _router;
        RmServer        _rmserver;
        RmDirs          _dirs;
        RmUser          _user;
        RmClientId      _clientId;
        RmRestClient    _restClient;
		RmViewerManager _viewerMgr;
		RmActivityLog	_actLog;
    };
}


#endif