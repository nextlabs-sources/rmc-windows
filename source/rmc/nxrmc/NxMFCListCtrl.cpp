// NxMFCListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "RepoPage.h"
#include "NxMFCListCtrl.h"


// CNxMFCListCtrl

IMPLEMENT_DYNAMIC(CNxMFCListCtrl, CMFCListCtrl)

CNxMFCListCtrl::CNxMFCListCtrl()
{

}

CNxMFCListCtrl::~CNxMFCListCtrl()
{
}


BEGIN_MESSAGE_MAP(CNxMFCListCtrl, CMFCListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, &CNxMFCListCtrl::OnLvnColumnclick)
    ON_WM_DROPFILES()
END_MESSAGE_MAP()



// CNxMFCListCtrl message handlers




void CNxMFCListCtrl::OnLvnColumnclick(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	CFlatDlgBase *pRepoPage = (CFlatDlgBase *)GetParent();
	pRepoPage->OnColumnclickList(pNMHDR, pResult);
	*pResult = 0;
}


bool SeparatePathAndName(const CString fullPath, CString &path, CString &name, const wchar_t sepChar = L'\\')
{
    int pos = fullPath.ReverseFind(sepChar);
    if (pos == -1)
    {
        path = L"";
        name = fullPath;
        return false;
    }
    else
    {
        auto len = fullPath.GetLength();
        name = fullPath.Right(len - pos - 1);
        path = fullPath.Left(pos);
        return true;
    }

}

void CNxMFCListCtrl::OnDropFiles(HDROP hDropInfo)
{
    
    wchar_t sItem[4*MAX_PATH];
    int i = 0;
    while(DragQueryFile(hDropInfo, i++, sItem, 4 * MAX_PATH))
    {
        auto attrib = GetFileAttributes(sItem);
        if (!(attrib & FILE_ATTRIBUTE_NORMAL) && !(attrib & FILE_ATTRIBUTE_ARCHIVE))
        {
            continue;
        }
        CString fullPath = sItem;
        CString path;
        CString name;
        SeparatePathAndName(fullPath, path, name);
        CFlatDlgBase *pFilePage = (CFlatDlgBase *)GetParent();
    
        pFilePage->DropOneFile(path, name);
        break;//Only support one file at this time
    }
    CMFCListCtrl::OnDropFiles(hDropInfo);
}
