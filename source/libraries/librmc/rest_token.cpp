
#include "stdafx.h"

#include <nx\rmc\rest.h>

#include <nx\common\rapidjson.h>
#include <nx\common\stringex.h>
#include <nx\crypt\md5.h>


using namespace NX;
using namespace NX::rapidjson;
using namespace NX::REST::http;


namespace {

    class RequestGenerateToken : public REST::http::StringBodyRequest
    {
    public:
        RequestGenerateToken(const RmRestCommonParams& param, const std::wstring& membership, const std::wstring& agreement, int count)
            : StringBodyRequest(REST::http::methods::PUT,
                std::wstring(L"/rms/rs/token"),
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
                std::string(BuildBody(param.GetUserId(), param.GetTicket(), membership, agreement, count)))
        {
        }
        virtual ~RequestGenerateToken() {}

    protected:
        std::string BuildBody(const std::wstring& userId, const std::wstring& ticket, const std::wstring& membership, const std::wstring& agreement, int count)
        {
            std::wstring s = L"{\"parameters\": {\"userId\":" + userId + L",";
            s += L"\"ticket\":\"" + ticket + L"\",";
            s += L"\"membership\":\"" + membership + L"\",";
            s += L"\"agreement\":\"" + agreement + L"\",";
            s += L"\"count\":" + NX::itos<wchar_t>(count) + L"}}";
            return NX::conv::utf16toutf8(s);
        }
    };

    class RequestRetrieveToken : public REST::http::StringBodyRequest
    {
    public:
        RequestRetrieveToken(const RmRestCommonParams& param, const std::wstring& membership, const std::wstring& agreement, const std::wstring& duid, __int64 ml)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"/rms/rs/token"),
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
                std::string(BuildBody(param.GetUserId(), param.GetTicket(), param.GetTenant(), membership, agreement, duid, ml)))
        {
        }
        virtual ~RequestRetrieveToken() {}

    protected:
        std::string BuildBody(const std::wstring& userId,
            const std::wstring& ticket,
            const std::wstring& tenant,
            const std::wstring& membership,
            const std::wstring& agreement,
            const std::wstring& duid,
            __int64 ml)
        {
            std::wstring s = L"{\"parameters\": {\"userId\":" + userId + L",";
            s += L"\"ticket\":\"" + ticket + L"\",";
            s += L"\"tenant\":\"" + tenant + L"\",";
            s += L"\"owner\":\"" + membership + L"\",";
            s += L"\"agreement\":\"" + agreement + L"\",";
            s += L"\"duid\":\"" + duid + L"\",";
            s += L"\"ml\":" + NX::i64tos<wchar_t>(ml) + L"}}";
            return NX::conv::utf16toutf8(s);
        }
    };

}

Result RmRestClient::TokenGenerate(const std::wstring& rms_url,
    const std::wstring& membership,
    const std::wstring& agreement,
    int count,
    RmRestStatus& status,
    std::vector<NXL::FileToken>& tokens)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = TokenGenerate(p.get(), membership, agreement, count, status, tokens);
    }
    return res;
}

Result RmRestClient::TokenGenerate(NX::REST::http::Connection* conn,
    const std::wstring& membership,
    const std::wstring& agreement,
    int count,
    RmRestStatus& status,
    std::vector<NXL::FileToken>& tokens)
{
	RequestGenerateToken request(_commonParams, membership, agreement, count);
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
			if (status.GetStatus() == status_codes::OK.id) {
				const rapidjson::JsonValue* results = root->AsObject()->at(L"results");
				if (NULL != results && results->IsObject()) {
					unsigned int ml = 0;
					const rapidjson::JsonValue* mlevel = results->AsObject()->at(L"ml");
					if (IsValidJsonNumber(mlevel))
						ml = mlevel->AsNumber()->ToUint();
					else if (IsValidJsonString(mlevel))
						ml = (UINT)std::stoul(mlevel->AsString()->GetString());
					else
						ml = 0;
					const rapidjson::JsonValue* pTokens = results->AsObject()->at(L"tokens");
					if (pTokens && pTokens->IsObject()) {
						std::for_each(pTokens->AsObject()->cbegin(), pTokens->AsObject()->cend(), [&](const JsonObject::value_type& item) {
							if (item.second == NULL) return;
							if (!item.second->IsString()) return;
							const std::wstring& tokenValue = item.second->AsString()->GetString();
							NXL::FileToken ftoken(item.first, ml, tokenValue);
							if (!ftoken.Empty())
								tokens.push_back(ftoken);
						});
					}
				}
			}
		}
		catch (...) {
			// The JSON data is NOT correct
			res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
		}

	} while (FALSE);


	return RESULT(0);
}

Result RmRestClient::TokenRetrieve(const std::wstring& rms_url,
    const std::wstring& membership,
    const std::wstring& agreement,
    const std::wstring& duid,
	unsigned int ml,
    RmRestStatus& status,
    NXL::FileToken& token)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = TokenRetrieve(p.get(), membership, agreement, duid, ml, status, token);
    }
    return res;
}

Result RmRestClient::TokenRetrieve(NX::REST::http::Connection* conn,
    const std::wstring& membership,
    const std::wstring& agreement,
    const std::wstring& duid,
    unsigned int ml,
    RmRestStatus& status,
    NXL::FileToken& token)
{
	RequestRetrieveToken request(_commonParams, membership, agreement, duid, ml);
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
			if (status.GetStatus() == status_codes::OK.id) {
				const rapidjson::JsonValue* results = root->AsObject()->at(L"results");
				if (NULL != results && results->IsObject()) {
					const rapidjson::JsonValue* pToken = results->AsObject()->at(L"token");
					if (pToken && pToken->IsString()) {
						token = NXL::FileToken(duid, ml, pToken->AsString()->GetString());
					}
				}
			}
		}
		catch (...) {
			// The JSON data is NOT correct
			res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
		}

	} while (FALSE);


	return RESULT(0);
}

