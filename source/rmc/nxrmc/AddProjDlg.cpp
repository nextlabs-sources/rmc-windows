// AddProjDlg.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "AddProjDlg.h"
#include "afxdialogex.h"
//#include "createproject.h"


// CAddProjDlg dialog

IMPLEMENT_DYNAMIC(CAddProjDlg, CFlatDlgBase)

CAddProjDlg::CAddProjDlg(AddDlgStyle dlgStyle, CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_DIALOG_ADDPROJ, pParent),
    m_dlgStyle(dlgStyle),
    m_hbmp(0)
{
}

CAddProjDlg::~CAddProjDlg()
{
    DeleteObject(m_hbmp);
}

void CAddProjDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_VIEW_ALL, m_viewAll);
    DDX_Control(pDX, IDC_STATIC_ADDPROJ, m_addProjImage);
    DDX_Control(pDX, IDC_STATIC_VIEW_ALL, m_viewAllText);
}


BEGIN_MESSAGE_MAP(CAddProjDlg, CDialogEx)
    ON_WM_DESTROY()
    ON_STN_CLICKED(IDC_STATIC_ADDPROJ, &CAddProjDlg::OnStnClickedStaticAddproj)
    ON_WM_CTLCOLOR()
    ON_WM_ERASEBKGND()
    ON_BN_CLICKED(IDC_VIEW_ALL, &CAddProjDlg::OnBnClickedViewAll)
    ON_WM_LBUTTONUP()
	ON_WM_PAINT()
END_MESSAGE_MAP()


// CAddProjDlg message handlers


void CAddProjDlg::OnDestroy()
{
    CDialogEx::OnDestroy();
    // TODO: Add your message handler code here
}


void CAddProjDlg::OnStnClickedStaticAddproj()
{
    // TODO: Add your control notification handler code here
}


HBRUSH CAddProjDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here
    if (nCtlColor == CTLCOLOR_STATIC)
    {
        if (pWnd->GetDlgCtrlID() == IDC_STATIC_VIEW_ALL)
        {
            pDC->SetTextColor(RGB(0, 0, 255));
            pDC->SetBkMode(TRANSPARENT);
        }
    }

    // TODO:  Return a different brush if the default is not desired
    return hbr;
}


BOOL CAddProjDlg::OnEraseBkgnd(CDC* pDC)
{
    // TODO: Add your message handler code here and/or call default

    return CDialogEx::OnEraseBkgnd(pDC);
}


BOOL CAddProjDlg::OnInitDialog()
{
    CFlatDlgBase::OnInitDialog();
    m_ToolTip.Create(this);
    if (m_dlgStyle != NX_VIEW_ALL_PROJ)
    {
        m_hbmp = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDB_ADDPROJ));
        m_addProjImage.SetBitmap(m_hbmp);
    }
    else
    {
        m_addProjImage.ShowWindow(SW_HIDE);
        m_viewAll.ShowWindow(SW_SHOW);
        m_viewAllText.ShowWindow(SW_SHOW);
        AddFontSize(50, m_viewAllText);
        m_viewAll.SetImages(IDB_PNG_NEXT, 0, 0, NX_NEXTLABS_STANDARD_GRAY);
        m_ToolTip.AddTool(&m_viewAll, _T("Go all project page"));
        m_ToolTip.Activate(TRUE);

    }

	
    // TODO:  Add extra initialization here
    SetBackColor(NX_NEXTLABS_STANDARD_GRAY);

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


BOOL CAddProjDlg::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class
    m_ToolTip.RelayEvent(pMsg);

    return CFlatDlgBase::PreTranslateMessage(pMsg);
}


void CAddProjDlg::OnBnClickedViewAll()
{
    // TODO: Add your control notification handler code here
    GetParent()->GetParent()->GetParent()->SendMessage(WM_COMMAND, IDC_RADIO_PROJECTS, 0l);

}


void CAddProjDlg::OnLButtonDown(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default

    CFlatDlgBase::OnLButtonDown(nFlags, point);
}




void CAddProjDlg::OnLButtonUp(UINT nFlags, CPoint point)
{
    // TODO: Add your message handler code here and/or call default
    //theApp.AddNewProject(GetSafeHwnd());
    ::PostMessage(theApp.m_mainhWnd, WM_COMMAND, MAKELONG(ID_PROJECT_CREATEAPROJECT, 0), 0L);
    CFlatDlgBase::OnLButtonUp(nFlags, point);
}



void CAddProjDlg::OnPaint()
{
	//CPaintDC dc(this); // device context for painting
					   // TODO: Add your message handler code here
	CFlatDlgBase::OnPaint();	// for painting messages
}
