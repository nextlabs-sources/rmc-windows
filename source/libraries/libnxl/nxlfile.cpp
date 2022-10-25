

#include "stdafx.h"

#include <nx\common\macros.h>
#include <nx\common\rapidjson.h>
#include <nx\common\time.h>
#include <nx\winutil\path.h>

#include <nx\crypt\basic.h>
#include <nx\crypt\aes.h>
#include <nx\crypt\sha.h>

#include <nx\nxl\nxlfmt.h>
#include <nx\nxl\nxlutil.h>
#include <nx\nxl\nxlrights.h>
#include <nx\nxl\nxlfile.h>

#include <fstream>

using namespace NX;
using namespace NX::rapidjson;
using namespace NX::NXL;

static const UCHAR ZeroAgreement[256] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

Header::Header()
{
}

Header::~Header()
{
}


FileToken::FileToken() : _level(0)
{
}

FileToken::FileToken(const std::vector<UCHAR>& id, unsigned int level, const std::vector<UCHAR>& key)
	: _id(id), _level(level), _key(key)
{
}

FileToken::FileToken(const std::wstring& id, unsigned int ml, const std::wstring key)
	: _id(NX::hstobin<wchar_t>(id)), _level(ml), _key(NX::hstobin<wchar_t>(key))
{
}

FileToken::FileToken(const FileToken& rhs)
	: _id(rhs._id), _level(rhs._level), _key(rhs._key)
{
}

FileToken::~FileToken()
{
}

FileToken& FileToken::operator = (const FileToken& rhs)
{
	if (this != &rhs) {
		_id = rhs._id;
		_level = rhs._level;
		_key = rhs._key;
	}
	return *this;
}

void FileToken::Clear()
{
	_id.clear();
	_key.clear();
	_level = 0;
}



FileSecret::FileSecret() : _keyFlags(0), _secureMode(0)
{
}

FileSecret::FileSecret(const std::string& ownerId, const FileToken& token, ULONG keyFlags, const std::vector<UCHAR>& agreement0, const std::vector<UCHAR>& agreement1, ULONG secureMode)
	: _keyFlags(keyFlags), _secureMode(secureMode), _token(token), _agreement0(agreement0), _agreement1(agreement1), _ownerId(ownerId)
{
	_ivSeed.resize(16);
	(void)NX::crypt::AesKey::GenerateRandom(_ivSeed.data(), 16);
}

FileSecret::FileSecret(const FileSecret& rhs)
	: _keyFlags(rhs._keyFlags), _secureMode(rhs._secureMode), _token(rhs._token), _ivSeed(rhs._ivSeed), _agreement0(rhs._agreement0), _agreement1(rhs._agreement1), _ownerId(rhs._ownerId)
{
}

FileSecret::~FileSecret()
{
}

FileSecret& FileSecret::operator = (const FileSecret& rhs)
{
	if (this != &rhs) {
		_keyFlags = rhs._keyFlags;
		_secureMode = rhs._secureMode;
		_token = rhs._token;
		_ivSeed = rhs._ivSeed;
		_agreement0 = rhs._agreement0;
		_agreement1 = rhs._agreement1;
		_ownerId = rhs._ownerId;
	}
	return *this;
}

void FileSecret::Clear()
{
	_keyFlags = 0;
	_secureMode = 0;
	_token.Clear();
	_ivSeed.clear();
	_agreement0.clear();
	_agreement1.clear();
	_ownerId.clear();
}


FileSection::FileSection()
{
	memset(&_s, 0, sizeof(NXL_SECTION_2));
}

FileSection::FileSection(const NXL_SECTION_2* p)
{
	memset(&_s, 0, sizeof(NXL_SECTION_2));
	memcpy(&_s, p, sizeof(NXL_SECTION_2));
}

FileSection::FileSection(const FileSection& rhs)
{
	memset(&_s, 0, sizeof(NXL_SECTION_2));
	memcpy(&_s, &rhs._s, sizeof(NXL_SECTION_2));
}

FileSection::~FileSection()
{
}

FileSection& FileSection::operator = (const FileSection& rhs)
{
	if (this != &rhs) {
		memset(&_s, 0, sizeof(NXL_SECTION_2));
		memcpy(&_s, &rhs._s, sizeof(NXL_SECTION_2));
	}
	return *this;
}

FileMeta::FileMeta()
{
}

FileMeta::FileMeta(const std::string& s)
{
	JsonDocument doc;
	if (doc.LoadJsonString<char>(s)) {
		PCJSONVALUE root = doc.GetRoot();
		if (root->IsObject()) {
			std::for_each(root->AsObject()->cbegin(), root->AsObject()->cend(), [&](const JsonObject::value_type& item) {
				if (!item.first.empty() && item.second && item.second->IsString()) {
					_metadata.push_back(FileMetaItem(item.first, item.second->AsString()->GetString()));
				}
			});
		}
	}
}

FileMeta::FileMeta(const FileMetaArray& ta) : _metadata(ta)
{
}

FileMeta::FileMeta(const FileMeta& rhs) : _metadata(rhs._metadata)
{
}

FileMeta::~FileMeta()
{
}

FileMeta& FileMeta::operator = (const FileMeta& rhs)
{
	if (this != &rhs) {
		_metadata = rhs._metadata;
	}
	return *this;
}

void FileMeta::Clear()
{
	_metadata.clear();
}

std::wstring FileMeta::Get(const std::wstring& key) const
{
	auto pos = std::find_if(_metadata.begin(), _metadata.end(), [&](const FileMetaItem& item) -> bool { return (0 == _wcsicmp(key.c_str(), item.first.c_str())); });
	return (pos == _metadata.end()) ? std::wstring() : (*pos).second;
}

Result FileMeta::Set(const std::wstring& key, const std::wstring& value)
{
	if (key.empty())
		return RESULT(ERROR_INVALID_PARAMETER);

	auto pos = std::find_if(_metadata.begin(), _metadata.end(), [&](const FileMetaItem& item) -> bool { return (0 == _wcsicmp(key.c_str(), item.first.c_str())); });
	if (pos == _metadata.end()) {
		_metadata.push_back(FileMetaItem(key, value));
	}
	else {
		(*pos).second = value;
	}

	return RESULT(0);
}

Result FileMeta::Remove(const std::wstring& key)
{
	if (key.empty())
		return RESULT(ERROR_INVALID_PARAMETER);

	auto pos = std::find_if(_metadata.begin(), _metadata.end(), [&](const FileMetaItem& item) -> bool { return (0 == _wcsicmp(key.c_str(), item.first.c_str())); });
	if (pos == _metadata.end()) {
		return RESULT(ERROR_NOT_FOUND);
	}

	_metadata.erase(pos);
	return RESULT(0);
}

std::string FileMeta::ToString() const
{
	std::string s;
	if (_metadata.empty())
		return std::string("{}");
	std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
	std::for_each(_metadata.begin(), _metadata.end(), [&](const FileMetaItem& item) {
		root->set(item.first, JsonValue::CreateString(item.second));
	});
	JsonStringWriter<char> writer;
	s = writer.Write(root.get());
	return s;
}



FileAdHocPolicy::FileAdHocPolicy() : _issueTime(NX::time::datetime::current_time())
{
}

FileAdHocPolicy::FileAdHocPolicy(const std::string& s) : _issueTime(0)
{
	JsonDocument doc;
	if (doc.LoadJsonString<char>(s)) {
		PCJSONVALUE root = doc.GetRoot();
		if (root->IsObject()) {

			PCJSONVALUE pv = NULL;

			pv = root->AsObject()->at(L"version");
			if (!IsValidJsonString(pv))
				return;

			pv = root->AsObject()->at(L"issuer");
			if (!IsValidJsonString(pv))
				return;
			_ownerId = pv->AsString()->GetString();
			if (_ownerId.empty())
				return;

			pv = root->AsObject()->at(L"issueTime");
			if (IsValidJsonString(pv)) {
				NX::time::datetime dt(pv->AsString()->GetString());
				_issueTime = (__int64)dt;
			}

			pv = root->AsObject()->at(L"policies");
			if (!IsValidJsonArray(pv))
				return;
			if (0 == pv->AsArray()->size())
				return;

			PCJSONVALUE policy = pv->AsArray()->at(0);
			if (!IsValidJsonObject(policy))
				return;

			pv = policy->AsObject()->at(L"name");
			if (!IsValidJsonString(pv))
				return;
			const std::wstring& policyName = pv->AsString()->GetString();
			if (0 != _wcsicmp(policyName.c_str(), L"Ad-hoc"))
				return;

			pv = policy->AsObject()->at(L"action");
			if (!IsValidJsonNumber(pv))
				return;
			if (1 != pv->AsNumber()->ToInt64())
				return;

			pv = policy->AsObject()->at(L"rights");
			if (!IsValidJsonArray(pv))
				return;
			std::for_each(pv->AsArray()->cbegin(), pv->AsArray()->cend(), [&](const JsonArray::value_type& item) {
				if (IsValidJsonString(item)) {
					const std::wstring& r = item->AsString()->GetString();
					if (!r.empty())
						_rights.push_back(r);
				}
			});

			pv = policy->AsObject()->at(L"obligations");
			if (!IsValidJsonArray(pv))
				return;
			std::for_each(pv->AsArray()->cbegin(), pv->AsArray()->cend(), [&](const JsonArray::value_type& item) {
				if (IsValidJsonObject(item)) {
					PCJSONVALUE ob = item->AsObject()->at(L"name");
					if (IsValidJsonString(ob)) {
						const std::wstring& obName = ob->AsString()->GetString();
						if (!obName.empty())
							_obligations.push_back(obName);
					}
				}
			});
		}
	}
}

