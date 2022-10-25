
#include "stdafx.h"

#include <nx\rmc\user.h>

#include <nx\common\result.h>
#include <nx\common\string.h>
#include <nx\common\time.h>
#include <nx\common\rapidjson.h>
#include <nx\crypt\md5.h>

using namespace NX;
using namespace NX::rapidjson;


RmUser::RmUser() : _idpType(SKYDRM)
{
}

RmUser::RmUser(const RmUser& rhs)
    : _name(rhs._name), _id(rhs._id), _email(rhs._email), _idpType(rhs._idpType), _preferences(rhs._preferences), _ticket(rhs._ticket), _memberships(rhs._memberships)
{
}

RmUser::~RmUser()
{
}

RmUser& RmUser::operator = (const RmUser& rhs)
{
    if (this != &rhs)
    {
        _name = rhs._name;
        _id = rhs._id;
        _email = rhs._email;
		_idpType = rhs._idpType;
        _preferences = rhs._preferences;
        _ticket = rhs._ticket;
        _memberships = rhs._memberships;
    }

    return *this;
}

void RmUser::Clear()
{
	_idpType = SKYDRM;
    _name.clear();
    _id.clear();
    _email.clear();
    _preferences.Clear();
    _ticket.Clear();
    _memberships.clear();
}

const RmUserMembership* RmUser::Findmembership(const std::wstring& membershipId) const
{
	if (membershipId.empty()) {
		return _memberships.empty() ? NULL : (&_memberships[0]);
	}
	else {
		auto pos = std::find_if(_memberships.cbegin(), _memberships.cend(), [&](const RmUserMembership& member) -> bool {
			return (0 == _wcsicmp(member.GetId().c_str(), membershipId.c_str()));
		});
		return (pos == _memberships.cend()) ? NULL : (&(*pos));
	}
}

RmUserMembership* RmUser::Findmembership(const std::wstring& membershipId)
{
	if (membershipId.empty()) {
		return _memberships.empty() ? NULL : (&_memberships[0]);
	}
	else {
		auto pos = std::find_if(_memberships.begin(), _memberships.end(), [&](const RmUserMembership& member) -> bool {
			return (0 == _wcsicmp(member.GetId().c_str(), membershipId.c_str()));
		});
		return (pos == _memberships.cend()) ? NULL : (&(*pos));
	}
}

PJSONVALUE RmUser::ToJsonValue(bool withTicket) const
{
	PJSONOBJECT root = NX::rapidjson::JsonValue::CreateObject();
	if (NULL == root)
		return NULL;

	bool bSuccess = false;

	do {

		PJSONVALUE pv = NULL;

		const __int64 nId = std::stoll(_id);
		pv = JsonValue::CreateNumber(nId);
		if (NULL == pv)
			break;
		root->set(L"userId", pv); pv = NULL;

		pv = JsonValue::CreateString(_name);
		if (NULL == pv)
			break;
		root->set(L"name", pv); pv = NULL;

		pv = JsonValue::CreateString(_email);
		if (NULL == pv)
			break;
		root->set(L"email", pv); pv = NULL;

		pv = JsonValue::CreateNumber(_idpType);
		if (NULL == pv)
			break;
		root->set(L"idpType", pv); pv = NULL;

		pv = _preferences.ToJsonValue();
		if (NULL == pv)
			break;
		root->set(L"preferences", pv); pv = NULL;

		pv = JsonValue::CreateArray();
		if (NULL != pv) {
			std::for_each(_memberships.begin(), _memberships.end(), [&](const RmUserMembership& member) {
				PJSONVALUE pMember = member.ToJsonValue();
				if (pMember) {
					pv->AsArray()->push_back(pMember);
				}
			});
			root->set(L"memberships", pv); pv = NULL;
		}

		if (withTicket) {

			pv = JsonValue::CreateString(_ticket.GetTicket());
			if (NULL == pv)
				break;
			root->set(L"ticket", pv); pv = NULL;

			pv = JsonValue::CreateNumber(_ticket.GetTTL());
			if (NULL == pv)
				break;
			root->set(L"ttl", pv); pv = NULL;
		}

		bSuccess = true;

	} while (FALSE);

	if (!bSuccess) {
		delete root;
		root = NULL;
	}

	return root;
}

