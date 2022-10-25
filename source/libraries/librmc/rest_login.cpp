
#include "stdafx.h"

#include <nx\rmc\rest.h>
#include <nx\rmc\user.h>

#include <nx\common\rapidjson.h>
#include <nx\common\stringex.h>
#include <nx\crypt\md5.h>


using namespace NX;
using namespace NX::REST::http;


namespace {

    class RequestLogin : public REST::http::StringBodyRequest
    {
    public:
        RequestLogin(const RmRestCommonParams& param, const std::wstring& account, const std::wstring& password)
            : StringBodyRequest(REST::http::methods::POST,
                std::wstring(L"/rms/rs/usr"),
                REST::http::HttpHeaders({
					std::pair<std::wstring, std::wstring>(L"clientId", param.GetClientId()),
					std::pair<std::wstring, std::wstring>(L"platformId", param.GetPlatformId()),
					std::pair<std::wstring, std::wstring>(L"deviceId", param.GetDeviceId()),
					std::pair<std::wstring, std::wstring>(REST::http::header_names::content_type, REST::http::mime_types::application_x_www_form_urlencoded)
				}),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }),
                std::string(BuildBody(account, password)))
        {
        }
        virtual ~RequestLogin() {}

    protected:
        std::string BuildBody(const std::wstring& account, const std::wstring& password)
        {
            UCHAR checksum[16] = { 0 };
            std::string ansi_password = NX::conv::utf16toutf8(password);
            NX::Result res = NX::crypt::CreateMd5((const UCHAR*)ansi_password.c_str(), (ULONG)ansi_password.length(), checksum);
            if (!res)
                return std::string();

            std::string s = "email=";
            s += NX::conv::utf16toutf8(account);
            s += "&password=";
            s += NX::bintohs<char>(checksum, 16);

            std::string encoded_url = NX::conv::UrlEncode(s);
            return encoded_url;
        }
    };

}

Result RmRestClient::LoginWithNativeAccount(const std::wstring& rms_url, const std::wstring& name, const std::wstring& password, RmRestStatus& status, RmUser& user)
{
    if (!Opened()) {
        Result res = Open();
        if (!res)
            return res;
    }

    std::shared_ptr<Connection> spConn(CreateConnection(rms_url));
    if (spConn == nullptr) {
        return RESULT(GetLastError());
    }

    RequestLogin    request(_commonParams, name, password);
    StringResponse  response;

    Result res = spConn->SendRequest(request, response);
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

        rapidjson::JsonValue* root = doc.GetRoot();
        if (!(NULL != root && root->IsObject())) {
            res = RESULT(ERROR_INVALID_DATA);
            break;
        }

        try {
            status._status = (USHORT)root->AsObject()->at(L"statusCode")->AsNumber()->ToInt();
            status._phrase = root->AsObject()->at(L"message")->AsString()->GetString();
            if (status.GetStatus() == status_codes::OK.id) {

                rapidjson::JsonValue* extra = root->AsObject()->at(L"extra");
				if (extra && extra->IsObject()) {
					user.FromJsonValue(extra);
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

Result RmRestClient::LoginWithGoogleAccount(const std::wstring& token, RmRestStatus& status)
{
    return RESULT(ERROR_NOT_SUPPORTED);
}

Result RmRestClient::LoginWithFacebookAccount(const std::wstring& token, RmRestStatus& status)
{
    return RESULT(ERROR_NOT_SUPPORTED);
}