FileAdHocPolicy::FileAdHocPolicy(const std::wstring& ownerId, const std::vector<std::wstring>& rights, const std::vector<std::wstring>& obligations)
	: _ownerId(ownerId), _obligations(obligations), _rights(rights), _issueTime(NX::time::datetime::current_time())
{
}

FileAdHocPolicy::FileAdHocPolicy(const FileAdHocPolicy& rhs) : _obligations(rhs._obligations), _rights(rhs._rights), _ownerId(rhs._ownerId), _issueTime(_issueTime)
{
}

FileAdHocPolicy::~FileAdHocPolicy()
{
}

FileAdHocPolicy& FileAdHocPolicy::operator = (const FileAdHocPolicy& rhs)
{
	if (this != &rhs) {
		_obligations = rhs._obligations;
		_rights = rhs._rights;
		_ownerId = rhs._ownerId;
		_issueTime = rhs._issueTime;
	}
	return *this;
}

unsigned __int64 FileAdHocPolicy::RightsToInt64() const
{
	unsigned __int64 r = 0;
	std::for_each(_rights.begin(), _rights.end(), [&](const std::wstring& s) {
		r |= ActionToRights(s.c_str());
	});
	return r;
}

void FileAdHocPolicy::Clear()
{
	_ownerId.clear();
	_rights.clear();
	_obligations.clear();
	_issueTime = 0;
}

std::string FileAdHocPolicy::ToString() const
{
	std::string s;
	if (_ownerId.empty() || _rights.empty())
		return s;
	std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
	if (root == NULL)
		return s;

	bool succeeded = false;
	do {

		PJSONVALUE pv = NULL;

		pv = JsonValue::CreateString(L"1.0");
		if (NULL == pv)
			break;
		root->set(L"version", pv);

		pv = JsonValue::CreateString(_ownerId);
		if (NULL == pv)
			break;
		root->set(L"issuer", pv);

		NX::time::datetime dt(_issueTime);
		const std::wstring& dts = dt.serialize(false, false);
		pv = JsonValue::CreateString(dts);
		if (NULL == pv)
			break;
		root->set(L"issueTime", pv);

		// Create Policy Array
		PJSONARRAY pps = JsonValue::CreateArray();
		if (NULL == pps)
			break;
		root->set(L"policies", pps);

		// Create Policy
		PJSONOBJECT pp = JsonValue::CreateObject();
		if (NULL == pp)
			break;
		pps->push_back(pp);


		// Init Policy
		pv = JsonValue::CreateNumber(0);
		if (NULL == pv)
			break;
		pp->set(L"id", pv);

		pv = JsonValue::CreateString(L"Ad-hoc");
		if (NULL == pv)
			break;
		pp->set(L"name", pv);

		pv = JsonValue::CreateNumber(1);
		if (NULL == pv)
			break;
		pp->set(L"action", pv);

		PJSONARRAY rights = JsonValue::CreateArray();
		if (NULL == rights)
			break;
		pp->set(L"rights", rights);
		std::for_each(_rights.begin(), _rights.end(), [&](const std::wstring& r) {
			rights->push_back(JsonValue::CreateString(r));
		});
		if (_rights.end() == std::find_if(_rights.begin(), _rights.end(), [&](const std::wstring& r) -> bool {
			return (0 == _wcsicmp(r.c_str(), RIGHT_ACTION_VIEW));
			})) {
			// View is not set
			rights->push_back(JsonValue::CreateString(RIGHT_ACTION_VIEW));
		}

		pv = JsonValue::CreateObject();
		if (NULL == pv)
			break;
		pp->set(L"conditions", pv);


		PJSONARRAY obs = JsonValue::CreateArray();
		if (NULL == obs)
			break;
		pp->set(L"obligations", obs);
		std::for_each(_obligations.begin(), _obligations.end(), [&](const std::wstring& ob) {
			PJSONOBJECT pOb = JsonValue::CreateObject();
			if (IsValidJsonObject(pOb)) {
				pOb->set(L"name", JsonValue::CreateString(ob));
				obs->push_back(pOb);
			}
		});
		
		succeeded = true;

	} while (FALSE);

	if (succeeded) {
		JsonStringWriter<char> writer;
		s = writer.Write(root.get());
	}
	return s;
}



FileTags::FileTags()
{
}

FileTags::FileTags(const std::string& s)
{
	JsonDocument doc;
	if (doc.LoadJsonString<char>(s)) {
		PCJSONVALUE root = doc.GetRoot();
		if (root->IsObject()) {
			std::for_each(root->AsObject()->cbegin(), root->AsObject()->cend(), [&](const JsonObject::value_type& item) {
				if (!item.first.empty() && item.second && item.second->IsArray()) {
					std::vector<std::wstring> values;
					std::for_each(item.second->AsArray()->cbegin(), item.second->AsArray()->cend(), [&](const JsonArray::value_type& v) {
						if (v && v->IsString()) {
							values.push_back(v->AsString()->GetString());
						}
					});
					_tags.push_back(FileTag(item.first, values));
				}
			});
		}
	}
}

FileTags::FileTags(const FileTagArray& ta) : _tags(ta)
{
}

FileTags::FileTags(const FileTags& rhs) : _tags(rhs._tags)
{
}

FileTags::~FileTags()
{
}

FileTags& FileTags::operator = (const FileTags& rhs)
{
	if (this != &rhs) {
		_tags = rhs._tags;
	}
	return *this;
}

void FileTags::Clear()
{
	_tags.clear();
}

void FileTags::RemoveKey(const std::wstring& key)
{
	auto pos = std::find_if(_tags.begin(), _tags.end(), [&](const FileTag& tag)->bool {
		return (0 == _wcsicmp(tag.first.c_str(), key.c_str()));
	});
	if (pos != _tags.end()) {
		_tags.erase(pos);
	}
}

void FileTags::RemoveValue(const std::wstring& key, const std::wstring& value)
{
	auto pos = std::find_if(_tags.begin(), _tags.end(), [&](const FileTag& tag)->bool {
		return (0 == _wcsicmp(tag.first.c_str(), key.c_str()));
	});
	if (pos != _tags.end()) {
		FileTagValues& values = (*pos).second;
		auto vpos = std::find_if(values.begin(), values.end(), [&](const std::wstring& v)->bool {
			return (0 == _wcsicmp(v.c_str(), value.c_str()));
		});
		if (vpos != values.end()) {
			values.erase(vpos);
		}
	}
}

void FileTags::AddValue(const std::wstring& key, const std::wstring& value)
{
	auto pos = std::find_if(_tags.begin(), _tags.end(), [&](const FileTag& tag)->bool {
		return (0 == _wcsicmp(tag.first.c_str(), key.c_str()));
	});

	if (pos != _tags.end()) {
		FileTagValues& values = (*pos).second;
		auto vpos = std::find_if(values.begin(), values.end(), [&](const std::wstring& v)->bool {
			return (0 == _wcsicmp(v.c_str(), value.c_str()));
		});
		if (vpos == values.end()) {
			values.push_back(value);
		}
	}
	else {
		_tags.push_back(FileTag(key, FileTagValues({value})));
	}
}

void FileTags::SetValues(const std::wstring& key, const FileTagValues& values)
{
	auto pos = std::find_if(_tags.begin(), _tags.end(), [&](const FileTag& tag)->bool {
		return (0 == _wcsicmp(tag.first.c_str(), key.c_str()));
	});

	if (pos != _tags.end()) {
		(*pos).second = values;
	}
	else {
		_tags.push_back(FileTag(key, values));
	}
}

FileTags::FileTagValues FileTags::GetValues(const std::wstring& key) const
{
	auto pos = std::find_if(_tags.begin(), _tags.end(), [&](const FileTag& tag)->bool {
		return (0 == _wcsicmp(tag.first.c_str(), key.c_str()));
	});
	if (pos != _tags.end()) {
		return (*pos).second;
	}
	return FileTagValues();
}

std::string FileTags::ToString() const
{
	std::string s;
	if (_tags.empty())
		return std::string("{}");
	std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
	std::for_each(_tags.begin(), _tags.end(), [&](const FileTag& tag) {
		PJSONARRAY pva = JsonValue::CreateArray();
		if (pva == NULL)
			return;
		std::for_each(tag.second.begin(), tag.second.end(), [&](const std::wstring& v) {
			if (!v.empty()) {
				PJSONSTRING ps = JsonValue::CreateString(v);
				if (ps)
					pva->push_back(ps);
			}
		});
		root->set(tag.first, pva);
	});
	JsonStringWriter<char> writer;
	s = writer.Write(root.get());
	return s;
}



File::File() : _h(INVALID_HANDLE_VALUE), _readOnly(false), _contentOffset(0)
{
}

File::~File()
{
	Close();
}

