#include <windows.h>
#include <commctrl.h>
#include "mfcutil2.h"
#include <string>

using namespace std;
using namespace uiUtilities;

#ifndef ASSERT
#define ASSERT(f) assert(f)
#endif

/////////////////////////////////////////////////////////////////////////////
// CTreeCtrl

//BEGIN_MESSAGE_MAP(CTreeCtrl, CWnd)
//	ON_WM_DESTROY()
//END_MESSAGE_MAP()



_AFXCMN_INLINE uiUtilities::CTreeCtrl::CTreeCtrl()
{ }
_AFXCMN_INLINE HTREEITEM uiUtilities::CTreeCtrl::InsertItem(_In_ LPTVINSERTSTRUCT lpInsertStruct)
{
	ASSERT(::IsWindow(m_hWnd));  return (HTREEITEM)::SendMessage(m_hWnd, TVM_INSERTITEM, 0, (LPARAM)lpInsertStruct);
}
//_AFXCMN_INLINE HTREEITEM uiUtilities::CTreeCtrl::InsertItem(_In_z_ LPCTSTR lpszItem, _In_ int nImage,
//	_In_ int nSelectedImage, _In_ HTREEITEM hParent, _In_ HTREEITEM hInsertAfter)
//{
//	ASSERT(::IsWindow(m_hWnd)); return InsertItem(TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE, lpszItem, nImage, nSelectedImage, 0, 0, 0, hParent, hInsertAfter);
//}
//_AFXCMN_INLINE HTREEITEM uiUtilities::CTreeCtrl::InsertItem(_In_z_ LPCTSTR lpszItem, _In_ HTREEITEM hParent, _In_ HTREEITEM hInsertAfter)
//{
//	ASSERT(::IsWindow(m_hWnd)); return InsertItem(TVIF_TEXT, lpszItem, 0, 0, 0, 0, 0, hParent, hInsertAfter);
//}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::DeleteItem(_In_ HTREEITEM hItem)
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_DELETEITEM, 0, (LPARAM)hItem);
}
#pragma warning(push)
#pragma warning(disable: 4310)
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::DeleteAllItems()
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_DELETEITEM, 0, (LPARAM)TVI_ROOT);
}
#pragma warning(pop)
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::Expand(_In_ HTREEITEM hItem, _In_ UINT nCode)
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_EXPAND, nCode, (LPARAM)hItem);
}
_AFXCMN_INLINE UINT uiUtilities::CTreeCtrl::GetCount() const
{
	ASSERT(::IsWindow(m_hWnd)); return (UINT)::SendMessage(m_hWnd, TVM_GETCOUNT, 0, 0);
}
_AFXCMN_INLINE UINT uiUtilities::CTreeCtrl::GetIndent() const
{
	ASSERT(::IsWindow(m_hWnd)); return (UINT)::SendMessage(m_hWnd, TVM_GETINDENT, 0, 0);
}
_AFXCMN_INLINE void uiUtilities::CTreeCtrl::SetIndent(_In_ UINT nIndent)
{
	ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TVM_SETINDENT, nIndent, 0);
}
//_AFXCMN_INLINE CImageList* uiUtilities::CTreeCtrl::GetImageList(_In_ int nImageList) const
//{
//	ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, TVM_GETIMAGELIST, (WPARAM)nImageList, 0));
//}
//_AFXCMN_INLINE CImageList* uiUtilities::CTreeCtrl::SetImageList(_In_opt_ CImageList* pImageList, _In_ int nImageList)
//{
//	ASSERT(::IsWindow(m_hWnd));
//#pragma warning(suppress : 6011) // GetSafeHandle does the "right" thing even when called on a null pointer, so the warning is spurious.
//	return CImageList::FromHandle((HIMAGELIST)::SendMessage(m_hWnd, TVM_SETIMAGELIST, (WPARAM)nImageList, (LPARAM)pImageList->GetSafeHandle()));
//}
_AFXCMN_INLINE UINT uiUtilities::CTreeCtrl::SetScrollTime(_In_ UINT uScrollTime)
{
	ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, TVM_SETSCROLLTIME, (WPARAM)uScrollTime, 0);
}
_AFXCMN_INLINE HTREEITEM uiUtilities::CTreeCtrl::GetNextItem(_In_ HTREEITEM hItem, _In_ UINT nCode) const
{
	ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, nCode, (LPARAM)hItem);
}
_AFXCMN_INLINE HTREEITEM uiUtilities::CTreeCtrl::GetChildItem(_In_ HTREEITEM hItem) const
{
	ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_CHILD, (LPARAM)hItem);
}
_AFXCMN_INLINE HTREEITEM uiUtilities::CTreeCtrl::GetNextSiblingItem(_In_ HTREEITEM hItem) const
{
	ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_NEXT, (LPARAM)hItem);
}
_AFXCMN_INLINE HTREEITEM uiUtilities::CTreeCtrl::GetPrevSiblingItem(_In_ HTREEITEM hItem) const
{
	ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_PREVIOUS, (LPARAM)hItem);
}
_AFXCMN_INLINE HTREEITEM uiUtilities::CTreeCtrl::GetParentItem(_In_ HTREEITEM hItem) const
{
	ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_PARENT, (LPARAM)hItem);
}
_AFXCMN_INLINE HTREEITEM uiUtilities::CTreeCtrl::GetFirstVisibleItem() const
{
	ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_FIRSTVISIBLE, 0);
}
_AFXCMN_INLINE HTREEITEM uiUtilities::CTreeCtrl::GetNextVisibleItem(_In_ HTREEITEM hItem) const
{
	ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_NEXTVISIBLE, (LPARAM)hItem);
}
_AFXCMN_INLINE HTREEITEM uiUtilities::CTreeCtrl::GetPrevVisibleItem(_In_ HTREEITEM hItem) const
{
	ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_PREVIOUSVISIBLE, (LPARAM)hItem);
}
_AFXCMN_INLINE HTREEITEM uiUtilities::CTreeCtrl::GetLastVisibleItem() const
{
	ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_LASTVISIBLE, 0);
}
_AFXCMN_INLINE HTREEITEM uiUtilities::CTreeCtrl::GetSelectedItem() const
{
	ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_CARET, 0);
}
_AFXCMN_INLINE UINT uiUtilities::CTreeCtrl::GetScrollTime() const
{
	ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, TVM_GETSCROLLTIME, 0, 0);
}
_AFXCMN_INLINE HTREEITEM uiUtilities::CTreeCtrl::GetDropHilightItem() const
{
	ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_DROPHILITE, 0);
}
_AFXCMN_INLINE HTREEITEM uiUtilities::CTreeCtrl::GetRootItem() const
{
	ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_GETNEXTITEM, TVGN_ROOT, 0);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::Select(_In_ HTREEITEM hItem, _In_ UINT nCode)
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_SELECTITEM, nCode, (LPARAM)hItem);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::SelectItem(_In_opt_ HTREEITEM hItem)
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TVM_SELECTITEM, TVGN_CARET, (LPARAM)hItem);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::SelectDropTarget(_In_opt_ HTREEITEM hItem)
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TVM_SELECTITEM, TVGN_DROPHILITE, (LPARAM)hItem);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::SelectSetFirstVisible(_In_ HTREEITEM hItem)
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TVM_SELECTITEM, TVGN_FIRSTVISIBLE, (LPARAM)hItem);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::GetItem(_Out_ TVITEM* pItem) const
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)pItem);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::SetItem(_In_ TVITEM* pItem)
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_SETITEM, 0, (LPARAM)pItem);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::SetItemText(_In_ HTREEITEM hItem, _In_z_ LPCTSTR lpszItem)
{
	ASSERT(::IsWindow(m_hWnd)); return SetItem(hItem, TVIF_TEXT, lpszItem, 0, 0, 0, 0, NULL);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::SetItemImage(_In_ HTREEITEM hItem, _In_ int nImage, _In_ int nSelectedImage)
{
	ASSERT(::IsWindow(m_hWnd)); return SetItem(hItem, TVIF_IMAGE | TVIF_SELECTEDIMAGE, NULL, nImage, nSelectedImage, 0, 0, NULL);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::SetItemState(_In_ HTREEITEM hItem, _In_ UINT nState, _In_ UINT nStateMask)
{
	ASSERT(::IsWindow(m_hWnd)); return SetItem(hItem, TVIF_STATE, NULL, 0, 0, nState, nStateMask, NULL);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::SetItemData(_In_ HTREEITEM hItem, _In_ DWORD_PTR dwData)
{
	ASSERT(::IsWindow(m_hWnd)); return SetItem(hItem, TVIF_PARAM, NULL, 0, 0, 0, 0, (LPARAM)dwData);
}
//_AFXCMN_INLINE CEdit* uiUtilities::CTreeCtrl::EditLabel(_In_ HTREEITEM hItem)
//{
//	ASSERT(::IsWindow(m_hWnd)); return (CEdit*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TVM_EDITLABEL, 0, (LPARAM)hItem));
//}
_AFXCMN_INLINE HTREEITEM uiUtilities::CTreeCtrl::HitTest(_In_ TVHITTESTINFO* pHitTestInfo) const
{
	ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM)::SendMessage(m_hWnd, TVM_HITTEST, 0, (LPARAM)pHitTestInfo);
}
//_AFXCMN_INLINE CEdit* uiUtilities::CTreeCtrl::GetEditControl() const
//{
//	ASSERT(::IsWindow(m_hWnd)); return (CEdit*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TVM_GETEDITCONTROL, 0, 0));
//}
_AFXCMN_INLINE UINT uiUtilities::CTreeCtrl::GetVisibleCount() const
{
	ASSERT(::IsWindow(m_hWnd)); return (UINT)::SendMessage(m_hWnd, TVM_GETVISIBLECOUNT, 0, 0);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::SortChildren(_In_opt_ HTREEITEM hItem)
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_SORTCHILDREN, 0, (LPARAM)hItem);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::EnsureVisible(_In_ HTREEITEM hItem)
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_ENSUREVISIBLE, 0, (LPARAM)hItem);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::SortChildrenCB(_In_ LPTVSORTCB pSort)
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, TVM_SORTCHILDRENCB, 0, (LPARAM)pSort);
}
//_AFXCMN_INLINE CToolTipCtrl* uiUtilities::CTreeCtrl::GetToolTips() const
//{
//	ASSERT(::IsWindow(m_hWnd)); return (CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TVM_GETTOOLTIPS, 0, 0L));
//}
//_AFXCMN_INLINE CToolTipCtrl* uiUtilities::CTreeCtrl::SetToolTips(_In_ CToolTipCtrl* pWndTip)
//{
//	ASSERT(::IsWindow(m_hWnd)); return (CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, TVM_SETTOOLTIPS, (WPARAM)pWndTip->GetSafeHwnd(), 0L));
//}
_AFXCMN_INLINE COLORREF uiUtilities::CTreeCtrl::GetBkColor() const
{
	ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, TVM_GETBKCOLOR, 0, 0L);
}
_AFXCMN_INLINE COLORREF uiUtilities::CTreeCtrl::SetBkColor(_In_ COLORREF clr)
{
	ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, TVM_SETBKCOLOR, 0, (LPARAM)clr);
}
_AFXCMN_INLINE SHORT uiUtilities::CTreeCtrl::GetItemHeight() const
{
	ASSERT(::IsWindow(m_hWnd)); return (SHORT) ::SendMessage(m_hWnd, TVM_GETITEMHEIGHT, 0, 0L);
}
_AFXCMN_INLINE SHORT uiUtilities::CTreeCtrl::SetItemHeight(_In_ SHORT cyHeight)
{
	ASSERT(::IsWindow(m_hWnd)); return (SHORT) ::SendMessage(m_hWnd, TVM_SETITEMHEIGHT, cyHeight, 0L);
}
_AFXCMN_INLINE COLORREF uiUtilities::CTreeCtrl::GetTextColor() const
{
	ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, TVM_GETTEXTCOLOR, 0, 0L);
}
_AFXCMN_INLINE COLORREF uiUtilities::CTreeCtrl::SetTextColor(_In_ COLORREF clr)
{
	ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, TVM_SETTEXTCOLOR, 0, (LPARAM)clr);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::SetInsertMark(_In_ HTREEITEM hItem, _In_ BOOL fAfter)
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TVM_SETINSERTMARK, fAfter, (LPARAM)hItem);
}
_AFXCMN_INLINE COLORREF uiUtilities::CTreeCtrl::GetInsertMarkColor() const
{
	ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, TVM_GETINSERTMARKCOLOR, 0, 0L);
}
_AFXCMN_INLINE COLORREF uiUtilities::CTreeCtrl::SetInsertMarkColor(_In_ COLORREF clrNew)
{
	ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, TVM_SETINSERTMARKCOLOR, 0, (LPARAM)clrNew);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::EndEditLabelNow(_In_ BOOL fCancelWithoutSave)
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TVM_ENDEDITLABELNOW, (WPARAM)fCancelWithoutSave, 0);
}

