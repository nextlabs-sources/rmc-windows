
#include "stdafx.h"

#include <Shellapi.h>

#include <string>
#include <memory>
#include <algorithm>

#include <nx\rmc\session.h>
#include <nx\common\stringex.h>
#include <nx\common\time.h>
#include <nx\winutil\path.h>
#include <nx\nxl\nxlrights.h>

#include "rmctest.h"


extern std::shared_ptr<NX::RmSession> gpSession;
extern std::vector<NX::RmRepository> gpRepos;

extern void MyDriveShowUsage();

static void MyVaultListFiles(const std::wstring& keywords, int pagesize, const std::wstring& filter);
static void MyVaultDeleteFile(const std::wstring& duid, const std::wstring& path);
static void MyVaultGetFileMeta(const std::wstring& duid, const std::wstring& path);
static void MyVaultGetFileLog(const std::wstring& duid);
static void MyVaultDownloadFile(const std::wstring& source, const std::wstring& target);
static void MyVaultViewFile(const std::wstring& path);

void MyVaultCommandProcessor()
{
	std::wstring currentPath(L"/");

	do {

		const std::wstring& folderName = currentPath.substr(currentPath.rfind('/') + 1);

		std::wcout << gpSession->GetCurrentUser().GetName() << L"/MyVault/" << folderName << "> ";

		const std::wstring& cmdline = GetInput();
		if (cmdline.empty()) continue;


		const CmdArgs args(cmdline);
		if (args.GetArgs().empty()) continue;

		const std::wstring& cmd = args.GetArgs()[0];
		if (0 == _wcsicmp(cmd.c_str(), L"quit") || 0 == _wcsicmp(cmd.c_str(), L"exit")) {
			std::cout << std::endl;
			break;
		}

		if (0 == _wcsicmp(cmd.c_str(), L"help") || 0 == _wcsicmp(cmd.c_str(), L"commands"))
		{
			std::cout << "MyVault Commands" << std::endl;
			std::cout << "  commands       - List all available commands for MyVault" << std::endl;
			std::cout << "  exit           - Go back to home command space" << std::endl;
			std::cout << "  usage          - Show MyVault usage" << std::endl;
			std::cout << "  rm <duid> <filepath> - Delete a file from MyVault" << std::endl;
			std::cout << "  view <filename> - View a file in MyVault" << std::endl;
			std::cout << "  list [all|active|shared|protected] - List files" << std::endl;
			std::cout << "  find <keyword> - Find files matching keywords" << std::endl;
			std::cout << "  getmeta <duid> <filepath> - Get a file's meta data" << std::endl;
			std::cout << "  getlog <duid> - Get a file's activity log" << std::endl;
			std::cout << std::endl;
			continue;
		}

		if (0 == _wcsicmp(cmd.c_str(), L"usage"))
		{
			MyDriveShowUsage();
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"list"))
		{
			std::wstring filter = L"allFiles";
			if (args.GetArgs().size() >= 2) {
				const std::wstring& sf = args.GetArgs()[1];
				if (0 == _wcsicmp(sf.c_str(), L"all"))
					filter = L"allFiles";
				else if (0 == _wcsicmp(sf.c_str(), L"active"))
					filter = L"activeTransaction";
				else if (0 == _wcsicmp(sf.c_str(), L"shared"))
					filter = L"allShared";
				else if (0 == _wcsicmp(sf.c_str(), L"protected"))
					filter = L"protected";
				else
					filter = L"allFiles";
			}
			MyVaultListFiles(L"", 10, filter);
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"rm") || 0 == _wcsicmp(cmd.c_str(), L"del"))
		{
			if (args.GetArgs().size() >= 3) {
				MyVaultDeleteFile(args.GetArgs()[1], args.GetArgs()[2]);
			}
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"view"))
		{
			if (args.GetArgs().size() >= 2) {
				std::wstring tmppath;
				//gpSession->ViewMyVaultFile(args.GetArgs()[1], tmppath);
			}
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"getmeta"))
		{
			if (args.GetArgs().size() >= 3) {
				MyVaultGetFileMeta(args.GetArgs()[1], args.GetArgs()[2]);
			}
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"getlog"))
		{
			if (args.GetArgs().size() >= 2) {
				MyVaultGetFileLog(args.GetArgs()[1]);
			}
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"download"))
		{
			if (args.GetArgs().size() >= 3) {
				MyVaultDownloadFile(args.GetArgs()[1], args.GetArgs()[2]);
			}
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"find"))
		{
			// Show current user's status on home page
			if (args.GetArgs().size() >= 2) {
				MyVaultListFiles(args.GetArgs()[1], 10, L"allFiles");
			}
			else {
				MyVaultListFiles(L"", 10, L"allFiles");
			}
			std::cout << std::endl;
		}
		else
		{
			std::cout << "Unknown command" << std::endl;
		}

	} while (TRUE);
}

