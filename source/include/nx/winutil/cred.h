

#ifndef __NX_WINUTIL_CRED_H__
#define __NX_WINUTIL_CRED_H__

#include <nx\common\result.h>

#include <AccCtrl.h>
#include <Aclapi.h>
#include <Sddl.h>

namespace NX {

	namespace win {

		class SidObject
		{
		public:
			SidObject();
			explicit SidObject(const char* s);
			explicit SidObject(const wchar_t* s);
			explicit SidObject(PSID sid);
			SidObject(const SidObject& rhs);
			SidObject(SidObject&& rhs);
			virtual ~SidObject();

			SidObject& operator = (const SidObject& rhs);
			SidObject& operator = (SidObject&& rhs);

			bool operator == (PSID rhs) const;
			bool operator == (const SidObject& rhs) const;


			static const SID_IDENTIFIER_AUTHORITY NullAuthority;
			static const SID_IDENTIFIER_AUTHORITY WorldAuthority;
			static const SID_IDENTIFIER_AUTHORITY NtAuthority;

			static const SidObject EveryoneSid;
			static const SidObject LocalAdminsSid;
			static const SidObject LocalUsersSid;
			static const SidObject LocalGuestsSid;
			static const SidObject LocalPowerUsersSid;
			static const SidObject LocalSystemSid;

			static SidObject Create(PSID_IDENTIFIER_AUTHORITY pIdentifierAuthority,
									BYTE nSubAuthorityCount,
									DWORD dwSubAuthority0,
									DWORD dwSubAuthority1,
									DWORD dwSubAuthority2,
									DWORD dwSubAuthority3,
									DWORD dwSubAuthority4,
									DWORD dwSubAuthority5,
									DWORD dwSubAuthority6,
									DWORD dwSubAuthority7);

			void Clear();
			bool Valid() const;
			ULONG Size() const;
			std::wstring Serialize() const;

			bool IsNullAuthority() const;
			bool IsWorldAuthority() const;
			bool IsNtAuthority() const;

			bool IsEveryone() const;
			bool IsNtLocalSystem() const;
			bool IsNtLocalService() const;
			bool IsNtNetworkService() const;
			bool IsNtDomainSid() const;
			bool IsNtBuiltinSid() const;

			inline operator PSID() const { return _sid; }
			inline bool Empty() const { return (NULL == _sid); }
			inline void Attach(PSID sid) { Clear(); _sid = sid; }
			inline PSID Detach() { PSID sid = _sid; _sid = NULL; return sid; }

		private:
			PSID    _sid;
		};


		class ExplicitAccess : public EXPLICIT_ACCESS
		{
		public:
			ExplicitAccess();
			explicit ExplicitAccess(PSID sid, DWORD access_permissions, TRUSTEE_TYPE trustee_type, DWORD inheritance);
			explicit ExplicitAccess(const char* sid, DWORD access_permissions, TRUSTEE_TYPE trustee_type, DWORD inheritance);
			explicit ExplicitAccess(const wchar_t* sid, DWORD access_permissions, TRUSTEE_TYPE trustee_type, DWORD inheritance);
			ExplicitAccess(const ExplicitAccess& rhs);
			ExplicitAccess(ExplicitAccess&& rhs);
			virtual ~ExplicitAccess();

			ExplicitAccess& operator = (const ExplicitAccess& rhs);
			ExplicitAccess& operator = (ExplicitAccess&& rhs);
		};

		class SecurityAttribute : public SECURITY_ATTRIBUTES
		{
		public:
			SecurityAttribute();
			explicit SecurityAttribute(const ExplicitAccess& ea);
			explicit SecurityAttribute(const std::vector<ExplicitAccess>& eas);
			virtual ~SecurityAttribute();

			void Clear();

		protected:
			void Init(const ExplicitAccess* pEa, DWORD dwSize);

		private:
			// Copy/Move are not allowed
			SecurityAttribute(const SecurityAttribute& rhs);
			SecurityAttribute(SecurityAttribute&& rhs);
			SecurityAttribute& operator = (const SecurityAttribute& rhs);
			SecurityAttribute& operator = (SecurityAttribute&& rhs);
		private:
			PACL    _dacl;
		};


	}	// NX::win

}   // NX

#endif
