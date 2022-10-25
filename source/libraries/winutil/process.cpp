
#include "stdafx.h"

#include <nx\winutil\process.h>


using namespace NX;
using namespace NX::win;


ProcessInfo::ProcessInfo() : _id(-1), _sId(-1)
{
}

ProcessInfo::ProcessInfo(unsigned int pid) : _id(pid)
{
}

ProcessInfo::ProcessInfo(const ProcessInfo& rhs)
	: _id(rhs._id), _sId(rhs._sId), _name(rhs._name), _image(rhs._image), _is64bit(rhs._is64bit)
{
}

ProcessInfo::~ProcessInfo()
{
}

ProcessInfo& ProcessInfo::operator = (const ProcessInfo& rhs)
{
	if (this != &rhs) {
		_id = rhs._id;
		_sId = rhs._sId;
		_name = rhs._name;
		_image = rhs._image;
		_is64bit = rhs._is64bit;
	}
	return *this;
}

void ProcessInfo::clear()
{
	_id = -1;
	_sId = -1;
	_name.clear();
	_image.clear();
	_is64bit = false;
}


ProcessEnvironment::ProcessEnvironment()
{
}

ProcessEnvironment::ProcessEnvironment(const std::vector<std::pair<std::wstring, std::wstring>>& envs, bool merge) : _envs(ProcessEnvironment::Init(envs, merge))
{
}

ProcessEnvironment::ProcessEnvironment(const ProcessEnvironment& rhs) : _envs(rhs._envs)
{
}

ProcessEnvironment::~ProcessEnvironment()
{
}

ProcessEnvironment& ProcessEnvironment::operator = (const ProcessEnvironment& rhs)
{
	if (this != &rhs) {
		_envs = rhs._envs;
	}
	return *this;
}

std::wstring ProcessEnvironment::GetVariable(const std::wstring& key) const
{
	auto pos = std::find_if(_envs.begin(), _envs.end(), [&](const std::pair<std::wstring, std::wstring>& var)->bool {
		return (0 == _wcsicmp(var.first.c_str(), key.c_str()));
	});

	return (pos == _envs.end()) ? std::wstring() : (*pos).second;
}

void ProcessEnvironment::SetVariable(const std::wstring& key, const std::wstring& value, bool merge)
{
	auto pos = std::find_if(_envs.begin(), _envs.end(), [&](const std::pair<std::wstring, std::wstring>& var)->bool {
		return (0 == _wcsicmp(var.first.c_str(), key.c_str()));
	});

	if (pos == _envs.end()) {
		_envs.push_back(std::pair<std::wstring, std::wstring>(key, value));
	}
	else {
		if (merge) {
			if (!(*pos).second.empty()) (*pos).second.append(L";");
			(*pos).second.append(value);
		}
		else {
			(*pos).second = value;
		}
	}
}

std::vector<wchar_t> ProcessEnvironment::CreateBlock() const
{
	static const wchar_t cZero = 0;
	static const wchar_t cEqual = L'=';
	std::vector<wchar_t> vs;
	std::for_each(_envs.cbegin(), _envs.cend(), [&](const std::pair<std::wstring, std::wstring>& var) {
		std::for_each(var.first.begin(), var.first.end(), [&](const wchar_t& c) {vs.push_back(c); });
		vs.push_back(cEqual);
		std::for_each(var.second.begin(), var.second.end(), [&](const wchar_t& c) {vs.push_back(c); });
		vs.push_back(cZero);
	});
	vs.push_back(cZero);
	return vs;
}

std::vector<std::pair<std::wstring, std::wstring>> ProcessEnvironment::Init(const std::vector<std::pair<std::wstring, std::wstring>>& envs, bool merge)
{
	data_type vs;

	LPWCH inEnvs = GetEnvironmentStringsW();
	if (NULL == inEnvs)
		return envs;

	LPWCH sVar = inEnvs;
	while (0 != sVar[0]) {
		std::wstring s(sVar);
		sVar += s.length();
		sVar++; // Last zero

		auto pos = s.find(L'=');
		if (pos != std::wstring::npos) {
			const std::wstring& key = s.substr(0, pos);
			const std::wstring& value = s.substr(pos + 1);
			if (!key.empty() && !value.empty())
				vs.push_back(Variable(key, value));
		}
	}
	FreeEnvironmentStringsW(inEnvs);
	inEnvs = NULL;

	assert(NULL == inEnvs);

	std::for_each(envs.begin(), envs.end(), [&](const std::pair<std::wstring, std::wstring>& item) {
		auto pos = std::find_if(vs.begin(), vs.end(), [&](const std::pair<std::wstring, std::wstring>& var)->bool {
			return (0 == _wcsicmp(var.first.c_str(), item.first.c_str()));
		});
		if (pos == vs.end()) {
			// Not exist
			vs.push_back(item);
		}
		else {
			// Already exist
			if (merge) {
				if (!(*pos).second.empty()) (*pos).second.append(L";");
				(*pos).second.append(item.second);
			}
			else {
				(*pos).second = item.second;
			}
		}
	});

	return vs;
}