_AFXCMN_INLINE COLORREF uiUtilities::CTreeCtrl::GetLineColor() const
{
	ASSERT(::IsWindow(m_hWnd)); return (COLORREF)::SendMessage(m_hWnd, TVM_GETLINECOLOR, 0, 0L);
}
_AFXCMN_INLINE COLORREF uiUtilities::CTreeCtrl::SetLineColor(_In_ COLORREF clrNew /*= CLR_DEFAULT*/)
{
	ASSERT(::IsWindow(m_hWnd)); return (COLORREF)::SendMessage(m_hWnd, TVM_SETLINECOLOR, 0, (LPARAM)clrNew);
}

#if defined(UNICODE)
_AFXCMN_INLINE UINT uiUtilities::CTreeCtrl::MapItemToAccId(_In_ HTREEITEM hItem) const
{
	ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, TVM_MAPHTREEITEMTOACCID, (WPARAM)hItem, 0);
}
_AFXCMN_INLINE HTREEITEM uiUtilities::CTreeCtrl::MapAccIdToItem(_In_ UINT uAccId) const
{
	ASSERT(::IsWindow(m_hWnd)); return (HTREEITEM) ::SendMessage(m_hWnd, TVM_MAPACCIDTOHTREEITEM, (WPARAM)uAccId, 0);
}
#endif // defined(UNICODE)

