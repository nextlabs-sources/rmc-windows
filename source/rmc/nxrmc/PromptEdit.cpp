// PromptEdit.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "PromptEdit.h"


// CPromptEdit

IMPLEMENT_DYNAMIC(CPromptEdit, CEdit)

CPromptEdit::CPromptEdit()
    : m_bEmpty(false)
{

}

CPromptEdit::~CPromptEdit()
{
}


BEGIN_MESSAGE_MAP(CPromptEdit, CEdit)
    ON_WM_SETFOCUS()
    ON_WM_KILLFOCUS()
    ON_CONTROL_REFLECT(EN_CHANGE, &CPromptEdit::OnEnChange)
    ON_WM_PAINT()
END_MESSAGE_MAP()



// CPromptEdit message handlers




void CPromptEdit::OnSetFocus(CWnd* pOldWnd)
{
    CEdit::OnSetFocus(pOldWnd);
    if (m_bEmpty)
    {
        SetWindowText(L"");
    }

    // TODO: Add your message handler code here
}


void CPromptEdit::OnKillFocus(CWnd* pNewWnd)
{
    CEdit::OnKillFocus(pNewWnd);

    if (m_bEmpty)
    {
        SetWindowText(theApp.LoadString(IDS_SEARCH_FILE_FOLDER)/*L"Search file/folder"*/);
    }


    // TODO: Add your message handler code here
}


void CPromptEdit::OnEnChange()
{
    // TODO:  If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CEdit::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO:  Add your control notification handler code here
    CString wndText;
    GetWindowText(wndText);
    m_bEmpty = !wndText.GetLength();
}


void CPromptEdit::OnPaint()
{
    CPaintDC dc(this); // device context for painting
                       // TODO: Add your message handler code here
                       // Do not call CEdit::OnPaint() for painting messages
}
