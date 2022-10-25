#pragma once
//#include "nxrmRMC.h"
#include <nx\rmc\session.h>
#include "resource.h"
// CProjData command target

using namespace std;

typedef enum SORTORDER {
    OrderByFileNameAscend = 0x00000001,
    OrderByFileNameDescend = 0x00000002,
    OrderByCreationTimeAscend = 0x00000004,
    OrderByCreationTimeDescend = 0x00000008,
    OrderBySizeAscend = 0x00000010,
    OrderBySizeDescend = 0x00000020,
    OrderByDisplayNameAscend = 0x00000040,
    OrderByDisplayNameDescend = 0x00000080,
    OrderByFileTypeAscend = 0x00000100,
    OrderByFileTypeDescend = 0x00000200,
    OrderByOwnerNameAscend = 0x00000400,
    OrderByOwnerNameDescend = 0x00000800

} SORTORDER;

typedef struct {
    CString lastName;
    CString firstName;
    HBITMAP hUserPic;
} ProjUserName;

typedef struct {
    CString fileName;
    SYSTEMTIME tm;
    int sizeInByte;
} F;

enum FromPage
{
    NXRMC_FROM_HOME_PAGE = 0,
    NXRMC_FROM_PROJECT_PAGE,
    NXRMC_FROM_PROJECT_SWITCH_PAGE
};

enum ProjFlag
{
    NXRMC_PROJ_NOT_VALID = 0,
    NXRMC_PROJ_BY_ME,
    NXRMC_PROJ_BY_OTHERS,
    NXRMC_FROM_BY_OTHERS_PENDING
};


class CProjData 
{
public:
    CProjData();
    CProjData(CString projName, int picID, int fileCount);
    CProjData(CString projName, int picID, CString owner, CString invitor);
    void LoadData();
	virtual ~CProjData();
    // //This is the function that is to add bitmap
    BOOL AppendBitmap(HBITMAP addOneUser);
    HBITMAP m_hbmpAllUsers;
    // //Project Name
    CString m_projName;
    int m_fileCount;
    wstring m_nID;
    wstring m_nCode;
    int m_imageID;
    ProjFlag m_flag;
    __int64 m_creationTime;
	int m_imembercount;
	vector<NX::RmProjectMemberSimple> m_Simmembers;
    vector<NX::RmProjectFile> m_files;
    vector<NX::RmProjectFile> m_Dirs;
    CString m_ownerName;
    CString m_invitorName;
    wstring m_ownerID;
    CString m_desc;
    //NX::RmProjectOwner m_owner;
    //NX::RmProject m_proj;
    vector <CString> m_initials;
};