void MyVaultListFiles(const std::wstring& keywords, int pagesize, const std::wstring& filter)
{
	NX::RmFinder finder(pagesize, std::vector<int>({ NX::OrderByCreationTimeDescend, NX::OrderByFileNameAscend }), filter, L"fileName", keywords);
	std::vector<NX::RmMyVaultFile> files;
	NX::Result res = gpSession->MyVaultGetFirstPageFiles(finder, NULL, files);
	if (!res) {
		std::cout << "Fail to list MyVault files (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::cout << std::endl;
	std::cout << (int)finder.GetTotalRecords() << (finder.GetTotalRecords() < 2 ? " record" : " records") << " was found" << std::endl;

	while (!files.empty()) {

		std::for_each(files.begin(), files.end(), [](const NX::RmMyVaultFile& file) {
			NX::time::datetime sharedTime = NX::time::datetime::from_unix_time(file.GetSharedTime());
			std::cout << std::endl;
			std::wcout << file.GetName() << std::endl;
			std::wcout << L"  - DUID: " << file.GetDuid() << std::endl;
			std::wcout << L"  - Path: " << file.GetPath() << std::endl;
			std::wcout << L"  - Display Path: " << file.GetDisplayPath() << std::endl;
			std::wcout << L"  - Size: " << file.GetSize() << std::endl;
			std::wcout << L"  - Repo Id: " << file.GetRepoId() << std::endl;
			std::wcout << L"  - Shared with: " << file.GetSerializedSharedWith() << std::endl;
			std::wcout << L"  - Shared time: " << sharedTime.serialize(true, false) << std::endl;
			std::wcout << L"  - Is Shared: " << (file.IsShared() ? L"Yes" : L"No") << std::endl;
			std::wcout << L"  - Is Revoked: " << (file.IsRevoked() ? L"Yes" : L"No") << std::endl;
			std::wcout << L"  - Is Deleted: " << (file.IsDeleted() ? L"Yes" : L"No") << std::endl;
		});

		if (finder.NoMoreData())
			break;

		std::cout << "Press any key to continue ...";
		(void)getchar();

		res = gpSession->MyVaultGetNextPageFiles(finder, NULL, files);
		if (!res) {
			std::cout << "Fail to list MyVault files in next page (err:" << res.GetCode()
				<< ", file:" << res.GetFile()
				<< ", line:" << res.GetLine()
				<< ", function:" << res.GetFunction()
				<< ", msg:" << res.GetMsg()
				<< ")" << std::endl;
			return;
		}
	}
}

void MyVaultDeleteFile(const std::wstring& duid, const std::wstring& path)
{
	NX::Result res = gpSession->MyVaultDeleteFile(duid, path);
	if (!res) {
		std::cout << "Fail to list MyVault files (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::cout << "File is deleted" << std::endl;
}


void MyVaultViewFile(const std::wstring& path)
{
	std::wstring tmppath;
	/*NX::Result res = gpSession->ViewMyVaultFile(path, tmppath);
	if (!res) {
		std::cout << "Fail to view file in MyVault (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}*/

	std::cout << "File is opened in remote viewer" << std::endl;
}

void MyVaultDownloadFile(const std::wstring& source, const std::wstring& target)
{
	NX::Result res = gpSession->MyVaultDownloadFile(source, target, false, NULL);
	if (!res) {
		std::cout << "Fail to download file from MyVault (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::cout << "File is downloaded" << std::endl;
}

void MyVaultGetFileMeta(const std::wstring& duid, const std::wstring& path)
{
	NX::RmMyVaultFileMetadata metadata;
	NX::Result res = gpSession->MyVaultGetFileInfo(duid, path, metadata);
	if (!res) {
		std::cout << "Fail to get MyVault file's metadata (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::wcout << metadata.GetName() << std::endl;
	std::wcout << L"  - Link: " << metadata.GetLink() << std::endl;
	std::wcout << L"  - Recipients: " << std::endl;
	const std::vector<std::wstring>& recipients = metadata.GetRecipients();
	std::for_each(recipients.begin(), recipients.end(), [](const std::wstring& s) {
		std::wcout << L"                " << s << std::endl;
	});
	NX::time::datetime t1 = NX::time::datetime::from_unix_time(metadata.GetProtectedTime());
	std::wcout << L"  - Protected Time: " << t1.serialize(true) << std::endl;
	NX::time::datetime t2 = NX::time::datetime::from_unix_time(metadata.GetSharedTime());
	std::wcout << L"  - Shared Time: " << t2.serialize(true) << std::endl;
	const std::vector<std::wstring>& rights = metadata.GetRights();
	std::wcout << L"  - Rights: " << std::endl;
	std::for_each(rights.begin(), rights.end(), [](const std::wstring& s) {
		std::wcout << L"            " << s << std::endl;
	});
}

void MyVaultGetFileLog(const std::wstring& duid)
{
	std::vector<NX::RmFileActivityRecord> records;
	std::wstring fileName;
	NX::Result res = gpSession->FetchAllActivityLogs(fileName, records, duid);
	if (!res) {
		std::cout << "Fail to get MyVault file's activities (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::wcout << L"Activities for file \"" << fileName << L"\"" << std::endl;
	std::wcout << L"Total: " << records.size() << L" record" << (records.size() > 1 ? L"s" : L"") << std::endl;

	/*	
		inline const std::wstring& GetDeviceId() const { return _deviceId; }
		inline const std::wstring& GetDeviceType() const { return _deviceType; }
		inline const std::wstring& GetAccessResult() const { return _accessResult; }
		inline const std::wstring& GetAccountType() const { return _accountType; }
		inline __int64 GetAccessTime() const { return _accessTime; }
	*/
	for (size_t i = 0; i < records.size(); i++) {
		std::wstring accessTime(L"N/A");
		if (0 != records[i].GetAccessTime()) {
			NX::time::datetime dt = NX::time::datetime::from_unix_time(records[i].GetAccessTime());
			accessTime = dt.serialize(true, false);
		}
		std::wcout << std::endl;
		std::wcout << L"Record - " << i << std::endl;
		std::wcout << L"    AccessTime: " << accessTime << std::endl;
		std::wcout << L"    AccessBy: " << records[i].GetEmail() << std::endl;
		std::wcout << L"    Operation: " << records[i].GetOperation() << std::endl;
		std::wcout << L"    DeviceType: " << records[i].GetDeviceType() << std::endl;
		std::wcout << L"    DeviceId: " << records[i].GetDeviceId() << std::endl;
		std::wcout << L"    Result: " << records[i].GetAccessResult() << std::endl;
	}
	std::wcout << std::endl;
}