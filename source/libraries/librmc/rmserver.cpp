

#include "stdafx.h"

#include <nx\rmc\rmserver.h>

#include <nx\common\string.h>
#include <nx\rest\uri.h>

using namespace NX;


RmServer::RmServer() : _port(0)
{
}


RmServer::RmServer(const std::wstring& tenant, const std::wstring& url)
    : _tenant(tenant), _port(0)
{
    NX::REST::Uri uri(url);
    _scheme = uri.scheme();
    _host = uri.host();
    _port = uri.resolve_default_port();
    _url = uri.scheme();
    _url += L"://";
    _url += uri.host();
    if (0 != uri.port()) {
        _url += L":";
        _url += NX::itos<WCHAR>((int)uri.port());
    }
}

RmServer::RmServer(const RmServer& rhs)
    : _tenant(rhs._tenant), _host(rhs._host), _port(rhs._port), _url(rhs._url)
{
}

RmServer::~RmServer()
{
}

RmServer& RmServer::operator = (const RmServer& rhs)
{
    if (this != &rhs) {
        _tenant = rhs._tenant;
        _host = rhs._host;
        _port = rhs._port;
        _url = rhs._url;
    }
    return *this;
}
