
#include "stdafx.h"

#include <nx\rmc\project.h>
#include <nx\common\string.h>

#include <Shlobj.h>

using namespace NX;
using namespace NX::rapidjson;


RmProject::RmProject() : _creationTime(0), _totalMembers(0), _totalFiles(0), _ownedByMe(false), _trialEndTime(0)
{
}

RmProject::RmProject(const RmProject& rhs)
	: _id(rhs._id),
	_name(rhs._name),
	_description(rhs._description),
	_displayName(rhs._displayName),
	_owner(rhs._owner),
	_creationTime(rhs._creationTime),
	_totalMembers(rhs._totalMembers),
	_totalFiles(rhs._totalFiles),
	_ownedByMe(rhs._ownedByMe),
	_accountType(rhs._accountType),
	_trialEndTime(rhs._trialEndTime),
	_members(rhs._members)
{
}

RmProject::~RmProject()
{
}

RmProject& RmProject::operator = (const RmProject& rhs)
{
	if (this != &rhs)
	{
		_id = rhs._id;
		_name = rhs._name;
		_description = rhs._description;
		_displayName = rhs._displayName;
		_owner = rhs._owner;
		_creationTime = rhs._creationTime;
		_totalMembers = rhs._totalMembers;
		_totalFiles = rhs._totalFiles;
		_ownedByMe = rhs._ownedByMe;
		_accountType = rhs._accountType;
		_trialEndTime = rhs._trialEndTime;
		_members = rhs._members;
	}

	return *this;
}

void RmProject::Clear()
{
	_id.clear();
	_name.clear();
	_description.clear();
	_displayName.clear();
	_owner.Clear();
	_accountType.clear();
	_members.clear();
	_trialEndTime = 0;
	_creationTime = 0;
	_totalMembers = 0;
	_totalFiles = 0;
	_ownedByMe = false;
}

NX::rapidjson::PJSONVALUE RmProject::ToJsonValue() const
{
	PJSONOBJECT root = JsonValue::CreateObject();
	if (NULL == root)
		return NULL;

	root->set(L"id", JsonValue::CreateNumber(std::stoll(_id)));
	root->set(L"name", JsonValue::CreateString(_name));
	root->set(L"description", JsonValue::CreateString(_description));
	root->set(L"displayName", JsonValue::CreateString(_displayName));
	root->set(L"creationTime", JsonValue::CreateNumber(_creationTime));
	root->set(L"totalMembers", JsonValue::CreateNumber(_totalMembers));
	root->set(L"totalFiles", JsonValue::CreateNumber(_totalFiles));
	root->set(L"ownedByMe", JsonValue::CreateBool(_ownedByMe));
	root->set(L"accountType", JsonValue::CreateString(_accountType));
	root->set(L"trialEndTime", JsonValue::CreateNumber(_trialEndTime));

	PJSONOBJECT owner = JsonValue::CreateObject();
	if (NULL == owner) {
		delete root;
		root = NULL;
		return NULL;
	}
	root->set(L"owner", owner);
	owner->set(L"userId", JsonValue::CreateNumber(std::stoll(_owner._userId)));
	owner->set(L"name", JsonValue::CreateString(_owner._name));
	owner->set(L"email", JsonValue::CreateString(_owner._email));

	PJSONOBJECT members = JsonValue::CreateObject();
	if (NULL == members) {
		delete root;
		root = NULL;
		return NULL;
	}
	root->set(L"projectMembers", members);

	members->set(L"totalMembers", JsonValue::CreateNumber((int)_members.size()));

	PJSONARRAY memberArray = JsonValue::CreateArray();
	if (NULL == memberArray) {
		delete root;
		root = NULL;
		return NULL;
	}
	members->set(L"members", memberArray);

	for (const RmProjectMemberSimple& m : _members) {
		if (m.empty())
			continue;

		PJSONOBJECT pMember = JsonValue::CreateObject();
		if (NULL == pMember)
			continue;
		pMember->set(L"userId", JsonValue::CreateNumber(std::stoll(m.GetUserId())));
		pMember->set(L"displayName", JsonValue::CreateString(m.GetDisplayName()));
		pMember->set(L"email", JsonValue::CreateString(m.GetEmail()));
		pMember->set(L"creationTime", JsonValue::CreateNumber(m.GetCreationTime()));
		memberArray->push_back(pMember);
	}

	return root;
}

