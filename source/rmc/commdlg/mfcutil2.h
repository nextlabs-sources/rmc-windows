#pragma once

#include <windows.h>
#include "..\uiutilities\uiutilities.hpp"
#include <string>
#define _AFXCMN_INLINE

namespace uiUtilities
{
/*============================================================================*/
// CTreeCtrl

// NOTE: This class must remain a binary-compatible subset
// of CTreeView. Do not add data members or virtual functions
// directly to this class.
class CTreeCtrl : public CWnd
{
	// DECLARE_DYNAMIC virtual OK - CWnd has DECLARE_DYNAMIC
	//DECLARE_DYNAMIC(CTreeCtrl)

	// Constructors
public:
	CTreeCtrl();

	// Generic creator
	BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	// Generic creator allowing extended style bits
	BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
		_In_ CWnd* pParentWnd, _In_ UINT nID);

	// Attributes

	// Retrieves the bounding rectangle for the specified item.
	BOOL GetItemRect(_In_ HTREEITEM hItem, _Out_ LPRECT lpRect, _In_ BOOL bTextOnly) const;

	// Gets the count of items in the control.
	UINT GetCount() const;

	// Returns the level-to-level indentation (in pixels).
	UINT GetIndent() const;

	// Sets the level-to-level indentation (in pixels).
	void SetIndent(_In_ UINT nIndent);

	// Retrieves the image list associated with the control.
	CImageList* GetImageList(_In_ int nImageList) const;

	// Sets the image list associated with this control.
	CImageList* SetImageList(_In_opt_ CImageList* pImageList, _In_ int nImageList);

	// Retrieves the next item having the given relationship with the
	// specified item.
	HTREEITEM GetNextItem(_In_ HTREEITEM hItem, _In_ UINT nCode) const;

	// Retrieves the next child item after the specified item.
	HTREEITEM GetChildItem(_In_ HTREEITEM hItem) const;

	// Retrieves the next sibling item after the specified item.
	HTREEITEM GetNextSiblingItem(_In_ HTREEITEM hItem) const;

	// Retrieves the previous sibling item of the specified item.
	HTREEITEM GetPrevSiblingItem(_In_ HTREEITEM hItem) const;

	// Retrieves the parent of the specified item.
	HTREEITEM GetParentItem(_In_ HTREEITEM hItem) const;

	// Retrieves the topmost visible item in the control.
	HTREEITEM GetFirstVisibleItem() const;

	// Retrieves the next visible item after the specified item.
	HTREEITEM GetNextVisibleItem(_In_ HTREEITEM hItem) const;

	// Retrieves the previous visible item before the specified item.
	HTREEITEM GetPrevVisibleItem(_In_ HTREEITEM hItem) const;

	// Retrieves the last expanded item in the tree. This does not retrieve the last item visible in the tree-view window.
	HTREEITEM GetLastVisibleItem() const;

	// Retrieves the higlighted item, NULL if none.
	HTREEITEM GetSelectedItem() const;

	// Retrieves the currently drop-highlighted item, NULL if none.
	HTREEITEM GetDropHilightItem() const;

	// Retrieves the root item of the control, NULL if none.
	HTREEITEM GetRootItem() const;

	// Retrieves information about a particular item in the control.
	BOOL GetItem(_Out_ TVITEM* pItem) const;

	// Retrieves the text associated with the given item.
	std::wstring GetItemText(_In_ HTREEITEM hItem) const;

	// Retrieves the images associated with the given item.
	BOOL GetItemImage(_In_ HTREEITEM hItem, _Out_ int& nImage,
		_Out_ int& nSelectedImage) const;

	// Retrieves the state of the given item.
	UINT GetItemState(_In_ HTREEITEM hItem, _In_ UINT nStateMask) const;

	// Retrieves the user-supplied data associated with the given item.
	DWORD_PTR GetItemData(_In_ HTREEITEM hItem) const;

	// Sets the state of the an item.
	BOOL SetItem(_In_ TVITEM* pItem);
	BOOL SetItem(_In_ HTREEITEM hItem, _In_ UINT nMask, _In_opt_z_ LPCTSTR lpszItem, _In_ int nImage,
		_In_ int nSelectedImage, _In_ UINT nState, _In_ UINT nStateMask, _In_ LPARAM lParam);

	BOOL SetItemEx(_In_ HTREEITEM hItem, _In_ UINT nMask, _In_opt_z_ LPCTSTR lpszItem, _In_ int nImage,
		_In_ int nSelectedImage, _In_ UINT nState, _In_ UINT nStateMask, _In_ LPARAM lParam,
		_In_ UINT uStateEx, _In_opt_ HWND hWnd, _In_ int iExpandedImage);

	// Sets the text of the specified item.
		BOOL SetItemText(_In_ HTREEITEM hItem, _In_z_ LPCTSTR lpszItem);

