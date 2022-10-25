
#include "stdafx.h"

#include <Shellapi.h>

#include <string>
#include <memory>
#include <algorithm>
#include <iostream>

#include <nx\rmc\session.h>
#include <nx\rmc\upgrade.h>
#include <nx\common\stringex.h>
#include <nx\common\time.h>
#include <nx\winutil\path.h>
#include <nx\nxl\nxlrights.h>

#include "rmctest.h"


static std::wstring CombinePath(const std::wstring& current, const std::wstring& subpath);

extern std::wstring GetRouter();
extern void Login();
extern void Logout();
extern void MyDriveShowUsage();

extern void LocalFileCommandProcessor();
extern void MyDriveCommandProcessor();
extern void MyVaultCommandProcessor();
extern void ReposCommandProcessor();
extern void ProjectsCommandProcessor();

std::shared_ptr<NX::RmSession> gpSession;
std::vector<NX::RmRepository> gpRepos;




int main(int argc, char** argv)
{
	bool bTestSkyDRM = false;

	std::cout << "NextLabs Rights Management Client Library Automation Test" << std::endl;
	std::cout << std::endl;

	if (argc > 1) {

		if (argc > 2 || 0 == _stricmp(argv[1], "--help") || 0 == _stricmp(argv[1], "-h") || 0 == _stricmp(argv[1], "/h")) {
			std::cout << "USAGE:" << std::endl;
			std::cout << "    rmctest.exe [skydrm|stage]" << std::endl;
			return 0;
		}

		bTestSkyDRM = (0 == _stricmp(argv[1], "skydrm")) ? true : false;
	}

	NX::Result res = RESULT(0);
    std::cout << std::endl;
    system("cls");
    std::cout << "SkyDRM" << std::endl;

    std::wstring routerUrl = GetRouter();

    std::wcout << L"Router is " << routerUrl << std::endl;

    gpSession = std::shared_ptr<NX::RmSession>(NX::RmSession::Create(NX::RmRouter(L"SkyDRM", routerUrl), res));
    if (gpSession == nullptr) {
        std::cout << "Fail to create session (err:" << res.GetCode()
            << ", file:" << res.GetFile()
            << ", line:" << res.GetLine()
            << ", function:" << res.GetFunction()
            << ", msg:" << res.GetMsg()
            << ")" << std::endl;
        return res.GetCode();
    }

    std::wcout << L"Initializing ..."  << std::endl;
    //res = gpSession->Initialize();
    //if(!res) {
    //    std::cout << "Fail to initialize session (err:" << res.GetCode()
    //        << ", file:" << res.GetFile()
    //        << ", line:" << res.GetLine()
    //        << ", function:" << res.GetFunction()
    //        << ", msg:" << res.GetMsg()
    //        << ")" << std::endl;
    //    return res.GetCode();
    //}
    std::wcout << L"  -> Client Id:  " << gpSession->GetClientId().GetClientId() << std::endl;
#ifdef _DEBUG
    std::wcout << L"  -> Client Key: " << NX::bintohs<wchar_t>(gpSession->GetClientId().GetClientKey().data(), gpSession->GetClientId().GetClientKey().size()) << std::endl;
#endif

_credCheck:
	if (!gpSession->ValidateCredential()) {
		Login();
		NX::time::datetime exptime = NX::time::datetime::from_unix_time(gpSession->GetCurrentUser().GetTicket().GetTTL());
		std::wcout << L"    Id: " << gpSession->GetCurrentUser().GetId() << std::endl;
		std::wcout << L"    Name: " << gpSession->GetCurrentUser().GetName() << std::endl;
		std::wcout << L"    Email: " << gpSession->GetCurrentUser().GetEmail() << std::endl;
		std::wcout << L"    Expire at: " << exptime.serialize(true, false) << std::endl;
		std::wcout << std::endl;
	}
	else {
		NX::time::datetime exptime = NX::time::datetime::from_unix_time(gpSession->GetCurrentUser().GetTicket().GetTTL());
		std::wcout << std::endl;
		std::wcout << L"Use existing valid credential"  << std::endl;
		std::wcout << L"    Id: " << gpSession->GetCurrentUser().GetId() << std::endl;
		std::wcout << L"    Name: " << gpSession->GetCurrentUser().GetName() << std::endl;
		std::wcout << L"    Email: " << gpSession->GetCurrentUser().GetEmail() << std::endl;
		std::wcout << L"    Expire at: " << exptime.serialize(true, false) << std::endl;
		std::wcout << std::endl;
	}


	res = gpSession->RepoGetReposListFromRemote(gpRepos);
	if (!res) {
		std::cout << "Fail to get all repositories (err:" << res.GetCode()
			<< ", file:" << res.GetFile()
			<< ", line:" << res.GetLine()
			<< ", function:" << res.GetFunction()
			<< ", msg:" << res.GetMsg()
			<< ")" << std::endl;
	}

    do {

        std::wcout << gpSession->GetCurrentUser().GetName() << L"> ";

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
            std::cout << "SkyDRM Commands" << std::endl;
            std::cout << "  commands     - List all available commands" << std::endl;
            std::cout << "  exit         - Exit from this application" << std::endl;
            std::cout << "  logout       - Log out" << std::endl;
            std::cout << "  update       - update profile" << std::endl;
            std::cout << "  upgrade      - upgrade software" << std::endl;
            std::cout << "  changepswd   - Change password" << std::endl;
            std::cout << "  changename   - Change display name" << std::endl;
            std::cout << "  version      - Show application version" << std::endl;
            std::cout << "  profile      - Show current user's profile" << std::endl;
            std::cout << "  status       - Show general status" << std::endl;
            std::cout << "  localfile    - Enter LocalFile command loop" << std::endl;
            std::cout << "  mydrive      - Enter MyDrive command loop" << std::endl;
            std::cout << "  myvault      - Enter MyVault command loop" << std::endl;
            std::cout << "  repositories - Enter Repositories command loop" << std::endl;
            std::cout << "  project      - Enter Project command loop" << std::endl;
            std::cout << std::endl;
            continue;
        }
        
        if (0 == _wcsicmp(cmd.c_str(), L"version"))
        {
            std::cout << "1.0.1607 " << std::endl;
            std::cout << std::endl;
        }
        else if (0 == _wcsicmp(cmd.c_str(), L"logout"))
        {
            NX::time::datetime dt = NX::time::datetime::from_unix_time(gpSession->GetCurrentUser().GetTicket().GetTTL());
            // Show current user's profile
			const std::wstring& tenantName = gpSession->GetCurrentTenantName();
            std::wcout << L"User " << gpSession->GetCurrentUser().GetName() << " (Id: " << gpSession->GetCurrentUser().GetId() << L") has logged out." << std::endl;
            std::cout << std::endl;
			gpSession->Logout();
			goto _credCheck;
        }
        else if (0 == _wcsicmp(cmd.c_str(), L"update"))
        {
			res = gpSession->UpdateProfile();
			if (res) {
				std::cout << "Profile has been changed" << std::endl;
			}
			else {
				std::cout << "Fail to update profile (err:" << res.GetCode()
					<< ", file:" << res.GetFile()
					<< ", line:" << res.GetLine()
					<< ", function:" << res.GetFunction()
					<< ", msg:" << res.GetMsg()
					<< ")" << std::endl;
			}
        }
		else if (0 == _wcsicmp(cmd.c_str(), L"upgrade"))
		{
			NX::RmUpgradeInfo uinf = gpSession->CheckForUpgrade();
			if (uinf.Empty()) {
				std::cout << "Your product is up-to-date" << std::endl;
			}
			else {

				std::wcout << L"A new version (" << uinf.GetVersion() << L") is available" << std::endl;

				if (uinf.GetDownloadedFile().empty()) {

					std::wcout << L"Do you want to download and install this new version? (Y|N): ";
					const std::wstring& yesno1 = GetInput();
					if (0 == NX::icompare<wchar_t>(L"y", yesno1)) {
						std::wcout << L"Downloading ..." << std::endl;
						res = gpSession->DownloadUpgradeInstaller(uinf);
						if (res) {
							assert(!uinf.GetDownloadedFile().empty());
							std::wcout << L"Done" << std::endl;
							//std::wcout << L"New version has been downloaded to:" << std::endl;
							//std::wcout << L"    " << uinf.GetDownloadedFile() << std::endl;
						}
						else {
							std::cout << "Fail to download new version (err:" << res.GetCode()
								<< ", file:" << res.GetFile()
								<< ", line:" << res.GetLine()
								<< ", function:" << res.GetFunction()
								<< ", msg:" << res.GetMsg()
								<< ")" << std::endl;
						}
					}

				}
				else {
					//std::wcout << L"New version has been downloaded to:" << std::endl;
					//std::wcout << L"    " << uinf.GetDownloadedFile() << std::endl;
					; //
				}

				if (!uinf.GetDownloadedFile().empty()) {
					std::wcout << L"New installer has been downloaded, do you want to install new version now? (Y|N): ";
					const std::wstring& yesno2 = GetInput();
					if (0 == NX::icompare<wchar_t>(L"y", yesno2)) {
						NX::RmUpgrade upgrader(uinf);
						std::cout << "Installing new version ..." << std::endl;
						res = upgrader.InstallNewVersion();
						if (!res)
							std::cout << "Fail to install new version" << std::endl;
						else
							std::cout << "Done" << std::endl;
					}
				}
			}
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"changepswd"))
		{
			std::cout << "Please input your current password: ";
			const std::wstring& sOldPassword = GetInput();
			std::cout << "Please input new password: ";
			const std::wstring& sNewPassword = GetInput();
			std::cout << "Please input new password again: ";
			const std::wstring& sNewPassword2 = GetInput();
			if (sNewPassword != sNewPassword2) {
				std::cout << "New password mismatch" << std::endl;
			}
			else {
				res = gpSession->ChangePassword(sOldPassword, sNewPassword);
				if (res) {
					std::cout << "Your password has been changed" << std::endl;
				}
				else {
					std::cout << "Fail to change password (err:" << res.GetCode()
						<< ", file:" << res.GetFile()
						<< ", line:" << res.GetLine()
						<< ", function:" << res.GetFunction()
						<< ", msg:" << res.GetMsg()
						<< ")" << std::endl;
				}
			}
		}
		else if (0 == _wcsicmp(cmd.c_str(), L"changename"))
		{
			std::wstring newName;
			do {
				std::cout << "Please input new display name: ";
				newName = GetInput();
			} while (newName.empty());

			res = gpSession->UpdateProfileDisplayName(newName);
			if (res) {
				std::wcout << L"Display name has been changed -> " << gpSession->GetCurrentUser().GetName() << std::endl;
			}
			else {
				std::cout << "Fail to change display name (err:" << res.GetCode()
					<< ", file:" << res.GetFile()
					<< ", line:" << res.GetLine()
					<< ", function:" << res.GetFunction()
					<< ", msg:" << res.GetMsg()
					<< ")" << std::endl;
			}
		}
        else if (0 == _wcsicmp(cmd.c_str(), L"profile"))
        {
            NX::time::datetime dt = NX::time::datetime::from_unix_time(gpSession->GetCurrentUser().GetTicket().GetTTL());
            // Show current user's profile
			const std::wstring& tenantName = gpSession->GetCurrentTenantName();
            std::wcout << L"Tenant: " << (tenantName.empty() ? L"skydrm.com" : tenantName) << std::endl;
            std::wcout << L"User: " << std::endl;
            std::wcout << L"   - Id: " << gpSession->GetCurrentUser().GetId() << std::endl;
            std::wcout << L"   - Name: " << gpSession->GetCurrentUser().GetName() << std::endl;
            std::wcout << L"   - Email: " << gpSession->GetCurrentUser().GetEmail() << std::endl;
            std::wcout << L"Session: " << std::endl;
            std::wcout << L"   - Ticket: " << gpSession->GetCurrentUser().GetTicket().GetTicket() << std::endl;
            std::wcout << L"   - Expire At: " << dt.serialize(true, false) << std::endl;
            std::wcout << L"Memberships: " << std::endl;
            const std::vector<NX::RmUserMembership>& memberships = gpSession->GetCurrentUser().GetMemberships();
            for (auto it = memberships.begin(); it != memberships.end(); ++it) {
                std::wcout << L"   " << (*it).GetId() << " - Type: " << (*it).GetType() << ", tenantId: " << (*it).GetTenantId() << std::endl;
            }
            std::cout << std::endl;
        }
        else if (0 == _wcsicmp(cmd.c_str(), L"status"))
        {
            // Show current user's status on home page
            MyDriveShowUsage();
            std::cout << std::endl;
        }
        else if (0 == _wcsicmp(cmd.c_str(), L"localfile"))
        {
            // Show current user's MyDrive usage
            LocalFileCommandProcessor();
            std::cout << std::endl;
        }
        else if (0 == _wcsicmp(cmd.c_str(), L"mydrive"))
        {
            // Show current user's MyDrive usage
            MyDriveCommandProcessor();
            std::cout << std::endl;
        }
        else if (0 == _wcsicmp(cmd.c_str(), L"myvault"))
        {
            // Show current user's MyVault usage
            MyVaultCommandProcessor();
            std::cout << std::endl;
        }
        else if (0 == _wcsicmp(cmd.c_str(), L"project"))
        {
            ProjectsCommandProcessor();
            std::cout << std::endl;
        }
        else if (0 == _wcsicmp(cmd.c_str(), L"repositories"))
        {
            ReposCommandProcessor();
            std::cout << std::endl;
        }
        else
        {
            std::cout << "Unknown command" << std::endl;
        }

    } while (TRUE);

	gpSession->Cleanup();
    return 0;
}

