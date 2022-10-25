// ListBoxCH.cpp : implementation file
//

#include "stdafx.h"
#include "ListBoxCH.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListBoxCH

CListBoxCH::CListBoxCH()
{
	m_clrSel  = RGB(57, 151, 74);		// default selected color (blue)
	m_clrBg   = RGB(255,255,255);	// default background color (white)
	m_clrText = RGB(255,255,255);	// default text color (white)
	m_clrSecondText = RGB(0, 0, 255);
	m_bEdge   = FALSE;				// default edge is enabled
	m_itemStyle = NX_ONE_LINE_ITEM;
}

CListBoxCH::~CListBoxCH()
{

}

void CListBoxCH::ReleaseItems()
{
	// release all list item.
	while (m_pItemList.GetCount())
	{
		CExtItem* pItem = m_pItemList.GetHead();
		delete pItem;
		m_pItemList.RemoveHead();
	}
}


BEGIN_MESSAGE_MAP(CListBoxCH, CListBox)
	//{{AFX_MSG_MAP(CListBoxCH)
	//ON_CONTROL_REFLECT(LBN_SELCHANGE, OnSelchange)
        
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListBoxCH message handlers
//void CListBoxCH::AddItem(LPCTSTR lpszItemName,HICON hIcon)
//{
//	//CExtItem* pItem = new CExtItem(lpszItemName,hIcon);
//	//m_pItemList.AddTail(pItem);
//
//	//int nIndex = AddString(L"");
//	//SetItemDataPtr(nIndex,pItem);
//}

int CListBoxCH::AddItem(LPCTSTR lpszItemName, int nIcoNormalID, int nIcoSelID, LPCTSTR lpszSecondLine)
{
    CExtItem* pItem = new CExtItem(lpszItemName, nIcoNormalID, nIcoSelID, lpszSecondLine);
    m_pItemList.AddTail(pItem);

    int nIndex = AddString(L"");
    SetItemDataPtr(nIndex, pItem);
	return nIndex;
}

void CListBoxCH::AddItem(LPCTSTR lpszItemName, int nIcoNormalID, LPCTSTR lpszSecondLine)
{
    CExtItem* pItem = new CExtItem(lpszItemName, nIcoNormalID, lpszSecondLine);
    m_pItemList.AddTail(pItem);

    int nIndex = AddString(L"");
    SetItemDataPtr(nIndex, pItem);
}


