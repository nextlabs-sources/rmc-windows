#pragma once


class CUIConnector
{
public:
	CUIConnector();
	~CUIConnector();

	bool SendNotify(_In_ const std::wstring message);
private:
	std::wstring m_pipename;
};

