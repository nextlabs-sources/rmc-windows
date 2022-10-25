

#ifndef __NXRMC_UPGRADE_H__
#define __NXRMC_UPGRADE_H__


#include <string>
#include <nx\common\result.h>

namespace NX {

	class RmUpgrade;

	class RmUpgradeInfo
	{
	public:
		RmUpgradeInfo();
		RmUpgradeInfo(const std::wstring& version, const std::wstring& downloadUrl, const std::wstring& checksum, const std::wstring& downloadedFilePath = std::wstring());
		RmUpgradeInfo(const RmUpgradeInfo& rhs);
		~RmUpgradeInfo();

		RmUpgradeInfo& operator = (const RmUpgradeInfo& rhs);

		std::string Serialize() const;
		Result Load(const std::string& s);
		ULONG VersionToNumber() const;
		void Clear();

		inline bool Empty() const { return (_version.empty() || _downloadUrl.empty() || _checksum.empty()); }
		inline const std::wstring& GetVersion() const { return _version; }
		inline const std::wstring& GetDownloadUrl() const { return _downloadUrl; }
		inline const std::wstring& GetChecksum() const { return _checksum; }
		inline const std::wstring& GetDownloadedFile() const { return _downloadedFile; }

	private:
		std::wstring	_version;
		std::wstring	_downloadUrl;
		std::wstring	_downloadedFile;
		std::wstring	_checksum;
		friend class RmUpgrade;
	};

	class RmUpgrade
	{
	public:
		RmUpgrade();
		RmUpgrade(const RmUpgradeInfo& existingInfo);
		~RmUpgrade();

		Result DownloadNewVersion();
		Result InstallNewVersion();

		inline const RmUpgradeInfo& GetNewVersionInfo() const { return _info; }

		static const std::wstring& GetUpgradeDir();
		static const std::wstring& GetNewVersionInfoFilePath();

	protected:
		Result VerifySha1Checksum(const std::wstring& file, const std::wstring& expectedChecksum);
		Result VerifyPublisher(const std::wstring& file, const std::wstring& publisher);
		Result UnpackCabFile(const std::wstring& file, std::wstring& dir);

		Result InstallMSI(const std::wstring& msiFile, bool quiet = false, bool noRestart = false, const std::wstring& logFile = std::wstring());
		Result InstallEXE(const std::wstring& exeFile);

		Result PrepareDirs();

	private:
		RmUpgradeInfo	_info;
	};

}


#endif