File* File::Create(const std::wstring& path,
	const std::wstring& origFileName,
	bool overWrite,
	const FileSecret& secret,
	_In_reads_bytes_opt_(32) const UCHAR* recoveryKey,
	_In_opt_ const FileMeta* metadata,
	_In_opt_ const FileAdHocPolicy* policy,
	_In_opt_ const FileTags* tags,
	const std::string& msg,
	Result& result)
{
	File* fp = new File();
	if (NULL == fp) {
		result = RESULT(ERROR_NOT_ENOUGH_MEMORY);
		return fp;
	}

	result = fp->CreateNew(path, origFileName, overWrite, secret, recoveryKey, metadata, policy, tags, msg);
	if (!result) {
		delete fp;
		fp = NULL;
	}

	return fp;
}

File* File::CreateFrom(const std::wstring& source,
	const std::wstring& target,
	bool overWrite,
	const FileSecret& secret,
	_In_reads_bytes_opt_(32) const UCHAR* recoveryKey,
	_In_opt_ const FileMeta* metadata,
	_In_opt_ const FileAdHocPolicy* policy,
	_In_opt_ const FileTags* tags,
	const std::string& msg,
	Result& result)
{
	File* fp = new File();
	
	result = fp->Protect(source, target, secret, recoveryKey, metadata, policy, tags, msg);
	if (!result) {
		delete fp;
		fp = NULL;
	}

	return fp;
}

File* File::Open(const std::wstring& path, bool readOnly, Result& result)
{
	File* fp = new File();
	if (NULL == fp) {
		result = RESULT(ERROR_NOT_ENOUGH_MEMORY);
		return fp;
	}

	result = fp->OpenExisting(path, readOnly);
	if (!result) {
		delete fp;
		fp = NULL;
	}

	return fp;
}

Result File::IsNXLFile(const std::wstring& path, _Out_opt_ NXL_SIGNATURE_LITE* sig)
{
	HANDLE h = ::CreateFileW(path.c_str(), GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == h)
		return RESULT(GetLastError());

	Result res = RESULT(0);
	do {

		NXL_SIGNATURE_LITE signature = { 0 };
		DWORD dwRead = 0;

		LARGE_INTEGER liSize = { 0, 0 };
		if (!GetFileSizeEx(h, &liSize)) {
			res = RESULT(GetLastError());
			break;
		}

		if(liSize.QuadPart < sizeof(NXL_SIGNATURE_LITE)) {
			res = RESULT(ERROR_BAD_FILE_TYPE);
			break;
		}

		if (!::ReadFile(h, &signature, sizeof(NXL_SIGNATURE_LITE), &dwRead, NULL)) {
			res = RESULT(GetLastError());
			break;
		}

		if (signature.magic.code == NXL_MAGIC_CODE_1) {
			signature.version = NXL_VERSION_10;
			if (sig)
				memcpy(sig, &signature, sizeof(NXL_SIGNATURE_LITE));
			if (liSize.QuadPart < sizeof(NXL_HEADER_1)) {
				res = RESULT(ERROR_FILE_CORRUPT);
				break;
			}
		}
		else if (signature.magic.code == NXL_MAGIC_CODE_2) {
			if (sig)
				memcpy(sig, &signature, sizeof(NXL_SIGNATURE_LITE));
			if (liSize.QuadPart < sizeof(NXL_HEADER_2)) {
				res = RESULT(ERROR_FILE_CORRUPT);
				break;
			}
		}
		else {
			res = RESULT(ERROR_BAD_FILE_TYPE);
			break;
		}

		if (!IS_ALIGNED(liSize.LowPart, 16)) {
			res = RESULT(ERROR_FILE_CORRUPT);
			break;
		}

	} while (FALSE);
	CloseHandle(h); h = INVALID_HANDLE_VALUE;
	if (!res)
		return res;


	return RESULT(0);
}

Result File::CreateNew(const std::wstring& path,
	const std::wstring& origFileName,
	bool overWrite,
	const FileSecret& secret,
	_In_reads_bytes_opt_(32) const UCHAR* recoveryKey,
	_In_opt_ const FileMeta* metadata,
	_In_opt_ const FileAdHocPolicy* policy,
	_In_opt_ const FileTags* tags,
	const std::string& msg)
{
	if(secret.GetToken().Empty() || secret.GetOwnerId().empty() || secret.GetAgreement0().empty())
		return RESULT(ERROR_INVALID_PARAMETER);
	if (!secret.GetToken().IsFullToken())
		return RESULT(ERROR_INVALID_PARAMETER);

	_h = ::CreateFileW(path.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, overWrite ? CREATE_ALWAYS : CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == _h)
		return RESULT(GetLastError());

	Result res = RESULT(0);
	DWORD dwSize = 0;

#ifdef _DEBUG
	std::wstring hdrChecksum;
#endif

	do {

		std::vector<UCHAR> buf;
		buf.resize(sizeof(ULONG) + sizeof(NXL_HEADER_2), 0);
		*((ULONG*)buf.data()) = (ULONG)sizeof(NXL_FIXED_HEADER);
		NXL_HEADER_2* header = (NXL_HEADER_2*)(buf.data() + sizeof(ULONG));
		UCHAR cek[32] = { 0 };
		
		memset(cek, 0, 32);
		res = NX::crypt::AesKey::GenerateRandom(cek, 32);
		if (!res)
			break;

		NX::crypt::AesKey tokenKey;
		res = tokenKey.Import(secret.GetToken().GetKey().data(), 32);
		if (!res)
			break;

		// copy secret
		_secret = secret;
		_signature.magic.code = NXL_MAGIC_CODE_2;
		_signature.version = NXL_VERSION_30;

		//
		// Header->fixed.signatrue
		//
		header->fixed.signature.magic.code = NXL_MAGIC_CODE_2;
		header->fixed.signature.version = NXL_VERSION_30;
		if (!msg.empty()) {
			memcpy(header->fixed.signature.message, msg.c_str(), min(243, msg.length()));
		}

		//
		// Header->fixed.file_info
		//
		memcpy(header->fixed.file_info.duid, secret._token.GetId().data(), 16);
		memcpy(header->fixed.file_info.owner_id, secret.GetOwnerId().c_str(), min(255, secret.GetOwnerId().length()));
		header->fixed.file_info.flags = 0;
		header->fixed.file_info.alignment = NXL_PAGE_SIZE;
		header->fixed.file_info.algorithm = NXL_ALGORITHM_AES256;
		header->fixed.file_info.block_size = NXL_BLOCK_SIZE;
		header->fixed.file_info.extended_data_offset = 0;
		// content_offset will be set at last step

		//
		// Header->fixed.keys
		//
        header->fixed.keys.mode_and_flags = secret.GetKeyFlags() & 0x00FFFFFF;
        header->fixed.keys.mode_and_flags |= (secret.GetSecureMode() << 24);
		header->fixed.keys.token_level = secret.GetToken().GetLevel();
		memcpy(header->fixed.keys.iv_seed, secret.GetIvSeed().data(), 16);
		memcpy(header->fixed.keys.public_key1, secret.GetAgreement0().data(), 256);
		if(!secret.GetAgreement1().empty())
			memcpy(header->fixed.keys.public_key2, secret.GetAgreement1().data(), 256);
        // Set CEK protected by Token
		memcpy(header->fixed.keys.token_cek, cek, 32);
		// Calculate HMAC(Token, Cek)
		res = CalcCekChecksum(header->fixed.keys.token_cek, header->fixed.keys.token_cek + 32, secret.GetToken().GetKey().data());
		if (!res)
			break;
		dwSize = 32;
		res = NX::crypt::AesBlockEncrypt(tokenKey, header->fixed.keys.token_cek, 32, header->fixed.keys.token_cek, &dwSize, header->fixed.keys.iv_seed);
		if (!res)
			break;
#ifdef _DEBUG
		const std::wstring& encryptedCek = NX::bintohs<wchar_t>(header->fixed.keys.token_cek, 32);
		const std::wstring& decryptedCek = NX::bintohs<wchar_t>(cek, 32);
		const std::wstring& cekChecksum = NX::bintohs<wchar_t>(header->fixed.keys.token_cek + 32, 32);
		const std::wstring& sTokenId = NX::bintohs<wchar_t>(secret.GetToken().GetId().data(), 16);
		const std::wstring& sTokenKey = NX::bintohs<wchar_t>(secret.GetToken().GetKey().data(), 32);
		const std::wstring& sIvecSeed = NX::bintohs<wchar_t>(header->fixed.keys.iv_seed, 16);
#endif


		if (NULL != recoveryKey) {
			dwSize = 4;
			memcpy(header->fixed.keys.recovery_cek, recoveryKey, 32);
			res = CalcCekChecksum(header->fixed.keys.recovery_cek, header->fixed.keys.recovery_cek + 32, secret.GetToken().GetKey().data());
			if (!res)
				break;
			dwSize = 32;
			res = NX::crypt::AesBlockEncrypt(tokenKey, header->fixed.keys.recovery_cek, 32, header->fixed.keys.recovery_cek, &dwSize, header->fixed.keys.iv_seed);
			if (!res)
				break;

			header->fixed.keys.mode_and_flags |= KF_RECOVERY_KEY_ENABLED;
		}

		//
		//  Sections
		//

		// Default 3 sections
		header->fixed.sections.section_map = 7;

		// Section 0: FileInfo
		FileMeta fileInfo = metadata ? (*metadata) : FileMeta();
		if (!origFileName.empty()) {
			fileInfo.Set(L"fileName", origFileName);
			const wchar_t* ext = wcsrchr(origFileName.c_str(), L'.');
			fileInfo.Set(L"fileExtension", ext ? ext : L"");
		}
		const std::string& sFileInfo = fileInfo.ToString();
		memcpy(header->fixed.sections.record[0].name, NXL_SECTION_NAME_FILEINFO, strlen(NXL_SECTION_NAME_FILEINFO));
		header->fixed.sections.record[0].flags = 0;
		header->fixed.sections.record[0].offset= (ULONG)sizeof(NXL_HEADER_2);
		header->fixed.sections.record[0].size  = NXL_PAGE_SIZE;
		header->fixed.sections.record[0].data_size = (USHORT)sFileInfo.length();
		header->fixed.sections.record[0].compressed_size = 0;
		header->fixed.sections.record[0].flags = 0;
		res = CalcHmacChecksum((const UCHAR*)sFileInfo.c_str(), (ULONG)sFileInfo.length(), secret.GetToken().GetKey().data(), 32, header->fixed.sections.record[0].checksum);
		if (!res)
			break;
		// Section 1: FilePolicy
		FileAdHocPolicy simplePolicy(secret.GetOwnerIdW(), std::vector<std::wstring>({ RIGHT_ACTION_VIEW }), std::vector<std::wstring>());
		const std::string& sPolicy = policy ? policy->ToString() : simplePolicy.ToString();
		memcpy(header->fixed.sections.record[1].name, NXL_SECTION_NAME_FILEPOLICY, strlen(NXL_SECTION_NAME_FILEPOLICY));
		header->fixed.sections.record[1].flags = 0;
		header->fixed.sections.record[1].offset= header->fixed.sections.record[0].offset + header->fixed.sections.record[0].size;
		header->fixed.sections.record[1].size  = NXL_PAGE_SIZE;
		header->fixed.sections.record[1].data_size = (USHORT)sPolicy.length();
		header->fixed.sections.record[1].compressed_size = 0;
		header->fixed.sections.record[1].flags = 0;
		if (!sPolicy.empty()) {
			res = CalcHmacChecksum((const UCHAR*)sPolicy.c_str(), (ULONG)sPolicy.length(), secret.GetToken().GetKey().data(), 32, header->fixed.sections.record[1].checksum);
			if (!res)
				break;
		}
		// Section 2: FileTag
		const std::string& sTags = tags ? tags->ToString() : std::string("{}");
		const ULONG tagsSize = (ULONG)sTags.length();
		memcpy(header->fixed.sections.record[2].name, NXL_SECTION_NAME_FILETAG, strlen(NXL_SECTION_NAME_FILETAG));
		header->fixed.sections.record[2].flags = 0;
		header->fixed.sections.record[2].offset= header->fixed.sections.record[1].offset + header->fixed.sections.record[1].size;
		header->fixed.sections.record[2].size = ROUND_TO_SIZE(max(tagsSize, NXL_PAGE_SIZE), NXL_PAGE_SIZE);
		header->fixed.sections.record[2].data_size = (USHORT)sTags.length();
		header->fixed.sections.record[2].compressed_size = 0;
		header->fixed.sections.record[2].flags = 0;
		if (!sTags.empty()) {
			res = CalcHmacChecksum((const UCHAR*)sTags.c_str(), tagsSize, secret.GetToken().GetKey().data(), 32, header->fixed.sections.record[2].checksum);
			if (!res)
				break;
		}

		header->fixed.file_info.content_offset = header->fixed.sections.record[2].offset + header->fixed.sections.record[2].size;
		header->dynamic.content_length = 0;
		res = NX::crypt::CreateHmacSha256(buf.data(), (ULONG)(sizeof(ULONG) + sizeof(NXL_FIXED_HEADER)), secret.GetToken().GetKey().data(), 32, header->dynamic.fixed_header_hash);
		if (!res)
			break;

#ifdef _DEBUG
		hdrChecksum = NX::bintohs<wchar_t>(header->dynamic.fixed_header_hash, 32);
#endif

		// Set File Size
		SetFilePointer(_h, header->fixed.file_info.content_offset, NULL, FILE_BEGIN);
		if (!SetEndOfFile(_h)) {
			res = RESULT(GetLastError());
			break;
		}

		// Set default section data
		if (!sFileInfo.empty()) {
			res = WriteFileEx(header->fixed.sections.record[0].offset, sFileInfo.c_str(), (ULONG)sFileInfo.length(), NULL);
			if (!res)
				break;
		}
		if (!sPolicy.empty()) {
			res = WriteFileEx(header->fixed.sections.record[1].offset, sPolicy.c_str(), (ULONG)sPolicy.length(), NULL);
			if (!res)
				break;
		}
		if (!sTags.empty()) {
			res = WriteFileEx(header->fixed.sections.record[2].offset, sTags.c_str(), (ULONG)sTags.length(), NULL);
			if (!res)
				break;
		}

		// Write header to file
		res = WriteFileEx(0, header, sizeof(NXL_HEADER_2), NULL);
		if (!res)
			break;

		_contentOffset = header->fixed.file_info.content_offset;

	} while (FALSE);

	if (!res) {
		if (INVALID_HANDLE_VALUE != _h) {
			CloseHandle(_h);
			_h = INVALID_HANDLE_VALUE;
			::DeleteFileW(path.c_str());
		}
		_contentOffset = 0;
		_signature.magic.code = 0;
		_signature.version = 0;
		_secret.Clear();
		return res;
	}

#ifdef _DEBUG
	res = VerifyHeaderChecksum();
#endif

	_path = path;
	return RESULT(0);
}

