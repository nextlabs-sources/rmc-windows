
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

extern std::wstring CombinePath(const std::wstring& current, const std::wstring& subpath);

static void RepoCommandProcessor(const std::wstring& id);
static void RepoListAll(NX::RmRepository& repo, const std::wstring& path);
static void RepoCreateFolder(NX::RmRepository& repo, const std::wstring& parentDir, const std::wstring& folderName);
static void RepoDeleteFolder(NX::RmRepository& repo, const std::wstring& parentDir, const std::wstring& folderName);
static void RepoGetFileInfo(NX::RmRepository& repo, const std::wstring& path);
static void RepoDeleteFile(NX::RmRepository& repo, const std::wstring& path);
static void RepoDownloadFile(NX::RmRepository& repo, const std::wstring& source, const std::wstring& target);
static void RepoShowInfo(const std::wstring& id);
static void RepoGetAuthUrl(const std::wstring& name, const std::wstring& type, const std::wstring& siteUrl);
static void RepoRename(const std::wstring& id, const std::wstring& name);
static void RepoRemove(const std::wstring& id);
static void RepoSearchFiles(NX::RmRepository& repo, const std::wstring& path, const std::wstring& keywords, int pagesize = 10);
static void RepoListAllMarkedFiles();
static void RepoListFavoriteFiles(NX::RmRepository& repo);
static void RepoAddFavoriteFiles(NX::RmRepository& repo, const std::wstring& id, const std::wstring& path);
static void RepoRemoveFavoriteFiles(NX::RmRepository& repo, const std::wstring& id, const std::wstring& path);
static void RepoListOfflineFiles(NX::RmRepository& repo);
static void RepoAddOfflineFiles(NX::RmRepository& repo, const std::wstring& id, const std::wstring& path);
static void RepoRemoveOfflineFiles(NX::RmRepository& repo, const std::wstring& id, const std::wstring& path);

static NX::RmRepository *FindRepoById(const std::wstring& id)
{
    for (auto& r : gpRepos) {
        if (r.GetId() == id) {
            return &r;
        }
    }

    return nullptr;
}

static void PrintFileInfo(const NX::RmRepoFile& file)
{
    std::wcout << std::endl;
    std::wcout << L"  " << file.GetName() << std::endl;
    std::wcout << L"      Id: " << file.GetId() << std::endl;
    std::wcout << L"      Path: " << file.GetPath() << std::endl;
    std::wcout << L"      Is Folder: " << (file.IsFolder() ? L"True" : L"False") << std::endl;
    if (file.IsFolder()) {
        std::wcout << L"      Size: N/A" << std::endl;
        std::wcout << L"      Last Modified Time: N/A" << std::endl;
    }
    else {
        NX::time::datetime dt = NX::time::datetime::from_unix_time(file.GetLastModifiedTime());
        std::wcout << L"      Size: " << NX::i64tos<WCHAR>(file.GetSize()) << L"Bytes" << std::endl;
        std::wcout << L"      Last Modified Time: " << dt.serialize(true, false) << std::endl;
    }
}

