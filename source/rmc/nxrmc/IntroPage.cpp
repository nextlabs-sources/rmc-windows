// IntroPage.cpp : implementation file
//

#include "stdafx.h"
#include <VersionHelpers.h>
#include "nxrmRMC.h"
#include "IntroPage.h"
#include "afxdialogex.h"
#include "weblogondlg.hpp"



// CIntroPage dialog

IMPLEMENT_DYNAMIC(CIntroPage, CFlatDlgBase)

CIntroPage::CIntroPage(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_DIALOG_INTRO, pParent), m_isProjIntro(false)
{
}

CIntroPage::~CIntroPage()
{
}

void CIntroPage::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_STATIC_INTRO, m_introImage);
    DDX_Control(pDX, IDC_BUTTON_NEXT, m_nextBtn);
    DDX_Control(pDX, IDC_BUTTON_PREV, m_prevBtn);
    DDX_Control(pDX, IDC_BUTTON_SIGNUP, m_signupBtn);
    DDX_Control(pDX, IDC_STEP_IMAGE, m_stepImage);
    DDX_Control(pDX, IDC_BUTTON_LOGIN, m_btnLogin);
    DDX_Control(pDX, IDC_BUTTON_LOGO, m_btnLogo);
    //DDX_Control(pDX, IDC_STATIC_ALREADY, m_already);
    //DDX_Control(pDX, IDC_STATIC_SEP, m_sep);
    //DDX_Control(pDX, IDC_STATIC_INTRO_DESC, m_introDesc);
    //DDX_Control(pDX, IDC_BUTTON_ALREADY, m_btnAlready);
}


BEGIN_MESSAGE_MAP(CIntroPage, CDialogEx)
    ON_BN_CLICKED(IDC_BUTTON_NEXT, &CIntroPage::OnBnClickedButtonNext)
    ON_BN_CLICKED(IDC_BUTTON_PREV, &CIntroPage::OnBnClickedButtonPrev)
    ON_BN_CLICKED(IDC_BUTTON_SIGNUP, &CIntroPage::OnBnClickedButtonSignup)
    ON_BN_CLICKED(IDC_BUTTON_LOGIN, &CIntroPage::OnBnClickedButtonLogin)
    ON_BN_CLICKED(IDC_SKIP_LOGIN, &CIntroPage::OnBnClickedSkipLogin)
    ON_WM_CTLCOLOR()
    ON_WM_PAINT()
    ON_COMMAND(IDCANCEL, &CIntroPage::OnIdcancel)
	ON_WM_TIMER()
    ON_WM_DESTROY()
END_MESSAGE_MAP()


// CIntroPage message handlers