Result File::OpenExisting(const std::wstring& path, bool readOnly)
{
	_h = ::CreateFileW(path.c_str(), readOnly ? GENERIC_READ : (GENERIC_READ | GENERIC_WRITE), 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == _h)
		return RESULT(GetLastError());

	DWORD dwSize = 0;
	std::vector<UCHAR> buf;
	buf.resize(sizeof(NXL_HEADER_2), 0);
	const NXL_HEADER_2* header = (const NXL_HEADER_2*)buf.data();

	Result res = ReadFileEx(0, buf.data(), (ULONG)buf.size(), &dwSize);
	if (!res)
		return res;
	if (dwSize != (ULONG)buf.size())
		return RESULT(ERROR_BAD_FILE_TYPE);

	res = RESULT(0);
	do {

		//
		// Get signature
		//
		if (header->fixed.signature.magic.code == NXL_MAGIC_CODE_1) {
			_signature.magic.code = NXL_MAGIC_CODE_1;
			_signature.version = NXL_VERSION_10;
		}
		else if (header->fixed.signature.magic.code == NXL_MAGIC_CODE_2) {
			_signature.magic.code = header->fixed.signature.magic.code;
			_signature.version = header->fixed.signature.version;
		}
		else {
			res = RESULT(ERROR_BAD_FILE_TYPE);
			break;
		}

		//
		//  Get Token
		//
		_secret._token._id.resize(16, 0);
		memcpy(_secret._token._id.data(), header->fixed.file_info.duid, 16);
		_secret._token._level = header->fixed.keys.token_level;
		_secret._keyFlags = header->fixed.keys.mode_and_flags & 0x00FFFFFF;
		_secret._secureMode = (header->fixed.keys.mode_and_flags & 0xFF000000) >> 24;
		_secret._ownerId = header->fixed.file_info.owner_id;
		_secret._ivSeed.resize(16, 0);
		memcpy(_secret._ivSeed.data(), header->fixed.keys.iv_seed, 16);
		_secret._agreement0.resize(256, 0);
		memcpy(_secret._agreement0.data(), header->fixed.keys.public_key1, 256);
		if (0 != memcmp(ZeroAgreement, header->fixed.keys.public_key2, 256)) {
			// It does use agreement1
			_secret._agreement1.resize(256, 0);
			memcpy(_secret._agreement1.data(), header->fixed.keys.public_key2, 256);
		}

		_contentOffset = header->fixed.file_info.content_offset;
		
	} while (FALSE);
	if (!res) {
		if (INVALID_HANDLE_VALUE != _h) {
			CloseHandle(_h);
		}
		_contentOffset = 0;
		_signature.magic.code = 0;
		_signature.version = 0;
		_secret.Clear();
		return res;
	}

	_path = path;
	return RESULT(0);
}

void File::Close()
{
	if (_h != INVALID_HANDLE_VALUE) {
		CloseHandle(_h);
		_h = INVALID_HANDLE_VALUE;
	}
	_readOnly = false;
	_contentOffset = 0;
	memset(&_signature, 0, sizeof(_signature));
	_secret.Clear();
	_path.clear();
}

