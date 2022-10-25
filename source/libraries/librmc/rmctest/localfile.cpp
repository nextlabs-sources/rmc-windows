
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

extern void StringToRightsAndObs(const std::wstring& s, std::vector<std::wstring>& rights, std::vector<std::wstring>& obs);

static void LocalFileShow(const std::wstring& file);
static void LocalFileView(const std::wstring& file, const std::wstring& displayName);
static void LocalFileProtect(const std::wstring& source, const std::wstring& target);
static void LocalFileUnprotect(const std::wstring& source, const std::wstring& target);
static void LocalFileShare(const std::wstring& source, const std::wstring& membershipId, const std::vector<std::wstring>& rights, const std::vector<std::wstring>& obs, const std::vector<std::wstring>& recipients);

void LocalFileCommandProcessor()
{
	do {

		std::wcout << gpSession->GetCurrentUser().GetName() << "/LocalFile> ";

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
			std::cout << "LocalFile Commands" << std::endl;
			std::cout << "  commands        - List all available commands for MyDrive" << std::endl;
			std::cout << "  exit            - Go back to home command space" << std::endl;
			std::cout << "  show <filePath> - Show NXL file information" << std::endl;
			std::cout << "  view <filePath> - View NXL file" << std::endl;
			std::cout << "  protect <filePath> [newFile] - Protect a normal file" << std::endl;
			std::cout << "  unprotect <filePath> [newFile] - Unprotect an NXL file" << std::endl;
			std::cout << "  share <filePath> <rights & effects> <recipeint> [recipeint] ... - Share a file" << std::endl;
			std::cout << "      Valid rights: V(View), P(Print), S(Share), D(Download)" << std::endl;
			std::cout << "      Valid effects: W(Watermark)" << std::endl;
			std::cout << std::endl;
			continue;
		}

		if (0 == _wcsicmp(cmd.c_str(), L"show"))
		{
			if (args.GetArgs().size() < 2) {
				std::cout << "Incorrect command parameter. Correct useage:" << std::endl;
				std::cout << "  show <filePath>" << std::endl;
				continue;
			}
			const std::wstring source(args.GetArgs()[1]);
			LocalFileShow(source);
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"view"))
		{
			if (args.GetArgs().size() < 2) {
				std::cout << "Incorrect command parameter. Correct useage:" << std::endl;
				std::cout << "  protect <filePath>" << std::endl;
				continue;
			}
			const std::wstring path(args.GetArgs()[1]);
			const std::wstring fileName((args.GetArgs().size() > 2) ? args.GetArgs()[2] : L"");
			LocalFileView(path, fileName);
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"protect"))
		{
			if (args.GetArgs().size() < 2) {
				std::cout << "Incorrect command parameter. Correct useage:" << std::endl;
				std::cout << "  protect <filePath> [newFile]" << std::endl;
				continue;
			}
			const std::wstring source(args.GetArgs()[1]);
			const std::wstring target((args.GetArgs().size() > 2) ? args.GetArgs()[2] : L"");
			LocalFileProtect(source, target);
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"unprotect"))
		{
			if (args.GetArgs().size() < 2) {
				std::cout << "Incorrect command parameter. Correct useage:" << std::endl;
				std::cout << "  unprotect <filePath> [newFile]" << std::endl;
				continue;
			}
			const std::wstring source(args.GetArgs()[1]);
			const std::wstring target((args.GetArgs().size() > 2) ? args.GetArgs()[2] : L"");
			LocalFileUnprotect(source, target);
			std::cout << std::endl;
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"share"))
		{
			if (args.GetArgs().size() < 4) {
				std::cout << "Incorrect command parameter. Correct useage:" << std::endl;
				std::cout << "  share <filePath> <rights & effects> <recipeint> [recipeint] ..." << std::endl;
				std::cout << "      Valid rights: V(View), P(Print), S(Share), D(Download)" << std::endl;
				std::cout << "      Valid effects: W(Watermark)" << std::endl;
				std::cout << "Example:" << std::endl;
				std::cout << "  share \"D:\\My Documents\\sals.xlsx\" VPDW mike@nextlabs.com david@nextlabs.com" << std::endl;
				continue;
			}
			const std::wstring& pathId = args.GetArgs()[1];
			std::vector<std::wstring> recipients;
			for (int i = 3; i < (int)args.GetArgs().size(); i++)
				recipients.push_back(args.GetArgs()[i]);

			const std::wstring source(args.GetArgs()[1]);
			const std::wstring& sRights = args.GetArgs()[2];
			std::vector<std::wstring> rights;
			std::vector<std::wstring> obs;
			StringToRightsAndObs(sRights, rights, obs);
			const std::wstring target((args.GetArgs().size() > 2) ? args.GetArgs()[2] : L"");
			LocalFileShare(source, gpSession->GetCurrentUser().GetMemberships().at(0).GetId(), rights, obs, recipients);
			std::cout << std::endl;
		}
		else
		{
			std::cout << "Unknown command" << std::endl;
		}

	} while (TRUE);
}