void CListBoxCH::SetSelColor(COLORREF clr)
{
	m_clrSel = clr;
}
void CListBoxCH::SetBgColor(COLORREF clr)
{
	m_clrBg = clr;
}
void CListBoxCH::SetTextColor(COLORREF clr)
{
	m_clrText = clr;
}
void CListBoxCH::EnableEdge(BOOL bEnable)
{
	m_bEdge = bEnable;
}
void CListBoxCH::SetItemDisplayStyle(ListItemDisplayStyle itemStyle)
{
	m_itemStyle = itemStyle;
}
void CListBoxCH::SetCurSel(int curSel)
{
	m_curSel = curSel;
	CListBox::SetCurSel(curSel);
	OnSelchange();
}
int CListBoxCH::GetCurSel()
{	
	return m_curSel;
}
void CListBoxCH::MeasureItem(LPMEASUREITEMSTRUCT lpMIS) 
{
	// TODO: Add your code to determine the size of specified item
	//lpMIS->itemWidth   = 100;
	auto pFont = GetFont();
	LOGFONT lf;
	pFont->GetLogFont(&lf);

	lpMIS->itemHeight = 4 * abs(lf.lfHeight);
	//if(m_itemStyle == NX_ONE_LINE_ITEM)
	//{
	//	lpMIS->itemHeight  = 40;
	//}
	//else
	//{
	//	lpMIS->itemHeight = 50;
	//}
}
void CListBoxCH::DrawItem(LPDRAWITEMSTRUCT lpDIS) 
{
	// TODO: Add your code to draw the specified item
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);

	CRect rectFull(lpDIS->rcItem);
	// icon and text rect
	CRect rectIcon(rectFull.left,rectFull.top,rectFull.left+ rectFull.Height(),rectFull.top+ rectFull.Height());
	CRect rectText(rectIcon.right,rectFull.top,rectFull.right,rectFull.bottom);	
	
	
	CExtItem* pItem = static_cast<CExtItem *>(GetItemDataPtr(lpDIS->itemID));

	if ( (lpDIS->itemAction & ODA_DRAWENTIRE) ||
	     ((!(lpDIS->itemState & ODS_SELECTED) && (lpDIS->itemAction & ODA_SELECT)))
	   )
	{
		// item has been selected or draw all item 
		// fill background for setted background color 
		CRect rect(rectFull);

		pDC->SetBkMode(TRANSPARENT);
		pDC->FillSolidRect(rect,m_clrBg);
        //CBrush br;
        //br.CreateSolidBrush(m_clrBg);

        //pDC->SelectObject(&br);
        //CPoint pt = { rect.Height() / 2, rect.Height() / 2 };
        //pDC->RoundRect(&rect, pt);

		if( m_bEdge )
			pDC->DrawEdge(rect,EDGE_SUNKEN,BF_BOTTOM);
		pDC->DrawIcon(rectIcon.left+2,rectIcon.top+6,pItem->m_hIcon);
		pDC->SetTextColor(RGB(0,0,0));			// unselected text color

		rect.CopyRect(rectText);
		rect.DeflateRect(2,2);
		rect.OffsetRect(2,0);
		if (pItem->m_szItemName != NULL)
		{
			if (m_itemStyle == NX_ONE_LINE_ITEM)
			{
				pDC->DrawText(pItem->m_szItemName, lstrlen(pItem->m_szItemName),
					rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
			}
			else
			{
				CRect rcUpper = rectText;
				rcUpper.bottom = rectText.top + rectText.Height() * 5 / 16;
				rcUpper.OffsetRect(0, rectText.Height() * 3 / 16);
				pDC->DrawText(pItem->m_szItemName, lstrlen(pItem->m_szItemName),
					rcUpper, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
				rcUpper.OffsetRect(0, rcUpper.Height());
				pDC->SetTextColor(m_clrSecondText);
				pDC->DrawText(pItem->m_emailAddr, lstrlen(pItem->m_emailAddr),
					rcUpper, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
			}
		}
	}

	
	if ((lpDIS->itemState & ODS_SELECTED) &&
		(lpDIS->itemAction & (ODA_SELECT | ODA_DRAWENTIRE)))
	{	
		// item has been selected - draw selected frame
		CRect rect(rectFull);

		pDC->SetBkMode(TRANSPARENT);
		CPen  Pen(PS_SOLID,1,RGB(200,200,200));				// draw dotted frame 
        CBrush  Br(RGB(240, 240, 240));				// draw dotted frame 
        CPen* pOldPen = pDC->SelectObject(&Pen);
        CBrush* pOldBr = pDC->SelectObject(&Br);
        pDC->Rectangle(rect);
		pDC->SelectObject(pOldPen);
        pDC->SelectObject(pOldBr);

		//rect.DeflateRect(1,1,1,1);
        
		//pDC->FillRect(rect,&CBrush(RGB(240, 240, 240)));	// fill item frame

        auto hIcon = (pItem->m_hSelIcon) ? pItem->m_hSelIcon : pItem->m_hIcon;
		pDC->DrawIcon(rectIcon.left+2,rectIcon.top+6, hIcon);
		pDC->SetTextColor(m_clrSel);			// selected text color
		rect.CopyRect(rectText);
		rect.DeflateRect(2,2);
		rect.OffsetRect(2,0);					// draw text
		if (pItem->m_szItemName != NULL)
		{
			if (m_itemStyle == NX_ONE_LINE_ITEM)
			{
				pDC->DrawText(pItem->m_szItemName, lstrlen(pItem->m_szItemName),
					rect, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
			}
			else
			{
				CRect rcUpper = rectText;
				rcUpper.bottom = rectText.top + rectText.Height() * 5/ 16;
				rcUpper.OffsetRect(0, rectText.Height() * 3 / 16);
				pDC->DrawText(pItem->m_szItemName, lstrlen(pItem->m_szItemName),
					rcUpper, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
				rcUpper.OffsetRect(0, rcUpper.Height());
				pDC->SetTextColor(m_clrSecondText);
				pDC->DrawText(pItem->m_emailAddr, lstrlen(pItem->m_emailAddr),
					rcUpper, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
			}
		}
	}
}

void CListBoxCH::PreSubclassWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	EnableToolTips(TRUE);
	m_curSel = -1;
}
void CListBoxCH::OnSelchange() 
{
	static int oldSel = m_curSel;

	if( oldSel != m_curSel )
	{	// refresh two rectangle,one is old item and another is new.
		CRect rect;					
		GetItemRect(oldSel,rect);
		InvalidateRect(rect,TRUE);

		GetItemRect(m_curSel,rect);
		InvalidateRect(rect,TRUE);
		oldSel = m_curSel;
	}
}
void CListBoxCH::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	// get selected index from mouse position
	BOOL data;
	m_curSel = ItemFromPoint(point,data);	
	OnSelchange();
	CListBox::OnLButtonUp(nFlags, point);
}
int CListBoxCH::VKeyToItem(UINT nKey, UINT nIndex) 
{
	// TODO: Add your code to handle a particular virtual key
	// return -1 = default action
	// return -2 = no further action
	// return index = perform default action for keystroke on
	//                item specified by index
	if ((nKey == VK_UP) && (nIndex > 0)) {
		SetCurSel(nIndex);
		m_curSel--;
	}
	
   // On key down, move the caret down one item.
	else if ((nKey == VK_DOWN) && (nIndex < (UINT)GetCount())) {
		SetCurSel(nIndex);
		m_curSel++;
   }
	return -1;
}



INT_PTR CListBoxCH::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	// TODO: Add your specialized code here and/or call the base class
	int row;
	RECT cellrect;   // cellrect		- to hold the bounding rect
	BOOL tmp = FALSE;
	row = ItemFromPoint(point, tmp);  //we call the ItemFromPoint function to determine the row,
									  //note that in NT this function may fail  use the ItemFromPointNT member function

	if (row == -1)
		return -1;

	//set up the TOOLINFO structure. GetItemRect(row,&cellrect);
	GetItemRect(row, &cellrect);
	pTI->rect = cellrect;
	pTI->hwnd = m_hWnd;
	pTI->uId = (UINT)((row));   //The uId is assigned a value according to the row value.
	pTI->lpszText = LPSTR_TEXTCALLBACK;
	return pTI->uId;
}


BOOL CListBoxCH::OnToolTipText(UINT id, NMHDR * pNMHDR, LRESULT * pResult)
{
	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	CString strTipText;
	UINT nID = (UINT)pNMHDR->idFrom;

	if (nID != -1)
	{
		CExtItem* pItem = static_cast<CExtItem *>(GetItemDataPtr(nID));
			lstrcpyn(pTTTW->szText, pItem->m_szItemName, 80);
	}


	*pResult = 0;

	return TRUE;
}


