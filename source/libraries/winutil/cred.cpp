

#include "stdafx.h"

#include <nx\winutil\cred.h>

using namespace NX;
using namespace NX::win;



static PSID StringToSidA(const char* s)
{
	PSID sid = NULL;
	if (s == NULL)
		return NULL;
	if (!ConvertStringSidToSidA(s, &sid))
		return NULL;
	return sid;
}

static PSID StringToSidW(const wchar_t* s)
{
	PSID sid = NULL;
	if (s == NULL)
		return NULL;
	if (!ConvertStringSidToSidW(s, &sid))
		return NULL;
	return sid;
}

static PSID MyCopySid(PSID sid)
{
	PSID p = NULL;
	if(sid == NULL)
		return NULL;
	DWORD len = GetLengthSid(sid);
	if (0 == len)
		return NULL;
	p = (PSID)LocalAlloc(LMEM_FIXED, len);
	if (p == NULL)
		return NULL;
	if (!CopySid(len, p, sid)) {
		LocalFree(p);
		p = NULL;
		return NULL;
	}

	return p;
}

const SID_IDENTIFIER_AUTHORITY SidObject::NullAuthority = { 0,0,0,0,0,0 };
const SID_IDENTIFIER_AUTHORITY SidObject::WorldAuthority = { 0,0,0,0,0,1 };
const SID_IDENTIFIER_AUTHORITY SidObject::NtAuthority = { 0,0,0,0,0,5 };

const SidObject SidObject::EveryoneSid(SidObject::Create((PSID_IDENTIFIER_AUTHORITY)&SidObject::WorldAuthority, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0));
const SidObject SidObject::LocalSystemSid(SidObject::Create((PSID_IDENTIFIER_AUTHORITY)&SidObject::NtAuthority, 1, SECURITY_LOCAL_SYSTEM_RID, 0, 0, 0, 0, 0, 0, 0));
const SidObject SidObject::LocalAdminsSid(SidObject::Create((PSID_IDENTIFIER_AUTHORITY)&SidObject::NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_ADMINS, 0, 0, 0, 0, 0, 0));
const SidObject SidObject::LocalUsersSid(SidObject::Create((PSID_IDENTIFIER_AUTHORITY)&SidObject::NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_USERS, 0, 0, 0, 0, 0, 0));
const SidObject SidObject::LocalGuestsSid(SidObject::Create((PSID_IDENTIFIER_AUTHORITY)&SidObject::NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_GUESTS, 0, 0, 0, 0, 0, 0));
const SidObject SidObject::LocalPowerUsersSid(SidObject::Create((PSID_IDENTIFIER_AUTHORITY)&SidObject::NtAuthority, 2, SECURITY_BUILTIN_DOMAIN_RID, DOMAIN_ALIAS_RID_POWER_USERS, 0, 0, 0, 0, 0, 0));

SidObject::SidObject() : _sid(NULL)
{
}

SidObject::SidObject(const char* s) : _sid(StringToSidA(s))
{
}

SidObject::SidObject(const wchar_t* s) : _sid(StringToSidW(s))
{
}

SidObject::SidObject(PSID sid) : _sid(sid)
{
}

SidObject::SidObject(const SidObject& rhs) : _sid(MyCopySid(rhs._sid))
{
}

SidObject::SidObject(SidObject&& rhs) : _sid(rhs.Detach())
{
}

SidObject::~SidObject()
{
	Clear();
}

SidObject& SidObject::operator = (const SidObject& rhs)
{
	if (this != &rhs) {
		Attach(MyCopySid(rhs._sid));
	}
	return *this;
}

SidObject& SidObject::operator = (SidObject&& rhs)
{
	if (this != &rhs) {
		Attach(rhs.Detach());
	}
	return *this;
}

bool SidObject::operator == (PSID rhs) const
{
	return (_sid == rhs) ? true : (Empty() ? false : (EqualSid(_sid, rhs) ? true : false));
}

bool SidObject::operator == (const SidObject& rhs) const
{
	return (this == &rhs) ? true : (Empty() ? false : (EqualSid(_sid, (PSID)rhs) ? true : false));
}

