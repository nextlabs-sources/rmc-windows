// UserEdit.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "UserEdit.h"
#include "afxdialogex.h"
#include "ChangePswd.h"
#include "EditProfileDlg.h"

#include <nx\rmc\session.h>
#include <nx\common\stringex.h>
#include <nx\common\time.h>
#include <thread>

using namespace NX;
// CUserEdit dialog
extern void CancelUpgradeCallback();
CString GetInitial(CString fullName);


IMPLEMENT_DYNAMIC(CUserEdit, CFlatDlgBase)

CUserEdit::CUserEdit(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_USER_EDIT, pParent), m_bNeedRefresh(false)
{

}

CUserEdit::~CUserEdit()
{
}

void CUserEdit::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_BUTTON_LOGOUT, m_logoutBtn);
    DDX_Control(pDX, IDC_BUTTON_CHANG_PICTURE, m_btnAddPhoto);
    DDX_Control(pDX, IDC_STATIC_EMAIL, m_emailAddr);
    DDX_Control(pDX, IDC_BUTTON_CHANGE_PSWD, m_changPswd);
    DDX_Control(pDX, IDC_BUTTON_MANAGEPROFILE, m_manageProf);
}


BEGIN_MESSAGE_MAP(CUserEdit, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_CHANGE_PSWD, &CUserEdit::OnBnClickedButtonChangePswd)
    ON_BN_CLICKED(IDC_BUTTON_LOGOUT, &CUserEdit::OnBnClickedButtonLogout)
    ON_BN_CLICKED(IDC_BUTTON_CHANG_PICTURE, &CUserEdit::OnBnClickedButtonChangPicture)
    ON_WM_CTLCOLOR()
    ON_WM_PAINT()
    ON_BN_CLICKED(IDC_BUTTON_MANAGEPROFILE, &CUserEdit::OnBnClickedButtonManageprofile)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CUserEdit message handlers


void CUserEdit::OnBnClickedButtonChangePswd()
{
    // TODO: Add your control notification handler code here
    CChangePswd dlg(this);
    if (dlg.DoModal() == IDOK)
    {
        OnCancel();
        m_bLogoff = true;
    }
}


void CUserEdit::OnBnClickedButtonLogout()
{
    // TODO: Add your control notification handler code here
	m_bLogoff = true;
	OnCancel();
}


void CUserEdit::OnBnClickedButtonChangPicture()
{
    // TODO: Add your control notification handler code here
    //CFileDialog dlg(TRUE, L"png", L"User picture files", OFN_FILEMUSTEXIST,
    //    L"Picture Files (*.png)|*.bmp|*.jpg", this);
    //dlg.DoModal();
}

void CUserEdit::RefreshUserName()
{
	auto displaName = theApp.m_pSession->GetCurrentUser().GetName();
	//m_userName.SetWindowTextW(displaName.c_str());
}

BOOL CUserEdit::OnInitDialog()
{
    CDialogEx::OnInitDialog();
    ShowWindow(SW_SHOW);
    m_logoutBtn.SetColors(NX_NEXTLABS_STANDARD_RED, RGB(255, 255, 255));
	//m_btnAddPhoto.SetColors(RGB(255, 255, 255), NX_NEXTLABS_STANDARD_GREEN);

    auto initial = GetInitial(theApp.m_usrName.c_str());
    m_btnAddPhoto.SetWindowText(initial);

    RmColorSchemeEntry clrScheme;
    if (initial.GetLength())
    {
        clrScheme = GetColorScheme(initial.GetAt(0));
    }
    else
    {
        clrScheme = GetColorScheme(L' '); //Get default color here
    }

    m_btnAddPhoto.SetColorsCircle(clrScheme.bg, clrScheme.fg);

    if (theApp.IsSkyDRMUser())
    {
        m_changPswd.SetColorsWithBorder(NX_NEXTLABS_STANDARD_WHITE, NX_NEXTLABS_STANDARD_BLACK);
    }
    else
    {
        m_changPswd.SetColorsWithBorder(NX_NEXTLABS_STANDARD_WHITE, NX_NEXTLABS_STANDARD_BLACK);
        m_changPswd.EnableWindow(FALSE);
    }


    m_manageProf.SetColorsWithBorder(NX_NEXTLABS_STANDARD_WHITE, NX_NEXTLABS_STANDARD_BLACK);



	RefreshUserName();
	auto emailAddress =  theApp.m_pSession->GetCurrentUser().GetEmail();
	m_emailAddr.SetWindowTextW(emailAddress.c_str());
    //GetParent->CenterWindow()
    CenterWindow(GetParent());
    return 0;
}


HBRUSH CUserEdit::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CFlatDlgBase::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here

    // TODO:  Return a different brush if the default is not desired
    return hbr;
}


void CUserEdit::OnPaint()
{
    CFlatDlgBase::OnPaint();
}


void CUserEdit::OnBnClickedButtonManageprofile()
{
	theApp.ShowEditUserProfile(this->m_hWnd);
}


void CUserEdit::OnDestroy()
{
	CFlatDlgBase::OnDestroy();
	// TODO: Add your message handler code here
}
