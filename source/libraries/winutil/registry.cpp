

#include "stdafx.h"

#include <nx\common\macros.h>
#include <nx\winutil\registry.h>

using namespace NX;
using namespace NX::win;


RegKey::RegKey() : _root(HKEY_LOCAL_MACHINE), _h(NULL)
{
}

RegKey::RegKey(HKEY root) : _root(root), _h(NULL)
{
}

RegKey::RegKey(const RegKey& rhs)
{
}

RegKey::RegKey(RegKey&& rhs)
{
}

RegKey::~RegKey()
{
	Close();
}

RegKey& RegKey::operator = (const RegKey& rhs)
{
	if (this != &rhs) {
	}
	return *this;
}

RegKey& RegKey::operator = (RegKey&& rhs)
{
	if (this != &rhs) {
	}
	return *this;
}


Result RegKey::Create(const std::wstring& path
#ifdef _WIN64
	, bool wow6432
#endif
)
{
	unsigned long disposition = 0;
	unsigned long desired_access = KEY_ALL_ACCESS;
#ifdef _WIN64
	desired_access |= (wow6432 ? KEY_WOW64_32KEY : KEY_WOW64_64KEY);
#endif

	long result = ::RegCreateKeyExW(_root, path.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, desired_access, NULL, &_h, &disposition);
	if (ERROR_SUCCESS != result) {
		_h = NULL;
		return RESULT(result);
	}

	return RESULT(0);
}

Result RegKey::Open(const std::wstring& path, bool readOnly
#ifdef _WIN64
	, bool wow6432
#endif
)
{
	unsigned long desired_access = readOnly ? KEY_READ : (KEY_READ | KEY_WRITE);
#ifdef _WIN64
	desired_access |= (wow6432 ? KEY_WOW64_32KEY : KEY_WOW64_64KEY);
#endif

	long result = ::RegOpenKeyExW(_root, path.c_str(), 0, desired_access, &_h);
	if (ERROR_SUCCESS != result) {
		_h = NULL;
		return RESULT(result);
	}

	return RESULT(0);
}

Result RegKey::Delete(const std::wstring& path
#ifdef _WIN64
	, bool wow6432
#endif
)
{
#ifdef _WIN64
	long result = RegDeleteKeyExW(_root, path.c_str(), wow6432 ? KEY_WOW64_32KEY : KEY_WOW64_64KEY, 0);
#else
	long result = RegDeleteKeyW(_root, path.c_str());
#endif
	if (ERROR_SUCCESS != result) {
		return RESULT(result);
	}
	return RESULT(0);
}

bool RegKey::Exists(const std::wstring& path
#ifdef _WIN64
	, bool wow6432
#endif
)
{
	unsigned long desired_access = KEY_READ;
#ifdef _WIN64
	desired_access |= (wow6432 ? KEY_WOW64_32KEY : KEY_WOW64_64KEY);
#endif

	HKEY h = NULL;
	long result = ::RegOpenKeyExW(_root, path.c_str(), 0, desired_access, &h);
	if (ERROR_SUCCESS == result) {
		RegCloseKey(h);
		return true;
	}
	else {
		assert(NULL == h);
		return (ERROR_FILE_NOT_FOUND == result) ? false : true;
	}
}

void RegKey::Close()
{
	if (NULL != _h
		&& HKEY_CLASSES_ROOT != _h
		&& HKEY_CURRENT_USER != _h
		&& HKEY_LOCAL_MACHINE != _h
		&& HKEY_USERS != _h
		&& HKEY_PERFORMANCE_DATA != _h
		&& HKEY_PERFORMANCE_TEXT != _h
		&& HKEY_PERFORMANCE_NLSTEXT != _h
		) {
		::RegCloseKey(_h);
		_h = NULL;
	}
}

Result RegKey::QueryValue(const std::wstring& name, _Out_opt_ unsigned long* type, _Out_opt_ unsigned long* size)
{
	long result = 0;
	unsigned long value_type = 0;
	unsigned long value_size = 0;
	result = ::RegQueryValueExW(_h, name.empty() ? NULL : name.c_str(), NULL, &value_type, NULL, &value_size);
	if (ERROR_SUCCESS != result && ERROR_MORE_DATA != result) {
		// failed
		return RESULT(result);
	}

	if (type) *type = value_type;
	if (size) *size = value_size;
	return RESULT(0);
}

Result RegKey::ReadValueEx(const std::wstring& name, std::vector<unsigned char>& v, _Out_opt_ unsigned long* type)
{
	long result = 0;
	unsigned long value_size = 0;

	Result res = QueryValue(name, type, &value_size);
	if (!res)
		return res;

	if (value_size == 0) {
		v.clear();
		return RESULT(0);
	}

	// reset buffer size
	v.resize(value_size, 0);
	// try to get data again
	result = ::RegQueryValueExW(_h, name.empty() ? NULL : name.c_str(), NULL, type, (LPBYTE)v.data(), &value_size);
	if (ERROR_SUCCESS != result) {
		return RESULT(result);
	}

	assert(value_size == v.size());
	return RESULT(0);
}

Result RegKey::WriteValueEx(const std::wstring& name, unsigned long type, const void* v, unsigned long size)
{
	long result = ::RegSetValueExW(_h, name.empty() ? NULL : name.c_str(), 0, type, (LPBYTE)v, size);
	return RESULT(result);
}

Result RegKey::ReadDefaultValue(std::wstring& s)
{
	return ReadValue(std::wstring(), s);
}