void RmUser::FromJsonValue(PCJSONVALUE pv)
{
	Clear();
	if (NULL == pv)
		return;
	if (!pv->IsObject())
		return;

	PCJSONVALUE item = pv->AsObject()->at(L"userId");
	if (item) {
		if (item->IsNumber())
			_id = NX::i64tos<wchar_t>(item->AsNumber()->ToInt64());
		else if (item->IsString())
			_id = item->AsString()->GetString();
		else
			; // Do nothing
	}
	if (_id.empty())
		return;

	item = pv->AsObject()->at(L"name");
	if (item && item->IsString())
		_name = item->AsString()->GetString();

	item = pv->AsObject()->at(L"email");
	if (item && item->IsString())
		_email = item->AsString()->GetString();

	item = pv->AsObject()->at(L"idpType");
	if (item && item->IsNumber())
		_idpType = (IDPTYPE)item->AsNumber()->ToInt();

	item = pv->AsObject()->at(L"preferences");
	if (item && item->IsObject())
		_preferences.FromJsonValue(item);

	_ticket.FromJsonValue(pv);

	item = pv->AsObject()->at(L"memberships");
	if (item && item->IsArray() && item->AsArray()->size() > 0) {
		const int count = (int)item->AsArray()->size();
		for (int i = 0; i < count; i++) {
			PCJSONVALUE pMember = item->AsArray()->at(i);
			if (pMember) {
				RmUserMembership member;
				member.FromJsonValue(pMember);
				if (!member.Empty()) {
					_memberships.push_back(member);
				}
			}
		}
	}
}



RmUserPreferences::RmUserPreferences() : _securityMode(0)
{
}

RmUserPreferences::RmUserPreferences(const std::wstring& defaultTenant, const std::wstring& defaultMember, int securityMode)
    : _defaultTenant(defaultTenant), _defaultMember(defaultMember), _securityMode(securityMode)
{
}

RmUserPreferences::RmUserPreferences(const RmUserPreferences& rhs)
    : _defaultTenant(rhs._defaultTenant), _defaultMember(rhs._defaultMember), _securityMode(rhs._securityMode)
{
}

RmUserPreferences::~RmUserPreferences()
{
}

RmUserPreferences& RmUserPreferences::operator = (const RmUserPreferences& rhs)
{
    if (this != &rhs)
    {
        _defaultTenant = rhs._defaultTenant;
        _defaultMember = rhs._defaultMember;
        _securityMode = rhs._securityMode;
    }

    return *this;
}

void RmUserPreferences::Clear()
{
    _defaultTenant.clear();
    _defaultMember.clear();
    _securityMode = 0;
}

PJSONVALUE RmUserPreferences::ToJsonValue() const
{
	PJSONOBJECT root = NX::rapidjson::JsonValue::CreateObject();
	if (NULL == root)
		return NULL;

	bool bSuccess = false;

	do {

		PJSONVALUE pv;

		pv = JsonValue::CreateString(_defaultTenant);
		if (NULL == pv)
			break;
		root->set(L"defaultTenant", pv); pv = NULL;

		pv = JsonValue::CreateNumber(_securityMode);
		if (NULL == pv)
			break;
		root->set(L"securityMode", pv); pv = NULL;

		pv = JsonValue::CreateString(_defaultMember);
		if (NULL == pv)
			break;
		root->set(L"defaultMember", pv); pv = NULL;

		bSuccess = true;

	} while (FALSE);

	if (!bSuccess) {
		delete root;
		root = NULL;
	}

	return root;
}

void RmUserPreferences::FromJsonValue(PCJSONVALUE pv)
{
	Clear();
	if (NULL == pv)
		return;
	if (!pv->IsObject())
		return;

	PCJSONVALUE item = pv->AsObject()->at(L"defaultTenant");
	if (item && item->IsString())
		_defaultTenant = item->AsString()->GetString();

	item = pv->AsObject()->at(L"securityMode");
	if (item && item->IsNumber())
		_securityMode = item->AsNumber()->ToInt();

	item = pv->AsObject()->at(L"defaultMember");
	if (item && item->IsString())
		_defaultMember = item->AsString()->GetString();
}


RmUserTicket::RmUserTicket() : _ttl(0)
{
}

RmUserTicket::RmUserTicket(const std::wstring& ticket, __int64 ttl) : _ttl(ttl), _ticket(ticket)
{
}

RmUserTicket::RmUserTicket(const RmUserTicket& rhs)
    : _ticket(rhs._ticket), _ttl(rhs._ttl)
{
}

RmUserTicket::~RmUserTicket()
{
}

RmUserTicket& RmUserTicket::operator = (const RmUserTicket& rhs)
{
    if (this != &rhs)
    {
        _ticket = rhs._ticket;
        _ttl = rhs._ttl;
    }

    return *this;
}

void RmUserTicket::Clear()
{
    _ticket.clear();
    _ttl = 0;
}

bool RmUserTicket::Expired() const
{
    const time::datetime& ct = time::datetime::current_time();
    return (ct.to_java_time() > _ttl);
}

