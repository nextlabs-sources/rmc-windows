

#ifndef __NXRM_COMMONUI_DLGPROJECT_HPP__
#define __NXRM_COMMONUI_DLGPROJECT_HPP__

#include <Windows.h>
#include <string>
#include <vector>
#include <memory>
#include <list>

#include "resource.h"
//#include "classifydata.hpp"
//#include "classifylistctrl.hpp"
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




class CDlgProject : public CEmailAddrDlgBase
{
public:
	//CDlgProject(NXProjData &projIn);

    CDlgProject(NXProjData & projIn, int DlgID, bool bEmptyIsValid);

    virtual ~CDlgProject();


    inline void SetFile(_In_ const std::wstring& file) throw() { _file = file; }



    virtual BOOL OnInitialize();
    void EnableShareButton();
    void EnlargeWindowToFit();
    virtual BOOL OnNotify(_In_ LPNMHDR lpnmhdr);
    virtual BOOL OnCommand(WORD notify, WORD id, HWND hwnd);
    virtual BOOL OnOwnerDraw(DRAWITEMSTRUCT* lpDrawItemStruct);
    virtual INT_PTR OnRestWMMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual BOOL OnLButtonDown(WPARAM wParam, int x, int y);
    virtual BOOL OnPaint();
    virtual BOOL OnSize(WPARAM wParam, LPARAM lParam);
    virtual void OnDestroy();


    void OnComboBoxSelChanged();
    virtual void OnOk();
    bool GetSharedData();
    void RemoveAddress(CDlgEmailAddr *p);
    int GetResult(const wchar_t **pResult);
	int GetJsonResult(const wchar_t ** pResult);
    POINT GetInitialEditPos() {
        return _ptInitEditPos;
    }

	bool GetDataIntoVector();


protected:
    void SetPage();
    void ShowSummary();
    BOOL InitClassifyItem(const std::wstring& name, const std::wstring& value);
    VOID GetDialogFontInfo(LOGFONTW* plFont);

    void DisplayPermissionText();
    void ShowAllPermissionButtons();

    void ResizeDialogBox();
    void ShowExpirationDateText();

private:
    std::wstring            _file;
    bool                    _readonly;
    //classify::CClassifyData _clsdata;
    HICON                   _hIcon;
    HFONT                   _hFontTitle0;
    HFONT                   _hFontTitle;
    HWND                    _tooltip;
    //CClassifyListCtrl       _listCtl;
    HIMAGELIST              _listImgList;
    bool                    _emptyAddress;
    POINT                   _ptInitEditPos;
    SIZE                    _initEditSize;
    bool                    _bAddressSpaceFull;
    int                     _minimalAddrWidth;
    //int                     _addressHeight;
    int                     _currentY;
    vector<wchar_t>         _resultString;
    wstring                 _jsonString;
    bool					_bAssignRights;
    //bool					_bAssignWatermark;
    UINT64					_uiDesitedRights;
    bool					_bHasWatermark;
    HFONT                   _hBoldFont; //Make bold font to controls, only can deleted at destroy time
    int                     _dlgType;   //0--Share, 1--Protect, 3 - property
    LPWSTR                  _fileAddresses;


    bool                    _bSharePermissions[TOTAL_PERMISSIONS];

    bool                    _bShowPermissions;
    bool                    _bShowExpiration;
    SYSTEMTIME              _sysTime;
    int                     _dateType;
    std::wstring            _addressStr;
    //list<CDlgEmailAddr*> _addressList;
    //bool                    _bScroll;

    //Following variables are for scroll
    HCURSOR m_hCursor1, m_hCursor2;

	NXProjData	&_projData;
    // dialog size as you see in the resource view (original size)

};


#endif