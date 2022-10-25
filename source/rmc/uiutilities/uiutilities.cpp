//#ifdef _IN_NXRMC_
//#include "stdafx.h"
//#endif
#include <windows.h>
#include <Commctrl.h>
#include <assert.h>
#include <string>
#include <regex>

#include "..\uiutilities\uiutilities.hpp"



using namespace uiUtilities;


void uiUtilities::DoEvents(HWND hWnd)
{
	MSG stMsg = { 0 };
	while (PeekMessage(&stMsg, hWnd, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&stMsg);
		DispatchMessage(&stMsg);
	}
}
int uiUtilities::RectHeight(RECT& Rect)
{
    return Rect.bottom - Rect.top;
}

int uiUtilities::RectWidth(RECT& Rect)
{
    return Rect.right - Rect.left;
}



int uiUtilities::ScreenToScroll(RECT& Rect, int nScrollPos)
{
    OffsetRect(&Rect, 0, nScrollPos);
    return 0;
}


int uiUtilities::ScrollToScreen(RECT& Rect, int nScrollPos)
{
    OffsetRect(&Rect, 0, -nScrollPos);
    return 0;
}

int uiUtilities::ScreenToScroll(int nScreenY, int nScrollPos)
{
   
    return nScreenY + nScrollPos;
}

int uiUtilities::ScrollToScreen(int nScreenY, int nScrollPos)
{
    return nScreenY - nScrollPos;;
}



BOOL uiUtilities::CenterWindow(HWND hParentWhd, HWND hChildWnd)
{
    HWND hbackWnd = NULL;
    if (hParentWhd == NULL)
    {
        hbackWnd = GetDesktopWindow();
    }
    else
    {
        hbackWnd = hParentWhd;
    }

    RECT rcParent, rcChild;
    GetWindowRect(hbackWnd, &rcParent);
    GetWindowRect(hChildWnd, &rcChild);
    int x, y;
    int w = RectWidth(rcChild);
    int h = RectHeight(rcChild);

    x = rcParent.left + (RectWidth(rcParent) - w) / 2;
    y = rcParent.top + (RectHeight(rcParent) - h) / 2;

    return MoveWindow(hChildWnd, x, y, w, h, TRUE);
}

BOOL uiUtilities::CenterWindowVertically(HWND hParentWhd, HWND hChildWnd)
{
    HWND hbackWnd = NULL;
    if (hParentWhd == NULL)
    {
        hbackWnd = GetDesktopWindow();
    }
    else
    {
        hbackWnd = hParentWhd;
    }

    RECT rcParent, rcChild;
    GetClientRect(hbackWnd, &rcParent);
    GetWindowRect(hChildWnd, &rcChild);
    int x, y;
    int w = RectWidth(rcChild);
    int h = RectHeight(rcChild);

    x = rcChild.left;
    y = rcParent.top + (RectHeight(rcParent) - h) / 2;

    return MoveWindow(hChildWnd, x, y, w, h, TRUE);
}


BOOL uiUtilities::GetWindowRectRelative(HWND hParentWhd, HWND hChildWnd, RECT *pRect)
{
    GetWindowRect(hChildWnd, pRect);
    MapWindowPoints(GetDesktopWindow(), hParentWhd, (LPPOINT)pRect, 2);
    return true;
}




bool uiUtilities::isCharacter(const wchar_t Character)
{
    return ((Character >= 'a' && Character <= 'z') || (Character >= 'A' && Character <= 'Z'));
}

bool uiUtilities::isNumber(const wchar_t Character)
{
    return (Character >= '0' && Character <= '9');
}

bool uiUtilities::isValidEmailAddress(const wchar_t * email)
{
    static const std::wregex pattern(LR"(^(([^<>()[\]\\.,;:\s@\"]+(\.[^<>()[\]\\.,;:\s@\"]+)*)|(\".+\"))@((\[[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\])|(([a-zA-Z\-0-9]+\.)+[a-zA-Z]{2,}))$)");
    if (!email)
        return 0;

    bool result = false;
    try {
        result = std::regex_match(email, pattern);
    }
    catch (...) {
        result = false;
    }
    return result;
}