void ReposCommandProcessor()
{
	do {

		std::wcout << gpSession->GetCurrentUser().GetName() << L"/Repos> ";

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
			std::cout << "Repositories Commands" << std::endl;
			std::cout << "  commands               - List all available commands for Repositories" << std::endl;
			std::cout << "  exit                   - Go back to home command space" << std::endl;
			std::cout << "  list                   - List all the repositories" << std::endl;
			std::cout << "  getauthurl <name> <type> [site-url] - Get add-repository auth URL" << std::endl;
			std::cout << "  rename <id> <new-name> - Rename a repository" << std::endl;
			std::cout << "  remove <id>            - Remove a repository" << std::endl;
			std::cout << "  select <id>            - Select a repository" << std::endl;
			std::cout << "  markedfiles            - List all marked files" << std::endl;
			std::cout << std::endl;
			continue;
		}

		if (0 == _wcsicmp(cmd.c_str(), L"list"))
		{
			const int total = (int)gpRepos.size();
			int i = 0;

			std::cout << "User Repositories" << std::endl;
			std::cout << "  Count: " << total << std::endl;
			std::for_each(gpRepos.cbegin(), gpRepos.cend(), [&](const NX::RmRepository& repo) {
				NX::time::datetime dt = NX::time::datetime::from_unix_time(repo.GetCreationTime());
				std::cout << std::endl;
				std::wcout << L"  Repo " << i++ << L": " << repo.GetName() << std::endl;
				std::wcout << L"      Id:           " << repo.GetId() << std::endl;
				std::wcout << L"      Type:         " << repo.GetTypeName() << std::endl;
				std::wcout << L"      Shared:       " << (repo.IsShared() ? L"True" : L"False") << std::endl;
				std::wcout << L"      Account Name: " << repo.GetAccountName() << std::endl;
				std::wcout << L"      Account Id:   " << repo.GetAccountId() << std::endl;
				std::wcout << L"      Token:        " << repo.GetToken() << std::endl;
				std::wcout << L"      Create Time:  " << dt.serialize(true, false) << std::endl;
			});
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"getauthurl"))
		{
			if (args.GetArgs().size() < 2)
			{
				std::cout << "Missing repo name" << std::endl;
				continue;
			}
			const std::wstring& name = args.GetArgs()[1];
			if (args.GetArgs().size() < 3)
			{
				std::cout << "Missing repo type" << std::endl;
				continue;
			}
			const std::wstring& type = args.GetArgs()[2];
			std::wstring siteUrl;
			if (type == L"SHAREPOINT_ONLINE")
			{
				if (args.GetArgs().size() < 4)
				{
					std::cout << "Missing site URL" << std::endl;
					continue;
				}
				siteUrl = args.GetArgs()[3];
			}
			RepoGetAuthUrl(name, type, siteUrl);
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"rename"))
		{
			if (args.GetArgs().size() < 2)
			{
				std::cout << "Missing ID" << std::endl;
				continue;
			}
			const std::wstring& id = args.GetArgs()[1];
			if (args.GetArgs().size() < 3)
			{
				std::cout << "Missing new name" << std::endl;
				continue;
			}
			const std::wstring& newName = args.GetArgs()[2];
			RepoRename(id, newName);
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"remove"))
		{
			if (args.GetArgs().size() < 2)
			{
				std::cout << "Missing ID" << std::endl;
			}
			else
			{
				const std::wstring& id = args.GetArgs()[1];
				RepoRemove(id);
				std::cout << std::endl;
			}
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"select"))
		{
			if (args.GetArgs().size() < 2)
			{
				std::cout << "Missing ID" << std::endl;
			}
			else
			{
				const std::wstring& id = args.GetArgs()[1];
				RepoCommandProcessor(id);
				std::cout << std::endl;
			}
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"markedfiles"))
		{
			RepoListAllMarkedFiles();
		}
		else
		{
			std::cout << "Unknown command" << std::endl;
		}

	} while (TRUE);
}

