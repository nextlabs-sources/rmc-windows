

#ifndef _NX_REST_H__
#define _NX_REST_H__


#include <nx\rest\http_client.h>

class RequestQueryTenant : public NX::REST::http::StringBodyRequest
{
public:
    RequestQueryTenant();
    RequestQueryTenant(const std::wstring& tenant);
    virtual ~RequestQueryTenant() {}
};

class RequestLogin : public NX::REST::http::StringBodyRequest
{
public:
    RequestLogin();
    RequestLogin(const std::wstring& account, const std::wstring& password);
    virtual ~RequestLogin() {}

protected:
    std::string BuildBody(const std::wstring& account, const std::wstring& password);
};

class NxRestTest
{
public:
    NxRestTest();
    ~NxRestTest();

    bool QueryTenant(std::wstring& rms_host);
    bool Login(const std::wstring& rms_host, const std::wstring& account,
        const std::wstring& password,
        __int64& user_id,
        std::wstring& user_name,
        std::wstring& user_email,
        std::wstring& user_ticket);

private:
    NX::REST::http::Client* _client;
};


#endif