void LocalFileShow(const std::wstring& file)
{
	if (!NX::iend_with<wchar_t>(file, L".nxl")) {
		std::wcout << L"Target is not a valid NXL file" << std::endl;
		return;
	}

	NX::Result res = RESULT(0);
	std::shared_ptr<NX::NXL::File> fp(gpSession->LocalFileOpen(file, true, res));
	if (fp == NULL) {
		std::wcout << L"Fail to open NXL file" << std::endl;
		std::cout << "Error information (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::wcout << L"File is opened" << std::endl;

	if (!fp->IsFullToken()) {
		std::cout << "Fail to retrieve file token (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
	}
	else {
		if (ERROR_DATA_CHECKSUM_ERROR == res.GetCode()) {
			std::cout << "File header has been compromised (Mimatched header checksum)" << std::endl;
		}
	}

	__int64 contentSize = 0;
	UCHAR headerChecksum[32] = { 0 };


	std::wcout << L"File information:" << std::endl;
	std::wcout << L"    Duid:    " << NX::bintohs<wchar_t>(fp->GetFileSecret().GetToken().GetId().data(), fp->GetFileSecret().GetToken().GetId().size()) << std::endl;
#ifdef _DEBUG
	std::wcout << L"    Token:   " << NX::bintohs<wchar_t>(fp->GetFileSecret().GetToken().GetKey().data(), fp->GetFileSecret().GetToken().GetKey().size()) << std::endl;
	std::wcout << L"    Level:   " << (int)fp->GetFileSecret().GetToken().GetLevel() << std::endl;
#endif
	std::wcout << L"    OwnerId: " << fp->GetFileSecret().GetOwnerIdW() << std::endl;
	std::wcout << L"    ContentOffset: " << NX::itohs<wchar_t>(fp->GetContentOffset()) << std::endl;
	res = fp->GetContentSize(&contentSize);
	if (!res)
		std::wcout << L"    ContentSize:   ERROR" << std::endl;
	else
		printf("    ContentSize:   %I64d (0x%08X%08X)\n", contentSize, (ULONG)(contentSize >> 32), (ULONG)contentSize);
	res = fp->GetHeaderChecksum(headerChecksum);
	if (!res)
		std::wcout << L"    HeaderChecksum: ERROR" << std::endl;
	else
		std::wcout << L"    HeaderChecksum: " << NX::bintohs<wchar_t>(headerChecksum, 32) << std::endl;

	std::vector<NX::NXL::FileSection> sections;
	res = fp->ReadAllSectionRecords(sections);
	if (!res) {
		std::cout << "Fail to read file sections (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::wcout << L"   Sections: " << std::endl;
	for (int i = 0; i < (int)sections.size(); i++) {
		std::wcout << L"     [" << i << L"]" << std::endl;
		std::cout << "        name:     " << sections[i].GetName() << std::endl;
		std::cout << "        offset:   " << NX::itohs<char>(sections[i].GetOffset()) << std::endl;
		std::cout << "        flags:    " << NX::itohs<char>(sections[i].GetFlags()) << std::endl;
		std::cout << "        size:     " << NX::itohs<char>(sections[i].GetSize()) << std::endl;
		std::cout << "        dataSize: " << NX::itohs<char>(sections[i].GetDataSize()) << std::endl;
		std::cout << "        checksum: " << NX::bintohs<char>(sections[i].GetChecksum(), 32) << std::endl;
	}

	NX::NXL::FileMeta metadata;
	std::wcout << std::endl;
	std::wcout << L"    Metadata: " << std::endl;
	res = fp->ReadMetadata(metadata);
	if (!res) {
		std::cout << "Fail to read file metadata (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}
	else {
		std::for_each(metadata.GetMetadata().begin(), metadata.GetMetadata().end(), [](const std::pair<std::wstring, std::wstring>& item) {
			std::wcout << L"        " << item.first << L" = " << item.second << std::endl;
		});
	}

	NX::NXL::FileAdHocPolicy ahPolicy;
	std::wcout << std::endl;
	std::wcout << L"    Ad-hoc Policy: " << std::endl;
	res = fp->ReadAdHocPolicy(ahPolicy);
	if (!res) {
		std::cout << "Fail to read file Ad-hoc Policy (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}
	else {
		NX::time::datetime dt(ahPolicy.GetIssueTime());
		std::wcout << L"        Issuer:    " << ahPolicy.GetOwnerId() << std::endl;
		std::wcout << L"        IssueDate: " << dt.serialize(true, false) << std::endl;
		std::wcout << L"        Rights:    ";
		for (int i = 0; i < (int)ahPolicy.GetRights().size(); i++) {
			if (i > 0) std::cout << ", ";
			std::wcout << ahPolicy.GetRights()[i];
		}
		std::wcout << std::endl;
		std::wcout << L"        Obligations: ";
		for (int i = 0; i < (int)ahPolicy.GetObligations().size(); i++) {
			if (i > 0) std::cout << ", ";
			std::wcout << ahPolicy.GetObligations()[i];
		}
		std::wcout << std::endl;
	}


	NX::NXL::FileTags fileTags;
	std::wcout << std::endl;
	std::wcout << L"    Tags: " << std::endl;
	res = fp->ReadTags(fileTags);
	if (!res) {
		std::cout << "Fail to read file Tags (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}
	else {
		const NX::NXL::FileTags::FileTagArray& tagArray = fileTags.GetTags();
		std::for_each(tagArray.cbegin(), tagArray.cend(), [](const NX::NXL::FileTags::FileTag& tag) {
			std::wcout << L"     " << tag.first << L" : ";
			const NX::NXL::FileTags::FileTagValues& values = tag.second;
			for (int i = 0; i < (int)values.size(); i++) {
				if (i > 0) std::cout << ", ";
				std::wcout << values[i];
			}
			std::wcout << std::endl;
		});
	}
}

void LocalFileView(const std::wstring& file, const std::wstring& displayName)
{
	NX::Result res = gpSession->ViewLocalFile(file, displayName, NULL);
	if (!res) {
		std::cout << "Fail to view file (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::cout << "Succeeded" << std::endl;
}

void LocalFileProtect(const std::wstring& source, const std::wstring& target)
{
	NX::NXL::FileMeta metadata;
	NX::NXL::FileTags tags;
	NX::NXL::FileSecret secret;
	std::wstring newFileId;
	std::wstring newFilePath;

	NX::Result res = gpSession->LocalFileProtect(source, target, L"", metadata, tags, BUILTIN_RIGHT_VIEW, NULL, newFileId, newFilePath, &secret);
	if (!res) {
		std::cout << "Fail to protect file (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
	}

	std::wcout << L"File is protected" << std::endl;
	std::wcout << L"    Duid:    " << NX::bintohs<wchar_t>(secret.GetToken().GetId().data(), secret.GetToken().GetId().size()) << std::endl;
#ifdef _DEBUG
	std::wcout << L"    Token:   " << NX::bintohs<wchar_t>(secret.GetToken().GetKey().data(), secret.GetToken().GetKey().size()) << std::endl;
	std::wcout << L"    Level:   " << (int)secret.GetToken().GetLevel() << std::endl;
#endif
	std::wcout << L"    OwnerId: " << secret.GetOwnerIdW() << std::endl;
}

void LocalFileUnprotect(const std::wstring& source, const std::wstring& target)
{
	NX::Result res = RESULT(0);
	std::shared_ptr<NX::NXL::File> fp(gpSession->LocalFileOpen(source, true, res));
	if (fp == NULL) {
		std::wcout << L"Fail to open NXL file" << std::endl;
		std::cout << "Error information (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	if (ERROR_DATA_CHECKSUM_ERROR == res.GetCode()) {
		std::cout << "File header has been compromised (Mimatched header checksum)" << std::endl;
		return;
	}

	std::wstring destPath(target);
	NX::win::FilePath srcPath(source);
	if (destPath.empty()) {
		const std::wstring& fileName = srcPath.GetFileName();
		const std::wstring& fileExtension = srcPath.GetFileExtension();
		destPath = srcPath.GetParentDir();
		destPath.append(L"\\");
		destPath.append(fileName.substr(0, fileName.length() - 4));
	}

	std::wcout << L"Decrypting file to " << destPath << std::endl;
	std::wcout << L"...";

	res = fp->Unprotect(destPath);
	std::cout << std::endl;
	if (!res) {
		std::cout << "Fail to decrypt file (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::wcout << L"File has been decrypted." << std::endl;
}

void StringToRightsAndObs(const std::wstring& s, std::vector<std::wstring>& rights, std::vector<std::wstring>& obs)
{
	std::for_each(s.begin(), s.end(), [&](const wchar_t& c) {
		switch (c)
		{
		case 'v':
		case 'V':
			rights.push_back(RIGHT_ACTION_VIEW);
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
		case 'w':
		case 'W':
			obs.push_back(OBLIGATION_NAME_WATERMARK);
			break;
		default:
			break;
		}
	});
}

void LocalFileShare(const std::wstring& source, const std::wstring& membershipId, const std::vector<std::wstring>& rights, const std::vector<std::wstring>& obs, const std::vector<std::wstring>& recipients)
{
	NX::RmFileShareResult sr;
	NX::time::datetime dt = NX::time::datetime::current_time();
	const NX::time::span& days = NX::time::span::days_span(180);
	dt.add(days);
	NX::Result res = gpSession->LocalFileShare(source, membershipId, NX::NXL::FileMeta(), NX::NXL::FileTags(), recipients, rights, obs, dt.to_java_time(), sr, NULL);
	if (!res) {
		std::cout << "Fail to share local file (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
		return;
	}

	std::cout << "File is shared, and an email notification has been sent to recipients:" << std::endl;
	std::for_each(recipients.cbegin(), recipients.cend(), [](const std::wstring& r) {
		std::wcout << L"    - " << r << std::endl;
	});
	std::cout << "Shared file information:" << std::endl;
	std::wcout << L"    - Duid: " << sr.GetDuid() << std::endl;
	std::wcout << L"    - Path: " << sr.GetPath() << std::endl;
	std::wcout << L"    - TransactionId: " << sr.GetTransactionId() << std::endl;
}