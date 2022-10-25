
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


static void ProjectCommandProcessor(const NX::RmProject& project);
static void ProjectListAll();
static void ProjectShowInfo(const std::wstring& id);
static void ProjectListMembers(const std::wstring& id);
static void ProjectSearchFile(const std::wstring& id, const std::wstring& path, const std::wstring& keywords, int pagesize = 10);
static void ProjectRemove(const std::wstring& id);
static void ProjectUploadFile(const NX::RmProject& project, const std::wstring& currentPath, const CmdArgs& args);
static void ProjectDownloadFile(const NX::RmProject& project, const std::wstring& currentPath, const std::wstring& target);
static void ProjectListPendingInvitations();
static std::wstring ProjectChangeDir(const NX::RmProject& project, const std::wstring& target);

void ProjectsCommandProcessor()
{
	do {

		std::wcout << gpSession->GetCurrentUser().GetName() << L"/Projects> ";
		NX::Result res = RESULT(0);

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
			std::cout << "Projects Commands" << std::endl;
			std::cout << "  commands       - List all available commands for Projects" << std::endl;
			std::cout << "  exit           - Go back to home command space" << std::endl;
			std::cout << "  list [all|me|others] - List projects" << std::endl;
			std::cout << "  select <id>    - Select a project" << std::endl;
			std::cout << "  listinvites    - List all pending invitations" << std::endl;
			std::cout << std::endl;
			continue;
		}

		if (0 == _wcsicmp(cmd.c_str(), L"list"))
		{
			NX::RmSession::PROJECT_OWNER_TYPE type = NX::RmSession::OwnedByMeAndOthers;
			std::vector<NX::RmProject> projects;

			if (args.GetArgs().size() >= 2) {
				if (0 == _wcsicmp(args.GetArgs()[1].c_str(), L"me"))
					type = NX::RmSession::OwnedByMe;
				else if (0 == _wcsicmp(args.GetArgs()[1].c_str(), L"others"))
					type = NX::RmSession::OwnedByOthers;
				else
					type = NX::RmSession::OwnedByMeAndOthers;
			}

			res = gpSession->ProjectListProjects(projects, NULL, type);
			if (!res) {
				std::cout << "Fail to list projects (err:" << res.GetCode()
					<< ", file:" << res.GetFile()
					<< ", line:" << res.GetLine()
					<< ", function:" << res.GetFunction()
					<< ", msg:" << res.GetMsg()
					<< ")" << std::endl;
			}
			else {
				std::for_each(projects.begin(), projects.end(), [](const NX::RmProject& project) {
					const NX::time::datetime& dt = NX::time::datetime::from_unix_time(project.GetCreationTime());
					std::wcout << std::endl;
					std::wcout << L"Project: " << project.GetDisplayName() << std::endl;
					std::wcout << L"    Id: " << project.GetId() << std::endl;
					std::wcout << L"    Name: " << project.GetName() << std::endl;
					std::wcout << L"    Description: " << project.GetDescription() << std::endl;
					std::wcout << L"    Created At: " << dt.serialize(true, false) << std::endl;
					std::wcout << L"    Members: " << project.GetMemberCount() << std::endl;
					std::wcout << L"    Files: " << project.GetFileCount() << std::endl;
				});
			}

			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"select"))
		{
			if (2 != args.GetArgs().size()) {
				std::cout << "Project is is not specified" << std::endl;
			}
			else {
				NX::RmProject selectedProject;
				res = gpSession->ProjectGetInfo(args.GetArgs()[1], selectedProject);
				if (!res) {
					std::cout << "Fail to select project (err:" << res.GetCode()
						<< ", file:" << res.GetFile()
						<< ", line:" << res.GetLine()
						<< ", function:" << res.GetFunction()
						<< ", msg:" << res.GetMsg()
						<< ")" << std::endl;
				}
				else {
					ProjectCommandProcessor(selectedProject);
				}
			}
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"listinvites"))
		{
			std::vector<NX::RmProjectInvitation> invitations;
			res = gpSession->ProjectGetUserPendingInvitations(invitations, NULL);
			if (!res) {
				std::cout << "Fail to list projects (err:" << res.GetCode()
					<< ", file:" << res.GetFile()
					<< ", line:" << res.GetLine()
					<< ", function:" << res.GetFunction()
					<< ", msg:" << res.GetMsg()
					<< ")" << std::endl;
			}
			else {
				std::for_each(invitations.begin(), invitations.end(), [](const NX::RmProjectInvitation& invitation) {
					const NX::time::datetime& dt = NX::time::datetime::from_unix_time(invitation.GetInviteTime());
					const NX::time::datetime& dtProject = NX::time::datetime::from_unix_time(invitation.GetProjecCreationTime());
					std::wcout << std::endl;
					std::wcout << invitation.GetInviterDisplayName() << L" invites you to join project " << invitation.GetProjectName() << std::endl;
					std::wcout << L"    Invite Time: " << dt.serialize(true, false) << std::endl;
					std::wcout << L"    Project Id: " << invitation.GetProjectId() << std::endl;
					std::wcout << L"    Project Name: " << invitation.GetProjectName() << std::endl;
					std::wcout << L"    Project Description: " << invitation.GetProjectDescription() << std::endl;
					std::wcout << L"    Project Created At: " << dtProject.serialize(true, false) << std::endl;
					std::wcout << L"    Invitation Code: " << invitation.GetCode() << std::endl;
					std::wcout << L"    Inviter's Email: " << invitation.GetInviterEmail() << std::endl;
					std::wcout << L"    Invitee's Email: " << invitation.GetInviteeEmail() << std::endl;
				});
			}
			std::cout << std::endl;
		}
		else
		{
			std::cout << "Unknown command" << std::endl;
		}

	} while (TRUE);
}