Result File::Protect(const std::wstring& source,
	const std::wstring& target,
	const FileSecret& secret,
	_In_reads_bytes_opt_(32) const UCHAR* recoveryKey,
	_In_opt_ const FileMeta* metadata,
	_In_opt_ const FileAdHocPolicy* policy,
	_In_opt_ const FileTags* tags,
	const std::string& msg)
{
	if (NX::iend_with<wchar_t>(source, L".nxl")) {
		return RESULT(ERROR_FILE_ENCRYPTED);
	}

	if(File::IsNXLFile(source, NULL)) {
		return RESULT(ERROR_FILE_ENCRYPTED);
	}

	NX::win::FilePath srcPath(source);

	std::wstring destPath(target);
	if (destPath.empty())
		destPath = source;
	if (!NX::iend_with<wchar_t>(destPath, L".nxl"))
		destPath.append(L".nxl");

	Result res = CreateNew(destPath, srcPath.GetFileName(), true, secret, recoveryKey, metadata, policy, tags, msg);
	if (!res)
		return res;

	HANDLE hSource = ::CreateFileW(source.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hSource) {
		res = RESULT(GetLastError());
		Close();
		::DeleteFileW(destPath.c_str());
		return res;
	}

	do {

		std::vector<UCHAR>	buf;
		buf.resize(OneMB, 0);

		LARGE_INTEGER liSourceSize = { 0, 0 };
		if (!GetFileSizeEx(hSource, &liSourceSize)) {
			res = RESULT(GetLastError());
			break;
		}

		if (0 == liSourceSize.QuadPart)
			break;

		// Move to
		SetFilePointer(_h, 0, NULL, FILE_END);

		NX::crypt::AesKey cek;
		res = UnlockContentKey(cek);
		if (!res)
			break;

		__int64 bytesOffset = 0;
		__int64 bytesNewSize = 0;
		while (liSourceSize.QuadPart) {

			const ULONG bytesToRead = (ULONG)min(liSourceSize.QuadPart, OneMB);
			const ULONG bytesToEncrypt = ROUND_TO_SIZE(bytesToRead, 16);
			const ULONG bytesBlockAligned = ROUND_TO_SIZE(bytesToRead, NXL_BLOCK_SIZE);
			ULONG bytesRead = 0;
			ULONG bytesWritten = 0;
			ULONG bytesEncrypted = bytesToEncrypt;

			assert(IS_ALIGNED(bytesOffset, NXL_BLOCK_SIZE));
			
			if (!::ReadFile(hSource, buf.data(), bytesToRead, &bytesRead, NULL)) {
				res = RESULT(GetLastError());
				break;
			}
			assert(bytesToRead == bytesRead);
			bytesNewSize += bytesToRead;

			// Put zero as padding
			if (bytesToRead < bytesBlockAligned)
				memset(buf.data() + bytesToRead, 0, bytesBlockAligned - bytesToRead);

			// Encrypt
			res = NX::crypt::AesBlockEncrypt(cek, buf.data(), bytesToEncrypt, buf.data(), &bytesEncrypted, _secret.GetIvSeed().data(), bytesOffset, NXL_BLOCK_SIZE);
			if (!res)
				break;

			// Write encrypted data
			if (!WriteFileEx(_contentOffset + bytesOffset, buf.data(), bytesBlockAligned, &bytesWritten)) {
				res = RESULT(GetLastError());
				break;
			}

			// Update file size
			res = WriteFileEx(FIELD_OFFSET(NXL_HEADER_2, dynamic.content_length), &bytesNewSize, sizeof(__int64), NULL);
			if (!res)
				break;
			
			// Move to next block
			liSourceSize.QuadPart -= bytesToRead;
			bytesOffset += bytesToRead;
		}

		if (!res)
			break;


	} while (FALSE);

	CloseHandle(hSource);
	hSource = INVALID_HANDLE_VALUE;

	if (!res) {
		Close();
		::DeleteFileW(destPath.c_str());
		return res;
	}

	return RESULT(0);
}

Result File::Unprotect(const std::wstring& dest)
{
	if(!IsOpened())
		return RESULT(ERROR_INVALID_HANDLE);
	if(!IsFullToken())
		return RESULT(ERROR_NO_USER_SESSION_KEY);
	if(NX::iend_with<wchar_t>(dest, L".nxl"))
		return RESULT(ERROR_INVALID_PARAMETER);


	HANDLE hTarget = ::CreateFileW(dest.c_str(), GENERIC_READ | GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE == hTarget)
		return RESULT(GetLastError());

	Result res = RESULT(0);

	do {

		DWORD dwRead = 0;
		DWORD contentOffset = 0;
		__int64 contentSize = 0;

		res = ReadFileEx(FIELD_OFFSET(NXL_HEADER_2, fixed.file_info.content_offset), &contentOffset, 4, &dwRead);
		if (!res)
			break;
		res = ReadFileEx(FIELD_OFFSET(NXL_HEADER_2, dynamic.content_length), &contentSize, 4, &dwRead);
		if (!res)
			break;

		if (0 == contentSize)
			break;

		NX::crypt::AesKey cek;
		res = UnlockContentKey(cek);
		if (!res)
			break;

		if (INVALID_SET_FILE_POINTER == SetFilePointer(_h, contentOffset, NULL, FILE_BEGIN)) {
			res = RESULT(GetLastError());
			break;
		}

		std::vector<UCHAR> buf;
		buf.resize(OneMB, 0);

		unsigned __int64 currentOffset = 0;
		while (contentSize > 0) {

			const ULONG bytesToWrite = (ULONG)min(contentSize, OneMB);
			const ULONG bytesToDecrypt = ROUND_TO_SIZE(bytesToWrite, 16);
			DWORD dwWritten = 0;
			DWORD dwDecryptedSize = bytesToDecrypt;

			res = ReadFile(buf.data(), bytesToDecrypt, &dwRead);
			if (!res)
				break;
			if (dwRead != bytesToDecrypt) {
				res = RESULT(GetLastError());
				break;
			}

			res = NX::crypt::AesBlockDecrypt(cek, buf.data(), bytesToDecrypt, buf.data(), &dwDecryptedSize, _secret.GetIvSeed().data(), currentOffset, NXL_BLOCK_SIZE);
			if (!res)
				break;
			assert(bytesToDecrypt == dwDecryptedSize);

			if (!::WriteFile(hTarget, buf.data(), bytesToWrite, &dwWritten, NULL)) {
				res = RESULT(GetLastError());
				break;
			}

			// Move to next
			contentSize -= bytesToWrite;
			currentOffset += bytesToDecrypt;
		}

	} while (FALSE);
	CloseHandle(hTarget); hTarget = INVALID_HANDLE_VALUE;
	if (!res) {
		::DeleteFileW(dest.c_str());
		return res;
	}

	return RESULT(0);
}

Result File::ResetSecret(NX::NXL::FileSecret& secret, _In_reads_bytes_opt_(32) const UCHAR* recoveryKey)
{
	if(!IsOpened())
		return RESULT(ERROR_INVALID_HANDLE);
	if(IsReadOnly())
		return RESULT(ERROR_ACCESS_DENIED);
	if(!secret.GetToken().IsFullToken())
		return RESULT(ERROR_INVALID_PARAMETER);
	if(!IsFullToken())
		return RESULT(ERROR_NO_USER_SESSION_KEY);

	const NXL::FileToken& token = secret.GetToken();

	const ULONG bufSize = (ULONG)(sizeof(ULONG) + sizeof(NXL_HEADER_2));
	std::vector<UCHAR> buf;
	buf.resize(bufSize, 0);
	NXL_HEADER_2* header = (NXL_HEADER_2*)buf.data();
	DWORD dwSize = 0;

	Result res = ReadFileEx(0, header, (ULONG)sizeof(NXL_HEADER_2), &dwSize);
	if (!res)
		return res;
	if (dwSize != (ULONG)sizeof(NXL_HEADER_2))
		return RESULT(ERROR_FILE_CORRUPT);

	// use existing IvSeed
	secret._ivSeed.resize(16, 0);
	memcpy(secret._ivSeed.data(), header->fixed.keys.iv_seed, 16);

	// Reset ownerId
	memset(header->fixed.file_info.owner_id, 0, 256);
	memcpy(header->fixed.file_info.owner_id, secret.GetOwnerId().c_str(), secret.GetOwnerId().length());

	// Reset agreement
	memset(header->fixed.keys.public_key1, 0, 256);
	memcpy(header->fixed.keys.public_key1, secret.GetAgreement0().data(), secret.GetAgreement0().size());
	memset(header->fixed.keys.public_key2, 0, 256);
	if (0 != secret.GetAgreement1().size())
		memcpy(header->fixed.keys.public_key2, secret.GetAgreement1().data(), secret.GetAgreement1().size());
	
	//
	// Decrypt CEK and re-encrypt it
	//
	NX::crypt::AesKey oldKey;
	res = oldKey.Import(_secret.GetToken().GetKey().data(), (ULONG)_secret.GetToken().GetKey().size());
	if (!res)
		return res;
	NX::crypt::AesKey newKey;
	res = newKey.Import(token.GetKey().data(), (ULONG)token.GetKey().size());
	if (!res)
		return res;

	dwSize = 32;
	res = NX::crypt::AesBlockDecrypt(oldKey, header->fixed.keys.token_cek, 32, header->fixed.keys.token_cek, &dwSize, header->fixed.keys.iv_seed);
	if (!res)
		return res;

	// Calculate HMAC(Token, Cek)
	res = CalcCekChecksum(header->fixed.keys.token_cek, header->fixed.keys.token_cek + 32, token.GetKey().data());
	if (!res)
		return res;

	dwSize = 32;
	res = NX::crypt::AesBlockEncrypt(newKey, header->fixed.keys.token_cek, 32, header->fixed.keys.token_cek, &dwSize, header->fixed.keys.iv_seed);
	if (!res)
		return res;

	//
	// Decrypt Recovery CEK and re-encrypt it
	//
	if (NULL != recoveryKey) {
		memcpy(header->fixed.keys.recovery_cek, recoveryKey, 32);
		dwSize = 32;
		res = NX::crypt::AesBlockEncrypt(newKey, header->fixed.keys.recovery_cek, 32, header->fixed.keys.recovery_cek, &dwSize, header->fixed.keys.iv_seed);
		if (!res)
			return res;
		// Calculate HMAC(Token, Cek)
		res = CalcCekChecksum(header->fixed.keys.recovery_cek, header->fixed.keys.recovery_cek + 32, token.GetKey().data());
		if (!res)
			return res;
		SetFlag(header->fixed.keys.mode_and_flags, KF_RECOVERY_KEY_ENABLED);
	}
	else {
		if (BoolFlagOn(header->fixed.keys.mode_and_flags, KF_RECOVERY_KEY_ENABLED)) {

			dwSize = 32;
			res = NX::crypt::AesBlockDecrypt(oldKey, header->fixed.keys.recovery_cek, 32, header->fixed.keys.recovery_cek, &dwSize, header->fixed.keys.iv_seed);
			if (!res)
				return res;

			dwSize = 32;
			res = NX::crypt::AesBlockEncrypt(newKey, header->fixed.keys.recovery_cek, 32, header->fixed.keys.recovery_cek, &dwSize, header->fixed.keys.iv_seed);
			if (!res)
				return res;

			// Calculate HMAC(Token, Cek)
			res = CalcCekChecksum(header->fixed.keys.recovery_cek, header->fixed.keys.recovery_cek + 32, token.GetKey().data());
			if (!res)
				return res;
		}
	}


	//
	//  Re-calculate sections's checksum
	//
	memcpy(header->fixed.file_info.duid, token.GetId().data(), 16);
	header->fixed.keys.token_level = token.GetLevel();

	for (int i = 0; i < 32; i++) {
		if (header->fixed.sections.record[i].name[0] == 0)
			break;
		res = CalcSectionChecksum(&header->fixed.sections.record[i], token.GetKey().data());
		if (!res)
			return res;
	}
	

	//
	//  Re-calculate fixed header's checksum
	//
	res = NX::crypt::CreateHmacSha256(buf.data(), bufSize, token.GetKey().data(), 32, header->dynamic.fixed_header_hash);
	if (!res)
		return res;

	// Okay, now write the header back
	res = WriteFileEx(0, header, (ULONG)sizeof(NXL_HEADER_2), &dwSize);
	if (!res)
		return res;

	// Update token
	_secret = secret;
	return RESULT(0);
}

