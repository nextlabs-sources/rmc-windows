

#include "stdafx.h"

#include <memory>

#include <nx\common\rapidjson.h>
#include <nx\crypt\md5.h>

#include "nxrest.h"

using namespace NX;
using namespace NX::REST::http;

NxRestTest::NxRestTest()
{
    _client = new Client(L"NXRMC/1.0", true);
}

NxRestTest::~NxRestTest()
{
    if (_client) {
        delete _client;
    }
}

bool NxRestTest::QueryTenant(std::wstring& rms_host)
{
    if (!_client->Opened()) {
        Result res = _client->Open();
        if (!res.Succeeded())
            return false;
    }

    std::shared_ptr<Connection> spConn(_client->CreateConnection(L"https://r.skydrm.com"));
    if (spConn == nullptr) {
        return false;
    }
    
    RequestQueryTenant  request;
    StringResponse      response;

    Result res = spConn->SendRequest(request, response);
    if (!res.Succeeded()) {
        printf("QueryTenant failed - error:%d, file:%s, line:%d, function:%s, message:%s\n",
            res.GetCode(), res.GetFile(), res.GetLine(), res.GetFunction(), res.GetMsg().c_str());
        return false;
    }

    USHORT status = response.GetStatus();
    const std::wstring& phrase = response.GetPhrase();
    const HttpHeaders& headers = response.GetHeaders();
    const std::string& jsondata = response.GetBody();

    NX::rapidjson::JsonDocument doc;
    int err_code = 0;
    size_t err_pos = 0;
    if (!doc.LoadJsonString(jsondata, &err_code, &err_pos)) {
        printf("QueryTenant fail to parse json response - error:%d, pos:%I64d\n", err_code, err_pos);
        return false;
    }

    rapidjson::JsonValue* root = doc.GetRoot();
    assert(root->AsObject()->at(L"statusCode")->AsNumber()->ToInt() == 200);
    rapidjson::JsonValue* results = root->AsObject()->at(L"results");
    rms_host = results->AsObject()->at(L"server")->AsString()->GetString();
    return true;
}

bool NxRestTest::Login(const std::wstring& rms_host, const std::wstring& account,
    const std::wstring& password,
    __int64& user_id,
    std::wstring& user_name,
    std::wstring& user_email,
    std::wstring& user_ticket)
{
    std::shared_ptr<Connection> spConn(_client->CreateConnection(rms_host));
    if (spConn == nullptr) {
        return false;
    }

    RequestLogin    request(account, password);
    StringResponse  response;

    Result res = spConn->SendRequest(request, response);
    if (!res.Succeeded()) {
        printf("Login failed - error:%d, file:%s, line:%d, function:%s, message:%s\n",
            res.GetCode(), res.GetFile(), res.GetLine(), res.GetFunction(), res.GetMsg().c_str());
        return false;
    }

    USHORT status = response.GetStatus();
    const std::wstring& phrase = response.GetPhrase();
    const HttpHeaders& headers = response.GetHeaders();
    const std::string& jsondata = response.GetBody();

    NX::rapidjson::JsonDocument doc;
    int err_code = 0;
    size_t err_pos = 0;
    if (!doc.LoadJsonString(jsondata, &err_code, &err_pos)) {
        printf("QueryTenant fail to parse json response - error:%d, pos:%I64d\n", err_code, err_pos);
        return false;
    }

    rapidjson::JsonValue* root = doc.GetRoot();
    assert(root->AsObject()->at(L"statusCode")->AsNumber()->ToInt() == 200);
    rapidjson::JsonValue* extra = root->AsObject()->at(L"extra");
    user_id = extra->AsObject()->at(L"userId")->AsNumber()->ToInt64();
    user_name = extra->AsObject()->at(L"name")->AsString()->GetString();
    user_email = extra->AsObject()->at(L"email")->AsString()->GetString();
    user_ticket = extra->AsObject()->at(L"ticket")->AsString()->GetString();
    return true;
}



RequestQueryTenant::RequestQueryTenant()
    : StringBodyRequest(methods::GET,
                        std::wstring(L"/router/rs/q/tenant/skydrm.com"),
                        HttpHeaders(),
                        std::vector<LPCWSTR>({ mime_types::application_json.c_str(), mime_types::text.c_str(), NULL }),
                        std::string())
{
}

RequestQueryTenant::RequestQueryTenant(const std::wstring& tenant)
    : StringBodyRequest(methods::GET,
                        std::wstring(L"/router/rs/q/tenant/" + tenant),
                        HttpHeaders(),
                        std::vector<LPCWSTR>({ mime_types::application_json.c_str(), mime_types::text.c_str(), NULL }),
                        std::string())
{
}


RequestLogin::RequestLogin()
    : StringBodyRequest(methods::POST,
        std::wstring(L"/rms/rs/usr"),
        HttpHeaders({ std::pair<std::wstring, std::wstring>(header_names::content_type, mime_types::application_x_www_form_urlencoded) }),
        std::vector<LPCWSTR>({ mime_types::application_json.c_str(), mime_types::text.c_str(), NULL }),
        std::string())
{
}

RequestLogin::RequestLogin(const std::wstring& account, const std::wstring& password)
    : StringBodyRequest(methods::POST,
        std::wstring(L"/rms/rs/usr"),
        HttpHeaders({std::pair<std::wstring, std::wstring>(header_names::content_type, mime_types::application_x_www_form_urlencoded)}),
        std::vector<LPCWSTR>({ mime_types::application_json.c_str(), mime_types::text.c_str(), NULL }),
        std::string(BuildBody(account, password)))
{
}

std::string RequestLogin::BuildBody(const std::wstring& account, const std::wstring& password)
{
    UCHAR checksum[16] = { 0 };
    std::string ansi_password = NX::conv::utf16toutf8(password);
    NX::Result res = NX::crypt::CreateMd5((const UCHAR*)ansi_password.c_str(), (ULONG)ansi_password.length(), checksum);
    if (!res.Succeeded())
        return std::string();

    std::string s = "email=";
    s += NX::conv::utf16toutf8(account);
    s += "&password=";
    s += NX::bintohs<char>(checksum, 16);

    std::string encoded_url = NX::url_encode(s);
    return std::move(encoded_url);
}