//std::wstring uiUtilities::LoadStringResource(HINSTANCE _hResource, int nID, const wchar_t *szDefaultString)
//{
//    if (!_hResource)
//    {
//        return szDefaultString;
//    }
//    std::wstring wsText = NX::RES::LoadMessage(_hResource, nID, Max_String_Len, _dwLangId, szDefaultString);
//    return wsText;
//}
//
//std::wstring uiUtilities::LoadStringResource(HINSTANCE _hResource, HINSTANCE _hInstance, int nID, int resID)
//{
//	wchar_t szDefaultString[Max_String_Len];
//	LoadString(_hInstance, resID, szDefaultString, Max_String_Len);
//
//	if (!_hResource)
//	{
//		return szDefaultString;
//	}
//
//	std::wstring wsText = NX::RES::LoadMessage(_hResource, nID, Max_String_Len, _dwLangId, szDefaultString);
//	return wsText;
//}
//
//std::wstring uiUtilities::LoadLocalStringResource(HINSTANCE _hInstance, int resID)
//{
//    wchar_t szDefaultString[Max_String_Len];
//    LoadString(_hInstance, resID, szDefaultString, Max_String_Len);
//
//    return szDefaultString;
//}


#pragma region
/////////////////////////////////////////////////////////////////////////////
// CListCtrl

//BOOL uiUtilities::CListCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
//	UINT nID)
//{
//	// initialize common controls
//	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_LISTVIEW_REG));
//
//	CWnd* pWnd = this;
//	return pWnd->Create(WC_LISTVIEW, NULL, dwStyle, rect, pParentWnd, nID);
//}
//
//BOOL uiUtilities::CListCtrl::CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
//	UINT nID)
//{
//	BOOL bRet = Create(dwStyle, rect, pParentWnd, nID);
//	if (bRet && dwExStyle != 0)
//	{
//		bRet = ModifyStyleEx(0, dwExStyle);
//	}
//	return bRet;
//}

//uiUtilities::CListCtrl::~CListCtrl()
//{
//	DestroyWindow();
//}
//
//BOOL uiUtilities::CListCtrl::GetItemRect(int nItem, LPRECT lpRect, UINT nCode) const
//{
//	ASSERT(::IsWindow(m_hWnd));
//	lpRect->left = nCode;
//	return (BOOL) ::SendMessage(m_hWnd, LVM_GETITEMRECT, (WPARAM)nItem,
//		(LPARAM)lpRect);
//}
//
//BOOL uiUtilities::CListCtrl::SetItemCountEx(int iCount, DWORD dwFlags /* = LVSICF_NOINVALIDATEALL */)
//{
//	ASSERT(::IsWindow(m_hWnd));
//
//	// can't have dwFlags on a control that isn't virutal
//	ASSERT(dwFlags == 0 || (GetStyle() & LVS_OWNERDATA));
//
//	return (BOOL) ::SendMessage(m_hWnd, LVM_SETITEMCOUNT, (WPARAM)iCount,
//		(LPARAM)dwFlags);
//}
//
//CSize uiUtilities::CListCtrl::SetIconSpacing(int cx, int cy)
//{
//	ASSERT(::IsWindow(m_hWnd));
//	DWORD dwRet = (DWORD) ::SendMessage(m_hWnd, LVM_SETICONSPACING,
//		0, (LPARAM)MAKELONG(cx, cy));
//
//	return CSize(dwRet);
//}
//
//CSize uiUtilities::CListCtrl::SetIconSpacing(CSize size)
//{
//	ASSERT(::IsWindow(m_hWnd));
//	DWORD dwRet = (DWORD) ::SendMessage(m_hWnd, LVM_SETICONSPACING,
//		0, (LPARAM)MAKELONG(size.cx, size.cy));
//
//	return CSize(dwRet);
//}
//
//BOOL uiUtilities::CListCtrl::GetSubItemRect(int iItem, int iSubItem, int nArea, CRect& ref) const
//{
//	ASSERT(::IsWindow(m_hWnd));
//	ASSERT(nArea == LVIR_BOUNDS || nArea == LVIR_ICON || nArea == LVIR_LABEL || nArea == LVIR_SELECTBOUNDS);
//
//	RECT rect;
//	rect.top = iSubItem;
//	rect.left = nArea;
//	BOOL bRet = (BOOL) ::SendMessage(m_hWnd, LVM_GETSUBITEMRECT,
//		iItem, (LPARAM)&rect);
//
//	if (bRet)
//		ref = rect;
//	return bRet;
//}
//
int uiUtilities::CListCtrl::InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat,
	int nWidth, int nSubItem)
{
	LVCOLUMN column;
	column.mask = LVCF_TEXT | LVCF_FMT;
	column.pszText = (LPTSTR)lpszColumnHeading;
	column.fmt = nFormat;
	if (nWidth != -1)
	{
		column.mask |= LVCF_WIDTH;
		column.cx = nWidth;
	}
	if (nSubItem != -1)
	{
		column.mask |= LVCF_SUBITEM;
		column.iSubItem = nSubItem;
	}
	return uiUtilities::CListCtrl::InsertColumn(nCol, &column);
}
//
int uiUtilities::CListCtrl::InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask,
	int nImage, LPARAM lParam)
{
	ASSERT(::IsWindow(m_hWnd));
	LVITEM item;
	item.mask = nMask;
	item.iItem = nItem;
	item.iSubItem = 0;
	item.pszText = (LPTSTR)lpszItem;
	item.state = nState;
	item.stateMask = nStateMask;
	item.iImage = nImage;
	item.lParam = lParam;
	return uiUtilities::CListCtrl::InsertItem(&item);
}

