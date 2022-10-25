

#ifndef __NXRM_COMMONUI_DLGSHARE_HPP__
#define __NXRM_COMMONUI_DLGSHARE_HPP__

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

//Following are the control state variables
enum {
    _FullControl = 0,
    _View,
    _Edit,
    _Print,
    _CopyPaste,
    _SaveAs,
    _RemoveProtection,
    _ScreenCapture,
    _Send,
    _Classify
};




const wstring           _permissionName[TOTAL_PERMISSIONS] = { L"view",L"edit",L"print",L"share",L"download",L"copy_paste",L"save_as",L"remove_protection",L"screen_capture",L"send",L"classify" };
const int               _nPermissionControlIds[TOTAL_PERMISSIONS] = { IDC_CHECK_VIEW, IDC_CHECK_EDIT, IDC_CHECK_PRINT, IDC_CHECK_SHARE, IDC_CHECK_COPY_PASTE,
IDC_CHECK_SAVEAS, IDC_CHECK_REMOVE_PROTECTION, IDC_CHECK_SCREEN_CAPTURE, IDC_CHECK_SEND, IDC_CHECK_CLASSIFY };
//const int               _nExpireControlIds[TOTAL_PERMISSIONS] = { IDC_RADIO_EXPIRE_AFTER_ACCESS, IDC_EDIT_EXPIRATION_TIME, IDC_SPIN_EXPIRATION, IDC_COMBO_UNITS, IDC_RADIO_EXPIRE_DATE,
//                                                                IDC_DATETIMEPICKER_EXPIRARION };

const int               _nAllExpireControlIds[TOTAL_EXPIRE_CONTROLS] = { IDC_EXPIRATION, IDC_STATIC_EXPIRATION, IDC_RADIO_EXPIRE_DATE,
IDC_DATETIMEPICKER_EXPIRARION };


const int               _nCollapseControlIds[TOTAL_COLLAPSECONTROLS] = { IDC_SYSLINK_PERMISSION, IDC_STATIC_PERMISSION,
IDC_CHECK_FULL_CONTROL, IDC_CHECK_VIEW, IDC_CHECK_EDIT, IDC_CHECK_PRINT, IDC_CHECK_COPY_PASTE,
IDC_CHECK_SAVEAS, IDC_CHECK_REMOVE_PROTECTION, IDC_CHECK_SCREEN_CAPTURE, IDC_CHECK_SEND, IDC_CHECK_CLASSIFY };

const int               _nMovingCtrlIds[TOTAL_MOVING_CONTROLS] = { IDC_STATIC_RIGHTS, IDC_CHECK_VIEW, IDC_CHECK_EDIT, IDC_CHECK_PRINT,IDC_CHECK_SHARE,IDC_CHECK_WATERMARK,
IDC_EXPIRATION, IDC_STATIC_EXPIRATION,
IDC_RADIO_EXPIRE_DATE, IDC_DATETIMEPICKER_EXPIRARION };


//const int               ADDRESS_MARGIN = 3;



class CDlgShare : public CEmailAddrDlgBase
{
public:
    CDlgShare();
	CDlgShare(const std::wstring & file, NXSharingData & sharingData, NXSharingData * pSharingDataOut);
	CDlgShare(const std::wstring& file, const int dlgID, NXSharingData &sharingData);
    virtual ~CDlgShare();

    //void GetClassificationTags(_Out_ std::vector<std::pair<std::wstring, std::wstring>>& tags);

    inline void SetFile(_In_ const std::wstring& file) throw() { _file = file; }
    //inline void SetReadOnly(_In_ bool readonly) throw() { _readonly = readonly; _listCtl.SetReadOnly(_readonly); }
    //void SetXml(_In_ const std::wstring& xml, _In_ const std::wstring& group) throw();
    //void SetJson(_In_ const std::wstring& ws, _In_ const std::wstring& group) throw();
    //void SetInitialData(_In_ const std::vector<std::pair<std::wstring, std::wstring>>& tags) throw();



    virtual BOOL OnInitialize();
    //bool IsAllAddressValid();
    void EnableShareButton();
    //bool ResizeWindowToFit(int addHeight);
    void EnlargeWindowToFit();
   // bool FindEmailInThelist(const wchar_t * pszAddr);
   // void CreateAddressWindow();
    //void CreateAddressWindow(const wchar_t * emailAddr);
    //void DeleteLastAddress();
    //void ReArrangeAddresses();
    //void SetEditFocus();
    //LRESULT OnSubclassEditProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual BOOL OnNotify(_In_ LPNMHDR lpnmhdr);
    virtual BOOL OnCommand(WORD notify, WORD id, HWND hwnd);
    //virtual BOOL OnVScroll(WPARAM wParam, LPARAM lParam);
    //virtual BOOL OnMouseWheel(WPARAM wParam, LPARAM lParam);
    virtual BOOL OnOwnerDraw(DRAWITEMSTRUCT* lpDrawItemStruct);
    //virtual BOOL OnMeasureItem(MEASUREITEMSTRUCT* lpMeasureItemStruct);
    virtual INT_PTR OnRestWMMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual BOOL OnLButtonDown(WPARAM wParam, int x, int y);
    virtual BOOL OnPaint();
    virtual BOOL OnSize(WPARAM wParam, LPARAM lParam);
    virtual void OnDestroy();


    //SCROLLINFO SetScrollbarInfo();




    //void OnBtnClickNext();
    //void OnBtnClickBack();
    void OnComboBoxSelChanged();
    virtual void OnOk();
    bool GetSharedData();
    //bool CreateJsonData();
    //bool AddPermissionToVector(const std::wstring & str);
    void RemoveAddress(CDlgEmailAddr *p);
    int GetResult(const wchar_t **pResult);
	//NXSharingData & GetResult();
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
    //void RepositionEditControl();
    //void SetEditTextColor(COLORREF rgbText);
    //void SetEditClean(HWND hDlg);
    //int ParsingPasteString(wchar_t *szAddrStr);
    //void CollapseExpiration();
    //void MoveRightsControls(int deltaY);
    //void MoveControlsBelowExpiration(int deltaY);
    //void SetStringToAllControls(HWND hDlg);

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

    // dialog size as you see in the resource view (original size)
    //RECT	m_rcOriginalRect;

    // dragging
    //BOOL	m_bDragging;
   // POINT	m_ptDragPoint;


    // actual scroll position
    //int		m_nScrollPos;

    //// actual dialog height
    //int		m_nCurHeight;
    //bool    m_bPasteFlag;
    //bool    m_bRet;

	NXSharingData m_sharingData;

	NXSharingData *m_pSharingDataOut;

};


#endif