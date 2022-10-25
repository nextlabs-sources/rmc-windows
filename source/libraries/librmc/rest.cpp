
#include "stdafx.h"

#include <nx\rmc\rest.h>

#include <nx\common\rapidjson.h>
#include <nx\winutil\host.h>

using namespace NX;
using namespace NX::REST::http;


#define NXRMC_CLIENT_NAME   L"NXRMC/1.0"


RmRestStatus::RmRestStatus() : _status(0)
{
}

RmRestStatus::RmRestStatus(const RmRestStatus& rhs) : _status(rhs._status), _phrase(rhs._phrase)
{
}

RmRestStatus::~RmRestStatus()
{
}

RmRestStatus& RmRestStatus::operator = (const RmRestStatus& rhs)
{
    if (this != &rhs) {
        _status = rhs._status;
        _phrase = rhs._phrase;
    }
    return *this;
}


RmRestCommonParams::RmRestCommonParams()
{
}

static std::wstring PrepareDeviceId()
{
	const std::wstring& id = NX::win::GetLocalHostInfo().GetFQDN().empty() ? NX::win::GetLocalHostInfo().GetHostName() : NX::win::GetLocalHostInfo().GetFQDN();
	std::wstring encodedId;
	std::for_each(id.begin(), id.end(), [&](const wchar_t c) {
		if (c > 31 && c < 127) {
			encodedId.append(&c, 1);
		}
		else {
			wchar_t wz[16] = { 0 };
			swprintf_s(wz, L"\\u%04X", c);
			encodedId.append(wz);
		}
	});
	return encodedId;
}

RmRestCommonParams::RmRestCommonParams(const std::wstring clientId)
    : _clientId(clientId), _platformId(L"0"),
	  _deviceId(PrepareDeviceId()),
	  _deviceType(L"Windows Desktop")
{
}

RmRestCommonParams::RmRestCommonParams(const RmRestCommonParams& rhs)
	: _tenant(rhs._tenant), _clientId(rhs._clientId), _platformId(rhs._platformId), _deviceId(rhs._deviceId), _deviceType(rhs._deviceType),
	  _userId(rhs._userId), _userName(rhs._userName), _ticket(rhs._ticket)
{
}

RmRestCommonParams::~RmRestCommonParams()
{
}

RmRestCommonParams& RmRestCommonParams::operator = (const RmRestCommonParams& rhs)
{
    if (this != &rhs) {
		_tenant = rhs._tenant;
        _clientId = rhs._clientId;
        _platformId = rhs._platformId;
        _deviceId = rhs._deviceId;
		_deviceType = rhs._deviceType;
		_userId = rhs._userId;
		_userName = rhs._userName;
		_ticket = rhs._ticket;
    }
    return *this;
}



//
//   class RmRestClient
//

RmRestClient::RmRestClient() : Client(NXRMC_CLIENT_NAME, true, 0)
{
}

RmRestClient::~RmRestClient()
{
}

Result RmRestClient::Init(const std::wstring& clientId)
{
	_commonParams = RmRestCommonParams(clientId);
    return RESULT(0);
}

void RmRestClient::Clear()
{
}

NX::REST::http::Connection* RmRestClient::Connect(const std::wstring& server_url, Result& res)
{
    if (!Opened()) {
        res = Open();
        if (!res)
            return nullptr;
    }

    return CreateConnection(server_url, &res);
}