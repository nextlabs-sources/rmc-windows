
#include "stdafx.h"

#include <nx\rmc\repo.h>
#include <nx\common\string.h>

#include <Shlobj.h>

using namespace NX;
using namespace NX::rapidjson;


RmMyDriveUsage::RmMyDriveUsage()
	: _quota(0), _used(0), _usedByMyVault(0)
{
}

RmMyDriveUsage::RmMyDriveUsage(__int64 quota, __int64 used, __int64 usedByMyVault)
	: _quota(quota), _used(used), _usedByMyVault(usedByMyVault)
{
}

RmMyDriveUsage::RmMyDriveUsage(const RmMyDriveUsage& rhs)
	: _quota(rhs._quota), _used(rhs._used), _usedByMyVault(rhs._usedByMyVault)
{
}

RmMyDriveUsage::~RmMyDriveUsage()
{
}

RmMyDriveUsage& RmMyDriveUsage::operator = (const RmMyDriveUsage& rhs)
{
	if (this != &rhs) {
		_quota = rhs._quota;
		_used = rhs._used;
		_usedByMyVault = rhs._usedByMyVault;
	}
	return *this;
}


RmRepository::RmRepository() : _type(UNKNOWNREPO), _creationTime(0), _shared(false)
{
}

RmRepository::RmRepository(const RmRepository& rhs)
    : _type(rhs._type),
    _id(rhs._id),
    _name(rhs._name),
    _accountName(rhs._accountName),
    _accountId(rhs._accountId),
    _token(rhs._token),
    _accessToken(rhs._accessToken),
    _creationTime(rhs._creationTime),
    _shared(rhs._shared)
{
}

RmRepository::~RmRepository()
{
}

RmRepository& RmRepository::operator = (const RmRepository& rhs)
{
    if (this != &rhs)
    {
        _type = rhs._type;
        _id = rhs._id;
        _name = rhs._name;
        _accountName = rhs._accountName;
        _accountId = rhs._accountId;
        _token = rhs._token;
        _accessToken = rhs._accessToken;
        _creationTime = rhs._creationTime;
        _shared = rhs._shared;
    }

    return *this;
}

static const std::wstring RepoNameMyDrive(L"MYDRIVE");
static const std::wstring RepoNameMyVault(L"MYVAULT");
static const std::wstring RepoNameGoogleDrive(L"GOOGLE_DRIVE");
static const std::wstring RepoNameOneDrive(L"ONE_DRIVE");
static const std::wstring RepoNameDropBox(L"DROPBOX");
static const std::wstring RepoNameSpOnline(L"SHAREPOINT_ONLINE");
static const std::wstring RepoNameSpOnPrem(L"SHAREPOINT_ONPREMISE");
static const std::wstring RepoNameSpCrossLaunch(L"SHAREPOINT_CROSSLAUNCH");
static const std::wstring RepoNameSpOnlineCrossLaunch(L"SHAREPOINT_ONLINE_CROSSLAUNCH");

RmRepository::REPO_TYPE RmRepository::StringToType(const std::wstring& s)
{
    
    if (0 == NX::icompare(s, RepoNameMyDrive)) {
        return MYDRIVE;
    }
    else if (0 == NX::icompare(s, RepoNameMyVault)) {
        return MYVAULT;
    }
    else if (0 == NX::icompare(s, RepoNameGoogleDrive)) {
        return GOOGLEDRIVE;
    }
    else if (0 == NX::icompare(s, RepoNameDropBox)) {
        return DROPBOX;
    }
    else if (0 == NX::icompare(s, RepoNameOneDrive)) {
        return ONEDRIVE;
    }
    else if (0 == NX::icompare(s, RepoNameSpOnline)) {
        return SHAREPOINTONLINE;
    }
    else if (0 == NX::icompare(s, RepoNameSpOnPrem)) {
        return SHAREPOINTONPREM;
    }
    else if (0 == NX::icompare(s, RepoNameSpCrossLaunch)) {
        return SHAREPOINTCROSSLAUNCH;
    }
    else if (0 == NX::icompare(s, RepoNameSpOnlineCrossLaunch)) {
        return SHAREPOINTONLINE_CROSSLAUNCH;
    }
    else {
        return UNKNOWNREPO;
    }
}

std::wstring RmRepository::TypeToString(RmRepository::REPO_TYPE t)
{
    switch (t)
    {
    case MYDRIVE:
        return RepoNameMyDrive;
    case MYVAULT:
        return RepoNameMyVault;
    case GOOGLEDRIVE:
        return RepoNameGoogleDrive;
    case ONEDRIVE:
        return RepoNameOneDrive;
    case DROPBOX:
        return RepoNameDropBox;
    case SHAREPOINTONLINE:
        return RepoNameSpOnline;
    case SHAREPOINTONPREM:
        return RepoNameSpOnPrem;
    case SHAREPOINTCROSSLAUNCH:
        return RepoNameSpCrossLaunch;
    case SHAREPOINTONLINE_CROSSLAUNCH:
        return RepoNameSpOnlineCrossLaunch;
    default:
        break;
    }
    return std::wstring();
}



