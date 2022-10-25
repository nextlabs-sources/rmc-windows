#pragma once
#include <vector>


// CFlatDlgBase dialog
using namespace std;

typedef struct {
    CRect rc;
    CWnd *pCtrl;

}   CtrlSizeData;


class CFlatDlgBase : public CDialogEx
{
	DECLARE_DYNAMIC(CFlatDlgBase)

public:
	CFlatDlgBase(int nID, CWnd* pParent = NULL);   // standard constructor
	virtual ~CFlatDlgBase();
        void AddFontSize(int nPercent, CWnd & ctrlWnd, bool bBold = true, bool bItalic = false);


// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_FILE_LIST };
#endif

protected:
	void SetBackColor(COLORREF rgbBkgrnd);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	CBrush m_bkBrush;
	COLORREF m_rgbBkgrnd;
		
	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnPaint();
    afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
    afx_msg BOOL OnEraseBkgnd(CDC * pDC);
    virtual void DropOneFile(CString folderToSave, CString fileToSave) {};
    virtual void OnColumnclickList(NMHDR *pNMHDR, LRESULT *pResult) {};
    void MoveWindowPos(CWnd * pwnd, int right, int bottom, BOOL bKeepLeft);
    void ScaleCtrlWnd(CWnd * pwnd, CRect & rc, float fFactor);
    void SetChildRect(CWnd * pWnd, vector<CtrlSizeData>& ctrlData);
    void ScaleControlHorizontally(int index, int cx, const vector<CtrlSizeData>& ctrlData);
    void ScaleRightHorizontally(int index, int cx, const vector<CtrlSizeData>& ctrlData);
    void ScaleLeftHorizontally(int index, int cx, const vector<CtrlSizeData>& ctrlData);
    void MoveRightHorizontally(int index, int cx, const vector<CtrlSizeData>& ctrlData);
    void ShrinkRightHorizontally(int index, int cx, const vector<CtrlSizeData>& ctrlData);
    bool IsTextTruncated(CWnd * pCtrl, int percent);
	int GetTextWidth(CWnd * pCtrl, int percent);
    //bool IsTextTruncated(CWnd *pCtrlID);

    CFont &GetFontFromSize(int nPercent, CWnd & ctrlWnd, bool bBold, bool bItalic);

    bool AddTooTipWhenTruncated(CWnd * pParentWnd, CWnd * pWnd, CToolTipCtrl * pTip, CString tipText, int nToolID);

};