void RmProject::FromJsonValue(NX::rapidjson::PCJSONVALUE root)
{
	Clear();
	if (NULL == root)
		return;
	if (!root->IsObject())
		return;

	PCJSONVALUE pv = root->AsObject()->at(L"id");
	if (pv && pv->IsNumber()) {
		_id = NX::i64tos<wchar_t>(pv->AsNumber()->ToInt64());
	}
	if (_id.empty())
		return;

	pv = root->AsObject()->at(L"name");
	if (pv && pv->IsString()) {
		_name = pv->AsString()->GetString();
	}

	pv = root->AsObject()->at(L"description");
	if (pv && pv->IsString()) {
		_description = pv->AsString()->GetString();
	}

	pv = root->AsObject()->at(L"displayName");
	if (pv && pv->IsString()) {
		_displayName = pv->AsString()->GetString();
	}
	
	pv = root->AsObject()->at(L"creationTime");
	if (pv && pv->IsNumber()) {
		_creationTime = pv->AsNumber()->ToInt64();
	}
	
	pv = root->AsObject()->at(L"totalMembers");
	if (pv && pv->IsNumber()) {
		_totalMembers = pv->AsNumber()->ToInt();
	}
	
	pv = root->AsObject()->at(L"totalFiles");
	if (pv && pv->IsNumber()) {
		_totalFiles = pv->AsNumber()->ToInt();
	}
	
	pv = root->AsObject()->at(L"ownedByMe");
	if (pv && pv->IsBool()) {
		_ownedByMe = pv->AsBool()->GetBool();
	}
	
	pv = root->AsObject()->at(L"owner");
	if (pv && pv->IsObject()) {

		PCJSONVALUE po = pv->AsObject()->at(L"userId");
		if(po && po->IsNumber()) {
			_owner._userId = NX::i64tos<wchar_t>(po->AsNumber()->ToInt64());
		}

		po = pv->AsObject()->at(L"name");
		if (po && po->IsString()) {
			_owner._name = po->AsString()->GetString();
		}

		po = pv->AsObject()->at(L"email");
		if (po && po->IsString()) {
			_owner._email = po->AsString()->GetString();
		}
	}
	
	pv = root->AsObject()->at(L"accountType");
	if (pv && pv->IsString()) {
		_accountType = pv->AsString()->GetString();
	}
	
	pv = root->AsObject()->at(L"trialEndTime");
	if (pv && pv->IsNumber()) {
		_trialEndTime = pv->AsNumber()->ToInt64();
	}

	pv = root->AsObject()->at(L"projectMembers");
	if (pv && pv->IsObject()) {
		PCJSONVALUE pMembers = pv->AsObject()->at(L"members");
		if (IsValidJsonArray(pMembers)) {
			std::for_each(pMembers->AsArray()->cbegin(), pMembers->AsArray()->cend(), [&](PCJSONVALUE p) {


				if (!IsValidJsonObject(p))
					return;

				RmProjectMemberSimple ms;

				PCJSONVALUE pmv = p->AsObject()->at(L"userId");
				if (!IsValidJsonNumber(pmv))
					return;
				ms._userId = NX::i64tos<wchar_t>(pmv->AsNumber()->ToInt64());

				pmv = p->AsObject()->at(L"displayName");
				if (IsValidJsonString(pmv))
					ms._displayName = pmv->AsString()->GetString();

				pmv = p->AsObject()->at(L"email");
				if (IsValidJsonString(pmv))
					ms._email = pmv->AsString()->GetString();

				pmv = p->AsObject()->at(L"creationTime");
				if (IsValidJsonNumber(pmv))
					ms._creationTime = pmv->AsNumber()->ToInt64();

				_members.push_back(ms);
			});
		}
	}
}