RmRepoFile::RmRepoFile() : _folder(false), _size(0), _time(0)
{
}

RmRepoFile::RmRepoFile(const std::wstring& id, const std::wstring& path, const std::wstring& name, bool folder, __int64 size, __int64 time)
    : _id(id), _path(path), _name(name), _folder(folder), _size(size), _time(time)
{
}

RmRepoFile::RmRepoFile(const RmRepoFile& rhs)
    : _id(rhs._id), _path(rhs._path), _name(rhs._name), _folder(rhs._folder),
    _size(rhs._size), _time(rhs._time), _props(rhs._props)
{
}

RmRepoFile::~RmRepoFile()
{
}

RmRepoFile& RmRepoFile::operator = (const RmRepoFile& rhs)
{
    if (this != &rhs) {
		_id = rhs._id;
		_path = rhs._path;
        _name = rhs._name;
        _folder = rhs._folder;
        _size = rhs._size;
		_time = rhs._time;
        _props = rhs._props;
    }
    return *this;
}



RmMyVaultSourceFile::RmMyVaultSourceFile()
{
}

RmMyVaultSourceFile::RmMyVaultSourceFile(const RmMyVaultSourceFile& rhs)
	: _id(rhs._id),_repoType(rhs._repoType),_repoName(rhs._repoName),_repoId(rhs._repoId),_displayPath(rhs._displayPath)
{
}

RmMyVaultSourceFile::~RmMyVaultSourceFile()
{
}

RmMyVaultSourceFile& RmMyVaultSourceFile::operator  = (const RmMyVaultSourceFile& rhs)
{
	if (this != &rhs) {
		_id =rhs._id;
		_repoType = rhs._repoType;
		_repoName = rhs._repoName;
		_repoId = rhs._repoId;
		_displayPath = rhs._displayPath;
	}
	return *this;
}

void RmMyVaultSourceFile::clear()
{
	_id.clear();
	_repoType.clear();
	_repoName.clear();
	_repoId.clear();
	_displayPath.clear();
}

bool RmMyVaultSourceFile::empty()
{
	return _id.empty();
}

Result RmMyVaultSourceFile::FromJson(PCJSONOBJECT root)
{
	Result res = RESULT(0);

	clear();
	if (root == NULL)
		return RESULT(ERROR_INVALID_DATA);
	if (!root->IsObject())
		return RESULT(ERROR_INVALID_DATA);

	PCJSONVALUE pv = root->at(L"SourceFilePathId");
	if (pv && pv->IsString())
		_id = pv->AsString()->GetString();

	pv = root->at(L"SourceFilePathDisplay");
	if (pv && pv->IsString())
		_displayPath = pv->AsString()->GetString();

	pv = root->at(L"SourceRepoId");
	if (pv && pv->IsString())
		_repoId = pv->AsString()->GetString();

	pv = root->at(L"SourceRepoType");
	if (pv && pv->IsString())
		_repoType = pv->AsString()->GetString();

	pv = root->at(L"SourceRepoName");
	if (pv && pv->IsString())
		_repoName = pv->AsString()->GetString();

	return RESULT(0);
}



RmMyVaultFile::RmMyVaultFile() : _sharedTime(0), _size(0), _revoked(false), _deleted(false), _shared(false)
{
}

RmMyVaultFile::RmMyVaultFile(const RmMyVaultFile& rhs)
	: _name(rhs._name),
	_duid(rhs._duid),
	_path(rhs._path),
	_displayPath(rhs._displayPath),
	_repoId(rhs._repoId),
	_sharedWith(rhs._sharedWith),
	_sourceFileInfo(rhs._sourceFileInfo),
	_sharedTime(rhs._sharedTime),
	_size(rhs._size),
	_revoked(rhs._revoked),
	_deleted(rhs._deleted),
	_shared(rhs._shared)
{
}

RmMyVaultFile::~RmMyVaultFile()
{
}

RmMyVaultFile& RmMyVaultFile::operator = (const RmMyVaultFile& rhs)
{
	if (this != &rhs) {
		_name = rhs._name;
		_duid = rhs._duid;
		_path = rhs._path;
		_displayPath = rhs._displayPath;
		_repoId = rhs._repoId;
		_sharedWith = rhs._sharedWith;
		_sourceFileInfo = rhs._sourceFileInfo;
		_sharedTime = rhs._sharedTime;
		_size = rhs._size;
		_revoked = rhs._revoked;
		_deleted = rhs._deleted;
		_shared = rhs._shared;
	}
	return *this;
}