void ProjectCommandProcessor(const NX::RmProject& project)
{
	std::wstring currentPath(L"/");

	do {

		std::wcout << gpSession->GetCurrentUser().GetName() << L"/Projects/" << project.GetName() << currentPath << L"> ";

		const std::wstring& cmdline = GetInput();
		if (cmdline.empty()) continue;


		const CmdArgs args(cmdline);
		if (args.GetArgs().empty()) continue;

		NX::Result res = RESULT(0);

		const std::wstring& cmd = args.GetArgs()[0];
		if (0 == _wcsicmp(cmd.c_str(), L"quit") || 0 == _wcsicmp(cmd.c_str(), L"exit")) {
			std::cout << std::endl;
			break;
		}

		if (0 == _wcsicmp(cmd.c_str(), L"help") || 0 == _wcsicmp(cmd.c_str(), L"commands"))
		{
			std::cout << "Project Commands" << std::endl;
			std::cout << "  commands       - List all available commands for Project" << std::endl;
			std::cout << "  exit           - Go back to Projects command space" << std::endl;
			std::cout << "  info           - Show current project information" << std::endl;
			std::cout << "  listmembers   - Show all the members of current project" << std::endl;
			std::cout << "  pwd                      - Show current path" << std::endl;
			std::cout << "  cd <path>                - Change current path" << std::endl;
			std::cout << "  dir                      - List files under current folder" << std::endl;
			std::cout << "  listfiles                - List all files under current folder" << std::endl;
			std::cout << "  findfiles <keywords>     - Find all files under current folder" << std::endl;
			std::cout << "  delete <file>  - Delete file from project a file" << std::endl;
			std::cout << "  upload <source> <rights & effects> [-d target] [-t TagKey=TagValue] [-t TagKey=TagValue] ...    - Upload a file" << std::endl;
			std::cout << "      Valid rights: V(View), P(Print), S(Share), D(Download)" << std::endl;
			std::cout << "      Valid effects: W(Watermark)" << std::endl;
			std::cout << "  download <source> [target]  - Download a file" << std::endl;
			std::cout << std::endl;
			continue;
		}

		if (0 == _wcsicmp(cmd.c_str(), L"info"))
		{
			const NX::time::datetime& dt = NX::time::datetime::from_unix_time(project.GetCreationTime());
			std::wcout << L"Project: " << project.GetDisplayName() << std::endl;
			std::wcout << L"    Id: " << project.GetId() << std::endl;
			std::wcout << L"    Name: " << project.GetName() << std::endl;
			std::wcout << L"    Description: " << project.GetDescription() << std::endl;
			std::wcout << L"    Created At: " << dt.serialize(true, false) << std::endl;
			std::wcout << L"    Members: " << project.GetMemberCount() << std::endl;
			std::wcout << L"    Files: " << project.GetFileCount() << std::endl;
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"listmembers"))
		{
			std::wstring memberId;
			if (args.GetArgs().size() == 2 && 0 != _wcsicmp(args.GetArgs()[1].c_str(), L"all")) {
				memberId = args.GetArgs()[1];
			}
			if (memberId.empty()) {
				NX::RmFinder finder(100, std::vector<int>({}), L"", L"", L"");
				std::vector<NX::RmProjectMember> members;
				res = gpSession->ProjectGetFirstPageMembers(finder, project.GetId(), false, members);
				if (!res) {
					std::cout << "Fail to list 1st page of members (err:" << res.GetCode()
						<< ", file:" << res.GetFile()
						<< ", line:" << res.GetLine()
						<< ", function:" << res.GetFunction()
						<< ", msg:" << res.GetMsg()
						<< ")" << std::endl;
				}
				else {

					do {

						std::for_each(members.cbegin(), members.cend(), [](const NX::RmProjectMember& member) {
							const NX::time::datetime& dt = NX::time::datetime::from_unix_time(member.GetCreationTime());
							std::wcout << std::endl;
							std::wcout << L"Member: " << member.GetDisplayName() << L" (" << member.GetUserId() << L")" << std::endl;
							std::wcout << L"    Creation Time: " << dt.serialize(true, false) << std::endl;
							std::wcout << L"    Email: " << member.GetEmail() << std::endl;
							if(!member.GetInviterDisplayName().empty())
								std::wcout << L"    Invited By: " << member.GetInviterDisplayName() << L" (" << member.GetInviterEmail() << L")" << std::endl;
						});

					} while (res = gpSession->ProjectGetNextPageMembers(finder, project.GetId(), false, members));

				}
			}
			else {
				NX::RmProjectMember member;
				res = gpSession->ProjectGetMemberInfo(project.GetId(), memberId, member);
				if (!res) {
					std::cout << "Fail to get member information (err:" << res.GetCode()
						<< ", file:" << res.GetFile()
						<< ", line:" << res.GetLine()
						<< ", function:" << res.GetFunction()
						<< ", msg:" << res.GetMsg()
						<< ")" << std::endl;
				}
				else {
					const NX::time::datetime& dt = NX::time::datetime::from_unix_time(member.GetCreationTime());
					std::wcout << std::endl;
					std::wcout << L"Member: " << member.GetDisplayName() << L" (" << member.GetUserId() << L")" << std::endl;
					std::wcout << L"    Creation Time: " << dt.serialize(true, false) << std::endl;
					std::wcout << L"    Email: " << member.GetEmail() << std::endl;
					if (!member.GetInviterDisplayName().empty())
						std::wcout << L"    Invited By: " << member.GetInviterDisplayName() << L" (" << member.GetInviterEmail() << L")" << std::endl;
				}
			}
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"pwd"))
		{
			std::wcout << currentPath << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"mkdir"))
		{
			if (args.GetArgs().size() < 2) {
				std::cout << "Invalid parameter" << std::endl;
			}
			else {
				NX::RmRepoFile newFolder;
				res = gpSession->ProjectCreateFolder(project.GetId(), currentPath, args.GetArgs()[1], true, newFolder);
				if (!res) {
					std::cout << "Fail to create new folder (err:" << res.GetCode()
						<< ", file:" << res.GetFile()
						<< ", line:" << res.GetLine()
						<< ", function:" << res.GetFunction()
						<< ", msg:" << res.GetMsg()
						<< ")" << std::endl;
				}
				else {
					std::wcout << L"Folder has been created" << std::endl;
					std::wcout << L"  - Path:" << newFolder.GetPath() << std::endl;
				}
			}
			std::wcout << currentPath << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"cd"))
		{
			if (args.GetArgs().size() >= 2) {
				std::wstring path = args.GetArgs()[1];
				if (path == L".") {
					;// Nothing
				}
				else if (path == L"..") {
					if (currentPath != L"/") {
						auto pos = currentPath.rfind(L'/');
						assert(pos != std::wstring::npos);
						currentPath = currentPath.substr(0, pos);
						if (currentPath.empty())
							currentPath = L"/";
					}
				}
				else {

					if (path[0] != L'/') {
						std::wstring tmp(currentPath);
						if (!NX::iend_with<wchar_t>(tmp, L"/"))
							tmp.append(L"/");
						path = std::wstring(tmp + path);
					}

					const std::wstring& newPath = ProjectChangeDir(project, path);
					if (!newPath.empty())
						currentPath = newPath;
					else
						std::cout << "Path not found" << std::endl;
				}
			}
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"listfiles") || 0 == _wcsicmp(cmd.c_str(), L"dir"))
		{
			NX::RmFinder finder(100, std::vector<int>({}), L"", L"", L"");
			std::vector<NX::RmProjectFile> files;
			std::wstring parentDir = currentPath;
			if (L'/' != parentDir[parentDir.length() - 1])
				parentDir.append(L"/");
			res = gpSession->ProjectGetFirstPageFiles(finder, NULL, project.GetId(), parentDir, NULL, NULL, files);
			if (!res) {
				std::cout << "Fail to list 1st page of files (err:" << res.GetCode()
					<< ", file:" << res.GetFile()
					<< ", line:" << res.GetLine()
					<< ", function:" << res.GetFunction()
					<< ", msg:" << res.GetMsg()
					<< ")" << std::endl;
			}
			else {

				do {
					
					std::for_each(files.cbegin(), files.cend(), [](const NX::RmProjectFile& file) {
						const NX::time::datetime& dt = NX::time::datetime::from_unix_time(file.GetCreationTime());
						const NX::time::datetime& dtm = NX::time::datetime::from_unix_time(file.GetLastModifiedTime());
						std::wcout << std::endl;
						std::wcout << L"File: " << file.GetName() << L" (" << file.GetDisplayPath() << L")" << std::endl;
						std::wcout << L"    Id: " << file.GetId() << std::endl;
						std::wcout << L"    Duid: " << file.GetDuid() << std::endl;
						std::wcout << L"    Path: " << file.GetPath() << std::endl;
						std::wcout << L"    Size: " << (int)file.GetSize() << std::endl;
						std::wcout << L"    Creation Time: " << dt.serialize(true, false) << std::endl;
						std::wcout << L"    Modified Time: " << dtm.serialize(true, false) << std::endl;
						std::wcout << L"    Owner: " << file.GetOwner().GetName() << std::endl;
						std::wcout << L"       Owner's UserId: " << file.GetOwner().GetUserId() << std::endl;
						std::wcout << L"       Owner's Email:  " << file.GetOwner().GetEmail() << std::endl;
					});

				} while (res = gpSession->ProjectGetNextPageFiles(finder, NULL, project.GetId(), parentDir, files));

			}

			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"findfiles"))
		{
			if (args.GetArgs().size() < 2) {
				std::cout << "Invalid parameter" << std::endl;
			} else {
				std::vector<NX::RmProjectFile> files;
				std::wstring parentDir = currentPath;
				if (L'/' != parentDir[parentDir.length() - 1])
					parentDir.append(L"/");
				res = gpSession->ProjectFindFiles(project.GetId(), parentDir, args.GetArgs()[1], NULL, files);
				if (!res) {
					std::cout << "Fail to list 1st page of files (err:" << res.GetCode()
						<< ", file:" << res.GetFile()
						<< ", line:" << res.GetLine()
						<< ", function:" << res.GetFunction()
						<< ", msg:" << res.GetMsg()
						<< ")" << std::endl;
				} else {
					std::for_each(files.cbegin(), files.cend(), [](const NX::RmProjectFile& file) {
						const NX::time::datetime& dt = NX::time::datetime::from_unix_time(file.GetCreationTime());
						const NX::time::datetime& dtm = NX::time::datetime::from_unix_time(file.GetLastModifiedTime());
						std::wcout << std::endl;
						std::wcout << L"File: " << file.GetName() << L" (" << file.GetDisplayPath() << L")" << std::endl;
						std::wcout << L"    Id: " << file.GetId() << std::endl;
						std::wcout << L"    Duid: " << file.GetDuid() << std::endl;
						std::wcout << L"    Path: " << file.GetPath() << std::endl;
						std::wcout << L"    Size: " << (int)file.GetSize() << std::endl;
						std::wcout << L"    Creation Time: " << dt.serialize(true, false) << std::endl;
						std::wcout << L"    Modified Time: " << dtm.serialize(true, false) << std::endl;
						std::wcout << L"    Owner: " << file.GetOwner().GetName() << std::endl;
						std::wcout << L"       Owner's UserId: " << file.GetOwner().GetUserId() << std::endl;
						std::wcout << L"       Owner's Email:  " << file.GetOwner().GetEmail() << std::endl;
					});
				}
			}

			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"delete"))
		{
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"upload"))
		{
			ProjectUploadFile(project, currentPath, args);
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"download"))
		{
			if (args.GetArgs().size() != 3) {
				std::cout << "Invalid parameter" << std::endl;
			}
			else {
				ProjectDownloadFile(project, args.GetArgs()[1], args.GetArgs()[2]);
			}
			std::cout << std::endl;
		}
		else
		{
			std::cout << "Unknown command" << std::endl;
		}

	} while (TRUE);
}

