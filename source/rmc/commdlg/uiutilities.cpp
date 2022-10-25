

#include <Windows.h>
#include <Windowsx.h>
#include <Commctrl.h>
#include <assert.h>

#include <string>
#include <nudf\resutil.hpp>

#include "commonui.hpp"
#include "uiutilities.hpp"

using namespace uiUtilities;



int uiUtilities::RectHeight(RECT& Rect)
{
    return Rect.bottom - Rect.top;
}

int uiUtilities::RectWidth(RECT& Rect)
{
    return Rect.right - Rect.left;
}


int uiUtilities::ScreenToScroll(RECT& Rect, int nScrollPos)
{
    OffsetRect(&Rect, 0, nScrollPos);
    return 0;
}


int uiUtilities::ScrollToScreen(RECT& Rect, int nScrollPos)
{
    OffsetRect(&Rect, 0, -nScrollPos);
    return 0;
}

int uiUtilities::ScreenToScroll(int nScreenY, int nScrollPos)
{
   
    return nScreenY + nScrollPos;
}

int uiUtilities::ScrollToScreen(int nScreenY, int nScrollPos)
{
    return nScreenY - nScrollPos;;
}



BOOL uiUtilities::CenterWindow(HWND hParentWhd, HWND hChildWnd)
{
    HWND hbackWnd = NULL;
    if (hParentWhd == NULL)
    {
        hbackWnd = GetDesktopWindow();
    }
    else
    {
        hbackWnd = hParentWhd;
    }

    RECT rcParent, rcChild;
    GetClientRect(hbackWnd, &rcParent);
    GetWindowRect(hChildWnd, &rcChild);
    int x, y;
    int w = RectWidth(rcChild);
    int h = RectHeight(rcChild);

    x = rcParent.left + (RectWidth(rcParent) - w) / 2;
    y = rcParent.top + (RectHeight(rcParent) - h) / 2;

    return MoveWindow(hChildWnd, x, y, w, h, TRUE);
}

BOOL uiUtilities::CenterWindowVertically(HWND hParentWhd, HWND hChildWnd)
{
    HWND hbackWnd = NULL;
    if (hParentWhd == NULL)
    {
        hbackWnd = GetDesktopWindow();
    }
    else
    {
        hbackWnd = hParentWhd;
    }

    RECT rcParent, rcChild;
    GetClientRect(hbackWnd, &rcParent);
    GetWindowRect(hChildWnd, &rcChild);
    int x, y;
    int w = RectWidth(rcChild);
    int h = RectHeight(rcChild);

    x = rcChild.left;
    y = rcParent.top + (RectHeight(rcParent) - h) / 2;

    return MoveWindow(hChildWnd, x, y, w, h, TRUE);
}


BOOL uiUtilities::GetWindowRectRelative(HWND hParentWhd, HWND hChildWnd, RECT *pRect)
{
    GetWindowRect(hChildWnd, pRect);
    MapWindowPoints(GetDesktopWindow(), hParentWhd, (LPPOINT)pRect, 2);
    return true;
}




bool uiUtilities::isCharacter(const wchar_t Character)
{
    return ((Character >= 'a' && Character <= 'z') || (Character >= 'A' && Character <= 'Z'));
}

bool uiUtilities::isNumber(const wchar_t Character)
{
    return (Character >= '0' && Character <= '9');
}

bool uiUtilities::isValidEmailAddress(const wchar_t * email)
{
    if (!email)
        return 0;
	int AtCount = 0;
    int AtOffset = -1;
    int DotOffset = -1;
    auto Length = lstrlen(email);
	if (!Length)
	{
		return false;
	}

    for (int i = 0; i < Length; i++)
    {
		if (email[i] == L' ')
			return false;
		if (email[i] == L';')
			return false;

		if (email[i] == L'@')
		{
			AtOffset = (int)i;
			AtCount++;
		}
        else if (email[i] == L'.')
            DotOffset = (int)i;
    }
	if (AtCount != 1)
		return false;
	if (AtOffset == 0)
		return false;
    if (AtOffset == -1 || DotOffset == -1)
        return false;
    if (AtOffset > DotOffset)
        return false;
    return !(DotOffset >= ((int)Length - 1));

}

std::wstring uiUtilities::LoadStringResource(int nID, const wchar_t *szDefaultString)
{
    if (!_hResource)
    {
        return szDefaultString;
    }
    std::wstring wsText = NX::RES::LoadMessage(_hResource, nID, Max_String_Len, _dwLangId, szDefaultString);
    return wsText;
}

std::wstring uiUtilities::LoadStringResource(int nID, int resID)
{
	wchar_t szDefaultString[Max_String_Len];
	LoadString(_hInstance, resID, szDefaultString, Max_String_Len);

	if (!_hResource)
	{
		return szDefaultString;
	}

	std::wstring wsText = NX::RES::LoadMessage(_hResource, nID, Max_String_Len, _dwLangId, szDefaultString);
	return wsText;
}

std::wstring uiUtilities::LoadLocalStringResource(int resID)
{
    wchar_t szDefaultString[Max_String_Len];
    LoadString(_hInstance, resID, szDefaultString, Max_String_Len);

    return szDefaultString;
}