#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
_AFXCMN_INLINE UINT uiUtilities::CTreeCtrl::GetSelectedCount()
{
	ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, TVM_GETSELECTEDCOUNT, 0, 0);
}
_AFXCMN_INLINE void uiUtilities::CTreeCtrl::ShowInfoTip(_In_ HTREEITEM hItem)
{
	ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, TVM_SHOWINFOTIP, 0, (LPARAM)hItem);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::SetItemStateEx(_In_ HTREEITEM hItem, _In_ UINT uStateEx)
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL)SetItemEx(hItem, TVIF_STATEEX, 0, 0, 0, 0, 0, 0, uStateEx, 0, 0);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::SetItemExpandedImageIndex(_In_ HTREEITEM hItem, _In_ int nIndex)
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL)SetItemEx(hItem, TVIF_EXPANDEDIMAGE, 0, 0, 0, 0, 0, 0, TVIF_EXPANDEDIMAGE, 0, nIndex);
}
_AFXCMN_INLINE DWORD uiUtilities::CTreeCtrl::GetExtendedStyle() const
{
	ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, TVM_GETEXTENDEDSTYLE, (WPARAM)0, 0);
}
_AFXCMN_INLINE DWORD uiUtilities::CTreeCtrl::SetExtendedStyle(_In_ DWORD dwExMask, _In_ DWORD dwExStyles)
{
	ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, TVM_SETEXTENDEDSTYLE, (WPARAM)dwExMask, (LPARAM)dwExStyles);
}
_AFXCMN_INLINE BOOL uiUtilities::CTreeCtrl::SetAutoscrollInfo(_In_ UINT uPixelsPerSec, _In_ UINT uUpdateTime)
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, TVM_SETAUTOSCROLLINFO, (WPARAM)uPixelsPerSec, (LPARAM)uUpdateTime);
}
#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)


