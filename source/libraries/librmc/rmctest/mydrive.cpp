
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

extern ULONGLONG StringRightsToUll(const std::wstring& s);

void MyDriveShowUsage();
static void MyDriveSearchFile(const std::wstring& path, const std::wstring& keywords, bool recursively = false);
static void MyDriveDownloadFile(const std::wstring& path, const std::wstring& download_path);

std::wstring CombinePath(const std::wstring& current, const std::wstring& subpath)
{
	std::wstring newPath(current);
	std::vector<std::wstring> vFolders;
	if (subpath.empty())
		return newPath;
	if (subpath[0] == L'/')
		return subpath;

	vFolders = NX::split<wchar_t, L'/'>(subpath);

	std::for_each(vFolders.begin(), vFolders.end(), [&](const std::wstring& folder) {
		if (0 == _wcsicmp(L".", folder.c_str())) {
			return;
		}
		else if (0 == _wcsicmp(L"..", folder.c_str())) {
			if (0 != _wcsicmp(L"/", newPath.c_str())) {
				NX::remove_tail<wchar_t, L'/'>(newPath);
				if (newPath.empty())
					newPath = L"/";
			}
		}
		else {
			if (newPath[newPath.length() - 1] != L'/')
				newPath.append(L"/");
			newPath.append(folder);
		}
	});

	return std::move(newPath);
}

template <typename T, char S>
std::basic_string<T> CombinePath(const std::basic_string<T>& current, const std::basic_string<T>& subpath)
{
	std::wstring newPath(current);
	std::vector<std::wstring> vFolders;
	if (subpath.empty())
		return current;
	if (subpath[0] == L'/' || subpath[0] == L'\\')
		return subpath;

	std::wstring::size_type pos = 0;
	do {
		std::wstring::size_type pos_end = subpath.find_first_of(L"/\\", pos);
		if (pos_end != std::wstring::npos) {
			vFolders.push_back(subpath.substr(pos, pos_end - pos));
		}
		else {
			vFolders.push_back(subpath.substr(pos));
		}
	} while (pos != std::wstring::npos);

	std::for_each(vFolders.begin(), vFolders.end(), [&](const std::wstring& folder) {
		if (0 == _wcsicmp(L".", folder.c_str())) {
			return;
		}
		else if (0 == _wcsicmp(L"..", folder.c_str())) {
			if (0 != _wcsicmp(L"/", newPath.c_str()) && 0 != _wcsicmp(L"\\", newPath.c_str())) {
				auto pos = newPath.rfind(L'/');
			}
		}
		else {
		}
	});
}


