
#include "stdafx.h"

#include <nx\rmc\rest.h>
#include <nx\rmc\user.h>

#include <nx\common\rapidjson.h>
#include <nx\common\stringex.h>
#include <nx\crypt\md5.h>



using namespace NX;
using namespace NX::rapidjson;
using namespace NX::REST::http;


namespace {

	class RequestGetCaptcha : public REST::http::StringBodyRequest
	{
	public:
		RequestGetCaptcha()
			: StringBodyRequest(REST::http::methods::GET,
				std::wstring(L"/rms/rs/usr/captcha"),
				REST::http::HttpHeaders(),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }),
                std::string())
        {
        }
        virtual ~RequestGetCaptcha() {}
    };

    class RequestResetPassword : public REST::http::StringBodyRequest
    {
    public:
        RequestResetPassword(const std::wstring& email, const std::wstring& nonce, const std::wstring& captcha)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"/rms/rs/usr/forgotPassword"),
                REST::http::HttpHeaders({ std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_x_www_form_urlencoded) }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }),
                std::string(BuildBody(email, nonce, captcha)))
        {
        }
        virtual ~RequestResetPassword() {}

    protected:
        std::string BuildBody(const std::wstring& email, const std::wstring& nonce, const std::wstring& captcha)
        {
            std::string s = "email=";
            s += NX::conv::utf16toutf8(email);
            s += "&nonce=";
            s += NX::conv::utf16toutf8(nonce);
            s += "&captcha=";
            s += NX::conv::utf16toutf8(captcha);

            std::string encoded_url = NX::conv::UrlEncode(s);
            return encoded_url;
        }
    };

	class RequestChangePassword : public REST::http::StringBodyRequest
	{
	public:
		RequestChangePassword(const RmRestCommonParams& param, const std::wstring& oldPassword, const std::wstring& newPassword)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/usr/changePassword"),
				REST::http::HttpHeaders({
					std::pair<std::wstring, std::wstring>(L"clientId", param.GetClientId()),
					std::pair<std::wstring, std::wstring>(L"platformId", param.GetPlatformId()),
					std::pair<std::wstring, std::wstring>(L"deviceId", param.GetDeviceId()),
					std::pair<std::wstring, std::wstring>(L"tenant", param.GetTenant()),
					std::pair<std::wstring, std::wstring>(L"userId", param.GetUserId()),
					std::pair<std::wstring, std::wstring>(L"ticket", param.GetTicket()),
					std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_json)
				}),
				std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }),
				std::string(BuildBody(oldPassword, newPassword)))
		{
		}
		virtual ~RequestChangePassword() {}

	protected:
		std::string BuildBody(const std::wstring& oldPassword, const std::wstring& newPassword)
		{
			std::string s;

			UCHAR checksumOld[16] = { 0 };
			UCHAR checksumNew[16] = { 0 };
			const std::string& ansiOldPassword = NX::conv::utf16toutf8(oldPassword);
			NX::Result res = NX::crypt::CreateMd5((const UCHAR*)ansiOldPassword.c_str(), (ULONG)ansiOldPassword.length(), checksumOld);
			if (!res)
				return std::string();
			const std::string& ansiNewPassword = NX::conv::utf16toutf8(newPassword);
			res = NX::crypt::CreateMd5((const UCHAR*)ansiNewPassword.c_str(), (ULONG)ansiNewPassword.length(), checksumNew);
			if (!res)
				return std::string();
			
			std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
			if (NULL == root)
				return s;

			JsonObject* params = JsonValue::CreateObject();
			if (NULL == params)
				return s;
			root->set(L"parameters", params);

			JsonValue* pv = JsonValue::CreateString(NX::bintohs<wchar_t>(checksumOld, 16));
			if (NULL == params)
				return s;
			params->set(L"oldPassword", pv);

			pv = JsonValue::CreateString(NX::bintohs<wchar_t>(checksumNew, 16));
			if (NULL == params)
				return s;
			params->set(L"newPassword", pv);

			JsonStringWriter<char> writer;
			s = writer.Write(root.get());
			root.reset();

			return s;
		}
	};

    class RequestGetBasicPorfile : public REST::http::StringBodyRequest
    {
    public:
        RequestGetBasicPorfile(const RmRestCommonParams& param)
            : StringBodyRequest(REST::http::methods::GET,
                std::wstring(L"/rms/rs/usr/v2/basic"),
                REST::http::HttpHeaders({
					std::pair<std::wstring, std::wstring>(L"clientId", param.GetClientId()),
					std::pair<std::wstring, std::wstring>(L"platformId", param.GetPlatformId()),
					std::pair<std::wstring, std::wstring>(L"deviceId", param.GetDeviceId()),
					std::pair<std::wstring, std::wstring>(L"tenant", param.GetTenant()),
					std::pair<std::wstring, std::wstring>(L"userId", param.GetUserId()),
					std::pair<std::wstring, std::wstring>(L"ticket", param.GetTicket()),
					std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_json)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }),
                std::string())
        {
        }
        virtual ~RequestGetBasicPorfile() {}
    };

    class RequestGetPorfile : public REST::http::StringBodyRequest
    {
    public:
        RequestGetPorfile(const RmRestCommonParams& param)
            : StringBodyRequest(REST::http::methods::GET,
                std::wstring(L"/rms/rs/usr/v2/profile"),
                REST::http::HttpHeaders({
					std::pair<std::wstring, std::wstring>(L"clientId", param.GetClientId()),
					std::pair<std::wstring, std::wstring>(L"platformId", param.GetPlatformId()),
					std::pair<std::wstring, std::wstring>(L"deviceId", param.GetDeviceId()),
					std::pair<std::wstring, std::wstring>(L"tenant", param.GetTenant()),
					std::pair<std::wstring, std::wstring>(L"userId", param.GetUserId()),
					std::pair<std::wstring, std::wstring>(L"ticket", param.GetTicket()),
					std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_json)
                }),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }),
                std::string())
        {
        }
        virtual ~RequestGetPorfile() {}
    };

	class RequestUpdateProfile : public REST::http::StringBodyRequest
	{
	public:
		RequestUpdateProfile(const RmRestCommonParams& param,
			const std::wstring& displayName,
			const std::wstring& securityMode,
			const std::wstring& profilePicture
			)
			: StringBodyRequest(REST::http::methods::POST,
				std::wstring(L"/rms/rs/usr/profile"),
				REST::http::HttpHeaders({
					std::pair<std::wstring, std::wstring>(L"clientId", param.GetClientId()),
					std::pair<std::wstring, std::wstring>(L"platformId", param.GetPlatformId()),
					std::pair<std::wstring, std::wstring>(L"deviceId", param.GetDeviceId()),
					std::pair<std::wstring, std::wstring>(L"tenant", param.GetTenant()),
					std::pair<std::wstring, std::wstring>(L"userId", param.GetUserId()),
					std::pair<std::wstring, std::wstring>(L"ticket", param.GetTicket()),
					std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_json)
				}),
				std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }),
				std::string(BuildBody(param.GetUserId(), param.GetTicket(), displayName, securityMode, profilePicture)))
		{
		}
		virtual ~RequestUpdateProfile() {}

	protected:
		std::string BuildBody(const std::wstring& userId, const std::wstring& ticket, const std::wstring& displayName, const std::wstring& securityMode, const std::wstring& profilePicture)
		{
			std::string s;

			std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
			if (NULL == root)
				return s;

			JsonObject* params = JsonValue::CreateObject();
			if (NULL == params)
				return s;
			root->set(L"parameters", params);

			JsonValue* pv = NULL;
			
			const __int64 nUid = std::stoll(userId.c_str());
			pv = JsonValue::CreateNumber(nUid);
			if (NULL == pv)
				return s;
			params->set(L"userId", pv);
			
			pv = JsonValue::CreateString(ticket);
			if (NULL == pv)
				return s;
			params->set(L"ticket", pv);
			
			pv = JsonValue::CreateString(displayName);
			if (NULL == pv)
				return s;
			params->set(L"displayName", pv);

			if (!securityMode.empty() || !profilePicture.empty()) {

				JsonObject* prefs = JsonValue::CreateObject();
				if (NULL == prefs)
					return s;
				params->set(L"preferences", prefs);

				if (!securityMode.empty()) {
					int n = std::stoi(securityMode);
					pv = JsonValue::CreateNumber(n);
					if (NULL == pv)
						return s;
					prefs->set(L"securityMode", pv);
				}

				if (!profilePicture.empty()) {
					std::ifstream ifs;
					ifs.open(profilePicture, std::ifstream::in | std::ifstream::binary);
					if (ifs.good() && ifs.is_open()) {
						ifs.seekg(0, std::ios::end);
						size_t fsize = (size_t)ifs.tellg();
						if (fsize > 0) {
							std::vector<UCHAR> buf;
							buf.resize(fsize, 0);
							ifs.read((char*)buf.data(), fsize);
							if (ifs.good()) {
								const std::string& base64Pic = NX::conv::Base64Encode(buf);
								pv = JsonValue::CreateString(std::wstring(L"data:image/png;base64," + std::wstring(base64Pic.begin(), base64Pic.end())));
								if (NULL != pv)
									prefs->set(L"profile_picture", pv);
							}
						}
					}
				}
			}

			JsonStringWriter<char> writer;
			s = writer.Write(root.get());
			root.reset();

			return s;
		}
	};

}