std::wstring GetInput()
{
    std::string s;
    do {
        char c = getchar();
        if (c == 0x0D || c == 0x0A)
            break;
        if (c == 8 && !s.empty()) {
            s = s.substr(0, s.length() - 1);
            continue;
        }
        if (c < 32 || c > 126)
            continue;
        s.push_back(c);
    } while (TRUE);
    return std::wstring(s.begin(), s.end());
}

std::wstring GetSilentInput()
{
    NoEchoInput disable_echo;
    std::wstring s = GetInput();
    std::cout << std::endl;
    return std::move(s);
}



ULONGLONG StringRightsToUll(const std::wstring& s)
{
	ULONGLONG ull = 0;
	std::for_each(s.begin(), s.end(), [&](const wchar_t& c) {
		switch (c)
		{
		case 'v':
		case 'V':
			ull |= BUILTIN_RIGHT_VIEW;
			break;
		case 'p':
		case 'P':
			ull |= BUILTIN_RIGHT_PRINT;
			break;
		case 's':
		case 'S':
			ull |= BUILTIN_RIGHT_SHARE;
			break;
		case 'd':
		case 'D':
			ull |= BUILTIN_RIGHT_DOWNLOAD;
			break;
		case 'w':
		case 'W':
			ull |= BUILTIN_OBRIGHT_WATERMARK;
			break;
		default:
			break;
		}
	});
	return ull;
}




AutoTest::AutoTest()
{
}

AutoTest::AutoTest(bool bTestSkyDRM)
{
}

AutoTest::~AutoTest()
{
}







