

#include "stdafx.h"

#include <nx\rmc\router.h>

#include <nx\common\string.h>
#include <nx\winutil\registry.h>

using namespace NX;


std::wstring regKeyNameRouter = L"SOFTWARE\\NextLabs\\SkyDRM\\Router";

const RmRouter& NX::GetDefaultRouter()
{
	static const RmRouter router(L"SkyDRM", L"https://r.skydrm.com");
	return router;
}

RmRouter::RmRouter()
{
}

RmRouter::RmRouter(const std::wstring& name, const std::wstring& hostUrl)
    : _name(name)
{
	_host = NormalizeHostUrl(hostUrl);
}

RmRouter::RmRouter(const RmRouter& rhs)
    : _name(rhs._name), _host(rhs._host), _hostUrl(rhs._hostUrl)
{
}

RmRouter::~RmRouter()
{
}

RmRouter& RmRouter::operator = (const RmRouter& rhs)
{
    if (this != &rhs) {
        _name = rhs._name;
        _host = rhs._host;
		_hostUrl = rhs._hostUrl;
    }
    return *this;
}

std::wstring RmRouter::NormalizeHostUrl(const std::wstring& url)
{
	std::wstring scheme;
	std::wstring host;
	std::wstring port;
	std::wstring::size_type pos = url.find(L"://");
	if (pos != std::wstring::npos) {
		scheme = url.substr(0, pos + 3);
		host = url.substr(pos + 3);
	}
	else {
		scheme = L"https://";
		host = url;
	}
	NX::tolower(host);

	pos = host.find(L"/");
	if (std::wstring::npos != pos) {
		host = host.substr(0, pos);
	}

	pos = host.find(L":");
	if (std::wstring::npos != pos) {
		port = host.substr(pos);
		host = host.substr(0, pos);
	}

	_hostUrl = std::wstring(scheme + host + port);
	return host;
}

std::vector<std::wstring> RmRouter::GetTenantHistory() const
{
	std::vector<std::wstring> tenants;

	const std::wstring regKeyName(regKeyNameRouter + L"\\" + _name);
	NX::win::RegUserKey key;
	Result res = key.Open(regKeyName, true);
	if (!res)
		return tenants;

	res = key.ReadValue(L"tenants", tenants);
	if (!res)
		tenants.clear();
	
	return tenants;
}

Result RmRouter::SaveTenantHistory(const std::vector<std::wstring>& tenants) const
{
	const std::wstring regKeyName(regKeyNameRouter + L"\\" + _name);
	NX::win::RegUserKey key;
	Result res = key.Create(regKeyName);
	if (!res) {
		const std::wstring regKeyRouter(regKeyNameRouter);
		res = key.Create(regKeyRouter);
		if (!res)
			return res;
		key.Close();
		res = key.Create(regKeyName);
		if (!res)
			return res;
	}

	return key.WriteValue(L"tenants", tenants);
}

bool RmRouter::ExistInRegistry() const
{
	if (Empty())
		return false;
	const std::wstring regKeyName(regKeyNameRouter + L"\\" + _name);
	NX::win::RegUserKey key;
	Result res = key.Open(regKeyName, true);
	if (!res)
		return false;

	key.Close();
	return true;
}

Result RmRouter::SaveToRegistry() const
{
	const std::wstring regKeyName(regKeyNameRouter + L"\\" + _name);
	NX::win::RegUserKey key;
	Result res = key.Create(regKeyName);
	if (!res) {
		const std::wstring regKeyRouter(regKeyNameRouter);
		res = key.Create(regKeyRouter);
		if (!res)
			return res;
		key.Close();
		res = key.Create(regKeyName);
		if (!res)
			return res;
	}

	res = key.WriteValue(L"url", _hostUrl);
	if (!res)
		return res;

	return RESULT(0);
}

RmRouter RmRouter::LoadFromRegistry(const std::wstring& name)
{
	const std::wstring regKeyName(regKeyNameRouter + L"\\" + name);
	NX::win::RegUserKey key;
	RmRouter router;

	Result res = key.Open(regKeyName, true);
	if (!res)
		return router;

	do {

		std::wstring url;
		res = key.ReadValue(L"url", url);
		if (res)
			router = RmRouter(name, url);

	} while (FALSE);

	return router;
}

std::vector<RmRouter> RmRouter::FindAvailableRouters()
{
	std::vector<RmRouter> routers;
	NX::win::RegUserKey key;

	// ALways write default router
	const RmRouter& defaultRouter = NX::GetDefaultRouter();
	if (!defaultRouter.ExistInRegistry()) {
		defaultRouter.SaveToRegistry();
	}

	routers.push_back(defaultRouter);

	Result res = key.Open(regKeyNameRouter, true);
	if (!res)
		return routers;

	WCHAR wzName[MAX_PATH] = { 0 };
	DWORD dwIndex = 0;

	while (ERROR_SUCCESS == ::RegEnumKeyW(key, dwIndex++, wzName, MAX_PATH)) {

		if (0 == _wcsicmp(wzName, defaultRouter.GetName().c_str()))
			continue;

		const RmRouter& router = RmRouter::LoadFromRegistry(wzName);
		if (router.Empty())
			continue;

		routers.push_back(router);
	}

	return routers;
}