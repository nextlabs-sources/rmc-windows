

#ifndef __LIBRMC_PROJECT_H__
#define __LIBRMC_PROJECT_H__


#include <nx\common\result.h>
#include <nx\common\rapidjson.h>
#include <nx\rmc\rest.h>

#include <string>
#include <map>

namespace NX {

	class RmProject;

#define PROJECT_TYPE_TRIAL		L"PROJECT_TRIAL"
#define PROJECT_TYPE_NORMAL		L"PROJECT"
#define PROJECT_TYPE_ENTERPRISE	L"ENTERPRISE"
	
	class RmProjectOwner
	{
	public:
		RmProjectOwner() {}
		RmProjectOwner(const RmProjectOwner& rhs) : _userId(rhs._userId), _name(rhs._name), _email(rhs._email) {}
		~RmProjectOwner() {}

		RmProjectOwner& operator = (const RmProjectOwner& rhs)
		{
			if (this != &rhs) {
				_userId = rhs._userId; _name = rhs._name; _email = rhs._email;
			}
			return *this;
		}

		void Clear()
		{
			_userId.clear();
			_name.clear();
			_email.clear();
		}

		bool Empty() const { return (_userId.empty() || _email.empty()); }
		inline const std::wstring& GetUserId() const { return _userId; }
		inline const std::wstring& GetName() const { return _name; }
		inline const std::wstring& GetEmail() const { return _email; }

	private:
		std::wstring    _userId;
		std::wstring    _name;
		std::wstring    _email;
		friend class RmProject;
		friend class RmProjectFile;
		friend class RmRestClient;
	};

	class RmProjectMemberSimple
	{
	public:
		RmProjectMemberSimple() : _creationTime(0) {}
		RmProjectMemberSimple(const RmProjectMemberSimple& rhs)
			: _userId(rhs._userId), _displayName(rhs._displayName), _email(rhs._email), _creationTime(rhs._creationTime)
		{
		}
		virtual ~RmProjectMemberSimple() {}
		RmProjectMemberSimple& operator = (const RmProjectMemberSimple& rhs) {
			if (this != &rhs) {
				_userId = rhs._userId;
				_displayName = rhs._displayName;
				_email = rhs._email;
				_creationTime = rhs._creationTime;
			}
			return *this;
		}

		void Clear() {
			_userId.clear();
			_displayName.clear();
			_email.clear();
			_creationTime = 0;
		}

		inline bool empty() const { return _userId.empty(); }
		inline const std::wstring& GetUserId() const { return _userId; }
		inline const std::wstring& GetDisplayName() const { return _displayName; }
		inline const std::wstring& GetEmail() const { return _email; }
		inline __int64 GetCreationTime() const { return _creationTime; }

	private:
		std::wstring	_userId;
		std::wstring	_displayName;
		std::wstring	_email;
		__int64			_creationTime;

		friend class RmProject;
	};

	class RmProject
	{
	public:
		RmProject();
		RmProject(const RmProject& rhs);
		~RmProject();

		RmProject& operator = (const RmProject& rhs);

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

		bool Empty() const { return _id.empty(); }
		inline const std::wstring& GetId() const { return _id; }
		inline const std::wstring& GetName() const { return _name; }
		inline const std::wstring& GetDescription() const { return _description; }
		inline const std::wstring& GetDisplayName() const { return _displayName; }
		inline __int64 GetCreationTime() const { return _creationTime; }
		inline int GetMemberCount() const { return _totalMembers; }
		inline const std::vector<RmProjectMemberSimple>& GetMembers() const { return _members; }
		inline int GetFileCount() const { return _totalFiles; }
		inline bool OwnedByMe() const { return _ownedByMe; }
		inline const RmProjectOwner& GetOwner() const { return _owner; }
		inline const std::wstring& GetAccountType() const { return _accountType; }
		inline __int64 GetTrialEndTime() const { return _trialEndTime; }

	private:
		std::wstring    _id;
		std::wstring    _name;
		std::wstring    _description;
		std::wstring    _displayName;
		__int64         _creationTime;
		int				_totalMembers;
		int				_totalFiles;
		bool            _ownedByMe;
		RmProjectOwner	_owner;

		std::wstring    _accountType;
		__int64			_trialEndTime;
		std::vector<RmProjectMemberSimple> _members;

		friend class RmRestClient;
	};

	class RmProjectInvitation
	{
	public:
		RmProjectInvitation();
		RmProjectInvitation(const RmProjectInvitation& rhs);
		~RmProjectInvitation();

		RmProjectInvitation& operator = (const RmProjectInvitation& rhs);

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