PJSONVALUE RmUserTicket::ToJsonValue() const
{
	PJSONOBJECT root = NX::rapidjson::JsonValue::CreateObject();
	if (NULL == root)
		return NULL;

	bool bSuccess = false;

	do {

		PJSONVALUE pv;

		pv = JsonValue::CreateString(_ticket);
		if (NULL == pv)
			break;
		root->set(L"ticket", pv); pv = NULL;

		pv = JsonValue::CreateNumber(_ttl);
		if (NULL == pv)
			break;
		root->set(L"ttl", pv); pv = NULL;

		bSuccess = true;

	} while (FALSE);

	if (!bSuccess) {
		delete root;
		root = NULL;
	}

	return root;
}

void RmUserTicket::FromJsonValue(PCJSONVALUE pv)
{
	Clear();
	if (NULL == pv)
		return;
	if (!pv->IsObject())
		return;

	PCJSONVALUE item = item = pv->AsObject()->at(L"ticket");
	if (item && item->IsString())
		_ticket = item->AsString()->GetString();

	item = pv->AsObject()->at(L"ttl");
	if (item && item->IsNumber())
		_ttl = item->AsNumber()->ToInt64();
}




RmUserMembership::RmUserMembership() : _type(0)
{
	::InitializeCriticalSection(&_tokensLock);
}

RmUserMembership::RmUserMembership(const std::wstring& id, int type, const std::wstring& tenantId)
    : _id(NX::duplower<wchar_t>(id)), _type(type), _tenantId(tenantId), _idHash(CreateIdHash(id))
{
	::InitializeCriticalSection(&_tokensLock);
}

RmUserMembership::RmUserMembership(const RmUserMembership& rhs)
    : _id(rhs._id), _idHash(rhs._idHash), _type(rhs._type), _tenantId(rhs._tenantId), _agreement0(rhs._agreement0), _agreement1(rhs._agreement1), _tokens(rhs._tokens)
{
	::InitializeCriticalSection(&_tokensLock);
}

RmUserMembership::~RmUserMembership()
{
	::DeleteCriticalSection(&_tokensLock);
}

void RmUserMembership::Clear()
{
    _id.clear();
	_idHash.clear();
    _tenantId.clear();
	_agreement0.clear();
	_agreement1.clear();
	_tokens.clear();
    _type = 0;
}

int RmUserMembership::GetFileTokenCount()
{
	int count = 0;
	::EnterCriticalSection(&_tokensLock);
	count = (int)_tokens.size();
	::LeaveCriticalSection(&_tokensLock);
	return count;
}

NXL::FileToken RmUserMembership::GetFileToken()
{
	NXL::FileToken token;
	::EnterCriticalSection(&_tokensLock);
	if (!_tokens.empty()) {
		token = _tokens.front();
		_tokens.pop_front();
	}
	::LeaveCriticalSection(&_tokensLock);
	return token;
}

int RmUserMembership::AddFileTokens(const std::vector<NXL::FileToken>& tokens)
{
	int count = 0;
	::EnterCriticalSection(&_tokensLock);
	std::for_each(tokens.cbegin(), tokens.cend(), [&](const NXL::FileToken& token) {
		if (!token.Empty() && 32 == token.GetKey().size()) {
			_tokens.push_back(token);
		}
	});
	count = (int)_tokens.size();
	::LeaveCriticalSection(&_tokensLock);
	return count;
}

std::string RmUserMembership::SerializeFileTokensCache() const
{
	std::string s;
	JsonObject* root(JsonValue::CreateObject());
	if (NULL == root)
		return s;

	do {

		root->set(L"membership", JsonValue::CreateString(_id));
		JsonArray* tknArray = JsonValue::CreateArray();
		if (NULL == tknArray)
			break;
		root->set(L"tokens", tknArray);

		::EnterCriticalSection(&_tokensLock);
		std::for_each(_tokens.cbegin(), _tokens.cend(), [&](const NXL::FileToken& token) {
			if (!token.Empty() && 32 == token.GetKey().size()) {
				JsonObject* tknObject = JsonValue::CreateObject();
				if (NULL != tknObject) {
					tknArray->push_back(tknObject);
					tknObject->set(L"id", JsonValue::CreateString(NX::bintohs<wchar_t>(token.GetId().data(), token.GetId().size())));
					tknObject->set(L"key", JsonValue::CreateString(NX::bintohs<wchar_t>(token.GetKey().data(), token.GetKey().size())));
					tknObject->set(L"level", JsonValue::CreateNumber(token.GetLevel()));
				}
			}
		});
		::LeaveCriticalSection(&_tokensLock);

		JsonStringWriter<char> writer;
		s = writer.Write(root);
	} while (FALSE);

	delete root;
	root = NULL;

	return s;
}

