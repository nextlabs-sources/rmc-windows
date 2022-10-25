
#include "stdafx.h"

#include <regex>

#include <nx\rmc\session.h>
#include <nx\rmc\securefile.h>

#include <nx\common\stringex.h>
#include <nx\common\rapidjson.h>
#include <nx\rest\uri.h>

using namespace NX;
using namespace NX::rapidjson;


Result RmSession::InitRMS(const std::wstring& tenantName)
{
	RmRestStatus status;
	std::wstring rmsUrl;
	Result res = _restClient.RouterQuery(_router.GetHostUrl(), tenantName, status, rmsUrl);
	if (!res) {
		return res;
	}

	if (!status) {
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	}

	_rmserver = RmServer(tenantName, rmsUrl);
	return RESULT(0);
}

Result RmSession::PrepareLogin(const std::wstring& tenantName)
{
	return InitRMS(tenantName);
}

Result RmSession::Login(const std::wstring& userName, const std::wstring& password)
{
	if (_rmserver.Empty() || _rmserver.GetTenantName().empty())
		return RESULT(ERROR_NOT_READY);

	RmRestStatus status;
	Result res = _restClient.LoginWithNativeAccount(_rmserver.GetUrl(), userName, password, status, _user);
	if (!res)
		return res;

	if (200 != status.GetStatus()) {
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	}
	
	FinalizeLogin();
	return RESULT(0);
}

Result RmSession::LoginFromResult(const std::string& jsonResult)
{
	if (jsonResult.empty())
		return RESULT(ERROR_INVALID_DATA);

	JsonDocument doc;
	int err_code = 0;
	size_t err_pos = 0;
	if (!doc.LoadJsonString(jsonResult, &err_code, &err_pos)) {
		return RESULT(ERROR_INVALID_DATA);
	}

	PCJSONVALUE root = doc.GetRoot();
	if(NULL == root)
		return RESULT(ERROR_INVALID_DATA);
	if(!root->IsObject())
		return RESULT(ERROR_INVALID_DATA);

	PCJSONVALUE pv = root->AsObject()->at(L"statusCode");
	if(!IsValidJsonNumber(pv))
		return RESULT(ERROR_INVALID_DATA);
	const int statusCode = pv->AsNumber()->ToInt();
	std::string statusPhrase;
	pv = root->AsObject()->at(L"message");
	if (IsValidJsonString(pv)) {
		statusPhrase = NX::conv::utf16toutf8(pv->AsString()->GetString());
	}

	if(statusCode != 200)
		return RESULT2(statusCode, statusPhrase);

	PCJSONVALUE extra = root->AsObject()->at(L"extra");
	if(NULL == extra)
		return RESULT(ERROR_INVALID_DATA);
	if(!extra->IsObject())
		return RESULT(ERROR_INVALID_DATA);


	RmUser user;
	user.FromJsonValue(extra);
	if(user.Empty() || user.GetEmail().empty())
		return RESULT(ERROR_INSUFFICIENT_LOGON_INFO);

	if(user.GetTicket().Empty())
		return RESULT(ERROR_NO_TOKEN);

	if (user.GetTicket().Expired())
		return RESULT(ERROR_INVALID_TOKEN);

	
	// Succeeded
	_user = user;
	_restClient.SetCredential(_rmserver.GetTenantName(), _user.GetId(), _user.GetName(), _user.GetTicket().GetTicket());
	FinalizeLogin();
	return RESULT(0);
}

Result RmSession::Logout()
{
	if (_user.Empty())
		return RESULT(ERROR_INSUFFICIENT_LOGON_INFO);
	
	// Save all the tokens for current user.
	SaveAllMembershipTokens();
	_user.GetTicket().Clear();
	RemoveCachedUserToken(_rmserver.GetTenantName(), _user.GetId());
	RemoveCachedLoginHistory();
	_user.Clear();
	_restClient.ClearCredential();
	return RESULT(0);
}

