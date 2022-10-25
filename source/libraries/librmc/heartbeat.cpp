

#include"stdafx.h"

#include <nx\rmc\heartbeat.h>

#include <nx\common\string.h>
#include <nx\common\rapidjson.h>

using namespace NX;
using namespace NX::rapidjson;


RmHeartbeatObject::RmHeartbeatObject()
{
}

RmHeartbeatObject::RmHeartbeatObject(const std::wstring& name, const std::wstring& serial, const std::wstring& content)
	: _name(name),
	_serial(serial),
	_data(content)
{
}

RmHeartbeatObject::RmHeartbeatObject(const RmHeartbeatObject& rhs)
	: _name(rhs._name),
	_serial(rhs._serial),
	_data(rhs._data)
{
}

RmHeartbeatObject::~RmHeartbeatObject()
{
}

RmHeartbeatObject& RmHeartbeatObject::operator = (const RmHeartbeatObject& rhs)
{
	if (this != &rhs) {
		_name = rhs._name;
		_serial = rhs._serial;
		_data = rhs._data;
	}
	return *this;
}

std::string RmHeartbeatObject::serialize() const
{
	std::string s;

	std::shared_ptr<JsonObject>	root(JsonValue::CreateObject());
	if (NULL == root)
		return std::string();

	do {

		PJSONVALUE	pName = JsonValue::CreateString(_name);
		if (NULL == pName)
			break;
		root->set(L"name", pName);
		PJSONVALUE	pSerial = JsonValue::CreateString(_serial);
		if (NULL == pSerial)
			break;
		root->set(L"serialNumber", pSerial);
		PJSONVALUE	pContent = JsonValue::CreateString(_data);
		if (NULL == pContent)
			break;
		root->set(L"content", pContent);

		rapidjson::JsonStringWriter<char> writer;
		s = writer.Write(root.get());

	} while (FALSE);
	
	root.reset();
	return s;
}
