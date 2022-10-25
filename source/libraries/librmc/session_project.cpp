
#include "stdafx.h"

#include <nx\rmc\session.h>
#include <nx\rmc\securefile.h>

#include <nx\common\stringex.h>
#include <nx\common\rapidjson.h>
#include <nx\winutil\file.h>
#include <nx\winutil\path.h>
#include <nx\crypt\sha.h>
#include <nx\rest\uri.h>

using namespace NX;





Result RmSession::ProjectListProjects(std::vector<RmProject>& projects, bool* cancelFlag, PROJECT_OWNER_TYPE type)
{
	RmFinder finder(100, std::vector<int>({ OrderByLastActionTimeDescend , OrderByNameAscend }), L"", L"", L"");
	Result res = ProjectListFirstPageProjects(finder, projects, cancelFlag, type);
	if (!res)
		return res;

	do {

		std::vector<RmProject> nextPageProjects;
		res = ProjectListNextPageProjects(finder, nextPageProjects, cancelFlag, type);
		if (res) {
			for (const RmProject& proj : nextPageProjects)
				projects.push_back(proj);
		}

	} while (res);

	return RESULT(0);
}

Result RmSession::ProjectListFirstPageProjects(RmFinder& finder, std::vector<RmProject>& projects, bool* cancelFlag, PROJECT_OWNER_TYPE type)
{
	RmRestStatus status;
	__int64 totalRecords = 0;

	finder.ResetPageId();
	Result res = _restClient.ProjectListProjectsEx(_rmserver.GetUrl(), cancelFlag,
		finder.GetPageId(),
		finder.GetPageSize(),
		finder.GetSortType(),
		finder.GetSearchKeyword(),
		(OwnedByMe == (OwnedByMe & type)),
		(OwnedByOthers == (OwnedByOthers & type)),
		status,
		&totalRecords,
		projects);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	finder.SetTotalRecords(totalRecords);
	return RESULT(0);
}