Result File::UnlockContentKey(NX::crypt::AesKey& cek) const
{
	if(!IsFullToken())
		return RESULT(ERROR_NO_USER_SESSION_KEY);

	UCHAR contentKey[64] = { 0 };
	PUCHAR checksumInFile = contentKey + 32;
	UCHAR cekChecksum[32] = { 0 };
	DWORD dwSize = 0;
	
	NX::crypt::AesKey tokenKey;
	Result res = tokenKey.Import(_secret.GetToken().GetKey().data(), (ULONG)_secret.GetToken().GetKey().size());
	if (!res)
		return res;

	res = ReadFileEx(FIELD_OFFSET(NXL_FIXED_HEADER, keys.token_cek), contentKey, 64, &dwSize);
	if (!res)
		return res;

#ifdef _DEBUG
	const std::wstring& encryptedCek = NX::bintohs<wchar_t>(contentKey, 32);
	const std::wstring& sChecksumInFile = NX::bintohs<wchar_t>(contentKey + 32, 32);
#endif

	dwSize = 32;
	res = NX::crypt::AesBlockDecrypt(tokenKey, contentKey, 32, contentKey, &dwSize, _secret.GetIvSeed().data());
	if (!res)
		return res;

	res = CalcCekChecksum(contentKey, cekChecksum, _secret.GetToken().GetKey().data());
	if (!res)
		return res;

#ifdef _DEBUG
	const std::wstring& decryptedCek = NX::bintohs<wchar_t>(contentKey, 32);
	const std::wstring& sChecksum = NX::bintohs<wchar_t>(cekChecksum, 32);
	const std::wstring& sTokenId = NX::bintohs<wchar_t>(_secret.GetToken().GetId().data(), 16);
	const std::wstring& sTokenKey = NX::bintohs<wchar_t>(_secret.GetToken().GetKey().data(), 32);
	const std::wstring& sIvecSeed = NX::bintohs<wchar_t>(_secret.GetIvSeed().data(), 16);
#endif

	if (0 != memcmp(cekChecksum, checksumInFile, 32))
		return RESULT(ERROR_DATA_CHECKSUM_ERROR);

	res = cek.Import(contentKey, 32);
	if (!res)
		return res;

	return RESULT(0);
}

Result File::ReadAllSectionRecords(std::vector<FileSection>& sections) const
{
	std::vector<UCHAR> buf;
	buf.resize(sizeof(NXL_SECTION_HEADER_2), 0);
	const NXL_SECTION_HEADER_2* p = (const NXL_SECTION_HEADER_2*)buf.data();
	DWORD dwSize = 0;

	Result res = ReadFileEx(FIELD_OFFSET(NXL_FIXED_HEADER, sections), buf.data(), sizeof(NXL_SECTION_HEADER_2), &dwSize);
	if (!res)
		return res;

	for (int i = 0; i < 32; i++) {
		const NXL_SECTION_2* ps = p->record + i;
		if (ps->name[0] == 0)
			break;
		sections.push_back(FileSection(ps));
	}

	return RESULT(0);
}

Result File::ReadMetadata(FileMeta& metadata)
{
	if (!IsOpened())
		return RESULT(ERROR_INVALID_HANDLE);

	FileSection section;
	Result res = FindSectionRecord(NXL_SECTION_NAME_FILEINFO, section, NULL);
	if (!res)
		return res;

	if (section.EmptyData())
		return RESULT(0);

	std::vector<UCHAR> buf;
	res = ReadSectionData(section, buf);
	if (!res)
		return res;

	metadata = FileMeta(std::string(buf.begin(), buf.end()));
	return RESULT(0);
}

Result File::WriteMetadata(const FileMeta& metadata)
{
	if (!IsOpened())
		return RESULT(ERROR_INVALID_HANDLE);
	if(IsReadOnly())
		return RESULT(ERROR_ACCESS_DENIED);
	if(!IsFullToken())
		return RESULT(ERROR_NO_USER_SESSION_KEY);

	FileSection section;
	Result res = FindSectionRecord(NXL_SECTION_NAME_FILEINFO, section, NULL);
	if (!res)
		return res;

	assert(!section.Empty());

	const std::string& s = metadata.ToString();

	res = WriteSectionData(section, (const UCHAR*)(s.empty() ? NULL : s.c_str()), (ULONG)s.size());
	if (!res)
		return res;
	
	return RESULT(0);
}

Result File::ReadAdHocPolicy(FileAdHocPolicy& adhocPolicy)
{
	if (!IsOpened())
		return RESULT(ERROR_INVALID_HANDLE);

	FileSection section;
	Result res = FindSectionRecord(NXL_SECTION_NAME_FILEPOLICY, section, NULL);
	if (!res)
		return res;

	if (section.EmptyData())
		return RESULT(0);

	std::vector<UCHAR> buf;
	res = ReadSectionData(section, buf);
	if (!res)
		return res;

	adhocPolicy = FileAdHocPolicy(std::string(buf.begin(), buf.end()));
	return RESULT(0);
}

Result File::WriteAdHocPolicy(const FileAdHocPolicy& adhocPolicy)
{
	if (!IsOpened())
		return RESULT(ERROR_INVALID_HANDLE);
	if (IsReadOnly())
		return RESULT(ERROR_ACCESS_DENIED);
	if (!IsFullToken())
		return RESULT(ERROR_NO_USER_SESSION_KEY);

	FileSection section;
	Result res = FindSectionRecord(NXL_SECTION_NAME_FILEPOLICY, section, NULL);
	if (!res)
		return res;

	assert(!section.Empty());

	const std::string& s = adhocPolicy.ToString();

	res = WriteSectionData(section, (const UCHAR*)(s.empty() ? NULL : s.c_str()), (ULONG)s.size());
	if (!res)
		return res;

	return RESULT(0);
}

Result File::ReadTags(FileTags& tags)
{
	if (!IsOpened())
		return RESULT(ERROR_INVALID_HANDLE);

	FileSection section;
	Result res = FindSectionRecord(NXL_SECTION_NAME_FILETAG, section, NULL);
	if (!res)
		return res;

	if (section.EmptyData())
		return RESULT(0);

	std::vector<UCHAR> buf;
	res = ReadSectionData(section, buf);
	if (!res)
		return res;

	tags = FileTags(std::string(buf.begin(), buf.end()));
	return RESULT(0);
}

Result File::WriteTags(const FileTags& tags)
{
	if (!IsOpened())
		return RESULT(ERROR_INVALID_HANDLE);
	if (IsReadOnly())
		return RESULT(ERROR_ACCESS_DENIED);
	if (!IsFullToken())
		return RESULT(ERROR_NO_USER_SESSION_KEY);

	FileSection section;
	Result res = FindSectionRecord(NXL_SECTION_NAME_FILETAG, section, NULL);
	if (!res)
		return res;

	assert(!section.Empty());

	const std::string& s = tags.ToString();

	res = WriteSectionData(section, (const UCHAR*)(s.empty() ? NULL : s.c_str()), (ULONG)s.size());
	if (!res)
		return res;

	return RESULT(0);
}