Result RmRestClient::UserGetCaptcha(const std::wstring& rms_url,
	RmRestStatus& status,
	std::vector<UCHAR>& captcha,
	std::wstring& nonce)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = UserGetCaptcha(p.get(), status, captcha, nonce);
    }
    return res;
}

Result RmRestClient::UserGetCaptcha(NX::REST::http::Connection* conn,
	RmRestStatus& status,
	std::vector<UCHAR>& captcha,
	std::wstring& nonce)
{
	RequestGetCaptcha request;
	StringResponse  response;

	Result res = conn->SendRequest(request, response);
	if (!res) {
		return res;
	}

	status._status = response.GetStatus();
	status._phrase = response.GetPhrase();
	const HttpHeaders& headers = response.GetHeaders();
	const std::string& jsondata = response.GetBody();

	res = RESULT(0);

	do {

		if (status.GetStatus() != status_codes::OK.id) {
			break;
		}

		if (jsondata.empty()) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		NX::rapidjson::JsonDocument doc;
		int err_code = 0;
		size_t err_pos = 0;
		if (!doc.LoadJsonString(jsondata, &err_code, &err_pos)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		const rapidjson::JsonValue* root = doc.GetRoot();
		if (!(NULL != root && root->IsObject())) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		try {
			status._status = (USHORT)root->AsObject()->at(L"statusCode")->AsNumber()->ToInt();
			status._phrase = root->AsObject()->at(L"message")->AsString()->GetString();
			if (status) {
				const rapidjson::JsonValue* results = root->AsObject()->at(L"results");
				if (results && results->IsObject()) {
					PCJSONVALUE pv = results->AsObject()->at(L"captcha");
					if (IsValidJsonString(pv)) {
						const std::wstring& base64Captcha = pv->AsString()->GetString();
						captcha = NX::conv::Base64Decode(base64Captcha);
					}
					pv = results->AsObject()->at(L"nonce");
					if (IsValidJsonString(pv)) {
						nonce = pv->AsString()->GetString();
					}
				}
			}

			if (captcha.empty() || nonce.empty()) {
				res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
			}
		}
		catch (...) {
			// The JSON data is NOT correct
			res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
		}

	} while (FALSE);
	if (!res)
		return res;

	return RESULT(0);
}

Result RmRestClient::UserQueryBasicProfile(const std::wstring& rms_url,
    RmRestStatus& status,
    RmUser& profile)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = UserQueryBasicProfile(p.get(), status, profile);
    }
    return res;
}

