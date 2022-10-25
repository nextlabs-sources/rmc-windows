#pragma once
#include <vector>

using namespace std;

enum StaticDrawType {
    NX_DRAWTYPE_TEXT_GRAY_LAST_BLACK, NX_DRAWTYPE_ROUNDRECT, NX_DRAWTYPE_WHITE_BKGRND, NX_DRAWTYPE_USERS_IMAGE, NX_DRAWTYPE_GREEN_BKGRND, 
    NX_DRAWTYPE_RECT, NX_STORAGE_CHART, NX_BULLET_TEXT, NX_SCALE_IMAGE
};

typedef struct {
    COLORREF bg;
    COLORREF fg;
} RmColorSchemeEntry;

// COwnerDrawStatic

const RmColorSchemeEntry colorSchemeDefault =
{ RGB(0x33,0x33,0x33), RGB(0xFF,0xFF,0xFF) };
const RmColorSchemeEntry colorSchemeGray = { RGB(225, 225, 225), RGB(0, 0, 0) };

const RmColorSchemeEntry colorSchemeAlpha[] = {
    { RGB(0xDD,0x21,0x2B), RGB(0xFF,0xFF,0xFF) },
    { RGB(0xFD,0xCB,0x8A), RGB(0x8F,0x93,0x94) },
    { RGB(0x98,0xC4,0x4A), RGB(0xFF,0xFF,0xFF) },
    { RGB(0x1A,0x52,0x79), RGB(0xFF,0xFF,0xFF) },
    { RGB(0xEF,0x66,0x45), RGB(0xFF,0xFF,0xFF) },
    { RGB(0x72,0xCA,0xC1), RGB(0xFF,0xFF,0xFF) },
    { RGB(0xB7,0xDC,0xAF), RGB(0x8F,0x93,0x94) },
    { RGB(0x70,0x5A,0x9E), RGB(0xFF,0xFF,0xFF) },
    { RGB(0xFC,0xDA,0x04), RGB(0x8F,0x93,0x94) },
    { RGB(0xED,0x1D,0x7C), RGB(0xFF,0xFF,0xFF) },
    { RGB(0xF7,0xAA,0xA5), RGB(0xFF,0xFF,0xFF) },
    { RGB(0x4A,0xB9,0xE6), RGB(0xFF,0xFF,0xFF) },
    { RGB(0x60,0x3A,0x18), RGB(0xFF,0xFF,0xFF) },
    { RGB(0x88,0xB8,0xBC), RGB(0xFF,0xFF,0xFF) },
    { RGB(0xEC,0xA8,0x1E), RGB(0xFF,0xFF,0xFF) },
    { RGB(0xDA,0xAC,0xD0), RGB(0xFF,0xFF,0xFF) },
    { RGB(0x6D,0x6E,0x73), RGB(0xFF,0xFF,0xFF) },
    { RGB(0x9D,0x9F,0xA2), RGB(0xFF,0xFF,0xFF) },
    { RGB(0xB5,0xE3,0xEE), RGB(0x8F,0x93,0x94) },
    { RGB(0x90,0x63,0x3D), RGB(0xFF,0xFF,0xFF) },
    { RGB(0xBD,0xAE,0x9E), RGB(0xFF,0xFF,0xFF) },
    { RGB(0xC8,0xB5,0x8E), RGB(0xFF,0xFF,0xFF) },
    { RGB(0xF8,0xBD,0xD2), RGB(0xFF,0xFF,0xFF) },
    { RGB(0xFE,0xD9,0x68), RGB(0x8F,0x93,0x94) },
    { RGB(0xF6,0x96,0x79), RGB(0xFF,0xFF,0xFF) },
    { RGB(0xEE,0x67,0x69), RGB(0xFF,0xFF,0xFF) }
};

