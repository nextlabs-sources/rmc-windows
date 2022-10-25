
#include "stdafx.h"

#include <nx\rmc\rest.h>

#include <nx\common\rapidjson.h>
#include <nx\common\stringex.h>


using namespace NX;
using namespace NX::REST::http;

namespace {

    class RequestQueryTenant : public NX::REST::http::StringBodyRequest
    {
    public:
        RequestQueryTenant()
            : StringBodyRequest(NX::REST::http::methods::GET,
                std::wstring(L"/router/rs/q/tenant/skydrm.com"),
                NX::REST::http::HttpHeaders(),
                std::vector<LPCWSTR>({ NX::REST::http::mime_types::application_json.c_str(), NX::REST::http::mime_types::text.c_str(), NULL }),
                std::string())
        {
        }
        RequestQueryTenant(const std::wstring& tenant)
            : StringBodyRequest(REST::http::methods::GET,
                std::wstring(L"/router/rs/q/tenant/" + tenant),
                REST::http::HttpHeaders(),
                std::vector<LPCWSTR>({ REST::http::mime_types::application_json.c_str(), REST::http::mime_types::text.c_str(), NULL }),
                std::string())
        {
        }
        virtual ~RequestQueryTenant() {}
    };
}


Result RmRestClient::RouterQuery(const std::wstring& router_url, const std::wstring& tenant_name, RmRestStatus& status, std::wstring& rms_url)
{
    if (!Opened()) {
        Result res = Open();
        if (!res)
            return res;
    }

    std::shared_ptr<Connection> spConn(CreateConnection(router_url));
    if (spConn == nullptr) {
        return RESULT(GetLastError());
    }

    RequestQueryTenant  request(tenant_name);
    StringResponse      response;

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
                rms_url = root->AsObject()->at(L"results")->AsObject()->at(L"server")->AsString()->GetString();
            }
        }
        catch (...) {
            // The JSON data is NOT correct
            res = RESULT2(ERROR_INVALID_DATA, "JSON response is not correct");
        }

    } while (FALSE);


    return RESULT(0);
}