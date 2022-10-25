#pragma once

#include "btnclass.h"
// CNxButton
#ifndef ImageVarType
//enum ImageVarType { NX_USE_IMAGE_ID, NX_USE_BMP_HANDLE, NX_USE_BMP_TEXT_VERTICAL, 
//    NX_USE_BMP_TEXT_HORIZONTAL, NX_USE_COLOR_AND_TEXT, NX_USE_IMAGE_AS_CHKBTN };
#endif


class CNxButton : public CMFCButton
{
	DECLARE_DYNAMIC(CNxButton)

public:
	CNxButton();
	virtual ~CNxButton();

    void SetTextAlignment(int align)
    {
        m_textAlign = align;
    }
    void SetColors(COLORREF rgbBkGrnd, COLORREF rgbFrGrnd,
        COLORREF rgbDisabledBkGrnd = RGB(204, 204, 204), COLORREF rgbDisabledFrGrnd = RGB(127, 127, 127));
    void SetColorsWithBorder(COLORREF rgbBkGrnd, COLORREF rgbFrGrnd, 
        COLORREF rgbDisabledBkGrnd = RGB(204, 204, 204), COLORREF rgbDisabledFrGrnd = RGB(127, 127, 127), COLORREF rgbBorder = RGB(225, 225, 225));


    void SetColorsCircle(COLORREF rgbBkGrnd, COLORREF rgbFrGrnd, 
        COLORREF rgbDisabledBkGrnd = RGB(204, 204, 204), COLORREF rgbDisabledFrGrnd = RGB(127, 127, 127), int font_size = 8);


protected:
	DECLARE_MESSAGE_MAP()
    ImageVarType m_imgFlag;
    COLORREF m_rgbBkGrnd;
    COLORREF m_rgbFrGrnd;
    COLORREF m_rgbDisabledBkGrnd;
    COLORREF m_rgbDisabledFrGrnd;
    COLORREF m_rgbBorder;
    int m_textAlign;
    int m_fontSize;

public:
    virtual void OnDraw(CDC* pDC, const CRect& rect, UINT uiState);
//    virtual void OnDrawBorder(CDC* pDC, CRect& rectClient, UINT uiState);
//    virtual void OnDrawFocusRect(CDC* pDC, const CRect& rectClient);
//    virtual void OnDrawText(CDC* pDC, const CRect& rect, const CString& strText, UINT uiDTFlags, UINT uiState);
//    virtual void OnFillBackground(CDC* pDC, const CRect& rectClient);
//    virtual void OnDrawParentBackground(CDC* pDC, CRect rectClient);
    virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
};