BOOL CIntroPage::OnInitDialog()
{
    CDialogEx::OnInitDialog();

    // TODO:  Add extra initialization here
    int abc = m_nIDHelp;
    m_nImgIndex = 0;

    const int *pImageArray;
    if (m_isProjIntro)
    {
        pImageArray = ProjImageArray;
    }
    else
    {
        pImageArray = ImageArray;
    }

    for (int i = 0; i < NX_RMC_MAX_INTROIMAGES; i++)
    {
        m_imageHandles[i] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(pImageArray[i]));
        m_stepimageHandles[i] = LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(stepImageArray[i]));
    }

    m_ToolTip.Create(this);
    m_prevBtn.SetImages(IDB_PNG_PREV, IDB_PNG_BACK_DISABLED, 0);
    m_ToolTip.AddTool(&m_prevBtn, _T("Go to previous page"));
    m_ToolTip.Activate(TRUE);

    m_nextBtn.SetImages(IDB_PNG_NEXT, IDB_PNG_NEXT_DISABLED, 0);
    m_ToolTip.AddTool(&m_nextBtn, _T("Go to next page"));
    m_ToolTip.Activate(TRUE);
    m_sep.SubclassDlgItem(IDC_STATIC_SEP, this);

    if (m_isProjIntro)
    {
        m_btnLogo.SetImages(IDB_PNG_PROJ_LOGO, 0, IDB_PNG_PROJ_LOGO, RGB(255, 255, 255));
        m_btnLogin.SetImages(IDB_PNG_BTN_CANCEL, 0, IDB_PNG_BTN_CANCEL, RGB(255, 255, 255));
        m_signupBtn.SetImages(  IDB_PNG_CREATE_PROJ,0, IDB_PNG_SIGNUP, RGB(255, 255, 255));
        m_introDesc.SubclassDlgItem(IDC_STATIC_INTRO_DESC, this);
        SetAllOrignRects();

        m_pBitmap = new CGdiPlusBitmapResource;
        bool bBmpLoaded = false;

        bBmpLoaded = m_pBitmap->Load(MAKEINTRESOURCE(IDB_PNG_PROJ_HELP), L"PNG", AfxGetInstanceHandle());

        if (bBmpLoaded)
        {
            HBITMAP hbmp;
            m_pBitmap->m_pBitmap->GetHBITMAP(Gdiplus::Color(255, 255, 255, 255), &hbmp);
            m_introDesc.SetScaledImage(hbmp);
        }
    }
    else
    {
        m_btnLogo.SetImages(IDB_PNG_INTRO_LOGO, 0, IDB_PNG_INTRO_LOGO, RGB(255, 255, 255));

        m_signupBtn.SetImages(IDB_PNG_SIGNUP, 0, IDB_PNG_SIGNUP, RGB(255, 255, 255));
        m_btnLogin.SetImages(IDB_PNG_INTRO_LOGIN, IDB_PNG_INTRO_LOGIN, 0);
        m_btnAlready.SubclassDlgItem(IDC_BUTTON_ALREADY, this);
       
        m_introDesc.SubclassDlgItem(IDC_STATIC_INTRO_DESC, this);

        m_btnAlready.SetImages(IDB_PNG_ALREADY, IDB_PNG_ALREADY, 0);

        SetAllOrignRects();

        m_pBitmap = new CGdiPlusBitmapResource;
        bool bBmpLoaded = false;

        bBmpLoaded = m_pBitmap->Load(MAKEINTRESOURCE(IDB_PNG_WELCOME_INTRO_DESC), L"PNG", AfxGetInstanceHandle());

        if (bBmpLoaded)
        {
            HBITMAP hbmp;
            m_pBitmap->m_pBitmap->GetHBITMAP(Gdiplus::Color(255, 255, 255, 255), &hbmp);
            m_introDesc.SetScaledImage(hbmp);
        }

    }

	SetTimer(1, 5000, 0);
    m_nAnimPos = NX_RMC_ANINATION_FRAMES;
    m_bGoRight = true;

    auto hIcon = AfxGetApp()->LoadIcon(IDI_NXRMC);
    SetIcon(hIcon, true);
    SetIcon(hIcon, false);
    
    m_stepImage.SetScaledImage(m_stepimageHandles[0]);
    m_introImage.SetScaledImage(m_imageHandles[0]);

 
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}

void CIntroPage::SetChildRect(CWnd *pWnd)
{
    ASSERT(pWnd != NULL);
    CRect rc;
    CtrlSizeData ctrlSize;
    pWnd->GetWindowRect(&rc);
    ScreenToClient(&rc);
    ctrlSize.rc = rc;
    ctrlSize.pCtrl = pWnd;
    //m_originRects.push_back(rc);
    //m_ctrls.push_back(pWnd);
    m_ctrlData.push_back(ctrlSize);
}
void  CIntroPage::SetAllOrignRects()
{
    CtrlSizeData ctrlSize;
    CRect rc;
    GetClientRect(&rc);

    ctrlSize.rc = rc;
    ctrlSize.pCtrl = this;
    m_ctrlData.push_back(ctrlSize);
    //SetChildRect(&)
    SetChildRect(&m_introImage);
    SetChildRect(&m_nextBtn);
    SetChildRect(&m_prevBtn);
    SetChildRect(&m_signupBtn);
    SetChildRect(&m_stepImage);
    SetChildRect(&m_btnLogo);
    SetChildRect(&m_sep);
    SetChildRect(&m_introDesc);

    if (m_isProjIntro)
    {
        SetChildRect(&m_btnLogin);
    }
    else
    {
        SetChildRect(&m_btnLogin);
        SetChildRect(&m_btnAlready);
    }
}


void CIntroPage::OnBnClickedButtonNext()
{
    if (m_nAnimPos < NX_RMC_ANINATION_FRAMES)
    {
        KillTimer(1);
        return;
    }
    m_nImgIndex++;

    m_nImgIndex = m_nImgIndex % NX_RMC_MAX_INTROIMAGES;
    //m_stepImage.SetBitmap(m_stepimageHandles[m_nImgIndex]);
    m_stepImage.SetScaledImage(m_stepimageHandles[m_nImgIndex]);
    m_introImage.SetScaledImage(m_imageHandles[m_nImgIndex]);
    StartAnimation(true);
    KillTimer(1);
}