RmProjectInvitation::RmProjectInvitation() : _inviteTime(0), _projecCreationTime(0)
{
}

RmProjectInvitation::RmProjectInvitation(const RmProjectInvitation& rhs)
	:
	_id(rhs._id),
	_code(rhs._code),
	_inviteeEmail(rhs._inviteeEmail),
	_inviterDisplayName(rhs._inviterDisplayName),
	_inviterEmail(rhs._inviterEmail),
	_inviteTime(rhs._inviteTime),
	_projectId(rhs._projectId),
	_projectName(rhs._projectName),
	_projectDescription(rhs._projectDescription),
	_projectDisplayName(rhs._projectDisplayName),
	_projecCreationTime(rhs._projecCreationTime)
{
}

RmProjectInvitation::~RmProjectInvitation()
{
}

RmProjectInvitation& RmProjectInvitation::operator = (const RmProjectInvitation& rhs)
{
	if (this != &rhs) {
		_id = rhs._id;
		_code = rhs._code;
		_inviteeEmail = rhs._inviteeEmail;
		_inviterDisplayName = rhs._inviterDisplayName;
		_inviterEmail = rhs._inviterEmail;
		_inviteTime = rhs._inviteTime;
		_projectId = rhs._projectId;
		_projectName = rhs._projectName;
		_projectDescription = rhs._projectDescription;
		_projectDisplayName = rhs._projectDisplayName;
		_projecCreationTime = rhs._projecCreationTime;
	}
	return *this;
}

void RmProjectInvitation::Clear()
{
	_id.clear();
	_code.clear();
	_inviteeEmail.clear();
	_inviterDisplayName.clear();
	_inviterEmail.clear();
	_inviteTime = 0;
	_projectId.clear();
	_projectName.clear();
	_projectDescription.clear();
	_projectDisplayName.clear();
	_projecCreationTime = 0;
}

NX::rapidjson::PJSONVALUE RmProjectInvitation::ToJsonValue() const
{
	PJSONOBJECT root = JsonValue::CreateObject();
	if (NULL == root)
		return NULL;

	root->set(L"invitationId", JsonValue::CreateNumber(std::stoll(_id)));
	root->set(L"code", JsonValue::CreateString(_code));
	root->set(L"inviteeEmail", JsonValue::CreateString(_inviteeEmail));
	root->set(L"inviterDisplayName", JsonValue::CreateString(_inviterDisplayName));
	root->set(L"inviterEmail", JsonValue::CreateString(_inviterEmail));
	root->set(L"inviteTime", JsonValue::CreateNumber(_inviteTime));

	PJSONOBJECT project = JsonValue::CreateObject();
	if (NULL == project) {
		delete root;
		root = NULL;
		return NULL;
	}
	root->set(L"project", project);
	project->set(L"id", JsonValue::CreateNumber(std::stoll(_projectId)));
	project->set(L"name", JsonValue::CreateString(_projectName));
	project->set(L"description", JsonValue::CreateString(_projectDescription));
	project->set(L"displayName", JsonValue::CreateString(_projectDisplayName));
	project->set(L"creationTime", JsonValue::CreateNumber(_projecCreationTime));

	return root;
}