void MyDriveCommandProcessor()
{
	std::wstring currentPath(L"/");

	auto pos = std::find_if(gpRepos.cbegin(), gpRepos.cend(), [](const NX::RmRepository& repo) -> bool {return (NX::RmRepository::MYDRIVE == repo.GetType()); });
	if (pos == gpRepos.cend()) {
		std::cout << "Cannot find my drive" << std::endl;
		return;
	}

	NX::RmRepository RepoMyDrive(*pos);
	NX::Result res = RESULT(0);

	do {

		const std::wstring& folderName = currentPath.substr(currentPath.rfind('/') + 1);

		std::wcout << gpSession->GetCurrentUser().GetName() << L"/MyDrive/" << folderName << "> ";

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
			std::cout << "MyDrive Commands" << std::endl;
			std::cout << "  commands        - List all available commands for MyDrive" << std::endl;
			std::cout << "  exit            - Go back to home command space" << std::endl;
			std::cout << "  usage           - Show MyDrive usage" << std::endl;
			std::cout << "  pwd             - Show current path" << std::endl;
			std::cout << "  cd <path>       - Change current path" << std::endl;
			std::cout << "  dir             - List files under current folder" << std::endl;
			std::cout << "  find <keywords> - Search files under current folder" << std::endl;
			std::cout << "  mkdir <name>    - Create folder" << std::endl;
			std::cout << "  del <name>      - Delete file" << std::endl;
			std::cout << "  view <file>     - View file" << std::endl;
			std::cout << "  download <file> [target] - Download file" << std::endl;
			std::cout << "  upload <file> [target] - Upload file" << std::endl;
			std::cout << "  protect <pathId> - Protect file" << std::endl;
			std::cout << "  favorite - list favorite files" << std::endl;
			std::cout << "  favorite mark <pathId> - mark favorite file" << std::endl;
			std::cout << "  favorite unmark <pathId> - unmark favorite file" << std::endl;
			std::cout << "  offline - list offline files" << std::endl;
			std::cout << "  offline mark <pathId> - mark offline file" << std::endl;
			std::cout << "  offline unmark <pathId> - unmark offline file" << std::endl;
			std::cout << "  share <pathId> <RIGHTS> <RECIPIENTS> [RECIPIENTS] ... - Share file" << std::endl;
			std::cout << "      Valid rights: V(View), P(Print), S(Share), D(Download)" << std::endl;
			std::cout << "      Valid effects: W(Watermark)" << std::endl;
			std::cout << std::endl;
			continue;
		}

		if (0 == _wcsicmp(cmd.c_str(), L"usage"))
		{
			MyDriveShowUsage();
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"dir") || 0 == _wcsicmp(cmd.c_str(), L"ls"))
		{
			MyDriveSearchFile(currentPath, L"", false);
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"find"))
		{
			const std::wstring keywords((args.GetArgs().size() > 1) ? args.GetArgs()[1] : L"");
			bool recursively = (args.GetArgs().size() > 2 && 0 == _wcsicmp(args.GetArgs()[2].c_str(), L"-r"));
			MyDriveSearchFile(currentPath, keywords, recursively);
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"pwd"))
		{
			std::wcout << currentPath << std::endl;
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"cd"))
		{
			if (args.GetArgs().size() > 1) {
				const std::wstring& newPath = CombinePath(currentPath, args.GetArgs()[1]);
				NX::RmRepoFile folder;
				NX::Result r = gpSession->MyDriveGetFileInfo(newPath, folder);
				if (r && folder.IsFolder()) {
					currentPath = folder.GetPath();
					if (0 != _wcsicmp(currentPath.c_str(), L"/") && currentPath[currentPath.length() - 1] == L'/') {
						currentPath = currentPath.substr(0, currentPath.length() - 1);
					}
				}
			}
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"mkdir"))
		{
			if (args.GetArgs().size() < 2) {
				std::cout << "Empty folder name" << std::endl;
			}
			const std::wstring& newFolderName = args.GetArgs()[1];
			if (0 == _wcsicmp(L".", newFolderName.c_str()) || 0 == _wcsicmp(L"..", newFolderName.c_str())) {
				std::cout << "Invalid folder name" << std::endl;
			}

			NX::RmRepoFile newFolder;

			res = gpSession->MyDriveCreateFolder(currentPath, newFolderName, newFolder);
			if (res) {
				if (0 != _wcsicmp(newFolder.GetName().c_str(), newFolderName.c_str())) {
					std::wcout << L"Folder \"" << newFolder.GetName() << L"(Auto renamed)" << L"\" has been created" << std::endl;
				}
				else {
					std::wcout << L"Folder \"" << newFolder.GetName() << L"\" has been created" << std::endl;
				}
			}
			else {
				std::cout << "Fail to create folder: " << res.GetMsg() << std::endl;
			}
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"del") || 0 == _wcsicmp(cmd.c_str(), L"rm"))
		{
			if (args.GetArgs().size() < 2) {
				std::cout << "Empty target name" << std::endl;
			}
			const std::wstring fileName(args.GetArgs()[1]);
			if (0 == _wcsicmp(L".", fileName.c_str()) || 0 == _wcsicmp(L"..", fileName.c_str())) {
				std::cout << "Invalid target name" << std::endl;
			}
			const std::wstring& newPath = CombinePath(currentPath, fileName);

			res = gpSession->MyDriveDeleteFile(newPath, NULL);
			if (res) {
				std::wcout << L"File has been deletd" << std::endl;
			}
			else {
				std::wcout << L"Fail to delete file: " << newPath << std::endl;
			}
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"view"))
		{
			if (args.GetArgs().size() < 2) {
				std::cout << "Empty target name" << std::endl;
			}
			const std::wstring fileName(args.GetArgs()[1]);
			if (0 == _wcsicmp(L".", fileName.c_str()) || 0 == _wcsicmp(L"..", fileName.c_str())) {
				std::cout << "Invalid target name" << std::endl;
			}
			const std::wstring& newPath = CombinePath(currentPath, fileName);

			res = gpSession->ViewRepoFile(RepoMyDrive, newPath);
			if (res) {
				std::wcout << L"File has been opened in remote viewer" << std::endl;
			}
			else {
				std::wcout << L"Fail to view file: " << newPath << std::endl;
			}
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"download"))
		{
			if (args.GetArgs().size() < 2) {
				std::cout << "Empty source name" << std::endl;
			}
			const std::wstring fileName(args.GetArgs()[1]);
			if (0 == _wcsicmp(L".", fileName.c_str()) || 0 == _wcsicmp(L"..", fileName.c_str())) {
				std::cout << "Invalid source name" << std::endl;
			}
			std::wstring sourceName(currentPath);
			if (fileName[0] == L'/') {
				sourceName = fileName;
			}
			else {
				if (sourceName[sourceName.length() - 1] != L'/') {
					sourceName.append(L"/");
				}
				sourceName.append(fileName);
			}
			std::wstring targetName;
			if (args.GetArgs().size() >= 3) {
				targetName = args.GetArgs()[2];
			}
			else {
				std::wstring parentFolder;
				std::wstring namePart;
				namePart = NX::remove_tail<wchar_t, L'/'>(fileName, parentFolder);
				GetCurrentDirectoryW(MAX_PATH, NX::wstring_buffer(targetName, MAX_PATH));
				targetName.append(L"\\");
				targetName.append(namePart);
			}

			NX::RmRepoFile repoFile;
			res = gpSession->MyDriveGetFileInfo(sourceName, repoFile);
			if (!res) {
				if (res.GetCode() == ERROR_NOT_FOUND) {
					std::wcout << L"Source file not found (" << sourceName << L")" << std::endl;
				}
				else {
					std::cout << "Fail to get source file information (" << res.GetCode() << res.GetMsg() << ")" << std::endl;
				}
				continue;
			}

			if (repoFile.IsFolder()) {
				std::wcout << L"Cannot download a folder (" << sourceName << L")" << std::endl;
				continue;
			}

			res = gpSession->MyDriveDownloadFile(repoFile, targetName, NULL, 0);
			if (res) {
				std::wcout << L"File has been downloaded --> " << targetName << std::endl;
			}
			else {
				std::wcout << L"Fail to download file: " << sourceName << std::endl;
			}
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"upload"))
		{
			if (args.GetArgs().size() < 2) {
				std::cout << "Empty source name" << std::endl;
			}
			const std::wstring sourceName(args.GetArgs()[1]);
			if (0 == _wcsicmp(L".", sourceName.c_str()) || 0 == _wcsicmp(L"..", sourceName.c_str())) {
				std::cout << "Invalid source name" << std::endl;
			}
			std::wstring targetName;
			if (args.GetArgs().size() >= 3) {
				std::wstring inputPath(args.GetArgs()[2]);
				for (int i = 0; i < (int)inputPath.length(); i++)
					if (inputPath[i] == L'\\') inputPath[i] = L'/';
				if (L'/' == inputPath[0]) {
					targetName = inputPath;
				}
				else {
					targetName = currentPath;
					if (!targetName.empty() && L'/' != targetName[targetName.length() - 1])
						targetName.append(L"/");
					targetName.append(inputPath);
				}
			}
			else {
				const wchar_t* pSourceName = wcsrchr(sourceName.c_str(), L'\\');
				const std::wstring namePart(pSourceName ? (pSourceName + 1) : sourceName);
				targetName = currentPath;
				if (!targetName.empty() && L'/' != targetName[targetName.length() - 1])
					targetName.append(L"/");
				targetName.append(namePart);
			}

			NX::RmRepoFile repoFile;
			NX::RmMyDriveUsage newUsage;
			res = gpSession->MyDriveUploadFile(sourceName, targetName, repoFile, NULL, &newUsage);
			if (res) {
				std::wcout << L"File has been uploadeded" << std::endl;
				std::wcout << L"    - Name: " << repoFile.GetName() << std::endl;
				std::wcout << L"    - Id: " << repoFile.GetId() << std::endl;
				std::wcout << L"    - Path: " << repoFile.GetPath() << std::endl;
				std::wcout << L"    - Size: " << repoFile.GetSize() << std::endl;
				std::wcout << L"MyDrive Storage Usage" << std::endl;
				std::wcout << L"    - Total: " << newUsage.GetQuota() << std::endl;
				std::wcout << L"    - Used:  " << newUsage.GetUsedBytes() << std::endl;
				std::wcout << L"    - Used By MyVault:  " << newUsage.GetUsedBytesByMyVault() << std::endl;
			}
			else {
				std::wcout << L"Fail to upload file: " << sourceName << std::endl;
				std::cout << "Error:" << res.GetCode()
					<< ", file:" << res.GetFile()
					<< ", line:" << res.GetLine()
					<< ", function:" << res.GetFunction()
					<< ", msg:" << res.GetMsg()
					<< std::endl;
			}
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"protect"))
		{
			if (args.GetArgs().size() < 2) {
				std::cout << "Too less parameters" << std::endl;
			}
			const std::wstring& pathId = args.GetArgs()[1];
			const ULONGLONG ullRights = StringRightsToUll(L"V");

			if (NX::iend_with<wchar_t>(pathId, L".nxl")) {
				std::cout << "File is NXL file already!" << std::endl;
			}
			else {
				NX::RmRepoFile file;
				res = gpSession->MyDriveGetFileInfo(pathId, file);
				if (!res) {
					std::cout << "Fail to get repository file info:" << res.GetCode()
						<< ", file:" << res.GetFile()
						<< ", line:" << res.GetLine()
						<< ", function:" << res.GetFunction()
						<< ", msg:" << res.GetMsg()
						<< std::endl;
				}
				else {

					NX::RmFileShareResult sr;
					NX::time::datetime dt = NX::time::datetime::current_time();
					const NX::time::span& days = NX::time::span::days_span(180);
					dt.add(days);
					res = gpSession->RepoProtectFile(gpSession->GetCurrentUser().GetMemberships().at(0).GetId(),
						RepoMyDrive,
						file,
						ullRights,
						NX::NXL::FileMeta(),
						NX::NXL::FileTags(),
						sr,
						NULL);
					if (!res) {
						std::cout << "Fail to protect repository file:" << res.GetCode()
							<< ", file:" << res.GetFile()
							<< ", line:" << res.GetLine()
							<< ", function:" << res.GetFunction()
							<< ", msg:" << res.GetMsg()
							<< std::endl;
					}
					else {
						std::cout << "File is protected" << std::endl;
						std::cout << "Protected file information:" << std::endl;
						std::wcout << L"    - Duid: " << sr.GetDuid() << std::endl;
						std::wcout << L"    - Path: " << sr.GetPath() << std::endl;
					}
				}
			}

		}
		else if (0 == _wcsicmp(cmd.c_str(), L"share"))
		{
			// share <file> <rights> <recipient0> [recipient1] ...
			if (args.GetArgs().size() < 4) {
				std::cout << "Too less parameters" << std::endl;
			}
			const std::wstring& pathId = args.GetArgs()[1];
			const std::wstring& sRights = args.GetArgs()[2];
			const ULONGLONG ullRights = StringRightsToUll(sRights);
			std::vector<std::wstring> recipients;
			for (int i = 3; i < (int)args.GetArgs().size(); i++)
				recipients.push_back(args.GetArgs()[i]);
			NX::RmRepoFile file;
			res = gpSession->MyDriveGetFileInfo(pathId, file);
			if (!res) {
				std::cout << "Fail to get repository file info:" << res.GetCode()
					<< ", file:" << res.GetFile()
					<< ", line:" << res.GetLine()
					<< ", function:" << res.GetFunction()
					<< ", msg:" << res.GetMsg()
					<< std::endl;
			}
			else {
				NX::RmFileShareResult sr;
				NX::time::datetime dt = NX::time::datetime::current_time();
				const NX::time::span& days = NX::time::span::days_span(180);
				dt.add(days);
				res = gpSession->RepoShareFile(gpSession->GetCurrentUser().GetMemberships().at(0).GetId(),
					RepoMyDrive,
					file,
					ullRights,
					NX::NXL::FileMeta(),
					NX::NXL::FileTags(),
					recipients,
					dt.to_java_time(),
					sr
				);

				if (!res) {
					std::cout << "Fail to share repository file:" << res.GetCode()
						<< ", file:" << res.GetFile()
						<< ", line:" << res.GetLine()
						<< ", function:" << res.GetFunction()
						<< ", msg:" << res.GetMsg()
						<< std::endl;
				}
				else {
					std::cout << "File is shared, and an email notification has been sent to recipients:" << std::endl;
					std::for_each(recipients.cbegin(), recipients.cend(), [](const std::wstring& r) {
						std::wcout << L"    - " << r << std::endl;
					});
					std::cout << "Shared file information:" << std::endl;
					std::wcout << L"    - Duid: " << sr.GetDuid() << std::endl;
					std::wcout << L"    - Path: " << sr.GetPath() << std::endl;
					std::wcout << L"    - TransactionId: " << sr.GetTransactionId() << std::endl;
				}
			}
		}
		else
		{
			std::cout << "Unknown command" << std::endl;
		}

	} while (TRUE);
}

