

#include "stdafx.h"

#include <nx\common\rapidjson.h>


using namespace NX::rapidjson;

JsonNull* JsonValue::CreateNull()
{
    JsonNull* p = new JsonNull();
    return p;
}

JsonBool* JsonValue::CreateBool(bool v)
{
    JsonBool* p = new JsonBool(v);
    return p;
}

JsonNumber* JsonValue::CreateNumber(int v)
{
    JsonNumber* p = new JsonNumber(v);
    return p;
}

JsonNumber* JsonValue::CreateNumber(__int64 v)
{
    JsonNumber* p = new JsonNumber(v);
    return p;
}

JsonNumber* JsonValue::CreateNumber(unsigned int v)
{
    JsonNumber* p = new JsonNumber(v);
    return p;
}

JsonNumber* JsonValue::CreateNumber(unsigned __int64 v)
{
    JsonNumber* p = new JsonNumber(v);
    return p;
}

JsonNumber* JsonValue::CreateNumber(float v)
{
    JsonNumber* p = new JsonNumber(v);
    return p;
}

JsonNumber* JsonValue::CreateNumber(double v)
{
    JsonNumber* p = new JsonNumber(v);
    return p;
}

JsonString* JsonValue::CreateString(const std::string& v)
{
    JsonString* p = new JsonString(NX::conv::utf8toutf16(v));
    return p;
}

JsonString* JsonValue::CreateString(const std::wstring& v)
{
    JsonString* p = new JsonString(v);
    return p;
}

JsonObject* JsonValue::CreateObject()
{
    JsonObject* p = new JsonObject();
    return p;
}

JsonArray* JsonValue::CreateArray()
{
    JsonArray* p = new JsonArray();
    return p;
}