Result RmRestClient::UserQueryBasicProfile(NX::REST::http::Connection* conn,
    RmRestStatus& status,
    RmUser& profile)
{
	RequestGetBasicPorfile request(_commonParams);
	StringResponse  response;

	Result res = conn->SendRequest(request, response);
	if (!res) {
		return res;
	}

	status._status = response.GetStatus();
	status._phrase = response.GetPhrase();
	const HttpHeaders& headers = response.GetHeaders();
	const std::string& jsondata = response.GetBody();

	res = RESULT(0);

	do {

		if (status.GetStatus() != status_codes::OK.id) {
			break;
		}

		if (jsondata.empty()) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		NX::rapidjson::JsonDocument doc;
		int err_code = 0;
		size_t err_pos = 0;
		if (!doc.LoadJsonString(jsondata, &err_code, &err_pos)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		const rapidjson::JsonValue* root = doc.GetRoot();
		if (!(NULL != root && root->IsObject())) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		try {
			status._status = (USHORT)root->AsObject()->at(L"statusCode")->AsNumber()->ToInt();
			status._phrase = root->AsObject()->at(L"message")->AsString()->GetString();
			if (status) {
				const rapidjson::JsonValue* results = root->AsObject()->at(L"results");
				if (results && results->IsObject()) {
					profile.FromJsonValue(results);
				}
			}
		}
		catch (...) {
			// The JSON data is NOT correct
			res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
		}

	} while (FALSE);
	if (!res)
		return res;


	return RESULT(0);
}

Result RmRestClient::UserQueryProfile(const std::wstring& rms_url,
    RmRestStatus& status,
    RmUser& profile)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = UserQueryProfile(p.get(), status, profile);
    }
    return res;
}

