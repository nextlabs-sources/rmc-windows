
#include "stdafx.h"

#include <nx\rmc\rest.h>
#include <nx\rmc\tenant.h>

#include <nx\common\rapidjson.h>
#include <nx\common\stringex.h>
#include <nx\crypt\md5.h>


using namespace NX;
using namespace NX::REST::http;


namespace {

    class RequestTenantQuery : public REST::http::StringBodyRequest
    {
    public:
        RequestTenantQuery(const RmRestCommonParams& param)
            : StringBodyRequest(REST::http::methods::GET,
                std::wstring(L"/rms/rs/tenant/v2/" + param.GetTenant()),
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
        virtual ~RequestTenantQuery() {}
    };

}

Result RmRestClient::TenantQueryPreference(const std::wstring& rms_url, RmRestStatus& status, RmTenantPreference& preference)
{
    Result res = RESULT(0);
    std::shared_ptr<Connection> p(Connect(rms_url, res));
    if (p) {
        res = TenantQueryPreference(p.get(), status, preference);
    }
    return res;
}

Result RmRestClient::TenantQueryPreference(NX::REST::http::Connection* conn, RmRestStatus& status, RmTenantPreference& preference)
{
    if (!Opened()) {
        Result res = Open();
        if (!res)
            return res;
    }

    RequestTenantQuery  request(_commonParams);
    StringResponse      response;

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
                preference._heartbeat_frequency = extra->CreateBool(L"heartbeat")->AsNumber()->ToInt64();
            }
        }
        catch (...) {
            // The JSON data is NOT correct
            res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
        }

    } while (FALSE);


    return RESULT(0);
}