	// Sets the image on the specified item.
	BOOL SetItemImage(_In_ HTREEITEM hItem, _In_ int nImage, _In_ int nSelectedImage);

	// Sets the state of the specified item.
	BOOL SetItemState(_In_ HTREEITEM hItem, _In_ UINT nState, _In_ UINT nStateMask);

	// Sets the user data on the specified item.
	BOOL SetItemData(_In_ HTREEITEM hItem, _In_ DWORD_PTR dwData);

	// Determines if the specified item has children.
	BOOL ItemHasChildren(_In_ HTREEITEM hItem) const;

	// Retrieves the edit control used to perform in-place editing.
	CEdit* GetEditControl() const;

	// Gets the count of items presently visible in the control.
	UINT GetVisibleCount() const;

	// Retrieves the tool tip control associated with this control.
	//CToolTipCtrl* GetToolTips() const;

	// Sets the tool tip control to be used by this control.
	//CToolTipCtrl* SetToolTips(_In_ CToolTipCtrl* pWndTip);

	// Retrieves the background colour used throughout the control.
	COLORREF GetBkColor() const;

	// Sets the background color to be used throughout the control.
	COLORREF SetBkColor(_In_ COLORREF clr);

	// Retrieves the height of items in the control.
	SHORT GetItemHeight() const;

	// Sets the height of items in the control.
	SHORT SetItemHeight(_In_ SHORT cyHeight);

	// Retrieves the text color used for all items in the control.
	COLORREF GetTextColor() const;

	// Sets the text color used for all items in the control.
	COLORREF SetTextColor(_In_ COLORREF clr);

	// Sets the insertion mark to the specified item in the control.
	BOOL SetInsertMark(_In_ HTREEITEM hItem, _In_ BOOL fAfter = TRUE);

	// Gets the checked state of the specified item in the control.
	// (Only useful on a control with the TVS_CHECKBOXES style.)
	BOOL GetCheck(_In_ HTREEITEM hItem) const;

	// Sets the checked state of the specified item in the control.
	// (Only useful on a control with the TVS_CHECKBOXES style.)
	BOOL SetCheck(_In_ HTREEITEM hItem, _In_ BOOL fCheck = TRUE);

	COLORREF GetInsertMarkColor() const;
	COLORREF SetInsertMarkColor(_In_ COLORREF clrNew);

	// Sets the maximum time (in milliseconds) the control will
	// spend smooth scrolling its content.
	UINT SetScrollTime(_In_ UINT uScrollTime);

	// Retrieves the maximum time (in milliseconds) the control will
	// spend smooth scrolling its content.
	UINT GetScrollTime() const;

	COLORREF GetLineColor() const;
	COLORREF SetLineColor(_In_ COLORREF clrNew = CLR_DEFAULT);

#if defined(UNICODE)
	// Maps treeview item id to accessibility identifier.
	UINT MapItemToAccId(_In_ HTREEITEM hItem) const;

	// Maps accessibility identifier id to treeview item.
	HTREEITEM MapAccIdToItem(_In_ UINT uAccId) const;

	// Set autoscrolling rate and delay (?).
	BOOL SetAutoscrollInfo(_In_ UINT uPixelsPerSec, _In_ UINT uUpdateTime);
#endif

#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
	// Get count of selected items in the tree control.
	UINT GetSelectedCount();

	// Get rectangle for the specified part of the specified item.
	BOOL GetItemPartRect(_In_ HTREEITEM hItem, _In_ TVITEMPART nPart, _Out_ LPRECT lpRect);

	// Get the extended state of the item
	UINT GetItemStateEx(_In_ HTREEITEM hItem) const;

	// Set the extended state of the item
	BOOL SetItemStateEx(_In_ HTREEITEM hItem, _In_ UINT uStateEx);

	// Get the expanded image index for the specified item
	int GetItemExpandedImageIndex(_In_ HTREEITEM hItem) const;

	// Set the expanded image index for the specified item
	BOOL SetItemExpandedImageIndex(_In_ HTREEITEM hItem, _In_ int nIndex);

	// Get extended styles for the tree control.
	DWORD GetExtendedStyle() const;

	// Set extended styles on the tree control.
	DWORD SetExtendedStyle(_In_ DWORD dwExMask, _In_ DWORD dwExStyles);
#endif //  (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

	// Operations

	// Inserts a new item to the control.
	HTREEITEM InsertItem(_In_ LPTVINSERTSTRUCT lpInsertStruct);
	HTREEITEM InsertItem(_In_ UINT nMask, _In_z_ LPCTSTR lpszItem, _In_ int nImage,
		_In_ int nSelectedImage, _In_ UINT nState, _In_ UINT nStateMask, _In_ LPARAM lParam,
		_In_ HTREEITEM hParent, _In_ HTREEITEM hInsertAfter);
	HTREEITEM InsertItem(_In_z_ LPCTSTR lpszItem, _In_ HTREEITEM hParent = TVI_ROOT,
		_In_ HTREEITEM hInsertAfter = TVI_LAST);
	HTREEITEM InsertItem(_In_z_ LPCTSTR lpszItem, _In_ int nImage, _In_ int nSelectedImage,
		_In_ HTREEITEM hParent = TVI_ROOT, _In_ HTREEITEM hInsertAfter = TVI_LAST);