//BOOL uiUtilities::CTreeCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
//	UINT nID)
//{
//	// initialize common controls
//	//VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_TREEVIEW_REG));
//
//	CWnd* pWnd = this;
//	return pWnd->Create(WC_TREEVIEW, NULL, dwStyle, rect, pParentWnd, nID);
//}
//
//BOOL uiUtilities::CTreeCtrl::CreateEx(DWORD dwExStyle, DWORD dwStyle, const RECT& rect,
//	CWnd* pParentWnd, UINT nID)
//{
//	BOOL bRet = Create(dwStyle, rect, pParentWnd, nID);
//	if (bRet && dwExStyle != 0)
//	{
//		bRet = ModifyStyleEx(0, dwExStyle);
//	}
//	return bRet;
//}

uiUtilities::CTreeCtrl::~CTreeCtrl()
{
	//DestroyWindow();
}

BOOL uiUtilities::CTreeCtrl::GetItemRect(HTREEITEM hItem, LPRECT lpRect, BOOL bTextOnly) const
{
	ASSERT(::IsWindow(m_hWnd));
	*(HTREEITEM*)lpRect = hItem;
	return (BOOL)::SendMessage(m_hWnd, TVM_GETITEMRECT, (WPARAM)bTextOnly,
		(LPARAM)lpRect);
}

