
#include"stdafx.h"

#include <nx\rmc\watermark.h>

#include <nx\common\string.h>
#include <nx\common\rapidjson.h>

using namespace NX;
using namespace NX::rapidjson;


RmWatermarkConfig::RmWatermarkConfig()
	: _text(L"$(User)\\nProtected by SkyDRM"), _fontName(L"Sitka Text"), _density(L"dense"),
	_transparency(70), _fontSize(26), _rotation(ANTICLOCKWISE), _repeat(true), _fontColor(RGB(0, 128, 0))
{
}

RmWatermarkConfig::RmWatermarkConfig(const std::wstring& s)
{
	(void)deserialize(s);
}

RmWatermarkConfig::RmWatermarkConfig(const RmWatermarkConfig& rhs)
	: _text(rhs._text), _fontName(rhs._fontName), _density(rhs._density),
	_transparency(rhs._transparency), _fontSize(rhs._fontSize), _rotation(rhs._rotation), _repeat(rhs._repeat), _fontColor(rhs._fontColor)
{
}

RmWatermarkConfig::~RmWatermarkConfig()
{
}

RmWatermarkConfig& RmWatermarkConfig::operator = (const RmWatermarkConfig& rhs)
{
	if (this != &rhs) {
		_text = rhs._text;
		_fontName = rhs._fontName;
		_density = rhs._density;
		_transparency = rhs._transparency;
		_fontSize = rhs._fontSize;
		_rotation = rhs._rotation;
		_repeat = rhs._repeat;
		_fontColor = rhs._fontColor;
	}
	return *this;
}

std::string RmWatermarkConfig::serialize() const
{
	std::string s;

	std::shared_ptr<JsonObject> root(JsonValue::CreateObject());
	if (!IsValidJsonObject(root.get()))
		return s;

	PJSONVALUE pv = JsonValue::CreateString(_text);
	if (!IsValidJsonString(pv))
		return s;
	root->set(L"text", pv);

	pv = JsonValue::CreateNumber(_transparency);
	if (!IsValidJsonNumber(pv))
		return s;
	root->set(L"transparentRatio", pv);

	pv = JsonValue::CreateString(_fontName);
	if (!IsValidJsonString(pv))
		return s;
	root->set(L"fontName", pv);

	pv = JsonValue::CreateNumber(_fontSize);
	if (!IsValidJsonNumber(pv))
		return s;
	root->set(L"fontSize", pv);

	pv = JsonValue::CreateNumber((int)_fontColor);
	if (!IsValidJsonNumber(pv))
		return s;
	root->set(L"fontColor", pv);

	pv = JsonValue::CreateString((ANTICLOCKWISE == _rotation) ? L"Anticlockwise" : ((CLOCKWISE == _rotation) ? L"Clockwise" : L"None"));
	if (!IsValidJsonString(pv))
		return s;
	root->set(L"rotation", pv);

	pv = JsonValue::CreateString(_density);
	if (!IsValidJsonString(pv))
		return s;
	root->set(L"density", pv);

	pv = JsonValue::CreateBool(_repeat);
	if (!IsValidJsonBool(pv))
		return s;
	root->set(L"repeat", pv);

	JsonStringWriter<char> writer;
	s = writer.Write(root.get());
	root.reset();

	return s;
}

Result RmWatermarkConfig::deserialize(const std::string& s)
{
	const std::wstring& ws = NX::conv::utf8toutf16(s);
	return deserialize(ws);
}

Result RmWatermarkConfig::deserialize(const std::wstring& s)
{
	if (s.empty())
		return RESULT(ERROR_INVALID_PARAMETER);

	JsonDocument doc;
	if(!doc.LoadJsonString<wchar_t>(s))
		return RESULT(ERROR_INVALID_DATA);

	PCJSONVALUE root = doc.GetRoot();
	if (!IsValidJsonObject(root))
		return RESULT(ERROR_INVALID_DATA);

	PCJSONVALUE pv = root->AsObject()->at(L"text");
	if (IsValidJsonString(pv))
		_text = pv->AsString()->GetString();

	pv = root->AsObject()->at(L"transparentRatio");
	if (IsValidJsonNumber(pv))
		_transparency = pv->AsNumber()->ToInt();

	pv = root->AsObject()->at(L"fontName");
	if (IsValidJsonString(pv))
		_fontName = pv->AsString()->GetString();

	pv = root->AsObject()->at(L"fontSize");
	if (IsValidJsonNumber(pv))
		_fontSize = pv->AsNumber()->ToInt();

	pv = root->AsObject()->at(L"fontColor");
	if (IsValidJsonNumber(pv))
		_fontColor = (UINT)pv->AsNumber()->ToInt();

	pv = root->AsObject()->at(L"rotation");
	if (IsValidJsonString(pv)) {
		const std::wstring& sRotation = pv->AsString()->GetString();
		_rotation = (0 == _wcsicmp(sRotation.c_str(), L"Anticlockwise")) ? ANTICLOCKWISE : ((0 == _wcsicmp(sRotation.c_str(), L"Clockwise")) ? CLOCKWISE : NOROTATION);
	}

	pv = root->AsObject()->at(L"density");
	if (IsValidJsonString(pv))
		_density = pv->AsString()->GetString();

	pv = root->AsObject()->at(L"repeat");
	if (IsValidJsonBool(pv))
		_repeat = pv->AsBool()->GetBool();

	return RESULT(0);
}