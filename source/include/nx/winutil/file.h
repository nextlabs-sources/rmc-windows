

#ifndef __NX_WINUTIL_FILE_H__
#define __NX_WINUTIL_FILE_H__

#include <nx\common\result.h>
#include <nx\winutil\cred.h>

namespace NX {

	namespace win {

		Result ReadSmallFile(const std::wstring& file, std::vector<UCHAR>& content);
		Result WriteSmallFile(const std::wstring& file, const std::vector<UCHAR>& content, bool overwrite = true, PSECURITY_ATTRIBUTES psa = NULL);


		Result GrantFileAccess(const std::wstring& file, const ExplicitAccess& ea);
		Result GrantFileAccess(const std::wstring& file, const ExplicitAccess* eas, unsigned long ea_count);

		Result GetFileTime(const std::wstring& file, _Out_opt_ LPFILETIME lpCreationTime, _Out_opt_ LPFILETIME lpLastAccessTime, _Out_opt_ LPFILETIME lpLastModifiedTime);
		Result SetFileTime(const std::wstring& file, _In_opt_ const FILETIME* lpCreationTime, _In_opt_ const FILETIME* lpLastAccessTime, _In_opt_ const FILETIME* lpLastModifiedTime);

		class FileInfo
		{
		public:
			FileInfo();
			FileInfo(const FileInfo& rhs);
			virtual ~FileInfo();

			FileInfo& operator = (const FileInfo& rhs);
			void Clear();

			static FileInfo LoadFileInfo(const std::wstring& file);

			inline bool Empty() const { return (0 == _file_type); }

			inline const std::wstring& GetCompanyName() const { return _company_name; }
			inline const std::wstring& GetProductName() const { return _product_name; }
			inline const std::wstring& GetProductVersionString() const { return _product_version_string; }
			inline const std::wstring& GetFileName() const { return _file_name; }
			inline const std::wstring& GetFileDescription() const { return _file_description; }
			inline const std::wstring& GetFileVersion_string() const { return _file_version_string; }

			inline ULARGE_INTEGER GetProductVersion() const { return _product_version; }
			inline unsigned short GetProductVersionMajor() const { return ((unsigned short)(_product_version.HighPart >> 16)); }
			inline unsigned short GetProductVersionMinor() const { return ((unsigned short)_product_version.HighPart); }
			inline unsigned short GetProductVersionBuild() const { return ((unsigned short)(_product_version.LowPart >> 16)); }
			inline unsigned short GetProductVersionPatch() const { return ((unsigned short)_product_version.LowPart); }

			inline ULARGE_INTEGER GetFileVersion() const { return _file_version; }
			inline unsigned short GetFileVersionMajor() const { return ((unsigned short)(_file_version.HighPart >> 16)); }
			inline unsigned short GetFileVersionMinor() const { return ((unsigned short)_file_version.HighPart); }
			inline unsigned short GetFileVersionBuild() const { return ((unsigned short)(_file_version.LowPart >> 16)); }
			inline unsigned short GetFileVersionPatch() const { return ((unsigned short)_file_version.LowPart); }

			inline FILETIME GetFileTime() const { return _file_time; }
			inline unsigned long GetFileFlags() const { return _file_flags; }
			inline unsigned long GetFileOs() const { return _file_os; }
			inline unsigned long GetFileType() const { return _file_type; }
			inline unsigned long GetFileSubtype() const { return _file_subtype; }

			// flags
			inline bool IsFlagDebugOn() const { return (VS_FF_DEBUG == (VS_FF_DEBUG & _file_flags)); }
			inline bool IsFlagPatchedOn() const { return (VS_FF_PATCHED == (VS_FF_PATCHED & _file_flags)); }
			inline bool IsFlagPrereleaseOn() const { return (VS_FF_PRERELEASE == (VS_FF_PRERELEASE & _file_flags)); }
			inline bool IsFlagPrivateBuildOn() const { return (VS_FF_PRIVATEBUILD == (VS_FF_PRIVATEBUILD & _file_flags)); }
			inline bool IsFlagSpecialBuildOn() const { return (VS_FF_SPECIALBUILD == (VS_FF_SPECIALBUILD & _file_flags)); }