Result RmRestClient::UserQueryProfile(NX::REST::http::Connection* conn,
    RmRestStatus& status,
    RmUser& profile)
{
	RequestGetPorfile request(_commonParams);
	StringResponse  response;

	Result res = conn->SendRequest(request, response);
	if (!res) {
		return res;
	}

	status._status = response.GetStatus();
	status._phrase = response.GetPhrase();
	const HttpHeaders& headers = response.GetHeaders();
	const std::string& jsondata = response.GetBody();

	res = RESULT(0);

	do {

		if (status.GetStatus() != status_codes::OK.id) {
			break;
		}

		if (jsondata.empty()) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		NX::rapidjson::JsonDocument doc;
		int err_code = 0;
		size_t err_pos = 0;
		if (!doc.LoadJsonString(jsondata, &err_code, &err_pos)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		const rapidjson::JsonValue* root = doc.GetRoot();
		if (!(NULL != root && root->IsObject())) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		try {
			status._status = (USHORT)root->AsObject()->at(L"statusCode")->AsNumber()->ToInt();
			status._phrase = root->AsObject()->at(L"message")->AsString()->GetString();
			if (status) {
				const rapidjson::JsonValue* results = root->AsObject()->at(L"extra");
				if (results && results->IsObject()) {
					profile.FromJsonValue(results);
				}
			}
		}
		catch (...) {
			// The JSON data is NOT correct
			res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
		}

	} while (FALSE);
	if (!res)
		return res;


	return RESULT(0);
}

Result RmRestClient::UserUpdateProfile(const std::wstring& rms_url,
	const std::wstring& displayName,
	const std::wstring& securityMode,
	const std::wstring& profilePicture,
	RmRestStatus& status)
{
	Result res = RESULT(0);
	std::shared_ptr<Connection> p(Connect(rms_url, res));
	if (p) {
		res = UserUpdateProfile(p.get(), displayName, securityMode, profilePicture, status);
	}
	return res;
}

Result RmRestClient::UserUpdateProfile(NX::REST::http::Connection* conn,
	const std::wstring& displayName,
	const std::wstring& securityMode,
	const std::wstring& profilePicture,
	RmRestStatus& status)
{
	RequestUpdateProfile request(_commonParams, displayName, securityMode, profilePicture);
	StringResponse  response;

	if (displayName.empty() && securityMode.empty() && profilePicture.empty())
		return RESULT(ERROR_INVALID_PARAMETER);

	Result res = conn->SendRequest(request, response);
	if (!res) {
		return res;
	}

	status._status = response.GetStatus();
	status._phrase = response.GetPhrase();
	const HttpHeaders& headers = response.GetHeaders();
	const std::string& jsondata = response.GetBody();

	res = RESULT(0);

	do {

		if (status.GetStatus() != status_codes::OK.id) {
			break;
		}

		if (jsondata.empty()) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		NX::rapidjson::JsonDocument doc;
		int err_code = 0;
		size_t err_pos = 0;
		if (!doc.LoadJsonString(jsondata, &err_code, &err_pos)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		const rapidjson::JsonValue* root = doc.GetRoot();
		if (!(NULL != root && root->IsObject())) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		try {
			status._status = (USHORT)root->AsObject()->at(L"statusCode")->AsNumber()->ToInt();
			status._phrase = root->AsObject()->at(L"message")->AsString()->GetString();
		}
		catch (...) {
			// The JSON data is NOT correct
			res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
		}

	} while (FALSE);
	if (!res)
		return res;


	return RESULT(0);
}

Result RmRestClient::UserResetPassword(const std::wstring& rms_url,
    const std::wstring& email,
    const std::wstring& nonce,
    const std::wstring& captcha,
    RmRestStatus& status)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = UserResetPassword(p.get(), email, nonce, captcha, status);
    }
    return res;
}