Result RmSession::ChangePassword(const std::wstring& oldPassword, const std::wstring& newPassword)
{
	if (_user.Empty())
		return RESULT(ERROR_INSUFFICIENT_LOGON_INFO);

	static const std::wregex pattern(L"^(?=.*[A-Za-z])(?=.*\\d)(?=.*[\\W])[A-Za-z\\d\\W]{8,}$", std::regex_constants::icase);
	bool isValid = false;
	try {
		isValid = std::regex_match(newPassword, pattern);
	}
	catch (...) {
		isValid = false;
	}
	if (!isValid)
		return RESULT(ERROR_INVALID_PASSWORD);

	RmRestStatus status;
	Result res = _restClient.UserChangePassword(_rmserver.GetUrl(), oldPassword, newPassword, status);
	if (!res)
		return res;
	if (!status) {
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	}

	return RESULT(0);
}

Result RmSession::PrepareResetPassword(std::wstring& nonce, std::vector<UCHAR>& captcha)
{
	RmRestStatus status;
	Result res = _restClient.UserGetCaptcha(_rmserver.GetUrl(), status, captcha, nonce);
	if (!res)
		return res;
	if (!status) {
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	}

	return RESULT(0);
}

Result RmSession::ResetPassword(const std::wstring& email, const std::wstring& nonce, const std::wstring& captcha)
{
	if (!ValidateCredential())
		return RESULT(ERROR_EXPIRED_HANDLE);

	RmRestStatus status;
	Result res = _restClient.UserResetPassword(_rmserver.GetUrl(), email, nonce, captcha, status);
	if (!res)
		return res;
	if (!status) {
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	}

	return RESULT(0);
}

Result RmSession::UpdateProfileDisplayName(const std::wstring& displayName)
{
	if (!ValidateCredential())
		return RESULT(ERROR_EXPIRED_HANDLE);

	RmRestStatus status;
	Result res = _restClient.UserUpdateProfile(_rmserver.GetUrl(), displayName, std::wstring(), std::wstring(), status);
	if (!res)
		return res;
	if (!status) {
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	}

	_user._name = displayName;
	// Update cached user profile
	CacheUserProfile(_rmserver.GetTenantName(), _user);
	return RESULT(0);
}

Result RmSession::UpdateProfile()
{
	if (!ValidateCredential())
		return RESULT(ERROR_EXPIRED_HANDLE);

	RmRestStatus status;
	RmUser profile;
	Result res = _restClient.UserQueryProfile(_rmserver.GetUrl(), status, profile);
	if (!res)
		return res;
	if (!status) {
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	}

	_user._id = profile._id;
	_user._name = profile._name;
	_user._email = profile._email;
	_user._preferences = profile._preferences;
	_user._memberships = profile._memberships;
	FinalizeLogin();
	return RESULT(0);
}

