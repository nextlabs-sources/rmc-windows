

#ifndef __NX_WINUTIL_REGISTRY_H__
#define __NX_WINUTIL_REGISTRY_H__

#include <nx\common\result.h>

#include <Windows.h>
#include <string>

namespace NX {

	namespace win {

		class RegKey
		{
		public:
			virtual ~RegKey();

			Result Create(const std::wstring& path
#ifdef _WIN64
				, bool wow6432 = false
#endif
			);
			Result Open(const std::wstring& path, bool readOnly
#ifdef _WIN64
				, bool wow6432 = false
#endif
			);
			Result Delete(const std::wstring& path
#ifdef _WIN64
				, bool wow6432 = false
#endif
			);
			bool Exists(const std::wstring& path
#ifdef _WIN64
				, bool wow6432 = false
#endif
			);

			void Close();

			inline operator HKEY() const { return _h; }
			inline bool IsOpened() const { return (NULL != _h); }

			Result QueryValue(const std::wstring& name, _Out_opt_ unsigned long* type, _Out_opt_ unsigned long* size);

			Result ReadDefaultValue(std::wstring& s);
			Result ReadValue(const std::wstring& name, unsigned int* pv);
			Result ReadValue(const std::wstring& name, unsigned __int64* pv);
			Result ReadValue(const std::wstring& name, std::wstring& s);
			Result ReadValue(const std::wstring& name, std::vector<std::wstring>& v);
			Result ReadValue(const std::wstring& name, std::vector<unsigned char>& v);

			Result WriteDefaultValue(const std::wstring& v);
			Result WriteValue(const std::wstring& name, unsigned int pv);
			Result WriteValue(const std::wstring& name, unsigned __int64 pv);
			Result WriteValue(const std::wstring& name, const std::wstring& v, bool expandable = false);
			Result WriteValue(const std::wstring& name, const std::vector<std::wstring>& v);
			Result WriteValue(const std::wstring& name, const std::vector<unsigned char>& v);

		protected:
			RegKey();
			RegKey(HKEY root);

			Result ReadValueEx(const std::wstring& name, std::vector<unsigned char>& v, _Out_opt_ unsigned long* type);
			Result WriteValueEx(const std::wstring& name, unsigned long type, const void* v, unsigned long size);

		private:
			// Copy/Move are not allowed
			RegKey(const RegKey& rhs);
			RegKey(RegKey&& rhs);
			RegKey& operator = (const RegKey& rhs);
			RegKey& operator = (RegKey&& rhs);

		private:
			HKEY	_root;
			HKEY	_h;
		};

		class RegMachineKey : public RegKey
		{
		public:
			RegMachineKey() : RegKey(HKEY_LOCAL_MACHINE) {}
			virtual ~RegMachineKey() {}
		};

		class RegUserKey : public RegKey
		{
		public:
			RegUserKey() : RegKey(HKEY_CURRENT_USER) {}
			virtual ~RegUserKey() {}
		};


	}	// NX::win

}   // NX

#endif
