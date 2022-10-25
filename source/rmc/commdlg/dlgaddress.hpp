

#ifndef __NXRM_COMMONUI_DLGADDRESS_HPP__
#define __NXRM_COMMONUI_DLGADDRESS_HPP__

#include <Windows.h>
#include <string>
#include <vector>
#include <memory>

#include "resource.h"
#include "dlgtemplate.hpp"
#include <string>
//#include "dlgshare.hpp"



class CDlgEmailAddr : public CDlgTemplate
{
public:
    CDlgEmailAddr();
    CDlgEmailAddr(_In_ const std::wstring& address, _In_ CDlgTemplate *p);
    virtual ~CDlgEmailAddr();
    virtual void OnDestroy();
    inline void SetFile(_In_ const std::wstring& address) throw() {_address = address;}
    
    virtual BOOL OnInitialize();
    void WindowScreenLocation(RECT * addrWndLocation, int nPos);
    BOOL IsTheSameAddress(std::wstring inputAddr);
    virtual BOOL OnNotify(_In_ LPNMHDR lpnmhdr);
    virtual BOOL OnCommand(WORD notify, WORD id, HWND hwnd);
    virtual BOOL OnOwnerDraw(DRAWITEMSTRUCT* lpDrawItemStruct);
    virtual INT_PTR OnRestWMMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual BOOL OnEraseBkGnd(HDC hDC);
    virtual BOOL OnPaint();
    virtual BOOL OnSize(WPARAM wParam, LPARAM lParam);




    virtual void OnOk();
    LRESULT OnSubclassButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
    virtual void OnCancel();

    void SetDisplayLocation(LPPOINT pPt, LPPOINT pPtMax, int nScrollPos);

    bool IsValidAddress() { return _bValidAddress; };

    void WindowScrollLocation(RECT * addrWndLocation);

    void ResizeAddressToFit();

    void MoveWindowLocation(POINT pt, int nPos);
    void ResizeWindow(int nWidth, int nHeight);
    std::wstring GetAddress() { return _address; };



protected:

    void ResizeDialogBox();

private:
    std::wstring            _address;
    bool                    _readonly;
    HICON                   _hIcon;
    HFONT                   _hFontTitle;
    HWND                    _tooltip;
    CDlgTemplate            *_dlgShare;
    bool                    _bValidAddress;
    RECT                    _rcLocation;
    HBRUSH                  _hValidBrush;
    HBRUSH                  _hInvalidBrush;
    int                     _minimalAddrWidth;
    bool                    _bMouseOverButton;
    int                     _nMaxRight;
    int                     _nScrollPos;
    //int                     _nMaxHeight;
};


#endif