//int uiUtilities::CListCtrl::HitTest(CPoint pt, UINT* pFlags) const
//{
//	ASSERT(::IsWindow(m_hWnd));
//	LVHITTESTINFO hti;
//	hti.pt = pt;
//	int nRes = (int) ::SendMessage(m_hWnd, LVM_HITTEST, 0, (LPARAM)&hti);
//	if (pFlags != NULL)
//		*pFlags = hti.flags;
//	return nRes;
//}
//
//BOOL uiUtilities::CListCtrl::SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem,
//	int nImage, UINT nState, UINT nStateMask, LPARAM lParam)
//{
//	ASSERT((nMask & LVIF_INDENT) == 0);
//	return SetItem(nItem, nSubItem, nMask, lpszItem, nImage, nState, nStateMask, lParam, 0);
//}
//
//BOOL uiUtilities::CListCtrl::SetItem(int nItem, int nSubItem, UINT nMask, LPCTSTR lpszItem,
//	int nImage, UINT nState, UINT nStateMask, LPARAM lParam, int nIndent)
//{
//	ASSERT(::IsWindow(m_hWnd));
//	ASSERT((GetStyle() & LVS_OWNERDATA) == 0);
//
//	LVITEM lvi;
//	lvi.mask = nMask;
//	lvi.iItem = nItem;
//	lvi.iSubItem = nSubItem;
//	lvi.stateMask = nStateMask;
//	lvi.state = nState;
//	lvi.pszText = (LPTSTR)lpszItem;
//	lvi.iImage = nImage;
//	lvi.lParam = lParam;
//	lvi.iIndent = nIndent;
//
//	return (BOOL) ::SendMessage(m_hWnd, LVM_SETITEM, 0, (LPARAM)&lvi);
//}
//
//BOOL uiUtilities::CListCtrl::SetItemState(int nItem, UINT nState, UINT nStateMask)
//{
//	ASSERT(::IsWindow(m_hWnd));
//	LVITEM lvi;
//	lvi.stateMask = nStateMask;
//	lvi.state = nState;
//	return (BOOL) ::SendMessage(m_hWnd, LVM_SETITEMSTATE, nItem, (LPARAM)&lvi);
//}
//
BOOL uiUtilities::CListCtrl::SetItemText(int nItem, int nSubItem, LPCTSTR lpszText)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT((GetStyle() & LVS_OWNERDATA) == 0);
	LVITEM lvi;
	lvi.iSubItem = nSubItem;
	lvi.pszText = (LPTSTR)lpszText;
	return (BOOL) ::SendMessage(m_hWnd, LVM_SETITEMTEXT, nItem, (LPARAM)&lvi);
}
//
//CString uiUtilities::CListCtrl::GetItemText(int nItem, int nSubItem) const
//{
//	ASSERT(::IsWindow(m_hWnd));
//	LVITEM lvi;
//	memset(&lvi, 0, sizeof(LVITEM));
//	lvi.iSubItem = nSubItem;
//	CString str;
//	int nLen = 128;
//	int nRes;
//	do
//	{
//		nLen *= 2;
//		lvi.cchTextMax = nLen;
//		lvi.pszText = str.GetBufferSetLength(nLen);
//		nRes = (int)::SendMessage(m_hWnd, LVM_GETITEMTEXT, (WPARAM)nItem,
//			(LPARAM)&lvi);
//	} while (nRes >= nLen - 1);
//	str.ReleaseBuffer();
//	return str;
//}
//
//int uiUtilities::CListCtrl::GetItemText(_In_ int nItem, _In_ int nSubItem, _Out_writes_to_(nLen, return +1) LPTSTR lpszText, _In_ int nLen) const
//{
//	ASSERT(::IsWindow(m_hWnd));
//	LVITEM lvi;
//	memset(&lvi, 0, sizeof(LVITEM));
//	lvi.iSubItem = nSubItem;
//	lvi.cchTextMax = nLen;
//	lvi.pszText = lpszText;
//	return (int)::SendMessage(m_hWnd, LVM_GETITEMTEXT, (WPARAM)nItem,
//		(LPARAM)&lvi);
//}
//
//DWORD_PTR uiUtilities::CListCtrl::GetItemData(int nItem) const
//{
//	ASSERT(::IsWindow(m_hWnd));
//	LVITEM lvi;
//	memset(&lvi, 0, sizeof(LVITEM));
//	lvi.iItem = nItem;
//	lvi.mask = LVIF_PARAM;
//	VERIFY(::SendMessage(m_hWnd, LVM_GETITEM, 0, (LPARAM)&lvi));
//	return lvi.lParam;
//}
//
//void uiUtilities::CListCtrl::DrawItem(LPDRAWITEMSTRUCT)
//{
//	ASSERT(FALSE);
//}
//
//BOOL uiUtilities::CListCtrl::OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam,
//	LRESULT* pResult)
//{
//	if (message != WM_DRAWITEM)
//		return CWnd::OnChildNotify(message, wParam, lParam, pResult);
//
//	ASSERT(pResult == NULL);       // no return value expected
//	UNUSED(pResult); // unused in release builds
//
//	DrawItem((LPDRAWITEMSTRUCT)lParam);
//	return TRUE;
//}
//
//void uiUtilities::CListCtrl::RemoveImageList(int nImageList)
//{
//	HIMAGELIST h = (HIMAGELIST)SendMessage(LVM_GETIMAGELIST,
//		(WPARAM)nImageList);
//	if (uiUtilities::CImageList::FromHandlePermanent(h) != NULL)
//		SendMessage(LVM_SETIMAGELIST, (WPARAM)nImageList, NULL);
//}
//
//void uiUtilities::CListCtrl::OnNcDestroy()
//{
//	RemoveImageList(LVSIL_NORMAL);
//	RemoveImageList(LVSIL_SMALL);
//	RemoveImageList(LVSIL_STATE);
//
//	CWnd::OnNcDestroy();
//}
//
//uiUtilities::CImageList* uiUtilities::CListCtrl::CreateDragImage(int nItem, LPPOINT lpPoint)
//{
//	ASSERT(::IsWindow(m_hWnd));
//
//	HIMAGELIST hImageList = (HIMAGELIST)::SendMessage(m_hWnd,
//		LVM_CREATEDRAGIMAGE, nItem, (LPARAM)lpPoint);
//	if (hImageList == NULL)
//		return NULL;
//
//	uiUtilities::CImageList* pImageList = new uiUtilities::CImageList;
//	VERIFY(pImageList->Attach(hImageList));
//	return pImageList;
//}
//#pragma endregion