	// Removes the specified item from the control.
	BOOL DeleteItem(_In_ HTREEITEM hItem);

	// Removes all items from the control.
	BOOL DeleteAllItems();

	// Expands the children of the specified item.
	BOOL Expand(_In_ HTREEITEM hItem, _In_ UINT nCode);

	// Selects the specified item.
	BOOL Select(_In_ HTREEITEM hItem, _In_ UINT nCode);

	// Selects the specified item.
	BOOL SelectItem(_In_opt_ HTREEITEM hItem);

	// Selects an item to be the drop target in the control.
	BOOL SelectDropTarget(_In_opt_ HTREEITEM hItem);

	// Draws the specified item as the drop target for the control.
	BOOL SelectSetFirstVisible(_In_ HTREEITEM hItem);

	// Begins editing the label of the specified item.
	CEdit* EditLabel(_In_ HTREEITEM hItem);

	// Determines the visual feature of the control under
	// the specified point.
	HTREEITEM HitTest(_In_ CPoint pt, _In_opt_ UINT* pFlags = NULL) const;
	HTREEITEM HitTest(_In_ TVHITTESTINFO* pHitTestInfo) const;

	// Create a drag image for the specified item.
	CImageList* CreateDragImage(_In_ HTREEITEM hItem);

	// Sorts all children of the specified item.
	BOOL SortChildren(_In_opt_ HTREEITEM hItem);

	// Scrolls the control to ensure the specified item is visible.
	BOOL EnsureVisible(_In_ HTREEITEM hItem);

	// Sorts items in the control using the provided callback function.
	BOOL SortChildrenCB(_In_ LPTVSORTCB pSort);

	// Terminates label editing operation.
	BOOL EndEditLabelNow(_In_ BOOL fCancelWithoutSave);

#if defined(UNICODE)
	// Shows information tooltip on the specified item.
	void ShowInfoTip(_In_ HTREEITEM hItem);
#endif

	// Implementation
protected:
	void RemoveImageList(_In_ int nImageList);

public:
	// virtual OK here - ~CWnd is virtual
	virtual ~CTreeCtrl();
	void OnDestroy();

};




/*============================================================================*/
// CProgressCtrl

class CProgressCtrl : public CWnd
{
	//DECLARE_DYNAMIC(CProgressCtrl)

	// Constructors
public:
	CProgressCtrl();

	// Generic creator
	//virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	//// Generic creator allowing extended style bits
	//virtual BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
	//	_In_ CWnd* pParentWnd, _In_ UINT nID);

	// Attributes

	// Sets range of values for the control. (16-bit limit)
	void SetRange(_In_ short nLower, _In_ short nUpper);

	// Sets range of values for the control. (32-bit limit)
	void SetRange32(_In_ int nLower, _In_ int nUpper);

	// Retrieves range of values for the control. (32-bit limit)
	void GetRange(_Out_ int& nLower, _Out_ int& nUpper) const;

	// Gets the current position within the set range of the control.
	int GetPos() const;

	// Sets the current position within the set range of the control.
	int SetPos(_In_ int nPos);

	// Displaces the current position within the set range of the
	// control by the passed value.
	int OffsetPos(_In_ int nPos);

	// Sets the step by which increments happen with a call to StepIt().
	int SetStep(_In_ int nStep);

	// Sets the control's background color.
	COLORREF SetBkColor(_In_ COLORREF clrNew);

	// Sets the color of the progress indicator bar in the progress bar control.
	COLORREF SetBarColor(_In_ COLORREF clrBar);

#if defined(UNICODE)
	// Sets the progress bar control to marquee mode.
	BOOL SetMarquee(_In_ BOOL fMarqueeMode, _In_ int nInterval);
#endif	// defined(UNICODE)

#if defined(UNICODE)
	// REVIEW: Retrieves the step increment for the progress bar control.
	int GetStep() const;

	// REVIEW: Retrieves the background color of the progress bar control.
	COLORREF GetBkColor() const;

	// REVIEW: Retrieves the color of the progress bar control.
	COLORREF GetBarColor() const;

	// REVIEW: Sets the state of the progress bar.
	int SetState(_In_ int iState);

	// REVIEW: Retrieves the state of the progress bar.
	int GetState() const;
#endif	// defined(UNICODE)

	// Operations

	// Steps the control by the value set with SetStep().
	int StepIt();

	// Implementation
public:
	virtual ~CProgressCtrl();
};


}