wstring uiUtilities::CTreeCtrl::GetItemText(HTREEITEM hItem) const
{
	ASSERT(::IsWindow(m_hWnd));
	TVITEM item;
	item.hItem = hItem;
	item.mask = TVIF_TEXT;
	wstring str;
	int nLen = 128;
	int nRes;
	do
	{
		nLen *= 2;
		str.resize(nLen);
		item.pszText = &str[0];
		item.cchTextMax = nLen;
		::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&item);
		nRes = lstrlen(item.pszText);
	} while (nRes >= nLen - 1);
	return str;
}

BOOL uiUtilities::CTreeCtrl::GetItemImage(HTREEITEM hItem, int& nImage, int& nSelectedImage) const
{
	ASSERT(::IsWindow(m_hWnd));
	TVITEM item;
	item.hItem = hItem;
	item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE;
	BOOL bRes = (BOOL)::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&item);
	if (bRes)
	{
		nImage = item.iImage;
		nSelectedImage = item.iSelectedImage;
	}
	return bRes;
}

UINT uiUtilities::CTreeCtrl::GetItemState(HTREEITEM hItem, UINT nStateMask) const
{
	ASSERT(::IsWindow(m_hWnd));
	TVITEM item;
	item.hItem = hItem;
	item.mask = TVIF_STATE;
	item.stateMask = nStateMask;
	item.state = 0;
	VERIFY(::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&item));
	return item.state;
}

DWORD_PTR uiUtilities::CTreeCtrl::GetItemData(HTREEITEM hItem) const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(hItem != NULL);
	TVITEM item;
	item.hItem = hItem;
	item.mask = TVIF_PARAM;
	VERIFY(::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&item));
	return item.lParam;
}

BOOL uiUtilities::CTreeCtrl::ItemHasChildren(HTREEITEM hItem) const
{
	ASSERT(::IsWindow(m_hWnd));
	TVITEM item;
	item.hItem = hItem;
	item.mask = TVIF_CHILDREN;
	::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&item);
	return item.cChildren;
}

BOOL uiUtilities::CTreeCtrl::SetItem(HTREEITEM hItem, UINT nMask, LPCTSTR lpszItem, int nImage,
	int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam)
{
	ASSERT(::IsWindow(m_hWnd));
	TVITEM item;
	item.hItem = hItem;
	item.mask = nMask;
	item.pszText = (LPTSTR)lpszItem;
	item.iImage = nImage;
	item.iSelectedImage = nSelectedImage;
	item.state = nState;
	item.stateMask = nStateMask;
	item.lParam = lParam;
	return (BOOL)::SendMessage(m_hWnd, TVM_SETITEM, 0, (LPARAM)&item);
}

BOOL uiUtilities::CTreeCtrl::SetItemEx(HTREEITEM hItem, UINT nMask, LPCTSTR lpszItem, int nImage,
	int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam,
	UINT uStateEx, HWND hWnd, int iExpandedImage)
{
	ASSERT(::IsWindow(m_hWnd));
	TVITEMEX item;
	item.hItem = hItem;
	item.mask = nMask;
	item.pszText = (LPTSTR)lpszItem;
	item.iImage = nImage;
	item.iSelectedImage = nSelectedImage;
	item.state = nState;
	item.stateMask = nStateMask;
	item.lParam = lParam;
	item.uStateEx = uStateEx;
	item.hwnd = hWnd;
	item.iExpandedImage = iExpandedImage;
	return (BOOL)::SendMessage(m_hWnd, TVM_SETITEM, 0, (LPARAM)&item);
}