void RmMyVaultFile::Clear()
{
	_name.clear();
	_duid.clear();
	_path.clear();
	_displayPath.clear();
	_repoId.clear();
	_sharedWith.clear();
	_sharedTime = 0;
	_size = 0;
	_revoked = false;
	_deleted = false;
	_shared = false;
}

NX::rapidjson::PJSONVALUE RmMyVaultFile::ToJsonValue() const
{
	PJSONOBJECT root = JsonValue::CreateObject();
	if (NULL == root)
		return NULL;

	std::wstring strSharedWith;
	for (const std::wstring& s : _sharedWith) {
		if (s.empty())
			continue;
		if (!strSharedWith.empty())
			strSharedWith.append(L",");
		strSharedWith.append(s);
	}

	root->set(L"pathId", JsonValue::CreateString(GetPath()));
	root->set(L"pathDisplay", JsonValue::CreateString(GetDisplayPath()));
	root->set(L"repoId", JsonValue::CreateString(GetRepoId()));
	root->set(L"sharedOn", JsonValue::CreateNumber(GetSharedTime()));
	root->set(L"sharedWith", JsonValue::CreateString(strSharedWith));
	root->set(L"name", JsonValue::CreateString(GetName()));
	root->set(L"duid", JsonValue::CreateString(GetDuid()));
	root->set(L"size", JsonValue::CreateNumber(GetSize()));
	root->set(L"revoked", JsonValue::CreateBool(IsRevoked()));
	root->set(L"deleted", JsonValue::CreateBool(IsDeleted()));
	root->set(L"shared", JsonValue::CreateBool(IsShared()));

	PJSONOBJECT meta = JsonValue::CreateObject();
	if (NULL != meta) {
		root->set(L"customMetadata", meta);
		std::for_each(_metaData.begin(), _metaData.end(), [&](const std::pair<std::wstring, std::wstring>& item) {
			if (item.first.empty())
				return;
			meta->set(item.first, JsonValue::CreateString(item.second));
		});
	}

	return root;
}

void RmMyVaultFile::FromJsonValue(NX::rapidjson::PCJSONVALUE root)
{
	Clear();
	if (root == NULL)
		return;
	if (!root->IsObject())
		return;

	PCJSONVALUE pv = root->AsObject()->at(L"pathId");
	if (pv && pv->IsString())
		_path = pv->AsString()->GetString();

	pv = root->AsObject()->at(L"pathDisplay");
	if (pv && pv->IsString())
		_displayPath = pv->AsString()->GetString();

	pv = root->AsObject()->at(L"repoId");
	if (pv && pv->IsString())
		_repoId = pv->AsString()->GetString();

	pv = root->AsObject()->at(L"sharedOn");
	if (pv && pv->IsNumber())
		_sharedTime = pv->AsNumber()->ToInt64();

	pv = root->AsObject()->at(L"sharedWith");
	if (pv && pv->IsString()) {
		std::wstring strSharedWith = pv->AsString()->GetString();
		_sharedWith = NX::split<wchar_t, ','>(strSharedWith, true);
	}

	pv = root->AsObject()->at(L"name");
	if (pv && pv->IsString())
		_name = pv->AsString()->GetString();

	pv = root->AsObject()->at(L"duid");
	if (pv && pv->IsString())
		_duid = pv->AsString()->GetString();

	pv = root->AsObject()->at(L"revoked");
	if (pv && pv->IsBool())
		_revoked = pv->AsBool()->GetBool();

	pv = root->AsObject()->at(L"deleted");
	if (pv && pv->IsBool())
		_deleted = pv->AsBool()->GetBool();

	pv = root->AsObject()->at(L"shared");
	if (pv && pv->IsBool())
		_shared = pv->AsBool()->GetBool();

	pv = root->AsObject()->at(L"size");
	if (pv && pv->IsNumber())
		_size = pv->AsNumber()->ToInt64();

	PCJSONVALUE metas = root->AsObject()->at(L"customMetadata");
	if (metas && metas->IsObject()) {

		_sourceFileInfo.FromJson(metas->AsObject());
		
		std::for_each(metas->AsObject()->cbegin(), metas->AsObject()->cend(), [&](const JsonObject::value_type& item) {
			if (item.first.empty())
				return;
			std::wstring value = (item.second && item.second->IsString()) ? item.second->AsString()->GetString() : L"";
			_metaData[item.first] = value;
		});
	}
}



RmMyVaultFileMetadata::RmMyVaultFileMetadata() : _sharedTime(0), _protectedTime(0)
{
}

