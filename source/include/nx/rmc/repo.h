

#ifndef __LIBRMC_REPOSITORY_H__
#define __LIBRMC_REPOSITORY_H__


#include <nx\common\result.h>
#include <nx\common\rapidjson.h>
#include <nx\rmc\rest.h>

#include <string>
#include <map>

namespace NX {

	class RmMyDriveUsage
	{
	public:
		RmMyDriveUsage();
		RmMyDriveUsage(__int64 quota, __int64 used, __int64 usedByMyVault);
		RmMyDriveUsage(const RmMyDriveUsage& rhs);
		~RmMyDriveUsage();

		RmMyDriveUsage& operator = (const RmMyDriveUsage& rhs);

		inline bool empty() const { return (0 == _quota); }
		inline __int64 GetQuota() const { return _quota; }
		inline __int64 GetFreeBytes() const { return (_quota - _used); }
		inline __int64 GetUsedBytes() const { return _used; }
		inline __int64 GetUsedBytesByMyVault() const { return _usedByMyVault; }

	private:
		__int64 _quota;
		__int64 _used;
		__int64 _usedByMyVault;
		friend class RmRestClient;
	};

    class RmRepository
    {
    public:
        typedef enum REPO_TYPE {
            UNKNOWNREPO = 0,
            MYDRIVE,
            MYVAULT,
            GOOGLEDRIVE,
            ONEDRIVE,
            DROPBOX,
            SHAREPOINTONLINE,
            SHAREPOINTONPREM,
            SHAREPOINTCROSSLAUNCH,
            SHAREPOINTONLINE_CROSSLAUNCH
        } REPO_TYPE;

    public:
        RmRepository();
        RmRepository(const RmRepository& rhs);
        ~RmRepository();

        RmRepository& operator = (const RmRepository& rhs);

        bool Empty() const { return _id.empty(); }
        inline REPO_TYPE GetType() const { return _type; }
        inline std::wstring GetTypeName() const { return TypeToString(_type); }
        inline const std::wstring& GetId() const { return _id; }
        inline const std::wstring& GetName() const { return _name; }
        inline const std::wstring& GetAccountName() const { return _accountName; }
        inline const std::wstring& GetAccountId() const { return _accountId; }
        inline const std::wstring& GetToken() const { return _token; }
        inline const std::wstring& GetAccessToken() const { return _accessToken; }
        inline void SetAccessToken(const std::wstring& accessToken) { _accessToken = accessToken; }
        inline __int64 GetCreationTime() const { return _creationTime; }
        inline bool IsShared() const { return _shared; }
        
        static REPO_TYPE StringToType(const std::wstring& s);
        static std::wstring TypeToString(REPO_TYPE t);

    private:
        REPO_TYPE       _type;
        std::wstring    _id;
        std::wstring    _name;
        std::wstring    _accountName;
        std::wstring    _accountId;
        std::wstring    _token;
        std::wstring    _accessToken;
        __int64         _creationTime;
        bool            _shared;

        friend class RmSession;
        friend class RmRestClient;
    };

    class RmRepoFile
    {
    public:
        RmRepoFile();
		RmRepoFile(const std::wstring& id, const std::wstring& path, const std::wstring& name, bool folder, __int64 size, __int64 time);
        RmRepoFile(const RmRepoFile& rhs);
        ~RmRepoFile();

        RmRepoFile& operator = (const RmRepoFile& rhs);

        inline bool Empty() const { return _id.empty(); }
        inline const std::wstring& GetId() const { return _id; }
        inline const std::wstring& GetPath() const { return _path; }
        inline const std::wstring& GetName() const { return _name; }
        inline bool IsFolder() const { return _folder; }
        inline __int64 GetSize() const { return _size; }
        inline __int64 GetLastModifiedTime() const { return _time; }
        inline const std::vector<std::pair<std::wstring, std::wstring>>& GetProperties() const { return _props; }

    private:
        std::wstring    _id;
        std::wstring    _path;
        std::wstring    _name;
        bool            _folder;
        __int64         _size;
        __int64         _time;
        std::vector<std::pair<std::wstring, std::wstring>> _props;

        friend class RmRestClient;
        friend class RmSession;
    };

	class RmMyVaultSourceFile
	{
	public:
		RmMyVaultSourceFile();
		RmMyVaultSourceFile(const RmMyVaultSourceFile& rhs);
		RmMyVaultSourceFile(RmMyVaultSourceFile&& rhs);
		virtual ~RmMyVaultSourceFile();
		RmMyVaultSourceFile& operator  = (const RmMyVaultSourceFile& rhs);
		RmMyVaultSourceFile& operator  = (RmMyVaultSourceFile&& rhs);

		void clear();
		bool empty();

		Result FromJson(rapidjson::PCJSONOBJECT root);

		inline const std::wstring& GetRepoType() const { return _repoType; }
		inline const std::wstring& GetRepoName() const { return _repoName; }
		inline const std::wstring& GetRepoId() const { return _repoId; }
		inline const std::wstring& GetId() const { return _id; }
		inline const std::wstring& GetDisplayPath() const { return _displayPath; }

	private:
		std::wstring	_repoType;
		std::wstring	_repoName;
		std::wstring	_repoId;
		std::wstring	_id;
		std::wstring	_displayPath;
	};