void ProjectListAll()
{
}

void ProjectShowInfo(const std::wstring& id)
{
}

void ProjectListMembers(const std::wstring& id)
{
}

void ProjectSearchFile(const std::wstring& id, const std::wstring& path, const std::wstring& keywords, int pagesize)
{
}

void ProjectRemove(const std::wstring& id)
{
}


static std::vector<std::wstring> StringToRightsAndObs(const std::wstring& s)
{
	bool hasViewRight = false;
	std::vector<std::wstring> rights;
	std::for_each(s.begin(), s.end(), [&](const wchar_t& c) {
		switch (c)
		{
		case 'v':
		case 'V':
			rights.push_back(RIGHT_ACTION_VIEW);
			hasViewRight = true;
			break;
		case 'p':
		case 'P':
			rights.push_back(RIGHT_ACTION_PRINT);
			break;
		case 's':
		case 'S':
			rights.push_back(RIGHT_ACTION_SHARE);
			break;
		case 'd':
		case 'D':
			rights.push_back(RIGHT_ACTION_DOWNLOAD);
			break;
		default:
			break;
		}
	});
	if (!hasViewRight)
		rights.push_back(RIGHT_ACTION_VIEW);
	return rights;
}

void ProjectUploadFile(const NX::RmProject& project, const std::wstring& currentDir, const CmdArgs& args)
{
	if (args.GetArgs().size() < 3) {
		std::cout << "Too less parameters" << std::endl;
		return;
	}

	const std::wstring& source = args.GetArgs()[1];
	const std::vector<std::wstring>& rights = StringToRightsAndObs(args.GetArgs()[2]);
	std::wstring target;
	NX::NXL::FileTags tags;
	int i = 3;
	while (i < (int)args.GetArgs().size()) {
		const std::wstring& s = args.GetArgs()[i++];
		if (0 == _wcsicmp(s.c_str(), L"/t") || 0 == _wcsicmp(s.c_str(), L"-t")) {
			if (i >= (int)args.GetArgs().size())
				break;
			const std::wstring& tag = args.GetArgs()[i++];
			std::wstring::size_type pos = tag.find(L'=');
			if (pos != std::wstring::npos) {
				const std::wstring& key = tag.substr(0, pos);
				const std::wstring& value = tag.substr(pos + 1);
				if (!key.empty() && !value.empty())
					tags.AddValue(key, value);
			}
		}
		else if (0 == _wcsicmp(s.c_str(), L"/d") || 0 == _wcsicmp(s.c_str(), L"-d")) {
			if (i >= (int)args.GetArgs().size())
				break;
			target = args.GetArgs()[i++];
		}
		else {
			; // Unknown
		}
	}

	std::wstring parentPathDir;
	std::wstring fileName;

	NX::win::FilePath srcPath(source);
	if (target.empty()) {
		parentPathDir = currentDir;
		fileName = srcPath.GetFileName();
	}
	else {
		if (L'/' != target[0]) {
			parentPathDir = currentDir;
			fileName = target;
		}
		else {
			auto pos = target.rfind(L'/');
			assert(pos != std::wstring::npos);
			parentPathDir = target.substr(0, pos + 1);
			fileName = target.substr(pos + 1);
		}
		if (parentPathDir.empty()) parentPathDir = L"/";
	}
	if (L'/' != parentPathDir[parentPathDir.length() - 1]) {
		parentPathDir.append(L"/");
	}

	NX::RmProjectFile newFile;
	NX::Result res = gpSession->ProjectUploadFile(project.GetId(), parentPathDir, fileName, source, rights, tags, newFile, NULL);
	if (!res) {
		std::cout << "Fail to upload file (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::wcout << std::endl;
	const NX::time::datetime& dtm = NX::time::datetime::from_unix_time(newFile.GetLastModifiedTime());
	std::wcout << L"File has been uploaded" << std::endl;
	std::wcout << L"    Path: " << newFile.GetPath() << std::endl;
	std::wcout << L"    Size: " << (int)newFile.GetSize() << std::endl;
	std::wcout << L"    Creation Time: " << dtm.serialize(true, false) << std::endl;
	std::wcout << L"    Modified Time: " << dtm.serialize(true, false) << std::endl;
	std::wcout << L"    Owner: " << newFile.GetOwner().GetName() << std::endl;
	std::wcout << L"       Owner's UserId: " << gpSession->GetCurrentUser().GetId() << std::endl;
	std::wcout << L"       Owner's Email:  " << gpSession->GetCurrentUser().GetEmail() << std::endl;
}

void ProjectListPendingInvitations()
{
}

std::wstring ProjectChangeDir(const NX::RmProject& project, const std::wstring& target)
{
	if (target.empty() || target == L"/")
		return target;

	std::wstring pathId(target);
	if (L'/' != pathId[pathId.length() - 1])
		pathId.append(L"/");

	NX::RmProjectFileMetadata metadata;
	NX::Result res = gpSession->ProjectGetFileMetadata(project.GetId(), pathId, metadata);
	if (!res)
		return std::wstring();

	return pathId.substr(0, pathId.length() - 1);
}

void ProjectDownloadFile(const NX::RmProject& project, const std::wstring& source, const std::wstring& target)
{
	NX::Result res = gpSession->ProjectDownloadFile(project.GetId(), source, target, false, NULL);
	if (!res) {
		std::cout << "Fail to download file (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::wcout << L"File has been downloaded" << std::endl;
}