Result RmRestClient::UserResetPassword(NX::REST::http::Connection* conn,
    const std::wstring& email,
    const std::wstring& nonce,
    const std::wstring& captcha,
    RmRestStatus& status)
{
	RequestResetPassword request(email, nonce, captcha);
	StringResponse  response;

	Result res = conn->SendRequest(request, response);
	if (!res) {
		return res;
	}

	status._status = response.GetStatus();
	status._phrase = response.GetPhrase();
	const HttpHeaders& headers = response.GetHeaders();
	const std::string& jsondata = response.GetBody();

	res = RESULT(0);

	do {

		if (status.GetStatus() != status_codes::OK.id) {
			break;
		}

		if (jsondata.empty()) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		NX::rapidjson::JsonDocument doc;
		int err_code = 0;
		size_t err_pos = 0;
		if (!doc.LoadJsonString(jsondata, &err_code, &err_pos)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		const rapidjson::JsonValue* root = doc.GetRoot();
		if (!(NULL != root && root->IsObject())) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		try {
			status._status = (USHORT)root->AsObject()->at(L"statusCode")->AsNumber()->ToInt();
			status._phrase = root->AsObject()->at(L"message")->AsString()->GetString();
		}
		catch (...) {
			// The JSON data is NOT correct
			res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
		}

	} while (FALSE);
	if (!res)
		return res;


	return RESULT(0);
}

Result RmRestClient::UserChangePassword(const std::wstring& rms_url,
	const std::wstring& oldPassword,
	const std::wstring& newPassword,
	RmRestStatus& status)
{
	Result res = RESULT(0);
	std::shared_ptr<Connection> p(Connect(rms_url, res));
	if (p) {
		res = UserChangePassword(p.get(), oldPassword, newPassword, status);
	}
	return res;
}

Result RmRestClient::UserChangePassword(NX::REST::http::Connection* conn,
	const std::wstring& oldPassword,
	const std::wstring& newPassword,
	RmRestStatus& status)
{
	RequestChangePassword request(_commonParams, oldPassword, newPassword);
	StringResponse  response;

	Result res = conn->SendRequest(request, response);
	if (!res) {
		return res;
	}

	status._status = response.GetStatus();
	status._phrase = response.GetPhrase();
	const HttpHeaders& headers = response.GetHeaders();
	const std::string& jsondata = response.GetBody();

	res = RESULT(0);

	do {

		if (status.GetStatus() != status_codes::OK.id) {
			break;
		}

		if (jsondata.empty()) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		NX::rapidjson::JsonDocument doc;
		int err_code = 0;
		size_t err_pos = 0;
		if (!doc.LoadJsonString(jsondata, &err_code, &err_pos)) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		const rapidjson::JsonValue* root = doc.GetRoot();
		if (!(NULL != root && root->IsObject())) {
			res = RESULT(ERROR_INVALID_DATA);
			break;
		}

		try {
			status._status = (USHORT)root->AsObject()->at(L"statusCode")->AsNumber()->ToInt();
			status._phrase = root->AsObject()->at(L"message")->AsString()->GetString();
		}
		catch (...) {
			// The JSON data is NOT correct
			res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
		}

	} while (FALSE);
	if (!res)
		return res;

	return RESULT(0);
}