BOOL uiUtilities::CDateTimeCtrl::SetTime(_In_ LPSYSTEMTIME pTimeNew /* = NULL */)
{
	ASSERT(::IsWindow(m_hWnd));
	WPARAM wParam = (pTimeNew == NULL) ? GDT_NONE : GDT_VALID;
	return (BOOL) ::SendMessage(m_hWnd, DTM_SETSYSTEMTIME,
		wParam, (LPARAM)pTimeNew);
}


/////////////////////////////////////////////////////////////////////////////
// uiUtilities::CImageList

//CHandleMap* PASCAL afxMapHIMAGELIST(BOOL bCreate)
//{
//	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState();
//	if (pState->m_pmapHIMAGELIST == NULL && bCreate)
//	{
//		BOOL bEnable = AfxEnableMemoryTracking(FALSE);
//		_PNH pnhOldHandler = AfxSetNewHandler(&AfxCriticalNewHandler);
//
//		pState->m_pmapHIMAGELIST = new CHandleMap(RUNTIME_CLASS(uiUtilities::CImageList),
//			ConstructDestruct<uiUtilities::CImageList>::Construct, ConstructDestruct<uiUtilities::CImageList>::Destruct,
//			offsetof(uiUtilities::CImageList, m_hImageList));
//
//		AfxSetNewHandler(pnhOldHandler);
//		AfxEnableMemoryTracking(bEnable);
//	}
//	return pState->m_pmapHIMAGELIST;
//}
//
uiUtilities::CImageList::CImageList()
{
	m_hImageList = NULL;
}