void MyDriveShowUsage()
{
	NX::RmMyDriveUsage usage;
	NX::Result res = gpSession->MyDriveGetUsage(usage);
	if (!res) {
		std::cout << "Fail to get MyDrive usage (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::cout << "MyDrive Status" << std::endl;
	std::cout << "  - Total:   " << usage.GetQuota() << " Bytes" << std::endl;
	std::cout << "  - Free:   " << usage.GetFreeBytes() << " Bytes" << std::endl;
	std::cout << "  - Used:    " << usage.GetUsedBytes() << " Bytes" << std::endl;
	std::cout << "  - MyVault: " << usage.GetUsedBytesByMyVault() << " Bytes" << std::endl;
	std::cout << std::endl;
}

void MyDriveSearchFile(const std::wstring& path, const std::wstring& keywords, bool recursively)
{
	std::vector<NX::RmRepoFile> files;
	if (keywords.empty()) {
		gpSession->MyDriveListFiles(path, NULL, files);
	}
	else {
		gpSession->MyDriveSearchFiles(path, NULL, keywords, recursively, files);
	}

	std::cout << std::endl;
	std::for_each(files.cbegin(), files.cend(), [&](const NX::RmRepoFile& file) {

		std::wcout << std::endl;
		std::wcout << L"  " << file.GetName() << std::endl;
		std::wcout << L"      Id: " << file.GetId() << std::endl;
		std::wcout << L"      Is Folder: " << (file.IsFolder() ? L"True" : L"False") << std::endl;
		if (file.IsFolder()) {
			std::wcout << L"      Size: N/A" << std::endl;
			std::wcout << L"      Last Modified Time: N/A" << std::endl;
		}
		else {
			NX::time::datetime dt = NX::time::datetime::from_unix_time(file.GetLastModifiedTime());
			std::wcout << L"      Size: " << NX::i64tos<WCHAR>(file.GetSize()) << L" Bytes" << std::endl;
			std::wcout << L"      Last Modified Time: " << dt.serialize(true, false) << std::endl;
		}
	});
}

void MyDriveDownloadFile(const std::wstring& path, const std::wstring& download_path)
{
}