Result RmSession::FinalizeLogin()
{
	// This function will only be called after successfully login
	if (!ValidateCredential()) {
		assert(false);
		return RESULT(ERROR_INSUFFICIENT_LOGON_INFO);
	}


	_restClient.SetCredential(_rmserver.GetTenantName(), _user.GetId(), _user.GetName(), _user.GetTicket().GetTicket());

	// Cache login information
	CacheLoginHistory();
	CacheUserProfile(_rmserver.GetTenantName(), _user);
	CacheUserToken(_rmserver.GetTenantName(), _user);

	//
	// Synchronize memberships (profile & agreements)
	//
	std::list<std::wstring> cachedMembers = FindAllCachedMemberships();
	for (std::vector<RmUserMembership>::iterator it = _user.GetMemberships().begin(); it != _user.GetMemberships().end(); ++it) {

		RmUserMembership& membership = *it;

		assert(!membership.Empty());

		// Remove it from cached list, and we will delete those finally left (means they are not on server side)
		cachedMembers.remove_if([&](const std::wstring& hash) -> bool {return (0 == _wcsicmp(hash.c_str(), membership.GetIdHash().c_str())); });

		RmUserMembership cachedMembership;
		Result res = LoadCachedMembership(membership.GetIdHash(), cachedMembership);
		if (!res || cachedMembership.GetAgreement0().empty()) {
			// No cached data/certificates on disk
			// Send request to server
			// In this case, we need to cache the agreement
			res = QueryMembershipAgreements(membership);
			// If succeed, cache both profile and certificates, otherwise only cache profile
			SaveMembership(membership, res ? false : true);
		}
		else {
			// Certificates has been cached before, We only update membership profile cache (profile only)
			SaveMembership(membership, true);
			// Always loading cached tokens & Cached Agreement
			membership.SetAgreement0(cachedMembership.GetAgreement0());
			membership.SetAgreement1(cachedMembership.GetAgreement1());
			membership._tokens = cachedMembership._tokens;
		}
		// Always ensure there are enough file tokens
		if (it == _user.GetMemberships().begin()) {
			// Only do it for first membership (default)
			EnsureEnoughFileToken(membership);
		}
	}
	const std::wstring& membersDir = GetCurrentUserMembershipsDir();
	std::for_each(cachedMembers.begin(), cachedMembers.end(), [&](const std::wstring& hash) {
		// All files related with these left membership id hash will be removed from disk (because server doesn't have them any more)
		const std::wstring memberProfile(membersDir + L"\\" + hash + L".profile");
		const std::wstring memberAgreement0(membersDir + L"\\" + hash + L".agreement0");
		const std::wstring memberAgreement1(membersDir + L"\\" + hash + L".agreement1");
		const std::wstring memberTokens(membersDir + L"\\" + hash + L".tokens");
		::DeleteFileW(memberProfile.c_str());
		::DeleteFileW(memberAgreement0.c_str());
		::DeleteFileW(memberAgreement1.c_str());
		::DeleteFileW(memberTokens.c_str());
	});

	return RESULT(0);
}

Result RmSession::LoadExistingCredential()
{
	std::wstring tenantName;
	std::wstring rmsUrl;
	std::wstring userId;
	RmRouter     router;

	Result res = QueryLoginHistoryEx(router, tenantName, rmsUrl, userId);
	if (!res)
		return res;

	if (_router.Empty()) {
		_router = router;
	}

	_rmserver = RmServer(tenantName, rmsUrl);

	// Try to find existing user home directory
	RmUser cachedUser;
	res = QueryCachedUserProfile(tenantName, userId, cachedUser);
	if (!res)
		return res;

	RmUserTicket cachedTicket;
	res = QueryCachedUserToken(tenantName, userId, cachedTicket);
	if (!res)
		return res;

	if (!cachedTicket.Empty() && !cachedTicket.Expired()) {
		cachedUser.SetTicket(cachedTicket);
		_user = cachedUser;
		_restClient.SetCredential(_rmserver.GetTenantName(), _user.GetId(), _user.GetName(), _user.GetTicket().GetTicket());
	}

	// Load user's agreements
	for (std::vector<RmUserMembership>::iterator it = _user.GetMemberships().begin(); it != _user.GetMemberships().end(); ++it) {

		RmUserMembership& membership = *it;

		assert(!membership.Empty());

		RmUserMembership cachedMembership;
		Result res = LoadCachedMembership(membership.GetIdHash(), cachedMembership);
		if (res) {
			assert(0 == _wcsicmp(membership.GetIdHash().c_str(), cachedMembership.GetIdHash().c_str()));
			membership.SetAgreement0(cachedMembership.GetAgreement0());
			membership.SetAgreement1(cachedMembership.GetAgreement1());
			membership._tokens = cachedMembership._tokens;
			// Always ensure there are enough file tokens
			if (it == _user.GetMemberships().begin()) {
				// Only do it for first membership (default)
				EnsureEnoughFileToken(membership);
			}
		}
	}

	return RESULT(0);
}