Result RmUserMembership::DeserializeFileTokensCache(const std::string& s)
{
	JsonDocument doc;
	if (!doc.LoadJsonString<char>(s))
		return RESULT(ERROR_INVALID_DATA);

	PCJSONVALUE root = doc.GetRoot();
	if(root == NULL)
		return RESULT(ERROR_INVALID_DATA);
	if(!IsValidJsonObject(root))
		return RESULT(ERROR_INVALID_DATA);

	PCJSONVALUE pv = root->AsObject()->at(L"membership");
	if(!IsValidJsonString(pv))
		return RESULT(ERROR_INVALID_DATA);
	const std::wstring& ownerId = pv->AsString()->GetString();
	if(0 != _wcsicmp(_id.c_str(), ownerId.c_str()))
		return RESULT(ERROR_INVALID_DATA);
	
	pv = root->AsObject()->at(L"tokens");
	if(!IsValidJsonArray(pv))
		return RESULT(ERROR_INVALID_DATA);

	std::for_each(pv->AsArray()->cbegin(), pv->AsArray()->cend(), [&](PCJSONVALUE item) {
		if (!IsValidJsonObject(item))
			return;
		PCJSONVALUE p = item->AsObject()->at(L"id");
		if (!IsValidJsonString(p))
			return;
		const std::wstring& tokenId = p->AsString()->GetString();
		p = item->AsObject()->at(L"key");
		if (!IsValidJsonString(p))
			return;
		const std::wstring& tokenKey = p->AsString()->GetString();
		p = item->AsObject()->at(L"level");
		if (!IsValidJsonNumber(p))
			return;
		const UINT tokenLevel = p->AsNumber()->ToUint();
		NXL::FileToken newToken(tokenId, tokenLevel, tokenKey);
		if (!newToken.Empty() && newToken.GetKey().size() == 32) {
			::EnterCriticalSection(&_tokensLock);
			_tokens.push_back(newToken);
			::LeaveCriticalSection(&_tokensLock);
		}
	});

	return RESULT(0);
}

PJSONVALUE RmUserMembership::ToJsonValue() const
{
	PJSONOBJECT root = NX::rapidjson::JsonValue::CreateObject();
	if (NULL == root)
		return NULL;

	bool bSuccess = false;

	do {

		PJSONVALUE pv;

		pv = JsonValue::CreateString(_id);
		if (NULL == pv)
			break;
		root->set(L"id", pv); pv = NULL;

		pv = JsonValue::CreateNumber(_type);
		if (NULL == pv)
			break;
		root->set(L"type", pv); pv = NULL;

		pv = JsonValue::CreateString(_tenantId);
		if (NULL == pv)
			break;
		root->set(L"tenantId", pv); pv = NULL;

		bSuccess = true;

	} while (FALSE);

	if (!bSuccess) {
		delete root;
		root = NULL;
	}

	return root;
}

void RmUserMembership::FromJsonValue(PCJSONVALUE pv)
{
	Clear();
	if (NULL == pv)
		return;
	if (!pv->IsObject())
		return;

	PCJSONVALUE item = pv->AsObject()->at(L"id");
	if (NULL == item)
		return;
	if (!item->IsString())
		return;
	_id = item->AsString()->GetString();
	if (_id.empty())
		return;
	NX::tolower<wchar_t>(_id);
	_idHash = CreateIdHash(_id);

	item = pv->AsObject()->at(L"type");
	if (item && item->IsNumber())
		_type = item->AsNumber()->ToInt();

	item = pv->AsObject()->at(L"tenantId");
	if (item && item->IsString())
		_tenantId = item->AsString()->GetString();
}

RmUserMembership& RmUserMembership::operator = (const RmUserMembership& rhs)
{
    if (this != &rhs) {
        _id = rhs._id;
		_idHash = rhs._idHash;
        _type = rhs._type;
        _tenantId = rhs._tenantId;
		_agreement0 = rhs._agreement0;
		_agreement1 = rhs._agreement1;
		_tokens = rhs._tokens;
    }
    return *this;
}

std::wstring RmUserMembership::CreateIdHash(const std::wstring& id)
{
	std::wstring lowId = NX::duplower<wchar_t>(id);
	const unsigned long lowIdSize = (unsigned long)(lowId.length() * 2);
	UCHAR hash[16] = { 0 };
	Result res = NX::crypt::CreateHmacMd5((const UCHAR*)lowId.c_str(), lowIdSize, (const unsigned char*)&lowIdSize, 4, hash);
	return res ? NX::bintohs<wchar_t>(hash, 16) : std::wstring();
}