Result File::ReadFile(PVOID data, ULONG bytesToRead, _Out_opt_ ULONG* bytesRead) const
{
	if (INVALID_HANDLE_VALUE == _h)
		return RESULT(ERROR_INVALID_HANDLE);

	DWORD dwRead = 0;
	if(!::ReadFile(_h, data, bytesToRead, &dwRead, NULL))
		return RESULT(GetLastError());

	if (NULL != bytesRead)
		*bytesRead = dwRead;
	return RESULT(0);
}

Result File::WriteFile(const VOID* data, ULONG bytesToWrite, _Out_opt_ ULONG* bytesWritten)
{
	if (INVALID_HANDLE_VALUE == _h)
		return RESULT(ERROR_INVALID_HANDLE);

	DWORD dwWritten = 0;
	if(!::WriteFile(_h, data, bytesToWrite, &dwWritten, NULL))
		return RESULT(GetLastError());

	if (NULL != bytesWritten)
		*bytesWritten = dwWritten;
	return RESULT(0);
}

Result File::ReadFileEx(LONGLONG offset, PVOID data, ULONG bytesToRead, _Out_opt_ ULONG* bytesRead) const
{
	if (INVALID_HANDLE_VALUE == _h)
		return RESULT(ERROR_INVALID_HANDLE);

	LARGE_INTEGER liOffset = { 0, 0 };
	LARGE_INTEGER liNewOffset = { 0, 0 };

	liOffset.QuadPart = offset;
	if (!SetFilePointerEx(_h, liOffset, &liNewOffset, FILE_BEGIN))
		return RESULT(GetLastError());
	if(liOffset.QuadPart != liNewOffset.QuadPart)
		return RESULT(ERROR_INVALID_DATA);

	return ReadFile(data, bytesToRead, bytesRead);
}

Result File::WriteFileEx(LONGLONG offset, const VOID* data, ULONG bytesToWrite, _Out_opt_ ULONG* bytesWritten)
{
	if (INVALID_HANDLE_VALUE == _h)
		return RESULT(ERROR_INVALID_HANDLE);

	LARGE_INTEGER liOffset = { 0, 0 };
	LARGE_INTEGER liNewOffset = { 0, 0 };

	liOffset.QuadPart = offset;
	if (!SetFilePointerEx(_h, liOffset, &liNewOffset, FILE_BEGIN))
		return RESULT(GetLastError());
	if(liOffset.QuadPart != liNewOffset.QuadPart)
		return RESULT(ERROR_INVALID_DATA);

	return WriteFile(data, bytesToWrite, bytesWritten);
}

Result File::ReadSignature(NXL_SIGNATURE_LITE* sig)
{
	DWORD bytesRead = 0;
	Result res = ReadFileEx(0, sig, sizeof(NXL_SIGNATURE_LITE), &bytesRead);
	if (!res)
		return res;
	if (bytesRead != (ULONG)sizeof(NXL_SIGNATURE_LITE))
		return RESULT(ERROR_BAD_FILE_TYPE);

	if (sig->magic.code == NXL_MAGIC_CODE_1) {
		if (0 != memcmp(sig->magic.name, NXL_MAGIC_NAME_1, 8))
			return RESULT(ERROR_BAD_FILE_TYPE);
		sig->version = NXL_VERSION_10;
	}
	else if (sig->magic.code == NXL_MAGIC_CODE_2) {
		if (0 != memcmp(sig->magic.name, NXL_MAGIC_NAME_2, 8))
			return RESULT(ERROR_BAD_FILE_TYPE);
		if(sig->version != NXL_VERSION_30)
			return RESULT(ERROR_BAD_FILE_TYPE);
	}
	else {
		return RESULT(ERROR_BAD_FILE_TYPE);
	}
	
	return RESULT(0);
}

Result File::ReadFileSecret(FileSecret& secret)
{
	DWORD bytesRead = 0;
	std::vector<UCHAR> buf;
	buf.resize(sizeof(NXL_FIXED_HEADER), 0);
	NXL_FIXED_HEADER* header = (NXL_FIXED_HEADER*)buf.data();
	Result res = ReadFileEx(0, header, sizeof(NXL_FIXED_HEADER), &bytesRead);
	if (!res)
		return res;
	if (bytesRead != (ULONG)sizeof(NXL_FIXED_HEADER))
		return RESULT(ERROR_BAD_FILE_TYPE);

	// Get DUID (File Token Id)
	secret._token._id.resize(16, 0);
	memcpy(secret._token._id.data(), header->file_info.duid, 16);

	header->file_info.owner_id[255] = 0;
	secret._ownerId = header->file_info.owner_id;

	secret._keyFlags = header->file_info.flags & 0x00FFFFFF;
	secret._secureMode = (header->file_info.flags & 0xFF000000) >> 24;

	return RESULT(0);
}


Result File::FindSectionRecord(const std::string& name, FileSection& section, _Out_opt_ ULONG* pId) const
{
	std::vector<UCHAR> buf;
	buf.resize(sizeof(NXL_SECTION_HEADER_2), 0);
	const NXL_SECTION_HEADER_2* sections = (const NXL_SECTION_HEADER_2*)buf.data();
	DWORD dwSize = 0;

	Result res = ReadFileEx(FIELD_OFFSET(NXL_FIXED_HEADER, sections), buf.data(), sizeof(NXL_SECTION_HEADER_2), &dwSize);
	if (!res)
		return res;

	for (int i = 0; i < 32; i++) {
		const NXL_SECTION_2* ps = sections->record + i;
		if (ps->name[0] == 0)
			break;
		if (0 == _stricmp(name.c_str(), ps->name)) {
			section = FileSection(ps);
			if (pId)
				*pId = i;
			return RESULT(0);
		}
	}
	return RESULT(ERROR_NOT_FOUND);
}

Result File::ReadSectionRecord(const std::string& name, FileSection& section) const
{
	return FindSectionRecord(name, section, NULL);
}

Result File::WriteSectionRecord(_In_ const FileSection& section)
{
	ULONG id = 0;
	FileSection origSection;
	Result res = FindSectionRecord(section.GetName(), origSection, &id);
	if (!res)
		return res;
	
	const LONGLONG recordOffset = FIELD_OFFSET(NXL_FIXED_HEADER, sections.record) + sizeof(NXL_SECTION_2) * id;
	res = WriteFileEx(recordOffset, &section._s, (ULONG)sizeof(NXL_SECTION_2), NULL);
	if (!res)
		return res;

	return RESULT(0);
}

Result File::ReadSectionData(const FileSection& section, std::vector<UCHAR>& data) const
{
	if (!IsOpened())
		return RESULT(ERROR_INVALID_HANDLE);

	if(section.Empty() || section.EmptyData() || section.GetOffset() == 0)
		return RESULT(ERROR_INVALID_PARAMETER);

	if(!IS_ALIGNED(section.GetSize(), NXL_PAGE_SIZE) || !IS_ALIGNED(section.GetOffset(), NXL_PAGE_SIZE))
		return RESULT(ERROR_INVALID_PARAMETER);

	if(section.GetDataSize() == 0)
		return RESULT(0);


	std::vector<UCHAR> buf;
	buf.resize(sizeof(ULONG) + section.GetSize());
	PUCHAR content = buf.data() + sizeof(ULONG);
	DWORD dwSize = section.GetDataSize();
	ULONG rawDataSize = BoolFlagOn(section.GetFlags(), NXL_SECTION_FLAG_COMPRESSED) ? section.GetCompressedSize() : section.GetDataSize();
	if (BoolFlagOn(section.GetFlags(), NXL_SECTION_FLAG_ENCRYPTED))
		rawDataSize = ROUND_TO_SIZE(rawDataSize, 32);
	memcpy(buf.data(), &rawDataSize, sizeof(rawDataSize));

	Result res = ReadFileEx(section.GetOffset(), content, section.GetSize(), &dwSize);
	if (!res)
		return res;
	if (dwSize != section.GetSize())
		return RESULT(ERROR_INVALID_DATA);

	if (IsFullToken()) {
		assert(_secret.GetIvSeed().size() == 16);
		assert(_secret.GetToken().GetKey().size() == 32);
		UCHAR checksum[32] = { 0 };
		res = NX::crypt::CreateHmacSha256(buf.data(), 4 + rawDataSize, _secret.GetToken().GetKey().data(), 32, checksum);
		if (!res)
			return res;
		if (0 != memcmp(checksum, section.GetChecksum(), 32))
			return RESULT(ERROR_DATA_CHECKSUM_ERROR);
	}

	if (BoolFlagOn(section.GetFlags(), NXL_SECTION_FLAG_ENCRYPTED)) {
		// Need decryption
		NX::crypt::AesKey tokenKey;
		tokenKey.Import(_secret.GetToken().GetKey().data(), 32);
		dwSize = section.GetSize();
		res = NX::crypt::AesBlockDecrypt(tokenKey, content, section.GetSize(), content, &dwSize, _secret.GetIvSeed().data());
		if (!res)
			return res;
	}

	if (BoolFlagOn(section.GetFlags(), NXL_SECTION_FLAG_COMPRESSED)) {
		// Need decompression
		return RESULT(ERROR_NOT_SUPPORTED);
	}

	data.resize(section.GetDataSize(), 0);
	memcpy(data.data(), content, section.GetDataSize());
	return RESULT(0);
}