HTREEITEM uiUtilities::CTreeCtrl::InsertItem(UINT nMask, LPCTSTR lpszItem, int nImage,
	int nSelectedImage, UINT nState, UINT nStateMask, LPARAM lParam,
	HTREEITEM hParent, HTREEITEM hInsertAfter)
{
	ASSERT(::IsWindow(m_hWnd));
	TVINSERTSTRUCT tvis;
	tvis.hParent = hParent;
	tvis.hInsertAfter = hInsertAfter;
	tvis.item.mask = nMask;
	tvis.item.pszText = (LPTSTR)lpszItem;
	tvis.item.iImage = nImage;
	tvis.item.iSelectedImage = nSelectedImage;
	tvis.item.state = nState;
	tvis.item.stateMask = nStateMask;
	tvis.item.lParam = lParam;
	return (HTREEITEM)::SendMessage(m_hWnd, TVM_INSERTITEM, 0, (LPARAM)&tvis);
}

HTREEITEM uiUtilities::CTreeCtrl::HitTest(CPoint pt, UINT* pFlags) const
{
	ASSERT(::IsWindow(m_hWnd));
	TVHITTESTINFO hti;
	hti.pt = pt;
	HTREEITEM h = (HTREEITEM)::SendMessage(m_hWnd, TVM_HITTEST, 0,
		(LPARAM)&hti);
	if (pFlags != NULL)
		*pFlags = hti.flags;
	return h;
}

//void uiUtilities::CTreeCtrl::RemoveImageList(int nImageList)
//{
//	HIMAGELIST h = (HIMAGELIST)SendMessage(TVM_GETIMAGELIST,
//		(WPARAM)nImageList);
//	if (CImageList::FromHandlePermanent(h) != NULL)
//		SendMessage(TVM_SETIMAGELIST, (WPARAM)nImageList, NULL);
//}

void uiUtilities::CTreeCtrl::OnDestroy()
{
	//RemoveImageList(LVSIL_NORMAL);
	//RemoveImageList(LVSIL_STATE);

	//CWnd::OnDestroy();
}

//CImageList* uiUtilities::CTreeCtrl::CreateDragImage(HTREEITEM hItem)
//{
//	ASSERT(::IsWindow(m_hWnd));
//
//	HIMAGELIST hImageList = (HIMAGELIST)::SendMessage(m_hWnd,
//		TVM_CREATEDRAGIMAGE, 0, (LPARAM)hItem);
//	if (hImageList == NULL)
//		return NULL;
//
//	CImageList* pImageList = new CImageList;
//	ASSERT(pImageList->Attach(hImageList));
//	return pImageList;
//}

BOOL uiUtilities::CTreeCtrl::GetCheck(HTREEITEM hItem) const
{
	ASSERT(::IsWindow(m_hWnd));
	TVITEM item;
	item.mask = TVIF_HANDLE | TVIF_STATE;
	item.hItem = hItem;
	item.stateMask = TVIS_STATEIMAGEMASK;
	ASSERT(::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&item));
	// Return zero if it's not checked, or nonzero otherwise.
	return ((BOOL)((item.state >> 12) - 1));
}

BOOL uiUtilities::CTreeCtrl::SetCheck(HTREEITEM hItem, BOOL fCheck)
{
	ASSERT(::IsWindow(m_hWnd));
	TVITEM item;
	item.mask = TVIF_HANDLE | TVIF_STATE;
	item.hItem = hItem;
	item.stateMask = TVIS_STATEIMAGEMASK;

	// Since state images are one-based, passing a 1 to the 
	// INDEXTOSTATEIMAGEMASK macro turns the check off, and
	// passing 2 turns it on.
	item.state = INDEXTOSTATEIMAGEMASK((fCheck ? 2 : 1));

	return (BOOL)::SendMessage(m_hWnd, TVM_SETITEM, 0, (LPARAM)&item);
}

#if defined(UNICODE)

BOOL uiUtilities::CTreeCtrl::GetItemPartRect(HTREEITEM hItem, TVITEMPART nPart, LPRECT lpRect)
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(lpRect);
	TVGETITEMPARTRECTINFO info;
	info.hti = hItem;
	info.prc = lpRect;
	info.partID = nPart;
	return (BOOL)::SendMessage((m_hWnd), TVM_GETITEMPARTRECT, 0, (LPARAM)&info);
}

UINT uiUtilities::CTreeCtrl::GetItemStateEx(HTREEITEM hItem) const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(hItem);

	TVITEMEX item;
	item.hItem = hItem;
	item.mask = TVIF_STATEEX;
	VERIFY(::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&item));
	return item.uStateEx;
}