void RmProjectInvitation::FromJsonValue(NX::rapidjson::PCJSONVALUE root)
{
	Clear();
	if (NULL == root)
		return;
	if (!root->IsObject())
		return;

	PCJSONVALUE pv = root->AsObject()->at(L"invitationId");
	if (pv && pv->IsNumber()) {
		_id = NX::i64tos<wchar_t>(pv->AsNumber()->ToInt64());
	}
	if (_id.empty())
		return;

	pv = root->AsObject()->at(L"code");
	if (pv && pv->IsString()) {
		_code = pv->AsString()->GetString();
	}

	pv = root->AsObject()->at(L"inviteeEmail");
	if (pv && pv->IsString()) {
		_inviteeEmail = pv->AsString()->GetString();
	}

	pv = root->AsObject()->at(L"inviterDisplayName");
	if (pv && pv->IsString()) {
		_inviterDisplayName = pv->AsString()->GetString();
	}

	pv = root->AsObject()->at(L"inviterEmail");
	if (pv && pv->IsString()) {
		_inviterEmail = pv->AsString()->GetString();
	}

	pv = root->AsObject()->at(L"inviteTime");
	if (pv && pv->IsNumber()) {
		_inviteTime = pv->AsNumber()->ToInt64();
	}

	pv = root->AsObject()->at(L"project");
	if (pv && pv->IsObject()) {

		PCJSONVALUE po = pv->AsObject()->at(L"id");
		if (po && po->IsNumber()) {
			_projectId = NX::i64tos<wchar_t>(po->AsNumber()->ToInt64());
		}

		po = pv->AsObject()->at(L"name");
		if (po && po->IsString()) {
			_projectName = po->AsString()->GetString();
		}

		po = pv->AsObject()->at(L"description");
		if (po && po->IsString()) {
			_projectDescription = po->AsString()->GetString();
		}

		po = pv->AsObject()->at(L"displayName");
		if (po && po->IsString()) {
			_projectDisplayName = po->AsString()->GetString();
		}

		po = pv->AsObject()->at(L"creationTime");
		if (po && po->IsNumber()) {
			_projecCreationTime = po->AsNumber()->ToInt64();
		}
	}
}



RmProjectFile::RmProjectFile()
	: _lastModifiedTime(0),
	_creationTime(0),
	_size(0),
	_folder(false)
{
}

RmProjectFile::RmProjectFile(const RmProjectFile& rhs)
	: _id(rhs._id),
	_duid(rhs._duid),
	_name(rhs._name),
	_path(rhs._path),
	_displayPath(rhs._displayPath),
	_lastModifiedTime(rhs._lastModifiedTime),
	_creationTime(rhs._creationTime),
	_size(rhs._size),
	_folder(rhs._folder),
	_owner(rhs._owner)
{
}

RmProjectFile::~RmProjectFile()
{
}

RmProjectFile& RmProjectFile::operator = (const RmProjectFile& rhs)
{
	if (this != &rhs) {
		_id = rhs._id;
		_duid = rhs._duid;
		_name = rhs._name;
		_path = rhs._path;
		_displayPath = rhs._displayPath;
		_lastModifiedTime = rhs._lastModifiedTime;
		_creationTime = rhs._creationTime;
		_size = rhs._size;
		_folder = rhs._folder;
		_owner = rhs._owner;
	}
	return *this;
}

void RmProjectFile::Clear()
{
	_id.clear();
	_duid.clear();
	_name.clear();
	_path.clear();
	_displayPath.clear();
	_lastModifiedTime = 0;
	_creationTime = 0;
	_size = 0;
	_folder = false;
	_owner.Clear();
}

NX::rapidjson::PJSONVALUE RmProjectFile::ToJsonValue() const
{
	PJSONOBJECT root = JsonValue::CreateObject();
	if (NULL == root)
		return NULL;

	root->set(L"id", JsonValue::CreateString(_id));
	root->set(L"duid", JsonValue::CreateString(_duid));
	root->set(L"pathDisplay", JsonValue::CreateString(_displayPath));
	root->set(L"pathId", JsonValue::CreateString(_path));
	root->set(L"fileName", JsonValue::CreateString(_name));
	root->set(L"lastModifiedTime", JsonValue::CreateNumber(_lastModifiedTime));
	root->set(L"creationTime", JsonValue::CreateNumber(_creationTime));
	root->set(L"fileSize", JsonValue::CreateNumber(_size));
	root->set(L"folder", JsonValue::CreateBool(_folder));

	PJSONOBJECT owner = JsonValue::CreateObject();
	if (NULL == owner) {
		delete root;
		root = NULL;
		return NULL;
	}
	root->set(L"owner", owner);
	owner->set(L"userId", JsonValue::CreateNumber(std::stoll(_owner.GetUserId())));
	owner->set(L"displayName", JsonValue::CreateString(_owner.GetName()));
	owner->set(L"email", JsonValue::CreateString(_owner.GetEmail()));

	return root;
}

