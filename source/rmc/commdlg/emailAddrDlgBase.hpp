

#ifndef __NXRM_COMMONUI_EMAILADDRDLGBASE_HPP__
#define __NXRM_COMMONUI_EMAILADDRDLGBASE_HPP__

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
//#include "dlgshare.hpp"
#include "dlgaddress.hpp"
#include <vector>
#include "commondlg.hpp"

using namespace std;


//Following are the control state variables

const int               ADDRESS_MARGIN = 3;


class CEmailAddrDlgBase : public CDlgTemplate
{
public:
	//CEmailAddrDlgBase();
	//CEmailAddrDlgBase(const std::wstring & file, NXSharingData & sharingData, NXSharingData * pSharingDataOut);
	CEmailAddrDlgBase(int nDlgID, bool _bEmptyIsValid = false);
	//CEmailAddrDlgBase(const std::wstring& file, const int dlgID, NXSharingData &sharingData);
    virtual ~CEmailAddrDlgBase();

 


    virtual BOOL OnInitialize();
    bool IsAllAddressValid();
    //void EnableShareButton();
    bool ResizeWindowToFit(int addHeight);
    //void EnlargeWindowToFit();
    bool FindEmailInThelist(const wchar_t * pszAddr);
    void CreateAddressWindow();
    void CreateAddressWindow(const wchar_t * emailAddr);
    BOOL DeleteLastAddress();
    void ReArrangeAddresses();
    void SetEditFocus();
    LRESULT OnSubclassEditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual BOOL OnNotify(_In_ LPNMHDR lpnmhdr);
    virtual BOOL OnCommand(WORD notify, WORD id, HWND hwnd);
    virtual BOOL OnVScroll(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnMouseWheel(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnOwnerDraw(DRAWITEMSTRUCT* lpDrawItemStruct);
    virtual BOOL OnMeasureItem(MEASUREITEMSTRUCT* lpMeasureItemStruct);
    virtual INT_PTR OnRestWMMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual BOOL OnLButtonDown(WPARAM wParam, int x, int y);
    virtual BOOL OnPaint();
    virtual BOOL OnSize(WPARAM wParam, LPARAM lParam);
    virtual void OnDestroy();


    SCROLLINFO SetScrollbarInfo();
	



    //void OnBtnClickNext();
    //void OnBtnClickBack();
    void OnComboBoxSelChanged();
    virtual void OnOk();
    //bool GetSharedData();
    //bool CreateJsonData();
    //bool AddPermissionToVector(const std::wstring & str);
    void RemoveAddress(CDlgEmailAddr *p);
    int GetResult(const wchar_t **pResult);
	//NXSharingData & GetResult();
	//int GetJsonResult(const wchar_t ** pResult);
    POINT GetInitialEditPos() {
        return _ptInitEditPos;
    }

	bool GetDataIntoVector();

    wchar_t _enterPrompt[100] = L"Add email addresses..."; //This string will be loaded 


protected:
    /*void SetPage();
    void ShowSummary();
    BOOL InitClassifyItem(const std::wstring& name, const std::wstring& value);
    VOID GetDialogFontInfo(LOGFONTW* plFont);

    void DisplayPermissionText();
    void ShowAllPermissionButtons();

    void ResizeDialogBox();
    void ShowExpirationDateText();*/
    void RepositionEditControl();
    void SetEditTextColor(COLORREF rgbText);
    void SetEditClean(HWND hDlg);
    int ParsingPasteString(wchar_t *szAddrStr);
    //void CollapseExpiration();
    //void MoveRightsControls(int deltaY);
    //void MoveControlsBelowExpiration(int deltaY);
    //void SetStringToAllControls(HWND hDlg);
	list<CDlgEmailAddr*> _addressList;
	bool                    _bClean;


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
    int                     _addressHeight;
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


    std::wstring            _addressStr;
    bool                    _bScroll;
    bool                    _bEmptyIsValid;

    //Following variables are for scroll
    HCURSOR m_hCursor1, m_hCursor2;

    // dialog size as you see in the resource view (original size)
    RECT	m_rcOriginalRect;

    // dragging
    BOOL	m_bDragging;
    POINT	m_ptDragPoint;


    // actual scroll position
    int		m_nScrollPos;

    // actual dialog height
    int		m_nCurHeight;
    bool    m_bPasteFlag;
    bool    m_bRet;

	//NXSharingData m_sharingData;

	//NXSharingData *m_pSharingDataOut;

};

extern wchar_t _enterPrompt[100];

#endif