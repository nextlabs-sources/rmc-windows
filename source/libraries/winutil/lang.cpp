

#include "stdafx.h"

#include <nx\common\string.h>
#include <nx\winutil\lang.h>

using namespace NX;
using namespace NX::win;



static std::wstring LocalIdToLocalName(LCID lcid)
{
	std::wstring s;
	if (0 != LCIDToLocaleName(lcid, NX::wstring_buffer(s, LOCALE_NAME_MAX_LENGTH), LOCALE_NAME_MAX_LENGTH, 0)) {
		s.clear();
	}
	return s;
}

Language::Language() : _lcid(0)
{
}

Language::Language(unsigned short langId, unsigned short sourtId) : _lcid(MAKELCID(langId, sourtId)), _name(LocalIdToLocalName(_lcid))
{
}

Language::Language(LCID lcid) : _lcid(lcid), _name(LocalIdToLocalName(lcid))
{
}

Language::Language(const Language& rhs) : _lcid(rhs._lcid), _name(rhs._name)
{
}

Language::~Language()
{
}

Language& Language::operator = (const Language& rhs)
{
	if (this != &rhs) {
		_lcid = rhs._lcid;
		_name = rhs._name;
	}
	return *this;
}

Language Language::GetSystemDefaultLanguage()
{
	return Language(GetSystemDefaultLCID());
}

Language Language::GetUserDefaultLanguage()
{
	return Language(GetUserDefaultLCID());
}
