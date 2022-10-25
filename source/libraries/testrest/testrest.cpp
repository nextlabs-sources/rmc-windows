
#include "stdafx.h"

#include <nx\rest\http_client.h>

#include "nxrest.h"

using namespace NX;
using namespace NX::REST::http;


NxRestTest test;

int main(int argc, char** argv)
{
    std::cout << "Hello, World!" << std::endl;

    std::wstring rms_host;

    test.QueryTenant(rms_host);

    __int64 user_id;
    std::wstring user_name;
    std::wstring user_email;
    std::wstring user_ticket;
    test.Login(rms_host, L"gavin.ye@nextlabs.com", L"123blue!", user_id, user_name, user_email, user_ticket);

    return 0;
}

