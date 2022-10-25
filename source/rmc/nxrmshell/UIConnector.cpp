
#include <Windows.h>
#include <string>
#include <xstring>

#include "UIConnector.h"


#ifndef _UIPIPE_BUFFER_SIZE
#define _UIPIPE_BUFFER_SIZE 4096UL
#endif

CUIConnector::CUIConnector() : m_pipename(L"")
{
	//initilaize pipename
	DWORD sid;
	
	if (ProcessIdToSessionId(GetCurrentProcessId(), &sid)) {
		m_pipename = L"nxrmtray_";
//		m_pipename += NX::conversion::to_wstring((int)sid);
	}
}


CUIConnector::~CUIConnector()
{
	m_pipename.clear();
}


bool CUIConnector::SendNotify(const std::wstring message)
{
	bool bret = false;
	if (m_pipename.empty())
		return bret;

/* 	NX::async_pipe::client pipeclient(_UIPIPE_BUFFER_SIZE);
 
 	if (pipeclient.connect(m_pipename, 1000UL)) {
		std::string uirequest;
		uirequest = "{\"code\":100,\"message\":\"";
		uirequest += NX::conversion::utf16_to_utf8(message);
		uirequest += "\"}";
		const std::vector<unsigned char> data(uirequest.c_str(), uirequest.c_str() + uirequest.length());
		if (pipeclient.write(data)) {
			bret = true;
		}
		pipeclient.disconnect();
	}*/

	return bret;
}