Result RegKey::ReadValue(const std::wstring& name, unsigned int* pv)
{
	std::vector<unsigned char> v;
	unsigned long type = 0;
	Result res = ReadValueEx(name, v, &type);
	if (!res)
		return res;

	if (REG_DWORD == type || REG_QWORD == type) {
		assert(v.size() >= 4);
		*pv = static_cast<unsigned int>(*((unsigned long*)v.data()));
	}
	else if (REG_DWORD_BIG_ENDIAN == type) {
		assert(v.size() >= 4);
		*pv = static_cast<unsigned int>(SWAP_ENDIAN(*((unsigned long*)v.data())));
	}
	else {
		return RESULT(ERROR_INVALID_DATATYPE);
	}
	return RESULT(0);
}

Result RegKey::ReadValue(const std::wstring& name, unsigned __int64* pv)
{
	std::vector<unsigned char> v;
	unsigned long type = 0;
	Result res = ReadValueEx(name, v, &type);
	if (!res)
		return res;

	if (REG_QWORD == type) {
		assert(v.size() == 8);
		*pv = *((unsigned __int64*)v.data());
	}
	else if (REG_DWORD == type) {
		assert(v.size() == 4);
		*pv = static_cast<unsigned __int64>(*((unsigned long*)v.data()));
	}
	else if (REG_DWORD_BIG_ENDIAN == type) {
		assert(v.size() == 4);
		*pv = static_cast<unsigned __int64>(SWAP_ENDIAN(*((unsigned long*)v.data())));
	}
	else {
		return RESULT(ERROR_INVALID_DATATYPE);
	}
	return RESULT(0);
}

static std::wstring ExpandEnvString(const std::wstring& s)
{
	static const size_t max_info_buf_size = 32767;
	std::wstring output;
	std::vector<wchar_t> buf;
	buf.resize(max_info_buf_size, 0);
	if (0 != ExpandEnvironmentStringsW(s.c_str(), buf.data(), max_info_buf_size)) {
		output = buf.data();
	}

	else {
		output = s;
	}
	return output;
}

static std::vector<std::wstring> ExpandMultiStrings(const wchar_t* s)
{
	std::vector<std::wstring> strings;
	while (0 != s[0]) {
		std::wstring ws(s);
		assert(ws.length() > 0);
		s += ws.length() + 1;
		strings.push_back(ws);
	}
	return strings;
}

Result RegKey::ReadValue(const std::wstring& name, std::wstring& s)
{
	std::vector<unsigned char> v;
	unsigned long type = 0;
	Result res = ReadValueEx(name, v, &type);
	if (!res)
		return res;

	if (REG_SZ == type || REG_MULTI_SZ == type) {
		s = v.empty() ? L"" : ((const wchar_t*)v.data());
	}
	else if (REG_EXPAND_SZ == type) {
		s = v.empty() ? L"" : ((const wchar_t*)v.data());
		s = ExpandEnvString(s);
	}
	else {
		return RESULT(ERROR_INVALID_DATATYPE);
	}
	return RESULT(0);
}

Result RegKey::ReadValue(const std::wstring& name, std::vector<std::wstring>& v)
{
	std::vector<unsigned char> buf;
	unsigned long type = 0;
	Result res = ReadValueEx(name, buf, &type);
	if (!res)
		return res;

	if (REG_MULTI_SZ == type) {
		if (!buf.empty()) {
			v = ExpandMultiStrings((const wchar_t*)buf.data());
		}
	}
	else {
		return RESULT(ERROR_INVALID_DATATYPE);
	}
	return RESULT(0);
}

Result RegKey::ReadValue(const std::wstring& name, std::vector<unsigned char>& v)
{
	unsigned long type = 0;
	return ReadValueEx(name, v, &type);
}

Result RegKey::WriteDefaultValue(const std::wstring& v)
{
	return WriteValue(std::wstring(), v, false);
}

Result RegKey::WriteValue(const std::wstring& name, unsigned int v)
{
	return WriteValueEx(name, REG_DWORD, &v, (unsigned long)sizeof(unsigned int));
}

Result RegKey::WriteValue(const std::wstring& name, unsigned __int64 v)
{
	return WriteValueEx(name, REG_QWORD, &v, (unsigned long)sizeof(unsigned __int64));
}

Result RegKey::WriteValue(const std::wstring& name, const std::wstring& v, bool expandable)
{
	return WriteValueEx(name, expandable ? REG_EXPAND_SZ : REG_SZ, v.c_str(), (unsigned long)((v.length() + 1) * 2));
}

Result RegKey::WriteValue(const std::wstring& name, const std::vector<std::wstring>& v)
{
	std::vector<wchar_t> buf;
	size_t buflen = 0;

	// Calculate required buffer size
	std::for_each(v.begin(), v.end(), [&](const std::wstring& s) {
		if (!s.empty()) {
			buflen += s.length() + 1;
		}
	});
	buflen += 1;

	// Copy strings to buffer
	buf.resize(buflen, 0);
	wchar_t* p = buf.data();
	std::for_each(v.begin(), v.end(), [&](const std::wstring& s) {
		if (!s.empty()) {
			memcpy(p, s.c_str(), sizeof(wchar_t) * s.length());
			p += (s.length() + 1);
		}
	});

	return WriteValueEx(name, REG_MULTI_SZ, buf.data(), (unsigned long)(buf.size() * 2));
}

Result RegKey::WriteValue(const std::wstring& name, const std::vector<unsigned char>& v)
{
	return WriteValueEx(name, REG_BINARY, v.data(), (unsigned long)v.size());
}