			// os
			inline bool IsOsUnknown() const { return (VOS_UNKNOWN == _file_os); }
			inline bool IsOsDos() const { return (VOS_DOS == (VOS_DOS & _file_os)); }
			inline bool IsOsDosWin16() const { return (VOS_DOS_WINDOWS16 == _file_os); }
			inline bool IsOsDosWwin32() const { return (VOS_DOS_WINDOWS32 == _file_os); }
			inline bool IsOsNt() const { return (VOS_NT == (VOS_NT & _file_os)); }
			inline bool IsOsNtWin32() const { return (VOS_NT_WINDOWS32 == _file_os); }
			inline bool IsOsWin16() const { return (VOS__WINDOWS16 == (VOS__WINDOWS16 & _file_os)); }
			inline bool IsOsWin32() const { return (VOS__WINDOWS32 == (VOS__WINDOWS32 & _file_os)); }
			inline bool IsOsOs216() const { return (VOS_OS216 == (VOS_OS216 & _file_os)); }
			inline bool IsOsOs232() const { return (VOS_OS232 == (VOS_OS232 & _file_os)); }
			inline bool IsOsPm16() const { return (VOS__PM16 == (VOS__PM16 & _file_os)); }
			inline bool IsOsPm32() const { return (VOS__PM32 == (VOS__PM32 & _file_os)); }
			inline bool IsOsOs216Pm16() const { return (VOS_OS216_PM16 == _file_os); }
			inline bool IsOsOs232Pm32() const { return (VOS_OS232_PM32 == _file_os); }

			// file type
			inline bool IsUnknownType() const { return (VFT_UNKNOWN == _file_type); }
			inline bool IsApplication() const { return (VFT_APP == _file_type); }
			inline bool IsDll() const { return (VFT_DLL == _file_type); }
			inline bool IsDriver() const { return (VFT_DRV == _file_type); }
			inline bool IsFont() const { return (VFT_FONT == _file_type); }
			inline bool IsStaticLib() const { return (VFT_STATIC_LIB == _file_type); }
			inline bool IsVxd() const { return (VFT_VXD == _file_type); }

			// sub-type
			inline bool IsPrinterDriver() const { return (IsDriver() && VFT2_DRV_PRINTER == _file_subtype); }
			inline bool IsCommunicationDriver() const { return (IsDriver() && VFT2_DRV_COMM == _file_subtype); }
			inline bool IsKeyboardDriver() const { return (IsDriver() && VFT2_DRV_KEYBOARD == _file_subtype); }
			inline bool IsLanguageDriver() const { return (IsDriver() && VFT2_DRV_LANGUAGE == _file_subtype); }
			inline bool IsDisplayDriver() const { return (IsDriver() && VFT2_DRV_DISPLAY == _file_subtype); }
			inline bool IsMouseDriver() const { return (IsDriver() && VFT2_DRV_MOUSE == _file_subtype); }
			inline bool IsNetworkDriver() const { return (IsDriver() && VFT2_DRV_NETWORK == _file_subtype); }
			inline bool IsSystemDriver() const { return (IsDriver() && VFT2_DRV_SYSTEM == _file_subtype); }
			inline bool IsSoundDriver() const { return (IsDriver() && VFT2_DRV_SOUND == _file_subtype); }
			inline bool IsVersioned_printerDriver() const { return (IsDriver() && VFT2_DRV_VERSIONED_PRINTER == _file_subtype); }
			inline bool IsRasterFont() const { return (IsFont() && VFT2_FONT_RASTER == _file_subtype); }
			inline bool IsTruetypeFont() const { return (IsFont() && VFT2_FONT_TRUETYPE == _file_subtype); }
			inline bool IsVectorFont() const { return (IsFont() && VFT2_FONT_VECTOR == _file_subtype); }

		private:
			std::wstring	_company_name;
			std::wstring	_product_name;
			std::wstring	_product_version_string;
			std::wstring	_file_name;
			std::wstring	_file_description;
			std::wstring	_file_version_string;
			ULARGE_INTEGER  _product_version;
			ULARGE_INTEGER  _file_version;
			FILETIME        _file_time;
			unsigned long   _file_flags;
			unsigned long   _file_os;
			unsigned long   _file_type;
			unsigned long   _file_subtype;
		};

		namespace Directory {

			typedef bool(*FILEFINDMATCHFUNC)(const WIN32_FIND_DATAW* fdw);
			Result DeleteFiles(const std::wstring& directory, std::vector<std::wstring>& failedList, bool recursively, FILEFINDMATCHFUNC Match);
			Result FindFiles(const std::wstring& directory, std::vector<std::wstring>& files, bool recursively, FILEFINDMATCHFUNC Match);

		}

	}	// NX::win

}   // NX

#endif