std::wstring SidObject::Serialize() const
{
	if (NULL == _sid) {
		return std::wstring();
	}

	std::wstring s;
	LPWSTR pws = NULL;
	if (ConvertSidToStringSidW(_sid, &pws)) {
		s = pws;
		LocalFree(pws);
		pws = NULL;
	}
	assert(NULL == pws);
	return s;
}

void SidObject::Clear()
{
	if (NULL != _sid) {
		LocalFree(_sid);
		_sid = NULL;
	}
}

bool SidObject::Valid() const
{
	return Empty() ? false : (IsValidSid(_sid) ? true : false);
}

ULONG SidObject::Size() const
{
	return Empty() ? 0 : GetLengthSid(_sid);
}

SidObject SidObject::Create(PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
							BYTE nSubAuthorityCount,
							DWORD dwSubAuthority0,
							DWORD dwSubAuthority1,
							DWORD dwSubAuthority2,
							DWORD dwSubAuthority3,
							DWORD dwSubAuthority4,
							DWORD dwSubAuthority5,
							DWORD dwSubAuthority6,
							DWORD dwSubAuthority7)
{
	PSID p = NULL;
	if (AllocateAndInitializeSid(pIdentifierAuthority,
		nSubAuthorityCount,
		dwSubAuthority0,
		dwSubAuthority1,
		dwSubAuthority2,
		dwSubAuthority3,
		dwSubAuthority4,
		dwSubAuthority5,
		dwSubAuthority6,
		dwSubAuthority7, &p) && p != NULL) {
		SidObject sid(MyCopySid(p));
		FreeSid(p);
		p = NULL;
		return sid;
	}

	return SidObject();
}


bool SidObject::IsNullAuthority() const
{
	if (Empty())
		return false;
	PSID_IDENTIFIER_AUTHORITY pAuth = GetSidIdentifierAuthority(_sid);
	return (NULL != pAuth && 0 == memcmp(pAuth, &SidObject::NullAuthority, sizeof(SID_IDENTIFIER_AUTHORITY)));
}

bool SidObject::IsWorldAuthority() const
{
	if (Empty())
		return false;
	PSID_IDENTIFIER_AUTHORITY pAuth = GetSidIdentifierAuthority(_sid);
	return (NULL != pAuth && 0 == memcmp(pAuth, &SidObject::WorldAuthority, sizeof(SID_IDENTIFIER_AUTHORITY)));
}

bool SidObject::IsNtAuthority() const
{
	if (Empty())
		return false;
	PSID_IDENTIFIER_AUTHORITY pAuth = GetSidIdentifierAuthority(_sid);
	return (NULL != pAuth && 0 == memcmp(pAuth, &SidObject::NtAuthority, sizeof(SID_IDENTIFIER_AUTHORITY)));
}

bool SidObject::IsEveryone() const
{
	return IsWellKnownSid(_sid, WinWorldSid) ? true : false;
}

bool SidObject::IsNtLocalSystem() const
{
	return IsWellKnownSid(_sid, WinLocalSystemSid) ? true : false;
}

bool SidObject::IsNtLocalService() const
{
	return IsWellKnownSid(_sid, WinLocalServiceSid) ? true : false;
}

bool SidObject::IsNtNetworkService() const
{
	return IsWellKnownSid(_sid, WinNetworkServiceSid) ? true : false;
}

bool SidObject::IsNtDomainSid() const
{
	return (IsNtAuthority() && (*GetSidSubAuthorityCount(_sid) != 0) && (SECURITY_NT_NON_UNIQUE == *GetSidSubAuthority(_sid, 0)));
}

bool SidObject::IsNtBuiltinSid() const
{
	return (IsNtAuthority() && (*GetSidSubAuthorityCount(_sid) != 0) && (SECURITY_BUILTIN_DOMAIN_RID == *GetSidSubAuthority(_sid, 0)));
}


//
//
//

ExplicitAccess::ExplicitAccess()
{
	grfAccessPermissions = 0;
	grfAccessMode = SET_ACCESS;
	grfInheritance = 0;
	Trustee.pMultipleTrustee = NULL;
	Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
	Trustee.TrusteeForm = TRUSTEE_IS_SID;
	Trustee.TrusteeType = TRUSTEE_IS_UNKNOWN;
	Trustee.ptstrName = NULL;

	memset(this, 0, sizeof(EXPLICIT_ACCESS));
}