void CIntroPage::OnBnClickedButtonPrev()
{
    if (m_nAnimPos < NX_RMC_ANINATION_FRAMES)
    {
        KillTimer(1);
        return;
    }

    m_nImgIndex--;

    m_nImgIndex = m_nImgIndex < 0 ? NX_RMC_MAX_INTROIMAGES - 1 : m_nImgIndex;
    //
    m_introImage.SetScaledImage(m_imageHandles[m_nImgIndex]);
    m_stepImage.SetScaledImage(m_stepimageHandles[m_nImgIndex]);
    StartAnimation(false);
	KillTimer(1);
}


void CIntroPage::OnBnClickedButtonSignup()
{
    if (m_isProjIntro)
    {
        if (theApp.AddNewProject(theApp.m_mainhWnd))
        {
            theApp.m_homePage.m_btnActivateProj.ShowWindow(SW_HIDE);
            theApp.m_homePage.m_strActivate.ShowWindow(SW_HIDE);
            theApp.m_homePage.m_grpActivate.ShowWindow(SW_HIDE);
            theApp.m_homePage.DisplayProjectList();
        }
    }
    else
    {
        ShowWindow(SW_HIDE);
        theApp.ShowSignup();
    }
}

void CIntroPage::OnBnClickedSkipLogin()
{
    ::ShowWindow(m_hWnd, SW_HIDE);
    theApp.ShowPage(MD_HOMEPAGE);
}



void CIntroPage::OnBnClickedButtonLogin()
{
    if (m_isProjIntro)
    {   //This is cancel button at this time
        theApp.ShowPage(MD_HOMEPAGE);
    }
    else
    {
        ShowWindow(SW_HIDE);
        theApp.ShowLogin();
    }
}


HBRUSH CIntroPage::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
    HBRUSH hbr = CFlatDlgBase::OnCtlColor(pDC, pWnd, nCtlColor);

    // TODO:  Change any attributes of the DC here

    if (nCtlColor == CTLCOLOR_STATIC &&
        pWnd->GetDlgCtrlID() == IDC_STATIC_MAIN_HELP1 || pWnd->GetDlgCtrlID() == IDC_STATIC_MAIN_HELP3)
    {
        pDC->SetTextColor(NX_NEXTLABS_STANDARD_GREEN);
    }
    return hbr;
}


void CIntroPage::OnPaint()
{
    CFlatDlgBase::OnPaint();
}


BOOL CIntroPage::PreTranslateMessage(MSG* pMsg)
{
    // TODO: Add your specialized code here and/or call the base class

    m_ToolTip.RelayEvent(pMsg);
    return CFlatDlgBase::PreTranslateMessage(pMsg);
}


void CIntroPage::OnIdcancel()
{
    // TODO: Add your command handler code here
}


void CIntroPage::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
    switch (nIDEvent)
    {
    case 1:
        if (!IsWindowVisible())
        {
            break;;
        }

        m_nImgIndex = (m_nImgIndex + 1) % NX_RMC_MAX_INTROIMAGES;

        //m_stepImage.SetBitmap(m_stepimageHandles[m_nImgIndex]);
        //m_introImage.SetScaledImage(m_imageHandles[m_nImgIndex]);
        m_stepImage.SetScaledImage(m_stepimageHandles[m_nImgIndex]);
        //m_stepImage.InvalidateRect(NULL);
        //m_stepImage.GetBitmap();
        StartAnimation(true);

        break;
    case 2:
        ShowImageInPos(m_nAnimPos);
        if (m_bGoRight && m_nAnimPos == NX_RMC_ANINATION_FRAMES || !m_bGoRight && m_nAnimPos == 0)
        {
            m_nAnimPos = NX_RMC_ANINATION_FRAMES; //Indecate no animation at this time.
            KillTimer(2);
            break;
        }
        if (m_bGoRight)
        {
            m_nAnimPos++;
        }
        else
        {
            m_nAnimPos--;
        }
        break;
    }
	CFlatDlgBase::OnTimer(nIDEvent);
}

void CIntroPage::StartAnimation(bool bGoRight)
{
    SetTimer(2, 1000 / NX_RMC_ANINATION_FRAMES, 0);
    m_nAnimPos = bGoRight ? 1 : NX_RMC_ANINATION_FRAMES - 1;
    m_bGoRight = bGoRight;
}