	class RmMyVaultFile
	{
	public:
		RmMyVaultFile();
		RmMyVaultFile(const RmMyVaultFile& rhs);
		virtual ~RmMyVaultFile();

		RmMyVaultFile& operator = (const RmMyVaultFile& rhs);

		void Clear();

		NX::rapidjson::PJSONVALUE ToJsonValue() const;
		void FromJsonValue(NX::rapidjson::PCJSONVALUE root);
		template<typename T>
		std::basic_string<T> ToJsonString() const
		{
			std::shared_ptr<NX::rapidjson::JsonValue> root(ToJsonValue());
			if (NULL == root)
				return std::basic_string<T>();
			NX::rapidjson::JsonStringWriter<T> writer;
			return writer.Write(root.get());
		}
		template<typename T>
		void FromJsonString(const std::basic_string<T>& s)
		{
			NX::rapidjson::JsonDocument doc;
			if (!doc.LoadJsonString<char>(s))
				return RESULT(ERROR_INVALID_DATA);
			FromJsonObject(doc.GetRoot());
		}

		inline bool Empty() const { return (_duid.empty() || _path.empty()); }
		inline const std::wstring& GetDuid() const { return _duid; }
		inline const std::wstring& GetName() const { return _name; }
		inline const std::wstring& GetPath() const { return _path; }
		inline const std::wstring& GetDisplayPath() const { return _displayPath; }
		inline const std::wstring& GetRepoId() const { return _repoId; }
		inline const std::vector<std::wstring>& GetSharedWith() const { return _sharedWith; }
		inline const RmMyVaultSourceFile& GetSourceFileInfo() const { return _sourceFileInfo; }
		inline __int64 GetSharedTime() const { return _sharedTime; }
		inline __int64 GetSize() const { return _size; }
		inline bool IsRevoked() const { return _revoked; }
		inline bool IsDeleted() const { return _deleted; }
		inline bool IsShared() const { return _shared; }

		inline std::wstring GetSerializedSharedWith() const
		{
			std::wstring s;
			for (auto recipient : _sharedWith) {
				if (!s.empty()) s.append(L",");
				s.append(recipient);
			}
			return s;
		}

	private:
        std::wstring    _name;
        std::wstring    _duid;
        std::wstring    _path;
        std::wstring    _displayPath;
        std::wstring    _repoId;
        std::vector<std::wstring> _sharedWith;
        __int64         _sharedTime;
        __int64         _size;
        bool            _revoked;
        bool            _deleted;
        bool            _shared;
		RmMyVaultSourceFile _sourceFileInfo;
        std::map<std::wstring, std::wstring> _metaData;
	};
	
	class RmMyVaultFileMetadata
	{
	public:
		RmMyVaultFileMetadata();
		RmMyVaultFileMetadata(const RmMyVaultFileMetadata& rhs);
		virtual ~RmMyVaultFileMetadata();

		RmMyVaultFileMetadata& operator = (const RmMyVaultFileMetadata& rhs);

		void Clear();

		NX::rapidjson::PJSONVALUE ToJsonValue() const;
		void FromJsonValue(NX::rapidjson::PCJSONVALUE root);
		template<typename T>
		std::basic_string<T> ToJsonString() const
		{
			std::shared_ptr<NX::rapidjson::JsonValue> root(ToJsonValue());
			if (NULL == root)
				return std::basic_string<T>();
			NX::rapidjson::JsonStringWriter<T> writer;
			return writer.Write(root.get());
		}
		template<typename T>
		void FromJsonString(const std::basic_string<T>& s)
		{
			NX::rapidjson::JsonDocument doc;
			if (!doc.LoadJsonString<char>(s))
				return RESULT(ERROR_INVALID_DATA);
			FromJsonObject(doc.GetRoot());
		}

		inline bool Empty() const { return _name.empty(); }
		inline const std::wstring& GetName() const { return _name; }
		inline const std::wstring& GetLink() const { return _link; }
		inline const std::vector<std::wstring>& GetRecipients() const { return _recipients; }
		inline const std::vector<std::wstring>& GetRights() const { return _rights; }
		inline __int64 GetProtectedTime() const { return _protectedTime; }
		inline __int64 GetSharedTime() const { return _sharedTime; }

	private:
		std::wstring    _name;
		std::wstring    _link;
		std::vector<std::wstring>    _recipients;
		std::vector<std::wstring>    _rights;
		__int64         _protectedTime;
		__int64         _sharedTime;
	};

	class RmRepoMarkedFile
	{
	public:
		RmRepoMarkedFile();
		RmRepoMarkedFile(const std::wstring& fileId, const std::wstring& filePath, const std::wstring& parentFileId);
		RmRepoMarkedFile(const RmRepoMarkedFile& rhs);
		virtual ~RmRepoMarkedFile();

		RmRepoMarkedFile& operator = (const RmRepoMarkedFile& rhs);

		inline bool Empty() const { return (_fileId.empty() || _filePath.empty()); }
		inline const std::wstring& GetFileId() const { return _fileId; }
		inline const std::wstring& GetFilePath() const { return _filePath; }
		inline const std::wstring& GetParentFileId() const { return _parentFileId; }

	private:
		std::wstring	_fileId;
		std::wstring	_filePath;
		std::wstring	_parentFileId;
	};

}


#endif