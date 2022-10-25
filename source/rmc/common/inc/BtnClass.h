#if !defined(AFX_MYCLASS_H__8E7A4504_82E5_4C81_8A30_642AC65A4550__INCLUDED_)
#define AFX_MYCLASS_H__8E7A4504_82E5_4C81_8A30_642AC65A4550__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyClass.h : header file
//

#include "CGdiPlusBitmap.h"

/////////////////////////////////////////////////////////////////////////////
// CBtnClass window

enum ImageVarType { NX_USE_IMAGE_ID, NX_USE_BMP_HANDLE, NX_USE_BMP_TEXT_VERTICAL, 
    NX_USE_BMP_TEXT_HORIZONTAL,NX_USE_COLOR_AND_TEXT, NX_USE_IMAGE_AS_CHKBTN, 
    NX_USE_COLOR_CIRCLE_TEXT, NX_USE_COLOR_AND_TEXT_BORDER, NX_USE_BMP_TEXT_HORIZONTAL_CIRCLE_TEXT
};

class CBtnClass : public ::CButton
{
// Construction
    DECLARE_DYNAMIC(CBtnClass)
public:
    CBtnClass(int nIdNormal, int nIdFocusDisable = 0, int nIdDepressed = 0, CString szLabel = L"")
        : m_BtnImgNormalID(nIdNormal), m_BtnImgDisabledlID(nIdFocusDisable), m_BtnImgPressedID(nIdDepressed), m_label(szLabel), m_nBtnFlag(BST_UNCHECKED), m_textAlign(BS_CENTER), m_circleText(L"1")
    {};
    CBtnClass()
        
    : m_BtnImgNormalID(0), m_BtnImgDisabledlID(0), m_BtnImgPressedID(0), m_label(L""), m_nBtnFlag(BST_UNCHECKED), m_textAlign(BS_CENTER), m_circleText(L"1")
    {};

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBtnClass)
	public:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBtnClass();

    void CBtnClass::SetImagetText(int activeID, int deactiveID, ImageVarType drawType, COLORREF rgbHighlight, CString text = L"");

	void SetColors(COLORREF rgbBkGrnd, COLORREF rgbFrGrnd, BOOL bBorder = FALSE,
        COLORREF rgbDisabledBkGrnd = RGB(204, 204, 204), COLORREF rgbDisabledFrGrnd = RGB(127, 127, 127));

    void SetImages(int normalID, int disableID, int depressedID, COLORREF rgbBk = RGB(255, 255, 255));

    void SetCheckBtnImages(int uncheckedID, int checkedID, COLORREF rgbBk = RGB(255, 255, 255));


    void SetImagesHandles(HBITMAP hNormalID, HBITMAP hDisableID, HBITMAP hDepressedID);

    void SetLabel(CString szLabel);

    void SetBtnCheck(int nflag)
    {
        m_nBtnFlag = nflag;
        InvalidateRect(NULL);
    }

    int GetBtnCheck()
    {
        return m_nBtnFlag;
    }

    void SetTextAlignment(int align)
    {
        m_textAlign = align;
    }
    void SetCircleText(const CString circleText)
    {
        m_circleText = circleText;
    }

    void SetCircleNumber(int circleNumber);

    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);

	// Generated message map functions
protected:
	//{{AFX_MSG(CBtnClass)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()


    ImageVarType m_imgFlag;
    int m_BtnImgNormalID;
    int m_BtnImgDisabledlID;
    int m_BtnImgPressedID;

    HBITMAP m_hBtnImgNormal;
    HBITMAP m_hBtnImgDisabled;
    HBITMAP m_hBtnImgPressed;
    int m_nBtnFlag;

    COLORREF m_rgbBkGrnd;
    COLORREF m_rgbFrGrnd;
    COLORREF m_rgbDisabledBkGrnd;
    COLORREF m_rgbDisabledFrGrnd;
    int m_textAlign;

    CString m_circleText;
    CString m_label;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYCLASS_H__8E7A4504_82E5_4C81_8A30_642AC65A4550__INCLUDED_)
