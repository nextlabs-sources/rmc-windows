// ActivityLogDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ActivityLogDlg.h"
#include "afxdialogex.h"
#include "nxrmRMC.h"
#include <algorithm>

// CActivityLogDlg dialog

IMPLEMENT_DYNAMIC(CActivityLogDlg, CFlatDlgBase)

CActivityLogDlg::CActivityLogDlg(CWnd* pParent /*=NULL*/)
	: CFlatDlgBase(IDD_DIALOG_FILEACTIVITY, pParent)
	, m_strFileName(_T(""))
{

}

CActivityLogDlg::~CActivityLogDlg()
{
}

void CActivityLogDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_FILEACTIVITY, m_lstFileActivity);
	DDX_Control(pDX, IDC_STATIC_FILENAME, m_stcFileName);
	DDX_Text(pDX, IDC_STATIC_FILENAME, m_strFileName);
}


BEGIN_MESSAGE_MAP(CActivityLogDlg, CDialogEx)
END_MESSAGE_MAP()


// CActivityLogDlg message handlers


BOOL CActivityLogDlg::OnInitDialog()
{
	CFlatDlgBase::OnInitDialog();

	CFont *m_pFont = new CFont();
	m_pFont->CreatePointFont(145, _T("Arial Bold"));
	m_stcFileName.SetFont(m_pFont, TRUE);

	CRect rcList;
	m_lstFileActivity.GetWindowRect(&rcList);
	int width = rcList.Width();
	m_lstFileActivity.InsertColumn(0, theApp.LoadString(IDS_ACTIVITY), LVCFMT_LEFT, width * 7/15);
	m_lstFileActivity.InsertColumn(1, theApp.LoadString(IDS_APPLICATION), LVCFMT_LEFT, width / 6);
	m_lstFileActivity.InsertColumn(2, theApp.LoadString(IDS_DEVICE), LVCFMT_LEFT, width / 6);
	m_lstFileActivity.InsertColumn(3, theApp.LoadString(IDS_TIME), LVCFMT_LEFT, width / 5);

	m_lstFileActivity.SetExtendedStyle(LVS_EX_SUBITEMIMAGES | LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP | LVS_EDITLABELS);

	m_imagelist.Create(32, 32, ILC_COLOR32, 0, 100);
	HICON icon = LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ALLOW));
	m_imagelist.Add(icon);
	DestroyIcon(icon);
	icon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDI_DENY));
	m_imagelist.Add(icon);
	DestroyIcon(icon);

	LVITEM listItem;
	if(m_FileActivities.size() != 0)
		m_lstFileActivity.SetImageList(&m_imagelist, LVSIL_SMALL);
	for (auto act : m_FileActivities) {
		int i = m_lstFileActivity.GetItemCount();
		m_lstFileActivity.InsertItem(i, L"");
		memset((void*)&listItem, 0, sizeof(LVITEM));
		listItem.mask = LVIF_IMAGE|LVIF_TEXT;
		std::transform(act.strOperation.begin(), act.strOperation.end(), act.strOperation.begin(), towlower);
		CString fName;
		if (act.bAllowed) {
			if (0 == act.strOperation.compare(L"share"))
				act.strOperation = L"shared";
			else if (0 == act.strOperation.compare(L"view"))
				act.strOperation = L"viewed";
			else if (0 == act.strOperation.compare(L"protect"))
				act.strOperation = L"protected";
			else if (0 == act.strOperation.compare(L"print"))
				act.strOperation = L"printed";
			else if (0 == act.strOperation.compare(L"download"))
				act.strOperation = L"downloaded";
			else if (0 == act.strOperation.compare(L"delete"))
				act.strOperation = L"deleted";
			else if (0 == act.strOperation.compare(L"revoke"))
				act.strOperation = L"revoked";
            else if (0 == act.strOperation.compare(L"remove user"))
                act.strOperation = L"removed user(s) from";
            else if (0 == act.strOperation.compare(L"edit/save"))
                act.strOperation = L"edited and saved";
            else if (0 == act.strOperation.compare(L"decrypt"))
                act.strOperation = L"decrypted";
            else if (0 == act.strOperation.compare(L"copy content"))
                act.strOperation = L"copied the content of";
            else if (0 == act.strOperation.compare(L"capture screen"))
                act.strOperation = L"captured the screen for";
            else if (0 == act.strOperation.compare(L"classify"))
                act.strOperation = L"classified";
            else if (0 == act.strOperation.compare(L"reshare"))
                act.strOperation = L"reshared";

			fName.Format(theApp.LoadString(IDS_FMT2_HAS_THE_FILE), act.strOperator.c_str(), act.strOperation.c_str());
			listItem.iImage = 0;
		}
		else {
			fName.Format(theApp.LoadString(IDS_FMT2_HAS_TRY_FILE), act.strOperator.c_str(), act.strOperation.c_str());
			listItem.iImage = 1;
		}
		listItem.pszText = fName.GetBuffer();
		listItem.iItem = i;
		listItem.iSubItem = 0;
		m_lstFileActivity.SetItem(&listItem);
		m_lstFileActivity.SetItemText(i, 1, act.strDevice.c_str());
		m_lstFileActivity.SetItemText(i, 2, act.strDeviceName.c_str());
		m_lstFileActivity.SetItemText(i, 3, act.strFileTime.c_str());
	}

	if (m_FileActivities.size() == 0) {
		m_lstFileActivity.InsertItem(0, L"");
		memset((void*)&listItem, 0, sizeof(LVITEM));
		listItem.mask = LVIF_TEXT;
		CString fName = theApp.LoadString(IDS_NO_ACTIVITY);
		listItem.pszText = fName.GetBuffer();
		listItem.iItem = 0;
		listItem.iSubItem = 0;
		m_lstFileActivity.SetItem(&listItem);
	}
	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}