Result RmSession::QueryLoginHistoryEx(RmRouter& router, std::wstring& tenantName, std::wstring& rmsUrl, std::wstring& userId)
{
	std::wstring lastLoginInfoFile = _dirs.GetDataRoot();
	if (lastLoginInfoFile.empty())
		return RESULT(ERROR_PATH_NOT_FOUND);
	lastLoginInfoFile += L"\\login.history";

	Result res = RESULT(0);
	
	do {
		std::string sHistory;
		res = SecureFileRead(lastLoginInfoFile, sHistory);
		if (!res)
			return res;

		JsonDocument doc;
		if (!doc.LoadJsonString<char>(sHistory))
			return RESULT(ERROR_INVALID_DATA);

		PCJSONVALUE root = doc.GetRoot();
		if (NULL == root)
			return RESULT(ERROR_INVALID_DATA);
		if (!root->IsObject())
			return RESULT(ERROR_INVALID_DATA);

		std::wstring routerName;
		std::wstring routerUrl;

		PCJSONVALUE pv = root->AsObject()->at(L"routerName");
		if (IsValidJsonString(pv)) {
			routerName = pv->AsString()->GetString();
		}
		if (routerName.empty()) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}
		
		pv = root->AsObject()->at(L"routerUrl");
		if (IsValidJsonString(pv)) {
			routerUrl = pv->AsString()->GetString();
		}
		if (routerUrl.empty()) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		if (!_router.Empty() && 0 != _wcsicmp(routerUrl.c_str(), _router.GetHostUrl().c_str())) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		router = RmRouter(routerName, routerUrl);

		pv = root->AsObject()->at(L"tenantName");
		if (IsValidJsonString(pv)) {
			tenantName = pv->AsString()->GetString();
		}
		if (tenantName.empty()) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		pv = root->AsObject()->at(L"rmsUrl");
		if (IsValidJsonString(pv)) {
			rmsUrl = pv->AsString()->GetString();
		}
		if (rmsUrl.empty()) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		pv = root->AsObject()->at(L"userId");
		if (IsValidJsonString(pv)) {
			userId = pv->AsString()->GetString();
		}
		if (userId.empty()) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}


	} while (FALSE);
	if (!res) {
		::DeleteFileW(lastLoginInfoFile.c_str());
		return res;
	}

	return RESULT(0);
}

Result RmSession::QueryLoginHistory(std::wstring& tenantName, std::wstring& rmsUrl, std::wstring& userId)
{
	std::wstring lastLoginInfoFile = _dirs.GetDataRoot();
	if (lastLoginInfoFile.empty())
		return RESULT(ERROR_PATH_NOT_FOUND);
	lastLoginInfoFile += L"\\login.history";

	std::string sHistory;
	Result res = SecureFileRead(lastLoginInfoFile, sHistory);
	if (!res)
		return res;

	JsonDocument doc;
	if (!doc.LoadJsonString<char>(sHistory))
		return RESULT(ERROR_INVALID_DATA);

	PCJSONVALUE root = doc.GetRoot();
	if(NULL == root)
		return RESULT(ERROR_INVALID_DATA);
	if (!root->IsObject())
		return RESULT(ERROR_INVALID_DATA);

	PCJSONVALUE pv = root->AsObject()->at(L"routerName");
	if (IsValidJsonString(pv)) {
	}

	PCJSONVALUE itemTenantName = root->AsObject()->at(L"tenantName");
	if (itemTenantName && itemTenantName->IsString()) {
		tenantName = itemTenantName->AsString()->GetString();
	}
	if(tenantName.empty())
		return RESULT(ERROR_INVALID_DATA);

	PCJSONVALUE itemUserUrl = root->AsObject()->at(L"rmsUrl");
	if (itemUserUrl && itemUserUrl->IsString()) {
		rmsUrl = itemUserUrl->AsString()->GetString();
	}
	if(rmsUrl.empty())
		return RESULT(ERROR_INVALID_DATA);

	PCJSONVALUE itemUserId = root->AsObject()->at(L"userId");
	if (itemUserId && itemUserId->IsString()) {
		userId = itemUserId->AsString()->GetString();
	}
	if(userId.empty())
		return RESULT(ERROR_INVALID_DATA);

	return RESULT(0);
}