RmMyVaultFileMetadata::RmMyVaultFileMetadata(const RmMyVaultFileMetadata& rhs)
	: _name(rhs._name),
	_link(rhs._link),
	_recipients(rhs._recipients),
	_rights(rhs._rights),
	_sharedTime(rhs._sharedTime),
	_protectedTime(rhs._protectedTime)
{
}

RmMyVaultFileMetadata::~RmMyVaultFileMetadata()
{
}

RmMyVaultFileMetadata& RmMyVaultFileMetadata::operator = (const RmMyVaultFileMetadata& rhs)
{
	if (this != &rhs) {
		_name = rhs._name;
		_link = rhs._link;
		_recipients = rhs._recipients;
		_rights = rhs._rights;
		_sharedTime = rhs._sharedTime;
		_protectedTime = rhs._protectedTime;
	}
	return *this;
}


void RmMyVaultFileMetadata::Clear()
{
	_name.clear();
	_link.clear();
	_recipients.clear();
	_rights.clear();
	_sharedTime = 0;
	_protectedTime = 0;
}

NX::rapidjson::PJSONVALUE RmMyVaultFileMetadata::ToJsonValue() const
{
	PJSONOBJECT root = JsonValue::CreateObject();
	if (NULL == root)
		return NULL;

	root->set(L"name", JsonValue::CreateString(GetName()));
	root->set(L"fileLink", JsonValue::CreateString(GetLink()));
	root->set(L"protectedOn", JsonValue::CreateNumber(GetProtectedTime()));
	root->set(L"sharedOn", JsonValue::CreateNumber(GetSharedTime()));

	PJSONARRAY recipients = JsonValue::CreateArray();
	if (NULL != recipients) {
		root->set(L"recipients", recipients);
		std::for_each(_recipients.begin(), _recipients.end(), [&](const std::wstring& item) {
			if (item.empty())
				return;
			recipients->push_back(JsonValue::CreateString(item));
		});
	}

	PJSONARRAY rights = JsonValue::CreateArray();
	if (NULL != rights) {
		root->set(L"rights", rights);
		std::for_each(_rights.begin(), _rights.end(), [&](const std::wstring& item) {
			if (item.empty())
				return;
			rights->push_back(JsonValue::CreateString(item));
		});
	}

	return root;
}

void RmMyVaultFileMetadata::FromJsonValue(NX::rapidjson::PCJSONVALUE root)
{
	Clear();
	if (root == NULL)
		return;
	if (!root->IsObject())
		return;

	PCJSONVALUE pv = root->AsObject()->at(L"name");
	if (pv && pv->IsString())
		_name = pv->AsString()->GetString();

	pv = root->AsObject()->at(L"fileLink");
	if (pv && pv->IsString())
		_link = pv->AsString()->GetString();

	pv = root->AsObject()->at(L"protectedOn");
	if (pv && pv->IsNumber())
		_protectedTime = pv->AsNumber()->ToInt64();

	pv = root->AsObject()->at(L"sharedOn");
	if (pv && pv->IsNumber())
		_sharedTime = pv->AsNumber()->ToInt64();

	PCJSONVALUE recipients = root->AsObject()->at(L"recipients");
	if (recipients && recipients->IsArray()) {
		std::for_each(recipients->AsArray()->cbegin(), recipients->AsArray()->cend(), [&](const JsonArray::value_type& item) {
			if (item) {
				const std::wstring& value = item->IsString() ? item->AsString()->GetString() : std::wstring();
				if(!value.empty())
					_recipients.push_back(value);
			}
		});
	}

	PCJSONVALUE rights = root->AsObject()->at(L"rights");
	if (rights && rights->IsArray()) {
		std::for_each(rights->AsArray()->cbegin(), rights->AsArray()->cend(), [&](const JsonArray::value_type& item) {
			if (item) {
				const std::wstring& value = item->IsString() ? item->AsString()->GetString() : std::wstring();
				if(!value.empty())
					_rights.push_back(value);
			}
		});
	}
}




RmRepoMarkedFile::RmRepoMarkedFile()
{
}

RmRepoMarkedFile::RmRepoMarkedFile(const std::wstring& fileId, const std::wstring& filePath, const std::wstring& parentFileId)
	: _fileId(fileId), _filePath(filePath), _parentFileId(parentFileId)
{
}

RmRepoMarkedFile::RmRepoMarkedFile(const RmRepoMarkedFile& rhs)
	: _fileId(rhs._fileId), _filePath(rhs._filePath), _parentFileId(rhs._parentFileId)
{
}

RmRepoMarkedFile::~RmRepoMarkedFile()
{
}

RmRepoMarkedFile& RmRepoMarkedFile::operator = (const RmRepoMarkedFile& rhs)
{
	if (this != &rhs) {
		_fileId = rhs._fileId;
		_filePath = rhs._filePath;
		_parentFileId = rhs._parentFileId;
	}
	return *this;
}
