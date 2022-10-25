// ProjData.cpp : implementation file
//

#include "stdafx.h"
#include "nxrmRMC.h"
#include "ProjData.h"



// CProjData


CProjData::CProjData() : m_flag(NXRMC_PROJ_NOT_VALID), m_projName(L"Reserved")
{

}

CProjData::CProjData(CString projName, int picID, int fileCount)
    : m_projName(projName)
    , m_fileCount(fileCount)
    , m_imageID(picID)
    ,m_hbmpAllUsers(0)
{
}

CProjData::CProjData(CString projName, int picID, CString owner, CString invitor)
    : m_projName(projName)
    , m_fileCount(0)
    , m_imageID(picID)
    , m_hbmpAllUsers(0)
    , m_ownerName(owner)
    , m_invitorName(invitor)

{
}

void CProjData::LoadData()
{
    if (!m_imageID)
    {
        m_hbmpAllUsers = NULL;
        return;
    }
    m_hbmpAllUsers = ::LoadBitmap(AfxGetInstanceHandle(), MAKEINTRESOURCE(m_imageID));
}


CProjData::~CProjData()
{
    DeleteObject(m_hbmpAllUsers);
    m_hbmpAllUsers = 0;
}


// CProjData member functions


// //This is the function that is to add bitmap
BOOL CProjData::AppendBitmap(HBITMAP addOneUser)
{
    return 0;
}
