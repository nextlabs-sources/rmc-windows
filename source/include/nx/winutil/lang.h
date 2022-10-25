

#ifndef __NX_WINUTIL_LANGUAGE_H__
#define __NX_WINUTIL_LANGUAGE_H__

#include <string>

namespace NX {

	namespace win {

		class Language
		{
		public:
			Language();
			explicit Language(unsigned short langId, unsigned short sourtId);
			explicit Language(LCID lcid);
			Language(const Language& rhs);
			virtual ~Language();

			static Language GetSystemDefaultLanguage();
			static Language GetUserDefaultLanguage();

			Language& operator = (const Language& rhs);

			inline bool Empty() const { return (0 == _lcid || _name.empty()); }
			inline LCID GetLcid() const { return _lcid; }
			inline const std::wstring& GetName() const { return _name; }
			inline unsigned short GetLanguageId() const { return LANGIDFROMLCID(_lcid); }
			inline unsigned short GetSortId() const { return SORTIDFROMLCID(_lcid); }

		private:
			LCID _lcid;
			std::wstring _name;
		};
	}	// NX::win

}   // NX

#endif