		inline bool Empty() const { return _id.empty(); }
		inline const std::wstring& GetId() const { return _id; }
		inline const std::wstring& GetCode() const { return _code; }
		inline const std::wstring& GetInviteeEmail() const { return _inviteeEmail; }
		inline const std::wstring& GetInviterDisplayName() const { return _inviterDisplayName; }
		inline const std::wstring& GetInviterEmail() const { return _inviterEmail; }
		inline __int64 GetInviteTime() const { return _inviteTime; }
		inline const std::wstring& GetProjectId() const { return _projectId; }
		inline const std::wstring& GetProjectName() const { return _projectName; }
		inline const std::wstring& GetProjectDisplayName() const { return _projectDisplayName; }
		inline const std::wstring& GetProjectDescription() const { return _projectDescription; }
		inline __int64 GetProjecCreationTime() const { return _projecCreationTime; }

	private:
		std::wstring    _id;
		std::wstring    _code;
		std::wstring    _inviteeEmail;
		std::wstring    _inviterDisplayName;
		std::wstring    _inviterEmail;
		__int64         _inviteTime;
		std::wstring    _projectId;
		std::wstring    _projectName;
		std::wstring    _projectDescription;
		std::wstring    _projectDisplayName;
		__int64         _projecCreationTime;

		friend class RmRestClient;
	};


	class RmProjectFile
	{
	public:
		RmProjectFile();
		RmProjectFile(const RmProjectFile& rhs);
		virtual ~RmProjectFile();

		RmProjectFile& operator = (const RmProjectFile& rhs);

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

		inline bool Empty() const { return (_id.empty() || _path.empty()); }
		inline const std::wstring& GetId() const { return _id; }
		inline const std::wstring& GetDuid() const { return _duid; }
		inline const std::wstring& GetName() const { return _name; }
		inline const std::wstring& GetPath() const { return _path; }
		inline const std::wstring& GetDisplayPath() const { return _displayPath; }
		inline __int64 GetLastModifiedTime() const { return _lastModifiedTime; }
		inline __int64 GetCreationTime() const { return _creationTime; }
		inline __int64 GetSize() const { return _size; }
		inline bool IsFolder() const { return _folder; }
		inline const RmProjectOwner& GetOwner() const { return _owner; }

	private:
		std::wstring    _id;
		std::wstring    _duid;
		std::wstring    _name;
		std::wstring    _path;
		std::wstring    _displayPath;
		__int64         _lastModifiedTime;
		__int64         _creationTime;
		__int64         _size;
		bool            _folder;
		RmProjectOwner	_owner;

		friend class RmRestClient;
	};

	class RmProjectFileMetadata
	{
	public:
		RmProjectFileMetadata();
		RmProjectFileMetadata(const RmProjectFileMetadata& rhs);
		virtual ~RmProjectFileMetadata();

		RmProjectFileMetadata& operator = (const RmProjectFileMetadata& rhs);

		void Clear();

		inline bool Empty() const { return _path.empty(); }
		inline const std::wstring& GetName() const { return _name; }
		inline const std::wstring& GetPath() const { return _path; }
		inline const std::wstring& GetDisplayPath() const { return _displayPath; }
		inline __int64 GetLastModifiedTime() const { return _lastModifiedTime; }
		inline __int64 GetSize() const { return _size; }
		inline const std::vector<std::wstring>& GetRights() const { return _rights; }
		inline bool IsNXL() const { return _isNXL; }
		inline bool OwnedByMe() const { return _ownedByMe; }

	private:
		std::wstring    _path;
		std::wstring    _displayPath;
		std::wstring    _name;
		__int64         _size;
		__int64         _lastModifiedTime;
		bool			_isNXL;
		bool			_ownedByMe;
		std::vector<std::wstring> _rights;

		friend class RmRestClient;
	};

	class RmProjectMember
	{
	public:
		RmProjectMember();
		RmProjectMember(const RmProjectMember& rhs);
		virtual ~RmProjectMember();

		RmProjectMember& operator = (const RmProjectMember& rhs);

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

		inline bool Empty() const { return _userId.empty(); }
		inline const std::wstring& GetUserId() const { return _userId; }
		inline const std::wstring& GetDisplayName() const { return _displayName; }
		inline const std::wstring& GetEmail() const { return _email; }
		inline const std::wstring& GetInviterDisplayName() const { return _inviterDisplayName; }
		inline const std::wstring& GetInviterEmail() const { return _inviterEmail; }
		inline __int64 GetCreationTime() const { return _creationTime; }
		inline const std::vector<unsigned char>& GetPicture() const { return _picture; }
		inline void FreePicture() { _picture.clear(); }

	private:
		std::wstring    _userId;
		std::wstring    _displayName;
		std::wstring    _email;
		std::wstring    _inviterDisplayName;
		std::wstring    _inviterEmail;
		__int64         _creationTime;
		std::vector<unsigned char> _picture;

		friend class RmRestClient;
	};

}


#endif