const RmColorSchemeEntry colorSchemeDigit[] = {
    { RGB(0xD3,0xE0,0x50), RGB(0x8F,0x93,0x94) },
    { RGB(0xD8,0xEB,0xD5), RGB(0x8F,0x93,0x94) },
    { RGB(0xF2,0x7E,0xA9), RGB(0xFF,0xFF,0xFF) },
    { RGB(0x17,0x82,0xC0), RGB(0xFF,0xFF,0xFF) },
    { RGB(0xCD,0xEC,0xF9), RGB(0x8F,0x93,0x94) },
    { RGB(0xFD,0xE9,0xE6), RGB(0x8F,0x93,0x94) },
    { RGB(0xFC,0xED,0x95), RGB(0x8F,0x93,0x94) },
    { RGB(0xF9,0x9D,0x21), RGB(0xFF,0xFF,0xFF) },
    { RGB(0xF9,0xA8,0x5D), RGB(0xFF,0xFF,0xFF) },
    { RGB(0xBC,0xE2,0xD7), RGB(0x8F,0x93,0x94) }
};

const RmColorSchemeEntry& GetColorScheme(wchar_t c);

class COwnerDrawStatic : public CStatic
{


	DECLARE_DYNAMIC(COwnerDrawStatic)

public:
	COwnerDrawStatic();
	virtual ~COwnerDrawStatic();

protected:
	DECLARE_MESSAGE_MAP()
    COLORREF m_rgbFirstPart;
    COLORREF m_rgbSecondPart;
    //for rectangle drawing
    COLORREF m_rgbInterior;
    COLORREF m_rgbBorder;
    //for space chart
    COLORREF m_rgbProtected;
    COLORREF m_rgbUnprotected;
    COLORREF m_rgbFree;
    //for bullet type 
    COLORREF m_rgbBullet;

    float m_protectInMB;
    float m_unprotectedInMB;
    float m_totalInMB;


    StaticDrawType m_drawType;
    vector<CString> m_initials;
    size_t m_totalPics;

    int m_fontPtSize;
    HBITMAP m_hBitmap;

    virtual void PreSubclassWindow();
public:
    virtual void DrawItem(LPDRAWITEMSTRUCT /*lpDrawItemStruct*/);
    void DrawScaledImage(LPDRAWITEMSTRUCT lpDrawItemStruct);
    void SetPointSize(int ptSize)
    {
        m_fontPtSize = ptSize;
    }

    void SetDrawType(StaticDrawType drawType)
    {
        m_drawType = drawType;
    }

    void SetDrawType(StaticDrawType drawType, COLORREF rgbInterior, COLORREF rgbBorder)
    {
        m_drawType = drawType;
        m_rgbInterior = rgbInterior;
        m_rgbBorder =  rgbBorder;
    }

    void SetBulletColor(COLORREF rgbBullet)
    {
        m_drawType = NX_BULLET_TEXT;
        m_rgbBullet = rgbBullet;
    }

    void SetScaledImage(HBITMAP hBitmap)
    {
        m_drawType = NX_SCALE_IMAGE;
        m_hBitmap = hBitmap;
        InvalidateRect(NULL);
    }


    void SetChartColorsAndInfo(COLORREF rgbProtected, COLORREF rgbUnprotected, COLORREF rgbFree, COLORREF rgbBorder, float totalInMB,float protectInMB,
                        float unprotectedInMB)
    {
        m_drawType = NX_STORAGE_CHART;
        m_rgbProtected=     rgbProtected;
        m_rgbUnprotected=   rgbUnprotected;
        m_rgbBorder = rgbBorder;
        m_rgbFree=          rgbFree;
        m_protectInMB=      protectInMB;
        m_unprotectedInMB=  unprotectedInMB;
        m_totalInMB = totalInMB;
    }

    vector<CString>& GetInitials()
    {
        return m_initials;
    }

    void SetTotalPics(size_t totalPics)
    {
        m_totalPics = totalPics;
    }

    void SetFirstPartColor(COLORREF rgbFirstPart)
    {
        m_rgbFirstPart = rgbFirstPart;
    }

    void SetSecondPartColor(COLORREF rgbSecondPart)
    {
        m_rgbSecondPart = rgbSecondPart;
    }

};


