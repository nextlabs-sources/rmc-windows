

#ifndef __NXRM_COMMONUI_DLGMANAGEUSERS_HPP__
#define __NXRM_COMMONUI_DLGMANAGEUSERS_HPP__

#include <Windows.h>
#include <string>
#include <vector>
#include <memory>
#include <list>

#include "resource.h"
#include "dlgtemplate.hpp"
#include <string>
#include "emailAddrDlgBase.hpp"
#include "dlgaddress.hpp"
#include <vector>
#include "commondlg.hpp"

using namespace std;


#define     TOTAL_COLLAPSECONTROLS   12

#define     TOTAL_PERMISSIONS   11
#define     TOTAL_EXPIRE_CONTROLS   4
#define     TOTAL_MOVING_CONTROLS            10




class CDlgManageUsers : public CEmailAddrDlgBase
{
public:
	CDlgManageUsers(const std::wstring & file, const std::vector<std::wstring> &peopleArr);

    virtual ~CDlgManageUsers();


    virtual BOOL OnInitialize();
    virtual BOOL OnNotify(_In_ LPNMHDR lpnmhdr);
    virtual BOOL OnCommand(WORD notify, WORD id, HWND hwnd);
    virtual BOOL OnOwnerDraw(DRAWITEMSTRUCT* lpDrawItemStruct);
    virtual INT_PTR OnRestWMMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual BOOL OnLButtonDown(WPARAM wParam, int x, int y);
    virtual BOOL OnPaint();
    virtual BOOL OnSize(WPARAM wParam, LPARAM lParam);
    virtual void OnDestroy();


    virtual void OnOk();
    int GetResult(const wchar_t **pResult);
	bool GetDataIntoVector();

private:
    std::wstring            _file;

    HFONT                   _hBoldFont; //Make bold font to controls, only can deleted at destroy time
	vector<wchar_t>         _resultString;
	std::vector<std::wstring> _peopleArr;

    //Following variables are for scroll
    HCURSOR m_hCursor1, m_hCursor2;
};


#endif