void CIntroPage::ShowImageInPos(int nAnimPos)
{

    //int nOldPos = (m_nImgIndex - 1) % NX_RMC_MAX_INTROIMAGES;
    auto pdc = m_introImage.GetDC();
    ASSERT(pdc != NULL);
    CDC memDC;
    memDC.CreateCompatibleDC(pdc);
    int nImgIdx;
    if (m_bGoRight)
    {
        nImgIdx = m_nImgIndex ? (m_nImgIndex - 1) : NX_RMC_MAX_INTROIMAGES - 1;
    }
    else
    {
        nImgIdx = m_nImgIndex == NX_RMC_MAX_INTROIMAGES - 1 ? 0 : m_nImgIndex+1;
    }
    auto oldOgj = memDC.SelectObject(m_imageHandles[nImgIdx]);

    CRect rc;
    m_introImage.GetWindowRect(&rc);

    CDC memDCNew;
    memDCNew.CreateCompatibleDC(pdc);
    auto oldOgj1 = memDCNew.SelectObject(m_imageHandles[m_nImgIndex]);
    BITMAP bm;
    GetObject(m_imageHandles[m_nImgIndex], sizeof(BITMAP), &bm);


    int startPos = rc.Width() * nAnimPos / 10;
    int startPosBm = bm.bmWidth * nAnimPos / 10;

    pdc->SetStretchBltMode(HALFTONE);
    if (m_bGoRight)
    {   
        pdc->StretchBlt(0, 0, rc.Width() - startPos, rc.Height(), &memDC, startPosBm, 0, bm.bmWidth - startPosBm, bm.bmHeight, SRCCOPY);
        pdc->StretchBlt(rc.Width() - startPos, 0, startPos, rc.Height(), &memDCNew, 0, 0, startPosBm, bm.bmHeight, SRCCOPY);
    }
    else
    {
        pdc->StretchBlt(0, 0, rc.Width() - startPos, rc.Height(), &memDCNew, startPos, 0, bm.bmWidth - startPosBm - 1, bm.bmHeight, SRCCOPY);
        pdc->StretchBlt(rc.Width() - startPos, 0, startPos, rc.Height(), &memDC, 0, 0, startPosBm-1, bm.bmHeight, SRCCOPY);
    }
    memDC.SelectObject(oldOgj);
    memDCNew.SelectObject(oldOgj1);
    memDC.DeleteDC();
    memDCNew.DeleteDC();
    ReleaseDC(pdc);
}

void CIntroPage::OnOK()
{

}


void CIntroPage::OnDestroy()
{
    CFlatDlgBase::OnDestroy();

    for (int i = 0; i < NX_RMC_MAX_INTROIMAGES; i++)
    {
        DeleteObject(m_imageHandles[i]);
        DeleteObject(m_stepimageHandles[i]);
    }
    delete(m_pBitmap);
    m_pBitmap = NULL;
}

int CIntroPage::ResizeWindow(int cx, int cy)
{
        if (cx == 0 || cy == 0)
        {
            return cy;
        }
        CRect rc;
        GetWindowRect(&rc);
        float compressRatioY = (float)cy / m_ctrlData[0].rc.Height();
        float compressRatioX = (float)cx / m_ctrlData[0].rc.Width();
        if (compressRatioY <= 1.0 || compressRatioX <= 1.0)
        {
            float compressRatio = compressRatioY < compressRatioX ? compressRatioY : compressRatioX;
            
            if (compressRatioY < compressRatioX)
            {
                int newWidth = (INT)(m_ctrlData[0].rc.Width()*compressRatio);
                SetWindowPos(GetParent(), (cx - newWidth)/2, 0, newWidth, (int)(m_ctrlData[0].rc.Height()*compressRatio), SWP_NOZORDER);
            }
            else
            {
                int newHeight = (INT)(m_ctrlData[0].rc.Height()*compressRatio);
                SetWindowPos(GetParent(), 0, (cy - newHeight) / 2, (INT)(m_ctrlData[0].rc.Width()*compressRatio), newHeight, SWP_NOZORDER);
            }
            for (size_t i = 1; i < m_ctrlData.size(); i++)
            {
                ScaleCtrlWnd(m_ctrlData[i].pCtrl, m_ctrlData[i].rc, compressRatio);
            }
        }
        else
        {
            SetWindowPos(GetParent(), (cx - m_ctrlData[0].rc.Width()) / 2, (cy - m_ctrlData[0].rc.Height()) / 2, (INT)(m_ctrlData[0].rc.Width()), (int)(m_ctrlData[0].rc.Height()), SWP_NOZORDER);
            for (size_t i = 1; i < m_ctrlData.size(); i++)
            {
                ScaleCtrlWnd(m_ctrlData[i].pCtrl, m_ctrlData[i].rc, 1.0);
            }
        }
        InvalidateRect(NULL);
    return cy;
}