Result RmSession::CacheLoginHistory()
{
	std::wstring lastLoginInfoFile = _dirs.GetDataRoot();
	if (lastLoginInfoFile.empty())
		return RESULT(ERROR_PATH_NOT_FOUND);
	lastLoginInfoFile += L"\\login.history";
	const std::wstring& serverRoot = GeServerHome(true);
	if (serverRoot.empty())
		return RESULT(ERROR_PATH_NOT_FOUND);

	if (_rmserver.GetTenantName().empty() || _rmserver.GetUrl().empty() || _user.Empty())
		return RESULT(ERROR_INVALID_DATA);

	std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
	if(root == nullptr)
		return RESULT(ERROR_NOT_ENOUGH_MEMORY);

	root->set(L"routerName", JsonValue::CreateString(_router.GetName()));
	root->set(L"routerUrl", JsonValue::CreateString(_router.GetHostUrl()));
	root->set(L"tenantName", JsonValue::CreateString(_rmserver.GetTenantName()));
	root->set(L"rmsUrl", JsonValue::CreateString(_rmserver.GetUrl()));
	root->set(L"userId", JsonValue::CreateString(_user.GetId()));

	rapidjson::JsonStringWriter<char> writer;
	std::string& s = writer.Write(root.get());
	if(s.empty())
		return RESULT(ERROR_INVALID_DATA);

	return SecureFileWrite(lastLoginInfoFile, s);
}

Result RmSession::RemoveCachedLoginHistory()
{
	std::wstring lastLoginInfoFile = GeServerHome(false);
	if (lastLoginInfoFile.empty())
		return RESULT(ERROR_PATH_NOT_FOUND);
	lastLoginInfoFile += L"\\login.history";

	if (!::DeleteFileW(lastLoginInfoFile.c_str())) {
		return RESULT(GetLastError());
	}

	return RESULT(0);
}

Result RmSession::QueryCachedUserProfile(const std::wstring& tenantName, const std::wstring& userId, RmUser& user)
{
	const std::wstring& userHome = GetUserHome(tenantName, userId, false);
	if (userHome.empty())
		return RESULT(ERROR_PATH_NOT_FOUND);

	const std::wstring cachedUserProfile(userHome + L"\\profile.dat");
	std::string s;
	Result res = SecureFileRead(cachedUserProfile, s);
	if (!res)
		return res;

	user.FromJsonString<char>(s);
	if(user.Empty())
		return RESULT(ERROR_INVALID_DATA);

	return RESULT(0);
}

Result RmSession::CacheUserProfile(const std::wstring& tenantName, const RmUser& user)
{
	if (user.Empty())
		return RESULT(ERROR_INVALID_PARAMETER);

	const std::wstring& userHome = GetUserHome(tenantName, user.GetId(), true);
	assert(!userHome.empty());
	if (userHome.empty()) {
		return RESULT(ERROR_PATH_NOT_FOUND);
	}

	const std::wstring cachedUserProfile(userHome + L"\\profile.dat");
	const std::string& s = user.ToJsonString<char>();
	if(s.empty())
		return RESULT(ERROR_INVALID_DATA);

	return SecureFileWrite(cachedUserProfile, s);
}