void RmProjectFile::FromJsonValue(NX::rapidjson::PCJSONVALUE root)
{
	Clear();
	if (NULL == root)
		return;
	if (!root->IsObject())
		return;

	PCJSONVALUE pv = root->AsObject()->at(L"id");
	if (pv && pv->IsString()) {
		_id = pv->AsString()->GetString();
	}
	if (_id.empty())
		return;

	pv = root->AsObject()->at(L"pathId");
	if (pv && pv->IsString()) {
		_path = pv->AsString()->GetString();
	}
	if (_path.empty())
		return;


	pv = root->AsObject()->at(L"pathDisplay");
	if (pv && pv->IsString()) {
		_displayPath = pv->AsString()->GetString();
	}

	pv = root->AsObject()->at(L"fileName");
	if (pv && pv->IsString()) {
		_name = pv->AsString()->GetString();
	}

	pv = root->AsObject()->at(L"duid");
	if (pv && pv->IsString()) {
		_duid = pv->AsString()->GetString();
	}

	pv = root->AsObject()->at(L"lastModified");
	if (pv && pv->IsNumber()) {
		_lastModifiedTime = pv->AsNumber()->ToInt64();
	}

	pv = root->AsObject()->at(L"creationTime");
	if (pv && pv->IsNumber()) {
		_creationTime = pv->AsNumber()->ToInt64();
	}

	pv = root->AsObject()->at(L"size");
	if (pv && pv->IsNumber()) {
		_size = pv->AsNumber()->ToInt64();
	}

	pv = root->AsObject()->at(L"folder");
	if (pv && pv->IsBool()) {
		_folder = pv->AsBool()->GetBool();
	}

	pv = root->AsObject()->at(L"owner");
	if (pv && pv->IsObject()) {

		PCJSONVALUE po = pv->AsObject()->at(L"userId");
		if (po && po->IsNumber()) {
			_owner._userId = NX::i64tos<wchar_t>(po->AsNumber()->ToInt64());
		}

		po = pv->AsObject()->at(L"displayName");
		if (po && po->IsString()) {
			_owner._name = po->AsString()->GetString();
		}

		po = pv->AsObject()->at(L"email");
		if (po && po->IsString()) {
			_owner._email = po->AsString()->GetString();
		}
	}
}


RmProjectFileMetadata::RmProjectFileMetadata() : _size(0), _lastModifiedTime(0), _isNXL(false), _ownedByMe(false)
{
}

RmProjectFileMetadata::RmProjectFileMetadata(const RmProjectFileMetadata& rhs)
	: _path(rhs._path), _displayPath(rhs._displayPath), _name(rhs._name), _size(rhs._size), _lastModifiedTime(rhs._lastModifiedTime), _rights(rhs._rights), _isNXL(rhs._isNXL), _ownedByMe(rhs._ownedByMe)
{
}

RmProjectFileMetadata::~RmProjectFileMetadata()
{
}

RmProjectFileMetadata& RmProjectFileMetadata::operator = (const RmProjectFileMetadata& rhs)
{
	if (this != &rhs) {
		_name = rhs._name;
		_path = rhs._path;
		_displayPath = rhs._displayPath;
		_lastModifiedTime = rhs._lastModifiedTime;
		_size = rhs._size;
		_rights = rhs._rights;
		_isNXL = rhs._isNXL;
		_ownedByMe = rhs._ownedByMe;
	}
	return *this;
}