uiUtilities::CImageList::~CImageList()
{

		DeleteImageList();

}

//HIMAGELIST uiUtilities::CImageList::Detach()
//{
//	HIMAGELIST hImageList = m_hImageList;
//	if (hImageList != NULL)
//	{
//		CHandleMap* pMap = afxMapHIMAGELIST();
//		if (pMap != NULL)
//			pMap->RemoveHandle(m_hImageList);
//	}
//
//	m_hImageList = NULL;
//	return hImageList;
//}

BOOL uiUtilities::CImageList::DeleteImageList()
{
	if (m_hImageList == NULL)
		return FALSE;
	auto bRet = ImageList_Destroy(m_hImageList);
	m_hImageList = bRet ? NULL : m_hImageList;
	return bRet;
}

//uiUtilities::CImageList* PASCAL uiUtilities::CImageList::FromHandle(HIMAGELIST h)
//{
//	CHandleMap* pMap = afxMapHIMAGELIST(TRUE);
//	ASSERT(pMap != NULL);
//	uiUtilities::CImageList* pImageList = (uiUtilities::CImageList*)pMap->FromHandle(h);
//	ASSERT(pImageList == NULL || pImageList->m_hImageList == h);
//	return pImageList;
//}
//
//uiUtilities::CImageList* PASCAL uiUtilities::CImageList::FromHandlePermanent(HIMAGELIST h)
//{
//	CHandleMap* pMap = afxMapHIMAGELIST();
//	uiUtilities::CImageList* pImageList = NULL;
//	if (pMap != NULL)
//	{
//		// only look in the permanent map - does no allocations
//		pImageList = (uiUtilities::CImageList*)pMap->LookupPermanent(h);
//		ASSERT(pImageList == NULL || pImageList->m_hImageList == h);
//	}
//	return pImageList;
//}

BOOL uiUtilities::CImageList::Create(int cx, int cy, UINT nFlags, int nInitial, int nGrow)
{
	m_hImageList = ImageList_Create(cx, cy, nFlags, nInitial, nGrow);
	return m_hImageList != NULL;
}