Result RmSession::QueryCachedUserToken(const std::wstring& tenantName, const std::wstring& userId, RmUserTicket& userTicket)
{
	const std::wstring& userHome = GetUserHome(tenantName, userId, false);
	if (userHome.empty())
		return RESULT(ERROR_PATH_NOT_FOUND);

	const std::wstring cachedUserTicket(userHome + L"\\ticket.dat");
	std::string s;
	Result res = SecureFileRead(cachedUserTicket, s);
	if (!res)
		return res;

	userTicket.FromJsonString<char>(s);
	if(userTicket.Empty())
		return RESULT(ERROR_INVALID_DATA);
	if (userTicket.Expired())
		return RESULT(ERROR_EXPIRED_HANDLE);

	return RESULT(0);
}

Result RmSession::RemoveCachedUserToken(const std::wstring& tenantName, const std::wstring& userId)
{
	const std::wstring& userHome = GetUserHome(tenantName, userId, false);
	if (userHome.empty())
		return RESULT(ERROR_PATH_NOT_FOUND);

	const std::wstring cachedUserTicket(userHome + L"\\ticket.dat");
	if (!DeleteFileW(cachedUserTicket.c_str())) {
		DWORD dwError = GetLastError();
		if (ERROR_NOT_FOUND != dwError && ERROR_FILE_NOT_FOUND != dwError)
			return RESULT(dwError);
	}

	return RESULT(0);
}

Result RmSession::CacheUserToken(const std::wstring& tenantName, const RmUser& user)
{
	if (user.Empty() || user.GetTicket().Empty())
		return RESULT(ERROR_NO_TOKEN);

	const std::wstring& userHome = GetUserHome(tenantName, user.GetId(), false);
	assert(!userHome.empty());
	if (userHome.empty()) {
		return RESULT(ERROR_PATH_NOT_FOUND);
	}

	const std::wstring cachedUserProfile(userHome + L"\\ticket.dat");
	const std::string& s = user.GetTicket().ToJsonString<char>();
	if (s.empty())
		return RESULT(ERROR_INVALID_TOKEN);

	return SecureFileWrite(cachedUserProfile, s);
}