void RmProjectFileMetadata::Clear()
{
	_name.clear();
	_path.clear();
	_displayPath.clear();
	_lastModifiedTime = 0;
	_size = 0;
	_isNXL = false;
	_ownedByMe = false;
	_rights.clear();
}



RmProjectMember::RmProjectMember() : _creationTime(0)
{
}

RmProjectMember::RmProjectMember(const RmProjectMember& rhs)
	:
	_userId(rhs._userId),
	_displayName(rhs._displayName),
	_email(rhs._email),
	_inviterDisplayName(rhs._inviterDisplayName),
	_inviterEmail(rhs._inviterEmail),
	_creationTime(rhs._creationTime),
	_picture(rhs._picture)
{
}

RmProjectMember::~RmProjectMember()
{
}

RmProjectMember& RmProjectMember::operator = (const RmProjectMember& rhs)
{
	if (this != &rhs) {
		_userId = rhs._userId;
		_displayName = rhs._displayName;
		_email = rhs._email;
		_inviterDisplayName = rhs._inviterDisplayName;
		_inviterEmail = rhs._inviterEmail;
		_creationTime = rhs._creationTime;
		_picture = rhs._picture;
	}
	return *this;
}

void RmProjectMember::Clear()
{
	_userId.clear();
	_displayName.clear();
	_email.clear();
	_inviterDisplayName.clear();
	_inviterEmail.clear();
	_creationTime = 0;
	_picture.clear();
}

NX::rapidjson::PJSONVALUE RmProjectMember::ToJsonValue() const
{
	PJSONOBJECT root = JsonValue::CreateObject();
	if (NULL == root)
		return NULL;

	root->set(L"userId", JsonValue::CreateNumber(std::stoll(_userId)));
	root->set(L"displayName", JsonValue::CreateString(_displayName));
	root->set(L"email", JsonValue::CreateString(_email));
	root->set(L"inviterDisplayName", JsonValue::CreateString(_inviterDisplayName));
	root->set(L"inviterEmail", JsonValue::CreateString(_inviterEmail));
	root->set(L"creationTime", JsonValue::CreateNumber(_creationTime));
	if (!_picture.empty()) {
		const std::string& s = NX::conv::Base64Encode(_picture);
		root->set(L"picture", JsonValue::CreateString(s));
	}

	return root;
}

void RmProjectMember::FromJsonValue(NX::rapidjson::PCJSONVALUE root)
{
	Clear();
	if (NULL == root)
		return;
	if (!root->IsObject())
		return;

	PCJSONVALUE pv = root->AsObject()->at(L"userId");
	if (pv && pv->IsNumber()) {
		_userId = NX::i64tos<wchar_t>(pv->AsNumber()->ToInt64());
	}
	if (_userId.empty())
		return;

	pv = root->AsObject()->at(L"displayName");
	if (pv && pv->IsString()) {
		_displayName = pv->AsString()->GetString();
	}

	pv = root->AsObject()->at(L"email");
	if (pv && pv->IsString()) {
		_email = pv->AsString()->GetString();
	}

	pv = root->AsObject()->at(L"inviterDisplayName");
	if (pv && pv->IsString()) {
		_inviterDisplayName = pv->AsString()->GetString();
	}

	pv = root->AsObject()->at(L"inviterEmail");
	if (pv && pv->IsString()) {
		_inviterEmail = pv->AsString()->GetString();
	}

	pv = root->AsObject()->at(L"creationTime");
	if (pv && pv->IsNumber()) {
		_creationTime = pv->AsNumber()->ToInt64();
	}

	pv = root->AsObject()->at(L"picture");
	if (pv && pv->IsString()) {
		const std::wstring& base64 = pv->AsString()->GetString();
		if (!base64.empty()) {
			_picture = NX::conv::Base64Decode(base64);
		}
	}
}