Result File::WriteSectionData(_Inout_ FileSection& section, const UCHAR* data, const ULONG size)
{
	if (!IsOpened())
		return RESULT(ERROR_INVALID_HANDLE);

	if (IsReadOnly())
		return RESULT(ERROR_ACCESS_DENIED);

	if (section.Empty() || section.EmptyData() || section.GetOffset() == 0)
		return RESULT(ERROR_INVALID_PARAMETER);

	if (!IS_ALIGNED(section.GetSize(), NXL_PAGE_SIZE) || !IS_ALIGNED(section.GetOffset(), NXL_PAGE_SIZE))
		return RESULT(ERROR_INVALID_PARAMETER);

	if (size > section.GetSize())
		return RESULT(ERROR_APP_DATA_LIMIT_EXCEEDED);

	assert(_secret.GetIvSeed().size() == 16);
	assert(_secret.GetToken().GetKey().size() == 32);

	std::vector<UCHAR> buf;
	buf.resize(sizeof(ULONG) + section.GetSize());
	PUCHAR content = buf.data() + sizeof(ULONG);
	DWORD dwSize = 0;
	Result res = RESULT(0);

	ULONG rawSize = 4 + size;

	if (0 != size) {

		section._s.data_size = (USHORT)size;
		memcpy(buf.data(), &size, sizeof(size));
		if (0 != size)
			memcpy(content, data, size);

		if (BoolFlagOn(section.GetFlags(), NXL_SECTION_FLAG_ENCRYPTED)) {
			// Need decryption
			NX::crypt::AesKey tokenKey;
			tokenKey.Import(_secret.GetToken().GetKey().data(), 32);
			dwSize = ROUND_TO_SIZE(size, 32);
			rawSize = 4 + ROUND_TO_SIZE(size, 32);
			res = NX::crypt::AesBlockEncrypt(tokenKey, content, dwSize, content, &dwSize, _secret.GetIvSeed().data());
			if (!res)
				return res;
		}
	}

	res = NX::crypt::CreateHmacSha256(buf.data(), rawSize, _secret.GetToken().GetKey().data(), 32, section.GetChecksum());
	if (!res)
		return res;

	res = WriteFileEx(section.GetOffset(), content, section.GetSize(), &dwSize);
	if (!res)
		return res;

	res = WriteSectionRecord(section);
	if (!res)
		return res;

	res = UpdateHeaderChecksum();
	if (!res)
		return res;

	return RESULT(0);
}

Result File::CalcHeaderChecksum(_Out_writes_bytes_(32) PUCHAR checksum, _In_reads_bytes_(32) const UCHAR* token) const
{
	const ULONG bufSize = (ULONG)(sizeof(ULONG) + sizeof(NXL_FIXED_HEADER));
	std::vector<UCHAR> buf;
	buf.resize(bufSize, 0);
	NXL_FIXED_HEADER* header = (NXL_FIXED_HEADER*)(buf.data() + sizeof(ULONG));
	DWORD dwSize = 0;

	if (NULL == token && (_secret.GetToken().Empty() || _secret.GetToken().GetKey().empty()))
		return RESULT(ERROR_NO_USER_SESSION_KEY);

	dwSize = sizeof(NXL_FIXED_HEADER);
	memcpy(buf.data(), &dwSize, sizeof(DWORD));

	Result res = ReadFileEx(0, header, (ULONG)sizeof(NXL_FIXED_HEADER), &dwSize);
	if (!res)
		return res;
	if (dwSize != (ULONG)sizeof(NXL_FIXED_HEADER))
		return RESULT(ERROR_INVALID_DATA);

	res = NX::crypt::CreateHmacSha256(buf.data(), bufSize, token ? token : _secret.GetToken().GetKey().data(), 32, checksum);
	if (!res)
		return res;

	return RESULT(0);
}

Result File::CalcSectionChecksum(_Inout_ NXL_SECTION_2* section, _In_reads_bytes_(32) const UCHAR* token) const
{
	if (0 == section->name[0] || 0 == section->size || 0 == section->offset || !IS_ALIGNED(section->offset, NXL_PAGE_SIZE) || !IS_ALIGNED(section->size, NXL_PAGE_SIZE))
		return RESULT(ERROR_INVALID_PARAMETER);

	if (NULL == token && (_secret.GetToken().Empty() || _secret.GetToken().GetKey().empty()))
		return RESULT(ERROR_NO_USER_SESSION_KEY);

	if (section->data_size == 0) {
		memset(section->checksum, 0, 32);
		return RESULT(0);
	}

	ULONG rawDataSize = BoolFlagOn(section->flags, NXL_SECTION_FLAG_COMPRESSED) ? section->compressed_size : section->data_size;
	if(BoolFlagOn(section->flags, NXL_SECTION_FLAG_ENCRYPTED))
		rawDataSize = ROUND_TO_SIZE(rawDataSize, 32);

	const ULONG bufSize = (ULONG)(sizeof(ULONG) + rawDataSize);
	std::vector<UCHAR> buf;
	buf.resize(bufSize);
	PUCHAR content = buf.data() + sizeof(ULONG);
	DWORD dwSize = 0;
	Result res = RESULT(0);

	memcpy(buf.data(), &rawDataSize, sizeof(ULONG));
	if (rawDataSize != 0) {
		res = ReadFileEx(section->offset, content, rawDataSize, &dwSize);
		if (!res)
			return res;
		if (dwSize != rawDataSize)
			return RESULT(ERROR_INVALID_DATA);
	}

	res = NX::crypt::CreateHmacSha256(buf.data(), (ULONG)buf.size(), token ? token : _secret.GetToken().GetKey().data(), 32, section->checksum);
	if (!res)
		return res;

	return RESULT(0);
}

Result File::CalcCekChecksum(_In_reads_bytes_(32) const UCHAR* cek, _Out_writes_bytes_(32) PUCHAR checksum, _In_reads_bytes_(32) const UCHAR* token) const
{
	const ULONG size = 32;
	UCHAR buf[36] = { 0 };
	memcpy(buf, &size, 4);
	memcpy(buf + 4, cek, 32);

	Result res = NX::crypt::CreateHmacSha256(buf, 36, token ? token : _secret.GetToken().GetKey().data(), 32, checksum);
	if (!res)
		return res;
	
	return RESULT(0);
}

Result File::CalcHmacChecksum(_In_opt_ const UCHAR* data, _In_ const ULONG dataSize, _In_ const UCHAR* key, _In_ ULONG keySize, _Out_writes_bytes_(32) PUCHAR checksum, _In_ const ULONG dataAlignment) const
{
	if (NULL == checksum || NULL == key || 0 == keySize)
		return RESULT(ERROR_INVALID_PARAMETER);

	memset(checksum, 0, 32);
	if (NULL != data && 0 != dataSize) {
		std::vector<UCHAR> buf;
		const ULONG dataBufSize = (dataAlignment < 2) ? dataSize : ROUND_TO_SIZE(dataSize, dataAlignment);
		buf.resize(sizeof(ULONG) + dataBufSize, 0);
		if (dataSize != 0 && NULL != data) {
			memcpy(buf.data() + sizeof(ULONG), data, dataSize);
		}
		*((ULONG*)buf.data()) = dataBufSize;
		Result res = NX::crypt::CreateHmacSha256(buf.data(), (ULONG)buf.size(), key, keySize, checksum);
		if (!res)
			return res;
	}

	return RESULT(0);
}

Result File::GetContentSize(__int64* size) const
{
	DWORD dwSize = 0;
	Result res = ReadFileEx(FIELD_OFFSET(NXL_HEADER_2, dynamic.content_length), size, sizeof(__int64), &dwSize);
	if (!res) {
		*size = 0;
		return res;
	}
	if (dwSize != (DWORD)sizeof(__int64)) {
		*size = 0;
		return RESULT(ERROR_INVALID_DATA);
	}
	return RESULT(0);
}

Result File::GetHeaderChecksum(_Out_writes_bytes_(32) PUCHAR checksum) const
{
	DWORD dwSize = 0;
	Result res = ReadFileEx(FIELD_OFFSET(NXL_HEADER_2, dynamic.fixed_header_hash), checksum, 32, &dwSize);
	if (!res)
		return res;
	if (32 != dwSize)
		return RESULT(ERROR_INVALID_DATA);
	return RESULT(0);
}

Result File::VerifyHeaderChecksum() const
{
	UCHAR checksum[32] = { 0 };
	UCHAR checksumInFile[32] = { 0 };
	DWORD dwSize = 0;

	Result res = CalcHeaderChecksum(checksum);
	if (!res)
		return res;

	res = ReadFileEx(FIELD_OFFSET(NXL_HEADER_2, dynamic), checksumInFile, 32, &dwSize);
	if (!res)
		return res;
	if (32 != dwSize)
		return RESULT(ERROR_INVALID_DATA);

#ifdef _DEBUG
	const std::wstring& checksum1 = NX::bintohs<wchar_t>(checksum, 32);
	const std::wstring& checksum2 = NX::bintohs<wchar_t>(checksumInFile, 32);
#endif

	if(0 != memcmp(checksum, checksumInFile, 32))
		return RESULT(ERROR_DATA_CHECKSUM_ERROR);

	return RESULT(0);
}

Result File::UpdateHeaderChecksum()
{
	UCHAR checksum[32] = { 0 };

	Result res = CalcHeaderChecksum(checksum);
	if (!res)
		return res;

	res = WriteFileEx(FIELD_OFFSET(NXL_HEADER_2, dynamic), checksum, 32, NULL);
	if (!res)
		return res;

	return RESULT(0);
}