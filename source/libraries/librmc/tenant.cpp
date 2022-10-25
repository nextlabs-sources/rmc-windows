
#include "stdafx.h"

#include <nx\rmc\tenant.h>

#include <nx\common\string.h>
#include <nx\winutil\registry.h>

using namespace NX;


extern std::wstring regKeyNameRouter;

const RmTenant& NX::GetDefaultTenant()
{
    static const RmTenant tenant(L"skydrm.com");
    return tenant;
}

RmTenant::RmTenant()
{
}

RmTenant::RmTenant(const RmTenant& rhs) : _name(rhs._name), _id(rhs._id)
{
}

RmTenant::~RmTenant()
{
}

RmTenant& RmTenant::operator = (const RmTenant& rhs)
{
    if (this != &rhs)
    {
        _name = rhs._name;
        _id = rhs._id;
    }

    return *this;
}

void RmTenant::Clear()
{
    _name.clear();
    _id.clear();
}


RmTenantPreference::RmTenantPreference() : _heartbeat_frequency(0)
{
}

RmTenantPreference::RmTenantPreference(const RmTenantPreference& rhs) : _heartbeat_frequency(rhs._heartbeat_frequency)
{
}

RmTenantPreference::~RmTenantPreference()
{
}

RmTenantPreference& RmTenantPreference::operator = (const RmTenantPreference& rhs)
{
    if (this != &rhs) {
        _heartbeat_frequency = rhs._heartbeat_frequency;
    }
    return *this;
}

void RmTenantPreference::Clear()
{
    _heartbeat_frequency = 0;
}

RmTenant RmTenant::LoadFromRegistry(const std::wstring& routerName)
{
    const std::wstring regKeyName(regKeyNameRouter + L"\\" + routerName);
    NX::win::RegUserKey key;
    RmTenant tenant;

    Result res = key.Open(regKeyName, true);
    if (!res)
        return tenant;

    do {

        std::wstring tenantStr;
        res = key.ReadValue(L"tenant", tenantStr);
        if (res)
            tenant = RmTenant(tenantStr);

    } while (FALSE);

    return tenant;
}