std::list<std::wstring> RmSession::FindAllCachedMemberships()
{
	std::list<std::wstring> memberList;

	const std::wstring& membersDir = GetCurrentUserMembershipsDir();
	if (membersDir.empty())
		return std::list<std::wstring>();

	WIN32_FIND_DATAW fdw = { 0 };
	const std::wstring findKey(membersDir + L"\\*.profile");
	HANDLE h = ::FindFirstFileW(findKey.c_str(), &fdw);
	if (h == INVALID_HANDLE_VALUE)
		return std::list<std::wstring>();

	do {
		if (0 != (fdw.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			std::wstring sName(fdw.cFileName);
			sName = sName.substr(0, sName.length() - 8);
			memberList.push_back(sName);
		}
	} while (::FindNextFileW(h, &fdw));
	FindClose(h);
	h = INVALID_HANDLE_VALUE;

	return memberList;
}

Result RmSession::LoadCachedMembership(const std::wstring& membershipIdHash, RmUserMembership& membership)
{
	const std::wstring& membersDir = GetCurrentUserMembershipsDir();
	if (membersDir.empty()) {
		return RESULT(ERROR_NOT_FOUND);
	}

	const std::wstring memberProfile(membersDir + L"\\" + membershipIdHash + L".profile");
	const std::wstring memberAgreement0(membersDir + L"\\" + membershipIdHash + L".agreement0");
	const std::wstring memberAgreement1(membersDir + L"\\" + membershipIdHash + L".agreement1");
	const std::wstring memberTokens(membersDir + L"\\" + membershipIdHash + L".tokens");

	Result res = LoadCachedMembershipProfile(memberProfile, membership);
	if (!res || membership.GetIdHash() != membershipIdHash) {
		// Data on disk is not correct, delete old data
		::DeleteFileW(memberProfile.c_str());
		::DeleteFileW(memberAgreement0.c_str());
		::DeleteFileW(memberAgreement1.c_str());
		return res ? RESULT(ERROR_INVALID_DATA) : res;
	}

	std::vector<UCHAR> agreement;
	res = LoadCachedMembershipAgreement(memberAgreement0, agreement);
	if (res && !agreement.empty()) {
		membership.SetAgreement0(agreement);
	}

	agreement.clear();
	res = LoadCachedMembershipAgreement(memberAgreement1, agreement);
	if (res && !agreement.empty()) {
		membership.SetAgreement1(agreement);
	}

	res = LoadCachedmembershipTokens(memberTokens, membership);
	if (!res) {
		; // Write some log, maybe
	}

	return RESULT(0);
}

Result RmSession::LoadCachedMembershipProfile(const std::wstring& file, RmUserMembership& membership)
{
	std::string		sProfile;
	Result res = SecureFileRead(file, sProfile);
	if (!res || sProfile.empty()) {
		return RESULT(ERROR_NOT_FOUND);
	}

	res = RESULT(0);

	do {

		NX::rapidjson::JsonDocument doc;
		if (!doc.LoadJsonString<char>(sProfile)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		NX::rapidjson::JsonValue* root = doc.GetRoot();
		if (NULL == root) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}
		if (!root->IsObject()) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		std::wstring sId;
		std::wstring sTenantId;
		int          nType = 0;

		NX::rapidjson::JsonValue* pMemberId = root->AsObject()->at(L"id");
		if (pMemberId && pMemberId->IsString()) {
			sId = pMemberId->AsString()->GetString();
		}
		if (sId.empty()) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		NX::rapidjson::JsonValue* pMemberTenantId = root->AsObject()->at(L"tenantId");
		if (pMemberTenantId && pMemberTenantId->IsString()) {
			sTenantId = pMemberTenantId->AsString()->GetString();
		}
		if (sTenantId.empty()) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		NX::rapidjson::JsonValue* pMemberType = root->AsObject()->at(L"type");
		if (pMemberType && pMemberType->IsNumber()) {
			nType = pMemberType->AsNumber()->ToInt();
		}

		membership = RmUserMembership(sId, nType, sTenantId);

	} while (FALSE);

	return res ? RESULT(0) : res;
}

Result RmSession::LoadCachedMembershipAgreement(const std::wstring& file, std::vector<UCHAR>& agreement)
{
	Result res = SecureFileRead(file, agreement);
	if (!res) {
		agreement.clear();
		return res;
	}

	return RESULT(0);
}

Result RmSession::LoadCachedmembershipTokens(const std::wstring& file, RmUserMembership& membership)
{
	std::string s;
	Result res = SecureFileRead(file, s);
	::DeleteFileW(file.c_str());
	if (!res)
		return res;
	if (!s.empty()) {
		res = membership.DeserializeFileTokensCache(s);
		if (!res)
			return res;
	}
	return RESULT(0);
}

Result RmSession::SaveMembershipTokens(const std::wstring& file, const RmUserMembership& membership)
{
	const std::string& s = membership.SerializeFileTokensCache();
	if (s.empty()) {
		::DeleteFileW(file.c_str());
	}
	else {
		Result res = SecureFileWrite(file, s);
		if (!res)
			return res;
	}
	return RESULT(0);
}

void RmSession::SaveAllMembershipTokens()
{
	if (_user.Empty())
		return;

	if (_user.GetMemberships().empty())
		return;

	std::for_each(_user.GetMemberships().cbegin(), _user.GetMemberships().cend(), [&](const RmUserMembership& membership) {

		const std::wstring& membersDir = GetCurrentUserMembershipsDir();
		if (membersDir.empty())
			return;
		const std::wstring file(membersDir + L"\\" + membership.GetIdHash() + L".tokens");
		(void)SaveMembershipTokens(file, membership);
	});
}

Result RmSession::SaveMembership(const RmUserMembership& membership, bool profileOnly)
{
	if (membership.Empty())
		return RESULT(ERROR_INVALID_PARAMETER);

	assert(membership.GetIdHash().length() == 32);

	const std::wstring& membersDir = GetCurrentUserMembershipsDir();
	if (membersDir.empty()) {
		return RESULT(ERROR_NOT_FOUND);
	}

	const std::wstring memberProfile(membersDir + L"\\" + membership.GetIdHash() + L".profile");
	const std::wstring memberAgreement0(membersDir + L"\\" + membership.GetIdHash() + L".agreement0");
	const std::wstring memberAgreement1(membersDir + L"\\" + membership.GetIdHash() + L".agreement1");
	const std::wstring memberTokens(membersDir + L"\\" + membership.GetIdHash() + L".tokens");

	std::string& profile = membership.ToJsonString<char>();
	if (profile.empty()) {
		return RESULT(ERROR_INVALID_DATA);
	}

	Result res = SecureFileWrite(memberProfile, profile);
	if (!res) {
		::DeleteFileW(memberProfile.c_str());
		return res;
	}

	if (!profileOnly) {

		if (!membership.GetAgreement0().empty()) {
			SecureFileWrite(memberAgreement0, membership.GetAgreement0());
		}

		if (!membership.GetAgreement1().empty()) {
			SecureFileWrite(memberAgreement1, membership.GetAgreement1());
		}

		SaveMembershipTokens(memberTokens, membership);
	}

	return RESULT(0);
}

Result RmSession::QueryMembershipAgreements(RmUserMembership& membership)
{
	if (_user.GetTicket().Empty() || _user.GetTicket().Expired()) {
		return RESULT(ERROR_EXPIRED_HANDLE);
	}

	RmRestStatus status;
	std::vector<std::wstring> certificates;
	Result res = _restClient.MembershipGetCertificates(_rmserver.GetUrl(), membership.GetId(), status, certificates);
	if (!res)
		return res;
	if (certificates.empty())
		return RESULT(ERROR_NOT_FOUND);

	std::vector<UCHAR> agreement0;
	res = NX::GetDHParameterY(certificates[0], agreement0);
	if (res && !agreement0.empty()) {
		membership.SetAgreement0(agreement0);
	}

	if (certificates.size() > 1) {
		std::vector<UCHAR> agreement1;
		res = NX::GetDHParameterY(certificates[1], agreement1);
		if (res && !agreement1.empty()) {
			membership.SetAgreement1(agreement1);
		}
	}

	return RESULT(0);
}

Result RmSession::EnsureEnoughFileToken(RmUserMembership& membership)
{
	static const int minTokenCount = 50;
	static const int requiredTokenCount = 100;

	if (membership.GetFileTokenCount() < minTokenCount) {
		// try to get more from server
		std::vector<NXL::FileToken> newTokens;
		RmRestStatus status;
		Result res = _restClient.TokenGenerate(_rmserver.GetUrl(),
			membership.GetId(),
			NX::bintohs<wchar_t>(membership.GetAgreement0().data(), membership.GetAgreement0().size()),
			requiredTokenCount,
			status,
			newTokens);
		if (!res)
			return res;
		if (!status)
			return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
		if (newTokens.empty())
			return RESULT(ERROR_NOT_FOUND);

		membership.AddFileTokens(newTokens);
	}

	return RESULT(0);
}

Result RmSession::PopEncryptToken(const std::wstring& membershipId, NXL::FileToken& token)
{
	auto pos = std::find_if(_user.GetMemberships().begin(), _user.GetMemberships().end(), [&](const RmUserMembership& membership) -> bool {
		return (0 == _wcsicmp(membershipId.c_str(), membership.GetId().c_str()));
	});

	if (pos == _user.GetMemberships().end())
		return RESULT(ERROR_NOT_FOUND);

	token = (*pos).GetFileToken();
	if (token.Empty()) {
		// No cached token
		return RESULT(ERROR_NO_MORE_ITEMS);
	}

	(void)EnsureEnoughFileToken(*pos);
	return RESULT(0);
}