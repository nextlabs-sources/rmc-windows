
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

std::wstring GetRouter();
void Login();
void Logout();

std::wstring GetRouter()
{
	std::cout << "Please set RMS router:" << std::endl;
	std::cout << "  Use empty or \"Default\" for \"https://r.skydrm.com\"" << std::endl;
	std::cout << "  Use \"Debug\" for \"https://rmtest.nextlabs.solutions\"" << std::endl;
	std::cout << "Router: ";

	std::wstring router = GetInput();

	if (router.empty() || 0 == _wcsicmp(router.c_str(), L"Default")) {
		router = L"https://r.skydrm.com";
	}
	else if (0 == _wcsicmp(router.c_str(), L"Debug")) {
		router = L"https://rmtest.nextlabs.solutions";
	}
	else {
		; //
	}

	return std::move(router);
}

void Login()
{
	std::wstring wsUserName;
	std::wstring wsPassword;

	std::cout << std::endl;
	std::cout << "Login to RMS >" << std::endl;

	do {

		do {
			std::cout << "    Email: ";
			wsUserName = GetInput();
			if (!wsUserName.empty())
				break;

			std::cout << "    Email cannot be empty, try again" << std::endl;
		} while (TRUE);

		do {
			std::cout << "    Password: ";
			wsPassword = GetSilentInput();
			if (!wsPassword.empty())
				break;

			std::cout << "    Password cannot be empty, try again" << std::endl;
		} while (TRUE);

		NX::Result res = gpSession->PrepareLogin(L"skydrm.com");
		if (!res) {
			std::cout << "Login failed (err:" << res.GetCode()
				<< ", file:" << res.GetFile()
				<< ", line:" << res.GetLine()
				<< ", function:" << res.GetFunction()
				<< ", msg:" << res.GetMsg()
				<< ")" << std::endl;
			continue;
		}

		res = gpSession->Login(wsUserName, wsPassword);
		if (!res) {
			std::cout << "Login failed (err:" << res.GetCode()
				<< ", file:" << res.GetFile()
				<< ", line:" << res.GetLine()
				<< ", function:" << res.GetFunction()
				<< ", msg:" << res.GetMsg()
				<< ")" << std::endl;
			continue;
		}

		std::cout << std::endl;
		std::wcout << gpSession->GetCurrentUser().GetName() << L" successfully login to RMS (" << gpSession->GetCurrentRMS().GetUrl() << ")" << std::endl;
		std::wcout << L" - Type \"Commands\" to get all available commands" << std::endl;
		std::cout << std::endl;
		break;

	} while (TRUE);
}

void Logout()
{
}