void RepoCommandProcessor(const std::wstring& id)
{
	NX::RmRepository *repo = FindRepoById(id);
	if (!repo) {
		std::wcout << L"No such repository: " << id << std::endl;
		return;
	}

	std::wstring currentPath(L"/");

	do {
		const std::wstring& folderName = currentPath.substr(currentPath.rfind('/') + 1);

		if (folderName.empty())
			std::wcout << gpSession->GetCurrentUser().GetName() << L"/Repos/" << id << "> ";
		else
			std::wcout << gpSession->GetCurrentUser().GetName() << L"/Repos/" << id << L"/" << folderName << "> ";

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
			std::cout << "Repository Commands" << std::endl;
			std::cout << "  commands                 - List all available commands for Repository" << std::endl;
			std::cout << "  exit                     - Go back to Repositories command space" << std::endl;
			std::cout << "  pwd                      - Show current path" << std::endl;
			std::cout << "  mkdir <name>             - Create folder" << std::endl;
			std::cout << "  rmdir <name>             - Delete folder" << std::endl;
			std::cout << "  cd <path>                - Change current path" << std::endl;
			std::cout << "  dir                      - List files under current folder" << std::endl;
			std::cout << "  find <keywords>          - Search files under current folder" << std::endl;
			std::cout << "  info <file>              - Get file info" << std::endl;
			std::cout << "  del <file>               - Delete file" << std::endl;
			std::cout << "  download <file> [target] - Download file" << std::endl;
			std::cout << "  upload <file> [target]   - Upload file" << std::endl;
			std::cout << "  favorite list            - List favorite files" << std::endl;
			std::cout << "  favorite add             - Add favorite files" << std::endl;
			std::cout << "  favorite remove          - Add favorite files" << std::endl;
			std::cout << "  offline list             - List favorite files" << std::endl;
			std::cout << "  offline add              - Add favorite files" << std::endl;
			std::cout << "  offline remove           - Add favorite files" << std::endl;
			std::cout << std::endl;
			continue;
		}

		if (0 == _wcsicmp(cmd.c_str(), L"dir") || 0 == _wcsicmp(cmd.c_str(), L"ls"))
		{
			RepoListAll(*repo, currentPath);
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"find"))
		{
			const std::wstring keywords((args.GetArgs().size() > 1) ? args.GetArgs()[1] : L"");
			RepoSearchFiles(*repo, currentPath, keywords);
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"pwd"))
		{
			std::wcout << currentPath << std::endl;
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"mkdir"))
		{
			if (args.GetArgs().size() < 2) {
				std::cout << "Empty folder name" << std::endl;
				continue;
			}
			const std::wstring& folderName = args.GetArgs()[1];
			RepoCreateFolder(*repo, currentPath, folderName);
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"rmdir"))
		{
			if (args.GetArgs().size() < 2) {
				std::cout << "Empty folder name" << std::endl;
				continue;
			}
			const std::wstring& folderName = args.GetArgs()[1];
			RepoDeleteFolder(*repo, currentPath, folderName);
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"cd"))
		{
			if (args.GetArgs().size() > 1) {
#if 0
				const std::wstring& newPath = CombinePath(currentPath, args.GetArgs()[1]);
				bool isFolder = false;
				std::wstring displayPath;
				NX::Result r = gpSession->MyDriveCheckFileExistence(newPath, displayPath, isFolder);
				if (r && isFolder) {
					currentPath = displayPath;
					if (0 != _wcsicmp(currentPath.c_str(), L"/") && currentPath[currentPath.length() - 1] == L'/') {
						currentPath = currentPath.substr(0, currentPath.length() - 1);
					}
				}
#else
                currentPath = args.GetArgs()[1];
#endif
			}
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"info"))
		{
			if (args.GetArgs().size() < 2) {
				std::cout << "Empty file name" << std::endl;
				continue;
			}
			const std::wstring& fileName = args.GetArgs()[1];
			RepoGetFileInfo(*repo, fileName);
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"del"))
		{
			if (args.GetArgs().size() < 2) {
				std::cout << "Empty file name" << std::endl;
				continue;
			}
			const std::wstring& fileName = args.GetArgs()[1];
			RepoDeleteFile(*repo, fileName);
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"download"))
		{
			if (args.GetArgs().size() >= 3) {
				RepoDownloadFile(*repo, args.GetArgs()[1], args.GetArgs()[2]);
			}
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"upload"))
		{
			if (args.GetArgs().size() < 2) {
				std::cout << "Empty source name" << std::endl;
				continue;
			}
			const std::wstring sourceName(args.GetArgs()[1]);
			if (0 == _wcsicmp(L".", sourceName.c_str()) || 0 == _wcsicmp(L"..", sourceName.c_str())) {
				std::cout << "Invalid source name" << std::endl;
				continue;
			}
			std::wstring targetName;
			if (args.GetArgs().size() >= 3) {
				if (L'/' == args.GetArgs()[2][0]) {
					targetName = args.GetArgs()[2];
				}
				else {
					targetName = currentPath;
					targetName.append(L"/");
					targetName.append(args.GetArgs()[2]);
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
			NX::Result res = gpSession->RepoUploadFile(*repo, sourceName, targetName, repoFile, NULL);
			if (res) {
				std::wcout << L"File has been uploadeded" << std::endl;
				std::wcout << L"    - Name: " << repoFile.GetName() << std::endl;
				std::wcout << L"    - Id: " << repoFile.GetId() << std::endl;
				std::wcout << L"    - Path: " << repoFile.GetPath() << std::endl;
				std::wcout << L"    - Size: " << repoFile.GetSize() << std::endl;
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
		else if (0 == _wcsicmp(cmd.c_str(), L"favorite"))
		{
			const std::wstring& action = (args.GetArgs().size() > 1) ? args.GetArgs()[1] : std::wstring();

			if (0 == _wcsicmp(action.c_str(), L"add")) {
				// Add
				if (args.GetArgs().size() < 4) {
					std::wcout << L"Invalid parameter " << std::endl;
				}
				else {
					RepoAddFavoriteFiles(*repo, args.GetArgs()[2], args.GetArgs()[3]);
				}
			}
			else if (0 == _wcsicmp(action.c_str(), L"remove")) {
				// Remove
				if (args.GetArgs().size() < 4) {
					std::wcout << L"Invalid parameter " << std::endl;
				}
				else {
					RepoRemoveFavoriteFiles(*repo, args.GetArgs()[2], args.GetArgs()[3]);
				}
			}
			else {
				// List
				RepoListFavoriteFiles(*repo);
			}
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"offline"))
		{
			const std::wstring& action = (args.GetArgs().size() > 1) ? args.GetArgs()[1] : std::wstring();

			if (0 == _wcsicmp(action.c_str(), L"add")) {
				// Add
				if (args.GetArgs().size() < 4) {
					std::wcout << L"Invalid parameter " << std::endl;
				}
				else {
					RepoAddOfflineFiles(*repo, args.GetArgs()[2], args.GetArgs()[3]);
				}
			}
			else if (0 == _wcsicmp(action.c_str(), L"remove")) {
				// Remove
				if (args.GetArgs().size() < 4) {
					std::wcout << L"Invalid parameter " << std::endl;
				}
				else {
					RepoRemoveOfflineFiles(*repo, args.GetArgs()[2], args.GetArgs()[3]);
				}
			}
			else {
				// List
				RepoListOfflineFiles(*repo);
			}
		}
		else
		{
			std::cout << "Unknown command" << std::endl;
		}

	} while (TRUE);
}

void RepoListAll(NX::RmRepository& repo, const std::wstring& path)
{
	std::vector<NX::RmRepoFile> files;

	gpSession->RepoListFiles(repo, path, NULL, files);

	std::cout << std::endl;
	std::for_each(files.cbegin(), files.cend(), [&](const NX::RmRepoFile& file) {
		PrintFileInfo(file);
	});
}

void RepoCreateFolder(NX::RmRepository& repo, const std::wstring& parentDir, const std::wstring& folderName)
{
	NX::RmRepoFile folder;
	NX::Result res = gpSession->RepoCreateFolder(repo, parentDir, folderName, folder);
	if (!res) {
		std::cout << "Fail to create folder (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	PrintFileInfo(folder);
}

void RepoDeleteFolder(NX::RmRepository& repo, const std::wstring& parentDir, const std::wstring& folderName)
{
	NX::Result res = gpSession->RepoDeleteFolder(repo, CombinePath(parentDir, folderName));
	if (!res) {
		std::cout << "Fail to delete folder (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::cout << "Folder is deleted" << std::endl;
}

void RepoGetFileInfo(NX::RmRepository& repo, const std::wstring& path)
{
	NX::RmRepoFile file;
	NX::Result res = gpSession->RepoGetFileInfo(repo, path, file);
	if (!res) {
		std::cout << "Fail to get file info from Repo (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	PrintFileInfo(file);
}

void RepoDeleteFile(NX::RmRepository& repo, const std::wstring& path)
{
	NX::Result res = gpSession->RepoDeleteFile(repo, path);
	if (!res) {
		std::cout << "Fail to delete file from Repo (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::cout << "File is deleted" << std::endl;
}

void RepoDownloadFile(NX::RmRepository& repo, const std::wstring& source, const std::wstring& target)
{
	NX::Result res = gpSession->RepoDownloadFile(repo, source, target, NULL);
	if (!res) {
		std::cout << "Fail to download file from Repo (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::cout << "File is downloaded" << std::endl;
}

void RepoShowInfo(const std::wstring& id)
{
}

void RepoGetAuthUrl(const std::wstring& name, const std::wstring& type, const std::wstring& siteUrl)
{
	std::wstring url;
	NX::Result res = gpSession->RepoGetAuthUrl(name, NX::RmRepository::StringToType(type), siteUrl, url);
	if (!res) {
		std::cout << "Fail to get add-repository auth url (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::wcout << "Add-repository URL is " << url << std::endl;
}

void RepoRename(const std::wstring& id, const std::wstring& name)
{
	NX::RmRepository *repo = FindRepoById(id);
	if (!repo) {
		std::wcout << L"No such repository: " << id << std::endl;
		return;
	}

	NX::Result res = gpSession->RepoRename(*repo, name);
	if (!res) {
		std::cout << "Fail to rename Repo (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::cout << "Repo is renamed" << std::endl;
    gpRepos.clear();
	res = gpSession->RepoGetReposListFromRemote(gpRepos);
}

void RepoRemove(const std::wstring& id)
{
	NX::RmRepository *repo = FindRepoById(id);
	if (!repo) {
		std::wcout << L"No such repository: " << id << std::endl;
		return;
	}

	NX::Result res = gpSession->RepoRemove(*repo);
	if (!res) {
		std::cout << "Fail to remove Repo (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::cout << "Repo is removed" << std::endl;
    gpRepos.clear();
	res = gpSession->RepoGetReposListFromRemote(gpRepos);
}

void RepoSearchFiles(NX::RmRepository& repo, const std::wstring& path, const std::wstring& keywords, int pagesize)
{
	std::vector<NX::RmRepoFile> files;

	gpSession->RepoSearchFiles(repo, path, keywords, NULL, files);

	std::cout << std::endl;
	std::for_each(files.cbegin(), files.cend(), [&](const NX::RmRepoFile& file) {
		PrintFileInfo(file);
	});
}

void RepoListAllMarkedFiles()
{
	std::vector<std::pair<std::wstring, std::vector<NX::RmRepoFile>>> favorites;
	std::vector<std::pair<std::wstring, std::vector<NX::RmRepoFile>>> offlines;

	NX::Result res = gpSession->RepoGetFavoriteFilesFromRemote(favorites);
	if (!res) {
		res = gpSession->RepoGetFavoriteFilesFromCache(favorites);
		if (!res) {
			std::cout << "Fail to list Repo favorite files (err:" << res.GetCode()
				<< ", file:" << res.GetFile()
				<< ", line:" << res.GetLine()
				<< ", function:" << res.GetFunction()
				<< ", msg:" << res.GetMsg()
				<< ")" << std::endl;
			return;
		}
	}

	std::cout << std::endl;
	std::cout << "MARKED FAVORITE FILES" << std::endl;

	std::for_each(favorites.begin(), favorites.end(), [](const std::pair<std::wstring, std::vector<NX::RmRepoFile>>& item) {

		std::wcout << std::endl;
		std::wcout << L"REPO: " << item.first << std::endl;

		std::for_each(item.second.begin(), item.second.end(), [](const NX::RmRepoFile& file) {
			std::cout << std::endl;
			std::wcout << L"    " << file.GetName() << L" (" << file.GetPath() << L")" << std::endl;
			if (file.IsFolder()) {
				std::wcout << L"      - Folder: Yes" << std::endl;
			}
			std::wcout << L"      - Size:   " << (int)file.GetSize() << std::endl;
			if (!file.IsFolder() && 0 != file.GetLastModifiedTime()) {
				NX::time::datetime dt = NX::time::datetime::from_unix_time(file.GetLastModifiedTime());
				const std::wstring& sTime = dt.serialize(true, false);
				std::wcout << L"      - Modified At:   " << sTime << std::endl;
			}
		});
	});

	res = gpSession->RepoGetOfflineFiles(offlines);
	if (!res) {
		std::cout << "Fail to list Repo offline files (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::cout << std::endl;
	std::cout << "MARKED OFFLINE FILES" << std::endl;

	std::for_each(offlines.begin(), offlines.end(), [](const std::pair<std::wstring, std::vector<NX::RmRepoFile>>& item) {

		std::wcout << std::endl;
		std::wcout << L"REPO: " << item.first << std::endl;

		std::for_each(item.second.begin(), item.second.end(), [](const NX::RmRepoFile& file) {
			std::cout << std::endl;
			std::wcout << L"    " << file.GetName() << L" (" << file.GetPath() << L")" << std::endl;
			std::wcout << L"      - Folder: " << (file.IsFolder() ? L"Yes" : L"No") << std::endl;
			std::wcout << L"      - Size:   " << (int)file.GetSize() << std::endl;
			if (!file.IsFolder() && 0 != file.GetLastModifiedTime()) {
				NX::time::datetime dt = NX::time::datetime::from_unix_time(file.GetLastModifiedTime());
				const std::wstring& sTime = dt.serialize(true, false);
				std::wcout << L"      - Modified At:   " << sTime << std::endl;
			}
		});
	});

	std::cout << std::endl;
}

void RepoListFavoriteFiles(NX::RmRepository& repo)
{
	std::vector<std::pair<std::wstring, std::vector<NX::RmRepoFile>>> favorites;
	NX::Result res =gpSession->RepoGetFavoriteFilesFromRemote(favorites);
	if (!res) {
		std::cout << "Fail to list Repo favorite files (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}


	std::cout << std::endl;
	std::cout << "MARKED FAVORITE FILES" << std::endl;

	std::for_each(favorites.begin(), favorites.end(), [](const std::pair<std::wstring, std::vector<NX::RmRepoFile>>& item) {

		std::wcout << std::endl;
		std::wcout << L"REPO: " << item.first << std::endl;

		std::for_each(item.second.begin(), item.second.end(), [](const NX::RmRepoFile& file) {
			std::cout << std::endl;
			std::wcout << L"    " << file.GetName() << L" (" << file.GetPath() << L")" << std::endl;
			if (file.IsFolder()) {
				std::wcout << L"      - Folder: Yes" << std::endl;
			}
			std::wcout << L"      - Size:   " << (int)file.GetSize() << std::endl;
			if (!file.IsFolder() && 0 != file.GetLastModifiedTime()) {
				NX::time::datetime dt = NX::time::datetime::from_unix_time(file.GetLastModifiedTime());
				const std::wstring& sTime = dt.serialize(true, false);
				std::wcout << L"      - Modified At:   " << sTime << std::endl;
			}
		});
	});
	std::cout << std::endl;
}

void RepoAddFavoriteFiles(NX::RmRepository& repo, const std::wstring& id, const std::wstring& path)
{
	NX::RmRepoFile file(id, path, L"", false, 0, 0);
	//NX::Result res = gpSession->RepoSetFavoriteFile(repo, file);
	//if (!res) {
	//	std::cout << "Fail to set Repo favorite files (err:" << res.GetCode()
	//		<< ", file:" << res.GetFile()
	//		<< ", line:" << res.GetLine()
	//		<< ", function:" << res.GetFunction()
	//		<< ", msg:" << res.GetMsg()
	//		<< ")" << std::endl;
	//	return;
	//}
	std::cout << "File has been marked as Favorite!" << std::endl;
}

void RepoRemoveFavoriteFiles(NX::RmRepository& repo, const std::wstring& id, const std::wstring& path)
{
	NX::RmRepoFile file(id, path, L"", false, 0, 0);
	NX::Result res = gpSession->RepoUnsetFavoriteFile(repo, file);
	if (!res) {
		std::cout << "Fail to unset Repo favorite files (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}
	std::cout << "Favorite file has been unmarked!" << std::endl;
}

void RepoListOfflineFiles(NX::RmRepository& repo)
{
	std::vector<std::pair<std::wstring, std::vector<NX::RmRepoFile>>> offlines;
	NX::Result res = gpSession->RepoGetOfflineFiles(offlines);
	if (!res) {
		std::cout << "Fail to list offline files (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}


	std::cout << std::endl;
	std::cout << "MARKED OFFLINE FILES" << std::endl;

	std::for_each(offlines.begin(), offlines.end(), [](const std::pair<std::wstring, std::vector<NX::RmRepoFile>>& item) {

		std::wcout << std::endl;
		std::wcout << L"REPO: " << item.first << std::endl;

		std::for_each(item.second.begin(), item.second.end(), [](const NX::RmRepoFile& file) {
			std::cout << std::endl;
			std::wcout << L"    " << file.GetName() << L" (" << file.GetPath() << L")" << std::endl;
			if (file.IsFolder()) {
				std::wcout << L"      - Folder: Yes" << std::endl;
			}
			std::wcout << L"      - Size:   " << (int)file.GetSize() << std::endl;
			if (!file.IsFolder() && 0 != file.GetLastModifiedTime()) {
				NX::time::datetime dt = NX::time::datetime::from_unix_time(file.GetLastModifiedTime());
				const std::wstring& sTime = dt.serialize(true, false);
				std::wcout << L"      - Modified At:   " << sTime << std::endl;
			}
		});
	});
	std::cout << std::endl;
}

void RepoAddOfflineFiles(NX::RmRepository& repo, const std::wstring& id, const std::wstring& path)
{
	//NX::RmRepoMarkedFile file(id, path);
	//std::wstring offlinePath;
	//__int64 fileTime = 0;

	//NX::Result res = gpSession->RepoFindOfflineFile(repo.GetId(), id, offlinePath, &fileTime);
	//if (res && !offlinePath.empty()) {
	//	std::cout << "File already exist, do you want to refresh it? [Y|N]: ";
	//	char c = getchar();
	//	if (c != 'y' && c != 'Y')
	//		return;
	//	::DeleteFileW(offlinePath.c_str());
	//}





	//NX::Result res = gpSession->RepoSetOfflineFile(repo, file);
	//if (!res) {
	//	std::cout << "Fail to set offline files (err:" << res.GetCode()
	//		<< ", file:" << res.GetFile()
	//		<< ", line:" << res.GetLine()
	//		<< ", function:" << res.GetFunction()
	//		<< ", msg:" << res.GetMsg()
	//		<< ")" << std::endl;
	//	return;
	//}
	//std::cout << "File has been marked as Offline!" << std::endl;
}

void RepoRemoveOfflineFiles(NX::RmRepository& repo, const std::wstring& id, const std::wstring& path)
{
	//NX::RmRepoMarkedFile file(id, path);
	//NX::Result res = gpSession->RepoUnsetOfflineFile(repo, file);
	//if (!res) {
	//	std::cout << "Fail to unset offline files (err:" << res.GetCode()
	//		<< ", file:" << res.GetFile()
	//		<< ", line:" << res.GetLine()
	//		<< ", function:" << res.GetFunction()
	//		<< ", msg:" << res.GetMsg()
	//		<< ")" << std::endl;
	//	return;
	//}
	//std::cout << "Offline file has been unmarked!" << std::endl;
}

