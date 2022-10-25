

#include"stdafx.h"

#include <nx\rmc\product.h>

#include <nx\common\string.h>
#include <nx\common\rapidjson.h>
#include <nx\winutil\file.h>
#include <nx\winutil\registry.h>

using namespace NX;
using namespace NX::rapidjson;


Product::Product(const std::wstring& name) : _name(name)
{
}

Product::Product(const Product& rhs)
	: _name(rhs._name), _version(rhs._version), _publisher(rhs._publisher), _installLoc(rhs._installLoc)
{
}

Product::~Product()
{
}

Product& Product::operator = (const Product& rhs)
{
	if (this != &rhs) {
		_name = rhs._name;
		_version = rhs._version;
		_publisher = rhs._publisher;
		_installLoc = rhs._installLoc;
	}
	return *this;
}

Result Product::Load()
{
	if (_name.empty()) {
		return LoadFromFile();
	}
	else {
		return LoadFromRegistry();
	}
}

Result Product::LoadFromFile()
{
	std::wstring file;

	if (0 == GetModuleFileNameW(NULL, NX::wstring_buffer(file, MAX_PATH), MAX_PATH))
		return RESULT(GetLastError());

	NX::win::FileInfo fi = NX::win::FileInfo::LoadFileInfo(file);
	if (fi.Empty())
		return RESULT(ERROR_INVALID_DATA);

	_name = fi.GetProductName();
	const std::wstring& verInFile = fi.GetProductVersionString();
	return LoadFromRegistry();
}

Result Product::LoadFromRegistry()
{
	assert(!_name.empty());

	NX::win::RegMachineKey appKeyRoot;
	Result res = appKeyRoot.Open(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall", true);
	if (!res)
		return res;

	DWORD dwIndex = 0;
	WCHAR wzKeyName[MAX_PATH] = { 0 };

	while (ERROR_SUCCESS == RegEnumKeyW(appKeyRoot, dwIndex++, wzKeyName, MAX_PATH)) {

		NX::win::RegMachineKey appKey;
		std::wstring keyPath(L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\");
		std::wstring prodName;
		keyPath.append(wzKeyName);
		res = appKey.Open(keyPath, true);
		wzKeyName[0] = 0;
		if (!res)
			continue;
		res = appKey.ReadValue(L"DisplayName", prodName);
		if (!res)
			continue;
		if (0 != _wcsicmp(prodName.c_str(), _name.c_str()))
			continue;
		
		res = appKey.ReadValue(L"DisplayVersion", _version);
		if (!res)
			_version.clear();
		res = appKey.ReadValue(L"Publisher", _publisher);
		if (!res)
			_publisher.clear();
		res = appKey.ReadValue(L"InstallLocation", _installLoc);
		if (!res)
			_installLoc.clear();

		break;
	}

	if (_version.empty())
		return RESULT(ERROR_NOT_FOUND);

	return RESULT(0);
}

ULONG Product::VersionToNumber() const
{
	ULONG uv = 0;
	int major = 0;
	int minor = 0;
	int build = 0;

	if (_version.empty())
		return 0U;
	swscanf_s(_version.c_str(), L"%d.%d.%d", &major, &minor, &build);
	uv |= (((ULONG)major) << 24);
	uv |= (((ULONG)minor) << 16);
	uv |= (((ULONG)build) & 0x0000FFFF);
	return uv;
}