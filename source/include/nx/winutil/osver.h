

#ifndef __NX_WINUTIL_OSVER_H__
#define __NX_WINUTIL_OSVER_H__

#include <nx\common\result.h>

namespace NX {

	namespace win {

		typedef enum WINPLATFORMID {
			PLATFORM_WINDOWS = 0,
			PLATFORM_WIN2000,
			PLATFORM_WINXP,
			PLATFORM_WINVISTA,
			PLATFORM_WIN7,
			PLATFORM_WIN8,
			PLATFORM_WIN8_1,
			PLATFORM_WIN10,
			PLATFORM_WINDOWS_SERVER = 100,
			PLATFORM_WINDOWS_SERVER_2003,
			PLATFORM_WINDOWS_SERVER_2003_R2,
			PLATFORM_WINDOWS_SERVER_2008,
			PLATFORM_WINDOWS_SERVER_2008_R2,
			PLATFORM_WINDOWS_SERVER_2012,
			PLATFORM_WINDOWS_SERVER_2012_R2,
			PLATFORM_WINDOWS_SERVER_2016
		} WINPLATFORMID;

		class OsVersion
		{
		public:
			OsVersion(const OsVersion& rhs);
			virtual ~OsVersion();

			OsVersion& operator = (const OsVersion& rhs);

			static OsVersion QueryOsVersion();

			inline unsigned long GetCpuArch() const { return _cpuArch; }
			inline unsigned long GetBuildNumber() const { return _buildNumber; }
			inline unsigned long GetPlatformId() const { return _platformId; }
			inline const std::wstring& GetName() const { return _name; }
			inline const std::wstring& GetVersionString() const { return _verStr; }
			inline const std::wstring& GetEdition() const { return _edition; }
			inline const std::wstring& GetWindowsDir() const { return _winDir; }

			inline bool IsServer() const { return _isServer; }
			inline bool IsArm() const { return (PROCESSOR_ARCHITECTURE_ARM == _cpuArch); };
			inline bool IsX86() const { return (PROCESSOR_ARCHITECTURE_INTEL == _cpuArch); };
			inline bool IsX64() const { return (PROCESSOR_ARCHITECTURE_AMD64 == _cpuArch); };
			inline bool IsIA64() const { return (PROCESSOR_ARCHITECTURE_IA64 == _cpuArch); };

			static bool IsXpOrGreater();
			static bool IsXpSp1OrGreater();
			static bool IsXpSp2OrGreater();
			static bool IsXpSp3OrGreater();
			static bool IsVistaOrGreater();
			static bool IsVistaSp1OrGreater();
			static bool IsVistaSp2OrGreater();
			static bool IsWin7OrGreater();
			static bool IsWin7Sp1OrGreater();
			static bool IsWin8OrGreater();
			static bool IsWin81OrGreater();
			static bool IsWin10OrGreater();

			static bool IsWinsrv03OrGreater();
			static bool IsWinsrv03_sp1OrGreater();
			static bool IsWinsrv03_sp2OrGreater();
			static bool IsWinsrv03_sp3OrGreater();
			static bool IsWinsrv03r2OrGreater();
			static bool IsWinsrv03r2_sp1OrGreater();
			static bool IsWinsrv03r2_sp2OrGreater();
			static bool IsWinsrv03r2_sp3OrGreater();
			static bool IsWinsrv08OrGreater();
			static bool IsWinsrv08_sp1OrGreater();
			static bool IsWinsrv08_sp2OrGreater();
			static bool IsWinsrv08_sp3OrGreater();
			static bool IsWinsrv08r2OrGreater();
			static bool IsWinsrv08r2_sp1OrGreater();
			static bool IsWinsrv12OrGreater();
			static bool IsWinsrv12r2OrGreater();
			static bool IsWinsrv16OrGreater();

		protected:
			OsVersion();

		private:
			bool            _isServer;
			unsigned long   _cpuArch;
			unsigned long   _buildNumber;
			unsigned long   _platformId;
			std::wstring    _name;
			std::wstring    _edition;
			std::wstring    _winDir;
			std::wstring    _verStr;
		};

		// Since OS Information won't change after system start
		// We keep a global value here for optmization
		const OsVersion& GetOsVersion();

		class IEVersion
		{
		public:
			IEVersion();
			~IEVersion() {}

			inline bool empty() const { return (0 == _major); }
			inline int GetMajor() const { return _major; }
			inline int GetMinor() const { return _minor; }
			inline int GetBuild() const { return _build; }

		private:
			int _major;
			int _minor;
			int _build;
		};

		class IEAppEmulation
		{
		public:
			IEAppEmulation(const std::wstring& appName) {}
			~IEAppEmulation() {}

			typedef enum {
				IE7Default = 7000,
				IE8Default = 8000,
				IE8Standard = 8888,
				IE9Default = 9000,
				IE9Standard = 9999,
				IE10Default = 10000,
				IE10Standard = 10001,
				IE11Default = 11000,
				IE11EdgeMode = 11001
			} IEEMULATECODE;

			IEEMULATECODE ReadEmulationCode();
			Result WriteEmulationCode(IEEMULATECODE code);

		protected:
			IEEMULATECODE ConvertCode(int code);
			
		private:
			std::wstring	_appName;
			IEEMULATECODE	_emulateCode;
		};

	}	// NX::win

}   // NX

#endif
