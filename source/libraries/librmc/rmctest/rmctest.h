

#ifndef __RMC_TEST_H__
#define __RMC_TEST_H__


#include <fstream>

class NoEchoInput
{
public:
	NoEchoInput() : h(GetStdHandle(STD_INPUT_HANDLE)), mode(0)
	{
		if (NULL != h)
		{
			GetConsoleMode(h, &mode);
			SetConsoleMode(h, mode & (~ENABLE_ECHO_INPUT));
		}
	}
	~NoEchoInput()
	{
		if (NULL != h)
		{
			SetConsoleMode(h, mode);
		}
	}

private:
	HANDLE h;
	DWORD  mode;
};

class CmdArgs
{
public:
	CmdArgs() {}
	CmdArgs(const std::wstring& s) : _args(SetCmd(s)) {}
	~CmdArgs() {}

	std::vector<std::wstring> SetCmd(const std::wstring& s)
	{
		std::vector<std::wstring> args;

		if (s.empty())
			return args;

		int nArgs = 0;
		LPWSTR* pArgs = CommandLineToArgvW(s.c_str(), &nArgs);
		for (int i = 0; i < nArgs; i++)
			args.push_back(pArgs[i]);

		return std::move(args);
	}

	inline const std::vector<std::wstring>& GetArgs() const { return _args; }

private:
	std::vector<std::wstring> _args;
};

std::wstring GetInput();
std::wstring GetSilentInput();


class TestOutput
{
public:
	TestOutput();
	~TestOutput();

private:
	std::ofstream fp;
};

class AutoTest
{
public:
	AutoTest();
	AutoTest(bool bTestSkyDRM);
	~AutoTest();

private:
	std::shared_ptr<NX::RmSession> m_spSession;
	bool m_bTerminate;
};


#endif