ExplicitAccess::ExplicitAccess(PSID sid, DWORD access_permissions, TRUSTEE_TYPE trustee_type, DWORD inheritance)
{
	memset(this, 0, sizeof(EXPLICIT_ACCESS));
	grfAccessPermissions = access_permissions;
	grfAccessMode = SET_ACCESS;
	grfInheritance = inheritance;
	Trustee.pMultipleTrustee = NULL;
	Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
	Trustee.TrusteeForm = TRUSTEE_IS_SID;
	Trustee.TrusteeType = trustee_type;
	Trustee.ptstrName = (LPWSTR)MyCopySid(sid);
}

ExplicitAccess::ExplicitAccess(const char* sid, DWORD access_permissions, TRUSTEE_TYPE trustee_type, DWORD inheritance)
{
	memset(this, 0, sizeof(EXPLICIT_ACCESS));
	grfAccessPermissions = access_permissions;
	grfAccessMode = SET_ACCESS;
	grfInheritance = inheritance;
	Trustee.pMultipleTrustee = NULL;
	Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
	Trustee.TrusteeForm = TRUSTEE_IS_SID;
	Trustee.TrusteeType = trustee_type;
	Trustee.ptstrName = (LPWSTR)StringToSidA(sid);
}

ExplicitAccess::ExplicitAccess(const wchar_t* sid, DWORD access_permissions, TRUSTEE_TYPE trustee_type, DWORD inheritance)
{
	memset(this, 0, sizeof(EXPLICIT_ACCESS));
	grfAccessPermissions = access_permissions;
	grfAccessMode = SET_ACCESS;
	grfInheritance = inheritance;
	Trustee.pMultipleTrustee = NULL;
	Trustee.MultipleTrusteeOperation = NO_MULTIPLE_TRUSTEE;
	Trustee.TrusteeForm = TRUSTEE_IS_SID;
	Trustee.TrusteeType = trustee_type;
	Trustee.ptstrName = (LPWSTR)StringToSidW(sid);
}

ExplicitAccess::ExplicitAccess(const ExplicitAccess& rhs)
{
	grfAccessPermissions = rhs.grfAccessPermissions;
	grfAccessMode = rhs.grfAccessMode;
	grfInheritance = rhs.grfInheritance;
	Trustee.pMultipleTrustee = NULL;
	Trustee.MultipleTrusteeOperation = rhs.Trustee.MultipleTrusteeOperation;
	Trustee.TrusteeForm = rhs.Trustee.TrusteeForm;
	Trustee.TrusteeType = rhs.Trustee.TrusteeType;
	Trustee.ptstrName = NULL;
	if (NULL != rhs.Trustee.ptstrName) {
		Trustee.ptstrName = (LPWSTR)MyCopySid((PSID)rhs.Trustee.ptstrName);
	}
}

ExplicitAccess::ExplicitAccess(ExplicitAccess&& rhs)
{
	grfAccessPermissions = rhs.grfAccessPermissions;
	grfAccessMode = rhs.grfAccessMode;
	grfInheritance = rhs.grfInheritance;
	Trustee.pMultipleTrustee = NULL;
	Trustee.MultipleTrusteeOperation = rhs.Trustee.MultipleTrusteeOperation;
	Trustee.TrusteeForm = rhs.Trustee.TrusteeForm;
	Trustee.TrusteeType = rhs.Trustee.TrusteeType;
	Trustee.ptstrName = rhs.Trustee.ptstrName;
	memset(&rhs, 0, sizeof(EXPLICIT_ACCESS));
}

ExplicitAccess::~ExplicitAccess()
{
	if (NULL != Trustee.ptstrName) {
		LocalFree((PSID)Trustee.ptstrName);
	}
	memset(this, 0, sizeof(EXPLICIT_ACCESS));
}

ExplicitAccess& ExplicitAccess::operator = (const ExplicitAccess& rhs)
{
	if (this != &rhs) {
		grfAccessPermissions = rhs.grfAccessPermissions;
		grfAccessMode = rhs.grfAccessMode;
		grfInheritance = rhs.grfInheritance;
		Trustee.pMultipleTrustee = NULL;
		Trustee.MultipleTrusteeOperation = rhs.Trustee.MultipleTrusteeOperation;
		Trustee.TrusteeForm = rhs.Trustee.TrusteeForm;
		Trustee.TrusteeType = rhs.Trustee.TrusteeType;
		Trustee.ptstrName = NULL;
		if (NULL != rhs.Trustee.ptstrName) {
			Trustee.ptstrName = (LPWSTR)MyCopySid((PSID)rhs.Trustee.ptstrName);
		}
	}
	return *this;
}