//BOOL uiUtilities::CImageList::Create(UINT nBitmapID, int cx, int nGrow, COLORREF crMask)
//{
//	ASSERT(HIWORD(nBitmapID) == 0);
//	HINSTANCE hInst = AfxFindResourceHandle((LPCTSTR)(DWORD_PTR)nBitmapID, RT_BITMAP);
//	ASSERT(hInst != NULL);
//	return Attach(ImageList_LoadBitmap(hInst,
//		(LPCTSTR)(DWORD_PTR)nBitmapID, cx, nGrow, crMask));
//}
//
//BOOL uiUtilities::CImageList::Create(LPCTSTR lpszBitmapID, int cx, int nGrow,
//	COLORREF crMask)
//{
//	HINSTANCE hInst = AfxFindResourceHandle(lpszBitmapID, RT_BITMAP);
//	ASSERT(hInst != NULL);
//	return Attach(ImageList_LoadBitmap(hInst, lpszBitmapID, cx, nGrow, crMask));
//}
//
//BOOL uiUtilities::CImageList::Create(uiUtilities::CImageList& imagelist1, int nImage1,
//	uiUtilities::CImageList& imagelist2, int nImage2, int dx, int dy)
//{
//	return Attach(ImageList_Merge(imagelist1.m_hImageList, nImage1,
//		imagelist2.m_hImageList, nImage2, dx, dy));
//}
//
//BOOL uiUtilities::CImageList::Attach(HIMAGELIST hImageList)
//{
//	ASSERT(m_hImageList == NULL);      // only attach once, detach on destroy
//	ASSERT(FromHandlePermanent(hImageList) == NULL);
//
//	if (hImageList == NULL)
//		return FALSE;
//
//	CHandleMap* pMap = afxMapHIMAGELIST(TRUE);
//	ASSERT(pMap != NULL);
//
//	pMap->SetPermanent(m_hImageList = hImageList, this);
//	return TRUE;
//}
//
//BOOL uiUtilities::CImageList::Read(CArchive* pArchive)
//{
//	ASSERT(m_hImageList == NULL);
//	ASSERT(pArchive != NULL);
//	ASSERT(pArchive->IsLoading());
//	CArchiveStream arcstream(pArchive);
//
//	m_hImageList = ImageList_Read(&arcstream);
//	return (m_hImageList != NULL);
//}
//
//BOOL uiUtilities::CImageList::Write(CArchive* pArchive)
//{
//	ASSERT(m_hImageList != NULL);
//	ASSERT(pArchive != NULL);
//	ASSERT(pArchive->IsStoring());
//	CArchiveStream arcstream(pArchive);
//	return ImageList_Write(m_hImageList, &arcstream);
//}
//
//#ifdef _DEBUG
//void uiUtilities::CImageList::Dump(CDumpContext& dc) const
//{
//	CObject::Dump(dc);
//
//	dc << "m_hImageList = " << (void*)m_hImageList;
//	dc << "\n";
//}
//
//void uiUtilities::CImageList::AssertValid() const
//{
//	CObject::AssertValid();
//	if (m_hImageList == NULL)
//		return;
//	// should also be in the permanent or temporary handle map
//	CObject* p = NULL;
//
//	CHandleMap* pMap = afxMapHIMAGELIST();
//	ASSERT(pMap != NULL);
//
//	if (pMap)
//	{
//		ASSERT((p = pMap->LookupPermanent(m_hImageList)) != NULL ||
//			(p = pMap->LookupTemporary(m_hImageList)) != NULL);
//	}
//	ASSERT((uiUtilities::CImageList*)p == this);   // must be us
//}

/////////////////////////////////////////////////////////////////////////////
// CComboBoxEx


//BOOL uiUtilities::CComboBoxEx::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
//	UINT nID)
//{
//	// initialize common controls
//	VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_USEREX_REG));
//
//	CWnd* pWnd = this;
//	return pWnd->Create(WC_COMBOBOXEX, NULL, dwStyle, rect, pParentWnd, nID);
//}
//
//BOOL uiUtilities::CComboBoxEx::CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT& rect,
//	CWnd* pParentWnd, UINT nID)
//{
//	BOOL bRet = Create(dwStyle, rect, pParentWnd, nID);
//	if (bRet && dwExStyle != 0)
//	{
//		bRet = ModifyStyleEx(0, dwExStyle);
//	}
//	return bRet;
//}

int uiUtilities::CComboBoxEx::DeleteItem(int iIndex)
{
	ASSERT(::IsWindow(m_hWnd));
	return (int) ::SendMessage(m_hWnd, CBEM_DELETEITEM, (WPARAM)iIndex, 0);
}

BOOL uiUtilities::CComboBoxEx::GetItem(COMBOBOXEXITEM* pCBItem)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pCBItem != NULL);
	//ASSERT(AfxIsValidAddress(pCBItem, sizeof(COMBOBOXEXITEM)));

	return (int) ::SendMessage(m_hWnd, CBEM_GETITEM, 0, (LPARAM)pCBItem);
}

int uiUtilities::CComboBoxEx::InsertItem(const COMBOBOXEXITEM* pCBItem)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pCBItem != NULL);
	//ASSERT(AfxIsValidAddress(pCBItem, sizeof(COMBOBOXEXITEM), FALSE));

	return (int) ::SendMessage(m_hWnd, CBEM_INSERTITEM, 0, (LPARAM)pCBItem);
}

BOOL uiUtilities::CComboBoxEx::SetItem(const COMBOBOXEXITEM* pCBItem)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(pCBItem != NULL);
	//ASSERT(AfxIsValidAddress(pCBItem, sizeof(COMBOBOXEXITEM), FALSE));

	return (int) ::SendMessage(m_hWnd, CBEM_SETITEM, 0, (LPARAM)pCBItem);
}

uiUtilities::CComboBoxEx::~CComboBoxEx()
{
	//DestroyWindow();
}