Result RmSession::ProjectListNextPageProjects(RmFinder& finder, std::vector<RmProject>& projects, bool* cancelFlag, PROJECT_OWNER_TYPE type)
{
	if (finder.NoMoreData())
		return RESULT(ERROR_NO_MORE_ITEMS);

	RmRestStatus status;
	Result res = _restClient.ProjectListProjectsEx(_rmserver.GetUrl(), cancelFlag,
		finder.NextPage(),
		finder.GetPageSize(),
		finder.GetSortType(),
		finder.GetSearchKeyword(),
		(OwnedByMe == (OwnedByMe & type)),
		(OwnedByOthers == (OwnedByOthers & type)),
		status,
		NULL,
		projects);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectCreate(const std::wstring& name, const std::wstring& description, const std::wstring& inviteMsg, const std::vector<std::wstring>& invitees, std::wstring& newProjectId)
{
	RmRestStatus status;
	Result res = _restClient.ProjectCreateProject(_rmserver.GetUrl(),
		name,
		description,
		inviteMsg,
		invitees,
		status,
		newProjectId);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectGetInfo(const std::wstring& projectId, RmProject& project)
{
	RmRestStatus status;
	Result res = _restClient.ProjectGetProjectMetadata(_rmserver.GetUrl(),
		projectId,
		status,
		project);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectInvite(const std::wstring& projectId, const std::vector<std::wstring>& invitees,
	std::vector<std::wstring>& alreadyInvited, std::vector<std::wstring>& nowInvited, std::vector<std::wstring>& alreadyMembers)
{
	RmRestStatus status;
	Result res = _restClient.ProjectInvitePeople(_rmserver.GetUrl(),
		projectId,
		invitees,
		status,
		alreadyInvited,
		nowInvited,
		alreadyMembers);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectAcceptInvitation(const std::wstring& id, const std::wstring& code, std::wstring& projectId)
{
	RmRestStatus status;
	Result res = _restClient.ProjectAcceptInvitation(_rmserver.GetUrl(),
		id,
		code,
		status,
		projectId);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectDeclineInvitation(const std::wstring& id, const std::wstring& code, const std::wstring& reason)
{
	RmRestStatus status;
	Result res = _restClient.ProjectDeclineInvitation(_rmserver.GetUrl(),
		id,
		code,
		reason,
		status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectSendInvitationReminder(const std::wstring& id)
{
	RmRestStatus status;
	Result res = _restClient.ProjectSendInvitationReminder(_rmserver.GetUrl(),
		id,
		status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectRevokeInvitation(const std::wstring& id)
{
	RmRestStatus status;
	Result res = _restClient.ProjectRevokeInvitation(_rmserver.GetUrl(),
		id,
		status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectGetFirstPagePendingInvitations(RmFinder& finder, const std::wstring& projectId, std::vector<RmProjectInvitation>& invitations)
{
	RmRestStatus status;
	__int64 totalRecords = 0;

	finder.ResetPageId();
	Result res = _restClient.ProjectListPendingInvitations(_rmserver.GetUrl(),
		projectId,
		finder.GetPageId(),
		finder.GetPageSize(),
		finder.GetSortType(),
		finder.GetSearchFields(),
		finder.GetSearchKeyword(),
		status,
		&totalRecords,
		invitations);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	finder.SetTotalRecords(totalRecords);
	return RESULT(0);
}

Result RmSession::ProjectGetNextPagePendingInvitations(RmFinder& finder, const std::wstring& projectId, std::vector<RmProjectInvitation>& invitations)
{
	if (finder.NoMoreData())
		return RESULT(ERROR_NO_MORE_ITEMS);

	RmRestStatus status;
	Result res = _restClient.ProjectListPendingInvitations(_rmserver.GetUrl(),
		projectId,
		finder.NextPage(),
		finder.GetPageSize(),
		finder.GetSortType(),
		finder.GetSearchFields(),
		finder.GetSearchKeyword(),
		status,
		NULL,
		invitations);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectGetUserPendingInvitations(std::vector<RmProjectInvitation>& invitations, bool* cancelFlag)
{
	RmRestStatus status;
	Result res = _restClient.ProjectListUserPendingInvitations(_rmserver.GetUrl(), cancelFlag,
		status,
		invitations);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectGetFirstPageFiles(RmFinder& finder, _In_opt_ bool* cancelFlag,
	const std::wstring& projectId,
	const std::wstring& parentFolder,
	_Out_opt_ __int64* usage,
	_Out_opt_ __int64* quota,
	_Out_opt_ __int64* lastUpdatedTime,
	std::vector<RmProjectFile>& files)
{
	RmRestStatus status;
	int totalRecords = 0;

	finder.ResetPageId();
	Result res = _restClient.ProjectListFiles(_rmserver.GetUrl(), cancelFlag,
		projectId,
		finder.GetPageId(),
		finder.GetPageSize(),
		finder.GetSortType(),
		parentFolder,
		finder.GetSearchKeyword(),
		status,
		&totalRecords,
		usage,
		quota,
		lastUpdatedTime,
		files);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	finder.SetTotalRecords(totalRecords);
	return RESULT(0);
}

Result RmSession::ProjectGetNextPageFiles(RmFinder& finder, _In_opt_ bool* cancelFlag, const std::wstring& projectId, const std::wstring& parentFolder, std::vector<RmProjectFile>& files)
{
	if (finder.NoMoreData())
		return RESULT(ERROR_NO_MORE_ITEMS);

	RmRestStatus status;
	Result res = _restClient.ProjectListFiles(_rmserver.GetUrl(), cancelFlag,
		projectId,
		finder.NextPage(),
		finder.GetPageSize(),
		finder.GetSortType(),
		parentFolder,
		finder.GetSearchKeyword(),
		status,
		NULL,
		NULL,
		NULL,
		NULL,
		files);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectFindFiles(const std::wstring& projectId, const std::wstring& parentFolder, const std::wstring& keyword, _In_opt_ bool* cancelFlag, std::vector<RmProjectFile>& files)
{
	std::vector<RmProjectFile> tmpFiles;
	RmFinder finder(100, std::vector<int>(), L"", L"", keyword);
	Result res = ProjectGetFirstPageFiles(finder, cancelFlag, projectId, parentFolder, NULL, NULL, NULL, tmpFiles);
	if (!res)
		return res;

	do {
		std::for_each(tmpFiles.begin(), tmpFiles.end(), [&](const NX::RmProjectFile& file) {
			files.push_back(file);
		});
		tmpFiles.clear();
	} while (res = ProjectGetNextPageFiles(finder, cancelFlag, projectId, parentFolder, tmpFiles));

	return RESULT(0);
}

Result RmSession::ProjectCreateFolder(const std::wstring& projectId, const std::wstring& parentDir, const std::wstring& folderName, bool autoRename, RmRepoFile& folder)
{
	RmRestStatus status;
	Result res = _restClient.ProjectCreateFolder(_rmserver.GetUrl(),
		projectId,
		parentDir,
		folderName,
		autoRename,
		status,
		folder);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectDeleteFile(const std::wstring& projectId, const std::wstring& filePath)
{
	RmRestStatus status;
	Result res = _restClient.ProjectDeleteFile(_rmserver.GetUrl(),
		projectId,
		filePath,
		status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectGetFolderFirstPageMetadata(RmFinder& finder, bool* cancelFlag,
	const std::wstring& projectId,
	const std::wstring& folderId,
	__int64 lastModifiedTime,
	_Out_opt_ __int64* usage,
	_Out_opt_ __int64* quota,
	std::vector<RmProjectFile>& files)
{
	RmRestStatus status;
	__int64 totalRecords = 0;

	finder.ResetPageId();
	Result res = _restClient.ProjectGetFolderMetadata(_rmserver.GetUrl(), cancelFlag,
		projectId,
		folderId,
		finder.GetPageId(),
		finder.GetPageSize(),
		finder.GetSortType(),
		lastModifiedTime,
		status,
		usage,
		quota,
		&totalRecords,
		files);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	finder.SetTotalRecords(totalRecords);
	return RESULT(0);
}

Result RmSession::ProjectGetFolderNextPageMetadata(RmFinder& finder,
	bool* cancelFlag,
	const std::wstring& projectId,
	const std::wstring& folderId,
	__int64 lastModifiedTime,
	std::vector<RmProjectFile>& files)
{
	if (finder.NoMoreData())
		return RESULT(ERROR_NO_MORE_ITEMS);

	RmRestStatus status;
	Result res = _restClient.ProjectGetFolderMetadata(_rmserver.GetUrl(), cancelFlag,
		projectId,
		folderId,
		finder.NextPage(),
		finder.GetPageSize(),
		finder.GetSortType(),
		lastModifiedTime,
		status,
		NULL,
		NULL,
		NULL,
		files);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectUploadFile(const std::wstring& projectId,
	const std::wstring& destDirPathId,
	const std::wstring& destFileName,
	const std::wstring& srcFilePath,
	const std::vector<std::wstring>& rights,
	const NXL::FileTags& tags,
	RmProjectFile& newFile,
	_In_opt_ bool* cancelFlag)
{
	std::wstring sRights(L"[");
	std::for_each(rights.begin(), rights.end(), [&](const std::wstring& s) {
		if (!s.empty()) {
			if (sRights.length() > 1)
				sRights.append(L",");
			sRights.append(L"\"");
			sRights.append(s);
			sRights.append(L"\"");
		}
	});
	sRights.append(L"]");

	if (sRights.empty())
		return RESULT(ERROR_INVALID_PARAMETER);

	RmRestStatus status;
	Result res = _restClient.ProjectUploadFile(_rmserver.GetUrl(),
		cancelFlag,
		projectId,
		destDirPathId,
		destFileName,
		srcFilePath,
		sRights,
		tags,
		status,
		newFile);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectDownloadFile(const std::wstring& projectId,
	const std::wstring& pathId,
	const std::wstring& targetFolder,
	bool failIfExists,
	_Out_ std::wstring& outFilePath,
	_In_opt_ bool* cancelFlag)
{
	std::wstring tmpFilePath;
	Result res = ProjectCreateTempFile(projectId, pathId, tmpFilePath);
	if (!res) {
		tmpFilePath.clear();
		return res;
	}

	std::wstring preferredFileName;
	res = ProjectInterDownloadFile(projectId, pathId, tmpFilePath, false, false, preferredFileName, cancelFlag);
	if (!res) {
		::DeleteFileW(tmpFilePath.c_str());
		tmpFilePath.clear();
		return res;
	}

	outFilePath = std::wstring(targetFolder + (NX::iend_with<wchar_t>(targetFolder, L"\\") ? L"" : L"\\") + preferredFileName);
	if (failIfExists && INVALID_FILE_ATTRIBUTES != GetFileAttributesW(outFilePath.c_str())) {
		return RESULT(ERROR_FILE_EXISTS);
	}

	if (!MoveFileExW(tmpFilePath.c_str(), outFilePath.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_COPY_ALLOWED)) {
		res = RESULT(GetLastError());
		::DeleteFileW(tmpFilePath.c_str());
		return res;
	}

	return RESULT(0);
}

Result RmSession::ProjectCreateTempFile(const std::wstring& projectId, const std::wstring& pathId, std::wstring& tmpFilePath)
{
	std::wstring s(pathId + L"@" + projectId);
	NX::tolower<wchar_t>(s);
	UCHAR hash[20] = { 0 };
	Result res = NX::crypt::CreateSha1((const unsigned char*)s.c_str(), (ULONG)(s.length() * 2), hash);
	if (!res) {
		return res;
	}

	tmpFilePath = GetCurrentUserTempDir();
	tmpFilePath.append(L"\\");
	tmpFilePath.append(NX::bintohs<wchar_t>(hash, 20));
	tmpFilePath.append(L".tmp");
	return RESULT(0);
}

Result RmSession::ProjectDownloadFileForViewer(const std::wstring& projectId, const std::wstring& pathId, std::wstring& tmpFilePath, _In_opt_ bool* cancelFlag)
{
	Result res = ProjectCreateTempFile(projectId, pathId, tmpFilePath);
	if (!res) {
		tmpFilePath.clear();
		return res;
	}

	std::wstring preferredFileName;
	res = ProjectInterDownloadFile(projectId, pathId, tmpFilePath, false, true, preferredFileName, cancelFlag);
	if (!res) {
		::DeleteFileW(tmpFilePath.c_str());
		tmpFilePath.clear();
		return res;
	}

	return RESULT(0);
}

Result RmSession::ProjectGetFileMetadata(const std::wstring& projectId, const std::wstring& source, RmProjectFileMetadata& metadata)
{
	RmRestStatus status;
	Result res = _restClient.ProjectGetFileMetadata(_rmserver.GetUrl(),
		projectId,
		source,
		status,
		metadata);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectGetFirstPageMembers(RmFinder& finder, const std::wstring& projectId, bool returnPicture, std::vector<RmProjectMember>& members)
{
	RmRestStatus status;
	int totalRecords = 0;

	finder.ResetPageId();
	Result res = _restClient.ProjectListMembers(_rmserver.GetUrl(),
		projectId,
		finder.GetPageId(),
		finder.GetPageSize(),
		finder.GetSortType(),
		finder.GetSearchFields(),
		finder.GetSearchKeyword(),
		returnPicture,
		status,
		&totalRecords,
		members);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	finder.SetTotalRecords(totalRecords);
	return RESULT(0);
}

Result RmSession::ProjectGetNextPageMembers(RmFinder& finder, const std::wstring& projectId, bool returnPicture, std::vector<RmProjectMember>& members)
{
	if (finder.NoMoreData())
		return RESULT(ERROR_NO_MORE_ITEMS);

	RmRestStatus status;
	Result res = _restClient.ProjectListMembers(_rmserver.GetUrl(),
		projectId,
		finder.GetPageId(),
		finder.GetPageSize(),
		finder.GetSortType(),
		finder.GetSearchFields(),
		finder.GetSearchKeyword(),
		returnPicture,
		status,
		NULL,
		members);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectRemoveMember(const std::wstring& projectId, const std::wstring& memberId)
{
	RmRestStatus status;
	Result res = _restClient.ProjectRemoveMember(_rmserver.GetUrl(),
		projectId,
		memberId,
		status);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectGetMemberInfo(const std::wstring& projectId, const std::wstring& memberId, RmProjectMember& member)
{
	RmRestStatus status;
	Result res = _restClient.ProjectGetMemberMetadata(_rmserver.GetUrl(),
		projectId,
		memberId,
		status,
		member);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));
	// Succeed
	return RESULT(0);
}

Result RmSession::ProjectInterDownloadFile(const std::wstring& projectId,
	const std::wstring& pathId,
	const std::wstring& target,
	bool failIfExists,
	bool forViewer,
	_Out_ std::wstring& preferredFileName,
	_In_opt_ bool* cancelFlag)
{
	if (failIfExists && INVALID_FILE_ATTRIBUTES != GetFileAttributesW(target.c_str()))
		return RESULT(ERROR_FILE_EXISTS);

	RmRestStatus status;
	Result res = _restClient.ProjectDownloadFile(_rmserver.GetUrl(),
		cancelFlag,
		projectId,
		pathId,
		target,
		forViewer,
		status,
		preferredFileName);
	if (!res)
		return res;
	if (!status)
		return RESULT2(status.GetStatus(), NX::conv::utf16toutf8(status.GetPhrase()));

	// Succeed
	return RESULT(0);
}