int uiUtilities::CTreeCtrl::GetItemExpandedImageIndex(HTREEITEM hItem) const
{
	ASSERT(::IsWindow(m_hWnd));
	ASSERT(hItem);

	TVITEMEX item;
	item.hItem = hItem;

	item.mask = TVIF_EXPANDEDIMAGE;
	item.uStateEx = TVIF_EXPANDEDIMAGE;
	VERIFY(::SendMessage(m_hWnd, TVM_GETITEM, 0, (LPARAM)&item));
	return item.iExpandedImage;
}
#endif

/////////////////////////////////////////////////////////////////////////////

_AFXCMN_INLINE uiUtilities::CProgressCtrl::CProgressCtrl()
{ }
_AFXCMN_INLINE COLORREF uiUtilities::CProgressCtrl::SetBkColor(_In_ COLORREF clrNew)
{
	ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, PBM_SETBKCOLOR, 0, (LPARAM)clrNew);
}
_AFXCMN_INLINE void uiUtilities::CProgressCtrl::SetRange(_In_ short nLower, _In_ short nUpper)
{
	ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, PBM_SETRANGE, 0, MAKELPARAM(nLower, nUpper));
}
_AFXCMN_INLINE void uiUtilities::CProgressCtrl::SetRange32(_In_ int nLower, _In_ int nUpper)
{
	ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, PBM_SETRANGE32, (WPARAM)nLower, (LPARAM)nUpper);
}
_AFXCMN_INLINE int uiUtilities::CProgressCtrl::GetPos() const
{
	ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, PBM_GETPOS, 0, 0);
}
_AFXCMN_INLINE int uiUtilities::CProgressCtrl::OffsetPos(_In_ int nPos)
{
	ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, PBM_DELTAPOS, nPos, 0L);
}
_AFXCMN_INLINE int uiUtilities::CProgressCtrl::SetStep(_In_ int nStep)
{
	ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, PBM_SETSTEP, nStep, 0L);
}
_AFXCMN_INLINE int uiUtilities::CProgressCtrl::StepIt()
{
	ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, PBM_STEPIT, 0, 0L);
}
#if defined(UNICODE)
_AFXCMN_INLINE BOOL uiUtilities::CProgressCtrl::SetMarquee(_In_ BOOL fMarqueeMode, _In_ int nInterval)
{
	ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, PBM_SETMARQUEE, (WPARAM)fMarqueeMode, (LPARAM)nInterval);
}
#endif	// defined(UNICODE)

#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
_AFXCMN_INLINE int uiUtilities::CProgressCtrl::GetStep() const
{
	ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, PBM_GETSTEP, 0, 0L);
}
_AFXCMN_INLINE COLORREF uiUtilities::CProgressCtrl::GetBkColor() const
{
	ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, PBM_GETBKCOLOR, 0, 0L);
}
_AFXCMN_INLINE COLORREF uiUtilities::CProgressCtrl::GetBarColor() const
{
	ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, PBM_GETBARCOLOR, 0, 0L);
}
_AFXCMN_INLINE int uiUtilities::CProgressCtrl::SetState(_In_ int iState)
{
	ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, PBM_SETSTATE, (WPARAM)iState, 0L);
}
_AFXCMN_INLINE int uiUtilities::CProgressCtrl::GetState() const
{
	ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, PBM_GETSTATE, 0, 0L);
}
#endif //(NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)


/////////////////////////////////////////////////////////////////////////////
// CProgressCtrl

//BOOL uiUtilities::CProgressCtrl::Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd,
//	UINT nID)
//{
//	// initialize common controls
//	//VERIFY(AfxDeferRegisterClass(AFX_WNDCOMMCTL_PROGRESS_REG));
//
//	CWnd* pWnd = this;
//	return pWnd->Create(PROGRESS_CLASS, NULL, dwStyle, rect, pParentWnd, nID);
//}
//
//BOOL uiUtilities::CProgressCtrl::CreateEx(DWORD dwExStyle, DWORD dwStyle,
//	const RECT& rect, CWnd* pParentWnd, UINT nID)
//{
//	BOOL bRet = Create(dwStyle, rect, pParentWnd, nID);
//	if (bRet && dwExStyle != 0)
//	{
//		bRet = ModifyStyleEx(0, dwExStyle);
//	}
//	return bRet;
//}

uiUtilities::CProgressCtrl::~CProgressCtrl()
{
	//DestroyWindow();
}