ExplicitAccess& ExplicitAccess::operator = (ExplicitAccess&& rhs)
{
	if (this != &rhs) {
		grfAccessPermissions = rhs.grfAccessPermissions;
		grfAccessMode = rhs.grfAccessMode;
		grfInheritance = rhs.grfInheritance;
		Trustee.pMultipleTrustee = NULL;
		Trustee.MultipleTrusteeOperation = rhs.Trustee.MultipleTrusteeOperation;
		Trustee.TrusteeForm = rhs.Trustee.TrusteeForm;
		Trustee.TrusteeType = rhs.Trustee.TrusteeType;
		Trustee.ptstrName = rhs.Trustee.ptstrName;
		memset(&rhs, 0, sizeof(EXPLICIT_ACCESS));
	}
	return *this;
}


//
//
//

SecurityAttribute::SecurityAttribute() : SECURITY_ATTRIBUTES({ sizeof(SECURITY_ATTRIBUTES), NULL, FALSE }), _dacl(nullptr)
{
}

SecurityAttribute::SecurityAttribute(const ExplicitAccess& ea) : SECURITY_ATTRIBUTES({ sizeof(SECURITY_ATTRIBUTES), NULL, FALSE }), _dacl(nullptr)
{
	Init(&ea, 1);
}

SecurityAttribute::SecurityAttribute(const std::vector<ExplicitAccess>& eas) : SECURITY_ATTRIBUTES({ sizeof(SECURITY_ATTRIBUTES), NULL, FALSE }), _dacl(nullptr)
{
	if (!eas.empty()) {
		Init(eas.data(), (DWORD)eas.size());
	}
	else {
		Init(NULL, 0);
	}
}

SecurityAttribute::SecurityAttribute(const SecurityAttribute& rhs) : SECURITY_ATTRIBUTES({ sizeof(SECURITY_ATTRIBUTES), NULL, FALSE }), _dacl(nullptr)
{
}

SecurityAttribute::SecurityAttribute(SecurityAttribute&& rhs) : SECURITY_ATTRIBUTES({ sizeof(SECURITY_ATTRIBUTES), NULL, FALSE }), _dacl(nullptr)
{
}

SecurityAttribute::~SecurityAttribute()
{
}

SecurityAttribute& SecurityAttribute::operator = (const SecurityAttribute& rhs)
{
	return *this;
}

SecurityAttribute& SecurityAttribute::operator = (SecurityAttribute&& rhs)
{
	return *this;
}

void SecurityAttribute::Clear()
{
	if (NULL != _dacl) {
		LocalFree(_dacl);
		_dacl = NULL;
	}
	if (NULL != lpSecurityDescriptor) {
		LocalFree(lpSecurityDescriptor);
		lpSecurityDescriptor = NULL;
	}

	nLength = sizeof(SECURITY_ATTRIBUTES);
	lpSecurityDescriptor = NULL;
	bInheritHandle = FALSE;
}

void SecurityAttribute::Init(const ExplicitAccess* pEa, DWORD dwEaCount)
{
	BOOL bSuccess = FALSE;

	Clear();

	do {

		// Initialize SA
		this->nLength = sizeof(SECURITY_ATTRIBUTES);
		this->bInheritHandle = FALSE;
		this->lpSecurityDescriptor = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);
		if (NULL == lpSecurityDescriptor)
			break;
		InitializeSecurityDescriptor(lpSecurityDescriptor, SECURITY_DESCRIPTOR_REVISION);

		if (NULL != pEa && 0 != dwEaCount) {
			// Create ACL
			SetEntriesInAclW(dwEaCount, (PEXPLICIT_ACCESS)pEa, NULL, &_dacl);
			if (NULL == _dacl)
				break;
		}

		// Set ACL
		if (!SetSecurityDescriptorDacl(lpSecurityDescriptor, TRUE, _dacl, FALSE))
			break;

		bSuccess = TRUE;

	} while (FALSE);

	if (!bSuccess) {
		Clear();
	}
}