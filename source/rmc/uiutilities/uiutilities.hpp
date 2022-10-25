#pragma once

#include <Windows.h>
#include <Windowsx.h>
#include <Commctrl.h>
#include <assert.h>

#include <string>
#include <stdio.h>

#ifdef _DEBUG
#ifndef ASSERT
#define ASSERT(f) assert(f)
#define VERIFY(f) ASSERT(f)
#endif
#else
#define ASSERT(f)
#define VERIFY(f)          f
#endif

namespace uiUtilities
{


	void DoEvents(HWND hWnd);
    int RectHeight(RECT &Rect);
    int RectWidth(RECT &Rect);

	template< typename... Args >
	std::wstring string_sprintf(const wchar_t* format, Args... args) {
		//int length = _snwprintf_s(nullptr, 0, 0, format, args...);
		int length = lstrlen(format) + 256;
		assert(length >= 0);

		wchar_t* buf = new wchar_t[length + 1];
		delete[] buf;
		buf = new wchar_t[length + 1];
		_snwprintf_s(buf, length, length, format, args...);

		std::wstring str = buf;
		delete[] buf;
		return std::move(str);
	}

    int ScreenToScroll(RECT & Rect, int nScrollPos);

    int ScrollToScreen(RECT & Rect, int nScrollPos);

    int ScreenToScroll(int nScreenY, int nScrollPos);

    int ScrollToScreen(int nScreenY, int nScrollPos);

    BOOL CenterWindow(HWND hParentWhd, HWND hChildWnd);

    BOOL CenterWindowVertically(HWND hParentWhd, HWND hChildWnd);

    BOOL GetWindowRectRelative(HWND hParentWhd, HWND hChildWnd, RECT * pRect);





    bool isCharacter(const wchar_t Character);

    bool isNumber(const wchar_t Character);

    bool isValidEmailAddress(const wchar_t * email);

	const int Max_String_Len = 1024;

    //std::wstring LoadStringResource(int nID, const wchar_t * szDefaultString);

	//std::wstring LoadStringResource(int nID, int resID);

	//std::wstring LoadLocalStringResource(int resID);

	inline void MoveWindow(HWND hWnd, LPRECT lpRect, BOOL bRepaint = TRUE)
	{
		::MoveWindow(hWnd, lpRect->left, lpRect->top, lpRect->right - lpRect->left,
			lpRect->bottom - lpRect->top, bRepaint);
	}



	//start of utility classes
#pragma region 
	/////////////////////////////////////////////////////////////////////////////
	// Classes declared in this file

	//class CSize;
	//class CPoint;
	//class CRect;

	/////////////////////////////////////////////////////////////////////////////
	// CSize - An extent, similar to Windows SIZE structure.

	class CSize :
		public tagSIZE
	{
	public:

		// Constructors
		// construct an uninitialized size
		//CSize() throw();
		//// create from two integers
		//CSize(
		//	_In_ int initCX,
		//	_In_ int initCY) throw();
		//// create from another size
		//CSize(_In_ SIZE initSize) throw();
		//// create from a point
		//CSize(_In_ POINT initPt) throw();
		//// create from a DWORD: cx = LOWORD(dw) cy = HIWORD(dw)
		//CSize(_In_ DWORD dwSize) throw();

		// Operations
		//BOOL operator==(_In_ SIZE size) const throw();
		//BOOL operator!=(_In_ SIZE size) const throw();
		//void operator+=(_In_ SIZE size) throw();
		//void operator-=(_In_ SIZE size) throw();
		//void SetSize(_In_ int CX, _In_ int CY) throw();

		//// Operators returning CSize values
		//CSize operator+(_In_ SIZE size) const throw();
		//CSize operator-(_In_ SIZE size) const throw();
		//CSize operator-() const throw();

		// Operators returning CPoint values
		//CPoint operator+(_In_ POINT point) const throw();
		//CPoint operator-(_In_ POINT point) const throw();

		//// Operators returning CRect values
		//CRect operator+(_In_ const RECT* lpRect) const throw();
		//CRect operator-(_In_ const RECT* lpRect) const throw();
		inline CSize::CSize() throw()
		{
			cx = 0;
			cy = 0;
		}

		inline CSize::CSize(
			_In_ int initCX,
			_In_ int initCY) throw()
		{
			cx = initCX;
			cy = initCY;
		}

		inline CSize::CSize(_In_ SIZE initSize) throw()
		{
			*(SIZE*)this = initSize;
		}

		inline CSize::CSize(_In_ POINT initPt) throw()
		{
			*(POINT*)this = initPt;
		}

		inline CSize::CSize(_In_ DWORD dwSize) throw()
		{
			cx = (short)LOWORD(dwSize);
			cy = (short)HIWORD(dwSize);
		}

		inline BOOL CSize::operator==(_In_ SIZE size) const throw()
		{
			return (cx == size.cx && cy == size.cy);
		}

		inline BOOL CSize::operator!=(_In_ SIZE size) const throw()
		{
			return (cx != size.cx || cy != size.cy);
		}

		inline void CSize::operator+=(_In_ SIZE size) throw()
		{
			cx += size.cx;
			cy += size.cy;
		}

		inline void CSize::operator-=(_In_ SIZE size) throw()
		{
			cx -= size.cx;
			cy -= size.cy;
		}

		inline void CSize::SetSize(
			_In_ int CX,
			_In_ int CY) throw()
		{
			cx = CX;
			cy = CY;
		}

		inline CSize CSize::operator+(_In_ SIZE size) const throw()
		{
			return CSize(cx + size.cx, cy + size.cy);
		}

		inline CSize CSize::operator-(_In_ SIZE size) const throw()
		{
			return CSize(cx - size.cx, cy - size.cy);
		}

		inline CSize CSize::operator-() const throw()
		{
			return CSize(-cx, -cy);
		}

	};

	/////////////////////////////////////////////////////////////////////////////
	// CPoint - A 2-D point, similar to Windows POINT structure.

	class CPoint :
		public tagPOINT
	{
	public:
		// Constructors

		// create an uninitialized point
		//CPoint() thrw();
		//// create from two integers
		//CPoint(
		//	_In_ int initX,
		//	_In_ int initY) throw();
		//// create from another point
		//CPoint(_In_ POINT initPt) throw();
		//// create from a size
		//CPoint(_In_ SIZE initSize) throw();
		//// create from an LPARAM: x = LOWORD(dw) y = HIWORD(dw)
		//CPoint(_In_ LPARAM dwPoint) throw();


		//// Operations

		//// translate the point
		//void Offset(
		//	_In_ int xOffset,
		//	_In_ int yOffset) throw();
		//void Offset(_In_ POINT point) throw();
		//void Offset(_In_ SIZE size) throw();
		//void SetPoint(
		//	_In_ int X,
		//	_In_ int Y) throw();

		//BOOL operator==(_In_ POINT point) const throw();
		//BOOL operator!=(_In_ POINT point) const throw();
		//void operator+=(_In_ SIZE size) throw();
		//void operator-=(_In_ SIZE size) throw();
		//void operator+=(_In_ POINT point) throw();
		//void operator-=(_In_ POINT point) throw();

		//// Operators returning CPoint values
		//CPoint operator+(_In_ SIZE size) const throw();
		//CPoint operator-(_In_ SIZE size) const throw();
		//CPoint operator-() const throw();
		//CPoint operator+(_In_ POINT point) const throw();

		//// Operators returning CSize values
		//CSize operator-(_In_ POINT point) const throw();

		//// Operators returning CRect values
		//CRect operator+(_In_ const RECT* lpRect) const throw();
		//CRect operator-(_In_ const RECT* lpRect) const throw();
		// CPoint
		inline CPoint::CPoint() throw()
		{
			x = 0;
			y = 0;
		}

		inline CPoint::CPoint(
			_In_ int initX,
			_In_ int initY) throw()
		{
			x = initX;
			y = initY;
		}

		inline CPoint::CPoint(_In_ POINT initPt) throw()
		{
			*(POINT*)this = initPt;
		}

		inline CPoint::CPoint(_In_ SIZE initSize) throw()
		{
			*(SIZE*)this = initSize;
		}

		inline CPoint::CPoint(_In_ LPARAM dwPoint) throw()
		{
			x = (short)LOWORD(dwPoint);
			y = (short)HIWORD(dwPoint);
		}

		inline void CPoint::Offset(
			_In_ int xOffset,
			_In_ int yOffset) throw()
		{
			x += xOffset;
			y += yOffset;
		}

		inline void CPoint::Offset(_In_ POINT point) throw()
		{
			x += point.x;
			y += point.y;
		}

		inline void CPoint::Offset(_In_ SIZE size) throw()
		{
			x += size.cx;
			y += size.cy;
		}

		inline void CPoint::SetPoint(
			_In_ int X,
			_In_ int Y) throw()
		{
			x = X;
			y = Y;
		}

		inline BOOL CPoint::operator==(_In_ POINT point) const throw()
		{
			return (x == point.x && y == point.y);
		}

		inline BOOL CPoint::operator!=(_In_ POINT point) const throw()
		{
			return (x != point.x || y != point.y);
		}

		inline void CPoint::operator+=(_In_ SIZE size) throw()
		{
			x += size.cx;
			y += size.cy;
		}

		inline void CPoint::operator-=(_In_ SIZE size) throw()
		{
			x -= size.cx;
			y -= size.cy;
		}

		inline void CPoint::operator+=(_In_ POINT point) throw()
		{
			x += point.x;
			y += point.y;
		}

		inline void CPoint::operator-=(_In_ POINT point) throw()
		{
			x -= point.x;
			y -= point.y;
		}

		inline CPoint CPoint::operator+(_In_ SIZE size) const throw()
		{
			return CPoint(x + size.cx, y + size.cy);
		}

		inline CPoint CPoint::operator-(_In_ SIZE size) const throw()
		{
			return CPoint(x - size.cx, y - size.cy);
		}

		inline CPoint CPoint::operator-() const throw()
		{
			return CPoint(-x, -y);
		}

	};

	/////////////////////////////////////////////////////////////////////////////
	// CRect - A 2-D rectangle, similar to Windows RECT structure.

	class CRect :
		public tagRECT
	{
		// Constructors
	public:
		// uninitialized rectangle
		CRect() throw();
		// from left, top, right, and bottom
		CRect(
			_In_ int l,
			_In_ int t,
			_In_ int r,
			_In_ int b) throw();
		// copy constructor
		CRect(_In_ const RECT& srcRect) throw();

		// from a pointer to another rect
		CRect(_In_ LPCRECT lpSrcRect) throw();
		// from a point and size
		CRect(
			_In_ POINT point,
			_In_ SIZE size) throw();
		// from two points
		CRect(
			_In_ POINT topLeft,
			_In_ POINT bottomRight) throw();

		// Attributes (in addition to RECT members)

		// retrieves the width
		int Width() const throw();
		// returns the height
		int Height() const throw();
		// returns the size
		CSize Size() const throw();
		// reference to the top-left point
		CPoint& TopLeft() throw();
		// reference to the bottom-right point
		CPoint& BottomRight() throw();
		// const reference to the top-left point
		const CPoint& TopLeft() const throw();
		// const reference to the bottom-right point
		const CPoint& BottomRight() const throw();
		// the geometric center point of the rectangle
		CPoint CenterPoint() const throw();
		// swap the left and right
		void SwapLeftRight() throw();
		static void WINAPI SwapLeftRight(_Inout_ LPRECT lpRect) throw();

		// convert between CRect and LPRECT/LPCRECT (no need for &)
		operator LPRECT() throw();
		operator LPCRECT() const throw();

		// returns TRUE if rectangle has no area
		BOOL IsRectEmpty() const throw();
		// returns TRUE if rectangle is at (0,0) and has no area
		BOOL IsRectNull() const throw();
		// returns TRUE if point is within rectangle
		BOOL PtInRect(_In_ POINT point) const throw();

		// Operations

		// set rectangle from left, top, right, and bottom
		void SetRect(
			_In_ int x1,
			_In_ int y1,
			_In_ int x2,
			_In_ int y2) throw();
		void SetRect(
			_In_ POINT topLeft,
			_In_ POINT bottomRight) throw();
		// empty the rectangle
		void SetRectEmpty() throw();
		// copy from another rectangle
		void CopyRect(_In_ LPCRECT lpSrcRect) throw();
		// TRUE if exactly the same as another rectangle
		BOOL EqualRect(_In_ LPCRECT lpRect) const throw();

		// Inflate rectangle's width and height by
		// x units to the left and right ends of the rectangle
		// and y units to the top and bottom.
		void InflateRect(
			_In_ int x,
			_In_ int y) throw();
		// Inflate rectangle's width and height by
		// size.cx units to the left and right ends of the rectangle
		// and size.cy units to the top and bottom.
		void InflateRect(_In_ SIZE size) throw();
		// Inflate rectangle's width and height by moving individual sides.
		// Left side is moved to the left, right side is moved to the right,
		// top is moved up and bottom is moved down.
		void InflateRect(_In_ LPCRECT lpRect) throw();
		void InflateRect(
			_In_ int l,
			_In_ int t,
			_In_ int r,
			_In_ int b) throw();

		// deflate the rectangle's width and height without
		// moving its top or left
		void DeflateRect(
			_In_ int x,
			_In_ int y) throw();
		void DeflateRect(_In_ SIZE size) throw();
		void DeflateRect(_In_ LPCRECT lpRect) throw();
		void DeflateRect(
			_In_ int l,
			_In_ int t,
			_In_ int r,
			_In_ int b) throw();

		// translate the rectangle by moving its top and left
		void OffsetRect(
			_In_ int x,
			_In_ int y) throw();
		void OffsetRect(_In_ SIZE size) throw();
		void OffsetRect(_In_ POINT point) throw();
		void NormalizeRect() throw();

		// absolute position of rectangle
		void MoveToY(_In_ int y) throw();
		void MoveToX(_In_ int x) throw();
		void MoveToXY(
			_In_ int x,
			_In_ int y) throw();
		void MoveToXY(_In_ POINT point) throw();

		// set this rectangle to intersection of two others
		BOOL IntersectRect(
			_In_ LPCRECT lpRect1,
			_In_ LPCRECT lpRect2) throw();

		// set this rectangle to bounding union of two others
		BOOL UnionRect(
			_In_ LPCRECT lpRect1,
			_In_ LPCRECT lpRect2) throw();

		// set this rectangle to minimum of two others
		BOOL SubtractRect(
			_In_ LPCRECT lpRectSrc1,
			_In_ LPCRECT lpRectSrc2) throw();

		// Additional Operations
		void operator=(_In_ const RECT& srcRect) throw();
		BOOL operator==(_In_ const RECT& rect) const throw();
		BOOL operator!=(_In_ const RECT& rect) const throw();
		void operator+=(_In_ POINT point) throw();
		void operator+=(_In_ SIZE size) throw();
		void operator+=(_In_ LPCRECT lpRect) throw();
		void operator-=(_In_ POINT point) throw();
		void operator-=(_In_ SIZE size) throw();
		void operator-=(_In_ LPCRECT lpRect) throw();
		void operator&=(_In_ const RECT& rect) throw();
		void operator|=(_In_ const RECT& rect) throw();

		// Operators returning CRect values
		CRect operator+(_In_ POINT point) const throw();
		CRect operator-(_In_ POINT point) const throw();
		CRect operator+(_In_ LPCRECT lpRect) const throw();
		CRect operator+(_In_ SIZE size) const throw();
		CRect operator-(_In_ SIZE size) const throw();
		CRect operator-(_In_ LPCRECT lpRect) const throw();
		CRect operator&(_In_ const RECT& rect2) const throw();
		CRect operator|(_In_ const RECT& rect2) const throw();
		CRect MulDiv(
			_In_ int nMultiplier,
			_In_ int nDivisor) const throw();
	};



	// CRect
	inline CRect::CRect() throw()
	{
		left = 0;
		top = 0;
		right = 0;
		bottom = 0;
	}

	inline CRect::CRect(
		_In_ int l,
		_In_ int t,
		_In_ int r,
		_In_ int b) throw()
	{
		left = l;
		top = t;
		right = r;
		bottom = b;
	}

	inline CRect::CRect(_In_ const RECT& srcRect) throw()
	{
		::CopyRect(this, &srcRect);
	}

	inline CRect::CRect(_In_ LPCRECT lpSrcRect) throw()
	{
		::CopyRect(this, lpSrcRect);
	}

	inline CRect::CRect(
		_In_ POINT point,
		_In_ SIZE size) throw()
	{
		right = (left = point.x) + size.cx;
		bottom = (top = point.y) + size.cy;
	}

	inline CRect::CRect(
		_In_ POINT topLeft,
		_In_ POINT bottomRight) throw()
	{
		left = topLeft.x;
		top = topLeft.y;
		right = bottomRight.x;
		bottom = bottomRight.y;
	}

	inline int CRect::Width() const throw()
	{
		return right - left;
	}

	inline int CRect::Height() const throw()
	{
		return bottom - top;
	}

	inline CSize CRect::Size() const throw()
	{
		return CSize(right - left, bottom - top);
	}

	inline CPoint& CRect::TopLeft() throw()
	{
		return *((CPoint*)this);
	}

	inline CPoint& CRect::BottomRight() throw()
	{
		return *((CPoint*)this + 1);
	}

	inline const CPoint& CRect::TopLeft() const throw()
	{
		return *((CPoint*)this);
	}

	inline const CPoint& CRect::BottomRight() const throw()
	{
		return *((CPoint*)this + 1);
	}

	inline CPoint CRect::CenterPoint() const throw()
	{
		return CPoint((left + right) / 2, (top + bottom) / 2);
	}

	inline void CRect::SwapLeftRight() throw()
	{
		SwapLeftRight(LPRECT(this));
	}

	inline void WINAPI CRect::SwapLeftRight(_Inout_ LPRECT lpRect) throw()
	{
		LONG temp = lpRect->left;
		lpRect->left = lpRect->right;
		lpRect->right = temp;
	}

	inline CRect::operator LPRECT() throw()
	{
		return this;
	}

	inline CRect::operator LPCRECT() const throw()
	{
		return this;
	}

	inline BOOL CRect::IsRectEmpty() const throw()
	{
		return ::IsRectEmpty(this);
	}

	inline BOOL CRect::IsRectNull() const throw()
	{
		return (left == 0 && right == 0 && top == 0 && bottom == 0);
	}

	inline BOOL CRect::PtInRect(_In_ POINT point) const throw()
	{
		return ::PtInRect(this, point);
	}

	inline void CRect::SetRect(
		_In_ int x1,
		_In_ int y1,
		_In_ int x2,
		_In_ int y2) throw()
	{
		::SetRect(this, x1, y1, x2, y2);
	}

	inline void CRect::SetRect(
		_In_ POINT topLeft,
		_In_ POINT bottomRight) throw()
	{
		::SetRect(this, topLeft.x, topLeft.y, bottomRight.x, bottomRight.y);
	}

	inline void CRect::SetRectEmpty() throw()
	{
		::SetRectEmpty(this);
	}

	inline void CRect::CopyRect(_In_ LPCRECT lpSrcRect) throw()
	{
		::CopyRect(this, lpSrcRect);
	}

	inline BOOL CRect::EqualRect(_In_ LPCRECT lpRect) const throw()
	{
		return ::EqualRect(this, lpRect);
	}

	inline void CRect::InflateRect(
		_In_ int x,
		_In_ int y) throw()
	{
		::InflateRect(this, x, y);
	}

	inline void CRect::InflateRect(_In_ SIZE size) throw()
	{
		::InflateRect(this, size.cx, size.cy);
	}

	inline void CRect::DeflateRect(
		_In_ int x,
		_In_ int y) throw()
	{
		::InflateRect(this, -x, -y);
	}

	inline void CRect::DeflateRect(_In_ SIZE size) throw()
	{
		::InflateRect(this, -size.cx, -size.cy);
	}

	inline void CRect::OffsetRect(
		_In_ int x,
		_In_ int y) throw()
	{
		::OffsetRect(this, x, y);
	}

	inline void CRect::OffsetRect(_In_ POINT point) throw()
	{
		::OffsetRect(this, point.x, point.y);
	}

	inline void CRect::OffsetRect(_In_ SIZE size) throw()
	{
		::OffsetRect(this, size.cx, size.cy);
	}

	inline void CRect::MoveToY(_In_ int y) throw()
	{
		bottom = Height() + y;
		top = y;
	}

	inline void CRect::MoveToX(_In_ int x) throw()
	{
		right = Width() + x;
		left = x;
	}

	inline void CRect::MoveToXY(
		_In_ int x,
		_In_ int y) throw()
	{
		MoveToX(x);
		MoveToY(y);
	}

	inline void CRect::MoveToXY(_In_ POINT pt) throw()
	{
		MoveToX(pt.x);
		MoveToY(pt.y);
	}

	inline BOOL CRect::IntersectRect(
		_In_ LPCRECT lpRect1,
		_In_ LPCRECT lpRect2) throw()
	{
		return ::IntersectRect(this, lpRect1, lpRect2);
	}

	inline BOOL CRect::UnionRect(
		_In_ LPCRECT lpRect1,
		_In_ LPCRECT lpRect2) throw()
	{
		return ::UnionRect(this, lpRect1, lpRect2);
	}

	inline void CRect::operator=(_In_ const RECT& srcRect) throw()
	{
		::CopyRect(this, &srcRect);
	}

	inline BOOL CRect::operator==(_In_ const RECT& rect) const throw()
	{
		return ::EqualRect(this, &rect);
	}

	inline BOOL CRect::operator!=(_In_ const RECT& rect) const throw()
	{
		return !::EqualRect(this, &rect);
	}

	inline void CRect::operator+=(_In_ POINT point) throw()
	{
		::OffsetRect(this, point.x, point.y);
	}

	inline void CRect::operator+=(_In_ SIZE size) throw()
	{
		::OffsetRect(this, size.cx, size.cy);
	}

	inline void CRect::operator+=(_In_ LPCRECT lpRect) throw()
	{
		InflateRect(lpRect);
	}

	inline void CRect::operator-=(_In_ POINT point) throw()
	{
		::OffsetRect(this, -point.x, -point.y);
	}

	inline void CRect::operator-=(_In_ SIZE size) throw()
	{
		::OffsetRect(this, -size.cx, -size.cy);
	}

	inline void CRect::operator-=(_In_ LPCRECT lpRect) throw()
	{
		DeflateRect(lpRect);
	}

	inline void CRect::operator&=(_In_ const RECT& rect) throw()
	{
		::IntersectRect(this, this, &rect);
	}

	inline void CRect::operator|=(_In_ const RECT& rect) throw()
	{
		::UnionRect(this, this, &rect);
	}

	inline CRect CRect::operator+(_In_ POINT pt) const throw()
	{
		CRect rect(*this);
		::OffsetRect(&rect, pt.x, pt.y);
		return rect;
	}

	inline CRect CRect::operator-(_In_ POINT pt) const throw()
	{
		CRect rect(*this);
		::OffsetRect(&rect, -pt.x, -pt.y);
		return rect;
	}

	inline CRect CRect::operator+(_In_ SIZE size) const throw()
	{
		CRect rect(*this);
		::OffsetRect(&rect, size.cx, size.cy);
		return rect;
	}

	inline CRect CRect::operator-(_In_ SIZE size) const throw()
	{
		CRect rect(*this);
		::OffsetRect(&rect, -size.cx, -size.cy);
		return rect;
	}

	inline CRect CRect::operator+(_In_ LPCRECT lpRect) const throw()
	{
		CRect rect(this);
		rect.InflateRect(lpRect);
		return rect;
	}

	inline CRect CRect::operator-(_In_ LPCRECT lpRect) const throw()
	{
		CRect rect(this);
		rect.DeflateRect(lpRect);
		return rect;
	}

	inline CRect CRect::operator&(_In_ const RECT& rect2) const throw()
	{
		CRect rect;
		::IntersectRect(&rect, this, &rect2);
		return rect;
	}

	inline CRect CRect::operator|(_In_ const RECT& rect2) const throw()
	{
		CRect rect;
		::UnionRect(&rect, this, &rect2);
		return rect;
	}

	inline BOOL CRect::SubtractRect(
		_In_ LPCRECT lpRectSrc1,
		_In_ LPCRECT lpRectSrc2) throw()
	{
		return ::SubtractRect(this, lpRectSrc1, lpRectSrc2);
	}

	inline void CRect::NormalizeRect() throw()
	{
		int nTemp;
		if (left > right)
		{
			nTemp = left;
			left = right;
			right = nTemp;
		}
		if (top > bottom)
		{
			nTemp = top;
			top = bottom;
			bottom = nTemp;
		}
	}

	inline void CRect::InflateRect(_In_ LPCRECT lpRect) throw()
	{
		left -= lpRect->left;
		top -= lpRect->top;
		right += lpRect->right;
		bottom += lpRect->bottom;
	}

	inline void CRect::InflateRect(
		_In_ int l,
		_In_ int t,
		_In_ int r,
		_In_ int b) throw()
	{
		left -= l;
		top -= t;
		right += r;
		bottom += b;
	}

	inline void CRect::DeflateRect(_In_ LPCRECT lpRect) throw()
	{
		left += lpRect->left;
		top += lpRect->top;
		right -= lpRect->right;
		bottom -= lpRect->bottom;
	}

	inline void CRect::DeflateRect(
		_In_ int l,
		_In_ int t,
		_In_ int r,
		_In_ int b) throw()
	{
		left += l;
		top += t;
		right -= r;
		bottom -= b;
	}

	inline CRect CRect::MulDiv(
		_In_ int nMultiplier,
		_In_ int nDivisor) const throw()
	{
		return CRect(
			::MulDiv(left, nMultiplier, nDivisor),
			::MulDiv(top, nMultiplier, nDivisor),
			::MulDiv(right, nMultiplier, nDivisor),
			::MulDiv(bottom, nMultiplier, nDivisor));
	}
#pragma endregion

#pragma region
	class CWnd
	{
	public:
		CWnd()
		{}

		CWnd(HWND hwnd)
		{
			m_hWnd = hwnd;
		}
		HWND GetSafeHwnd()
		{
			return m_hWnd;
		}
		inline virtual void EnableWindow(BOOL bEnable = TRUE)
		{
			if (m_hWnd != NULL)
			{
				::EnableWindow(m_hWnd, bEnable);
			}
		}
		inline void MoveWindow(LPCRECT lpRect, BOOL bRepaint = TRUE)
		{
			::MoveWindow(m_hWnd, lpRect->left, lpRect->top, lpRect->right - lpRect->left,
				lpRect->bottom - lpRect->top, bRepaint);
		}

		inline LONG_PTR GetStyle()
		{
			LONG_PTR lStyle;
			lStyle = ::GetWindowLongPtr(m_hWnd, GWL_STYLE);
			return lStyle;
		}
		inline void GetWindowRect(RECT *rc)
		{
			::GetWindowRect(m_hWnd, rc);
		}

		HWND m_hWnd;
	};
#pragma endregion


	//Definition for CListCtrl
#pragma region
	class CListCtrl : public CWnd
	{
		// DECLARE_DYNAMIC virtual is OK here - CWnd has DECLARE_DYNAMIC
		//DECLARE_DYNAMIC(CListCtrl)

		// Constructors
	public:
//		CListCtrl();
//
//		// Generic creator
//		//BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);
//
//		//// Generic creator allowing extended style bits
//		//BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
//		//	_In_ CWnd* pParentWnd, _In_ UINT nID);
//
//		// Attributes
//		// Retrieves the background color for the control.
//		COLORREF GetBkColor() const;
//
//		// Sets background color for the control.
//		BOOL SetBkColor(_In_ COLORREF cr);
//
//		// Retrieves the image list associated with the control.
//		CImageList* GetImageList(_In_ int nImageList) const;
//
//		// Sets the image list associated with this control.
//		CImageList* SetImageList(_In_ CImageList* pImageList, _In_ int nImageList);
//
//		// Retrieves the tool tip control associated with this control.
//		//CToolTipCtrl* GetToolTips() const;
//
//		//// Sets the tool tip control to be used by this control.
//		//CToolTipCtrl* SetToolTips(_In_ CToolTipCtrl* pWndTip);
//
//		// Retrieves the number of items in the control.
//		int GetItemCount() const;
//
//		// Retrieves a description of a particular item in the control.
//		BOOL GetItem(_Out_ LVITEM* pItem) const;
//
//		// Sets information to an existing item in the control.
//		BOOL SetItem(_In_ const LVITEM* pItem);
//		//BOOL SetItem(_In_ int nItem, _In_ int nSubItem, _In_ UINT nMask, _In_opt_z_ LPCTSTR lpszItem,
//		//	_In_ int nImage, _In_ UINT nState, _In_ UINT nStateMask, _In_ LPARAM lParam);
//		//BOOL SetItem(_In_ int nItem, _In_ int nSubItem, _In_ UINT nMask, _In_opt_z_ LPCTSTR lpszItem,
//		//	_In_ int nImage, _In_ UINT nState, _In_ UINT nStateMask, _In_ LPARAM lParam, _In_ int nIndent);
//
//		// Determines which item attributes are maintained by the application
//		// instead of the control itself.
//		UINT GetCallbackMask() const;
//
//		// Specifies which item attributes are maintained by the application
//		// instead of the control itself.
//		BOOL SetCallbackMask(_In_ UINT nMask);
//
//		// Get the next item after nItem matching flags in nFlags.
//		int GetNextItem(_In_ int nItem, _In_ int nFlags) const;
//
//		// Gets first item selected in the control and prepares for
//		// finding other selected items (if the control has the multiple
//		// selection style).
//		POSITION GetFirstSelectedItemPosition() const;
//
//		// Finds the next selected item, after a previous call
//		// to GetFirstSelectedItemPosition().
//		int GetNextSelectedItem(_Inout_ POSITION& pos) const;
//
//		// Retrieves the bounding rectangle for a particular item.
//		//BOOL GetItemRect(_In_ int nItem, _Out_ LPRECT lpRect, _In_ UINT nCode) const;
//
//		// Find the location of a particular item in the control,
//		// relative to the control's client area.
//		BOOL SetItemPosition(_In_ int nItem, _In_ POINT pt);
//		BOOL GetItemPosition(_In_ int nItem, _Out_ LPPOINT lpPoint) const;
//
//		// Determines the width of a string as displayed in report mode.
//		int GetStringWidth(_In_z_ LPCTSTR lpsz) const;
//
//		// Retrieves the edit control associated with the currently edited
//		// item in the control.
//		CEdit* GetEditControl() const;
//
//		// Retrieves information about a column in a report-mode control.
//		BOOL GetColumn(_In_ int nCol, _Out_ LVCOLUMN* pColumn) const;
//
//		// Sets information about a column in a report-mode control.
//		BOOL SetColumn(_In_ int nCol, _In_ const LVCOLUMN* pColumn);
//
//		// Retrieves the width of a column in a report-mode control.
//		int GetColumnWidth(_In_ int nCol) const;
//
//		// Sets the width of a column in a report-mode control.
//		BOOL SetColumnWidth(_In_ int nCol, _In_ int cx);
//
//		BOOL GetViewRect(_Out_ LPRECT lpRect) const;
//
//		// Retrieves foreground color of text labels in the control.
//		COLORREF GetTextColor() const;
//
//		// Sets foreground color of text labels in the control.
//		BOOL SetTextColor(_In_ COLORREF cr);
//
//		// Retrieves background color of text labels in the control.
//		COLORREF GetTextBkColor() const;
//
//		// Sets background color of text labels in the control.
//		BOOL SetTextBkColor(_In_ COLORREF cr);
//
//		// Retrieves the index of the topmost visible item in the control.
//		int GetTopIndex() const;
//
//		// Retrieves the number of items displayed at one time
//		// the control's client area.
//		int GetCountPerPage() const;
//
//		// Retrieves the current origin of the client area.
//		BOOL GetOrigin(_Out_ LPPOINT lpPoint) const;
//
//		// Sets the state of a particular item.
//		BOOL SetItemState(_In_ int nItem, _In_ LVITEM* pItem);
//		//BOOL SetItemState(_In_ int nItem, _In_ UINT nState, _In_ UINT nMask);
//
//		// Retrieves the state of a particular item.
//		UINT GetItemState(_In_ int nItem, _In_ UINT nMask) const;
//
//		// Retrieves the text associated with a particular item.
//		//CString GetItemText(_In_ int nItem, _In_ int nSubItem) const;
//		//int GetItemText(_In_ int nItem, _In_ int nSubItem, _Out_writes_to_(nLen, return +1) LPTSTR lpszText, _In_ int nLen) const;
//
//		//// Sets the text associated with a particular item.
		BOOL SetItemText(_In_ int nItem, _In_ int nSubItem, _In_z_ LPCTSTR lpszText);
//
//		// Sets the count of items in the control. The control will use
//		// this value to preallocate memory for its own storage; you may
//		// exceed the item count at any time, but accurate preallocation
//		// can help performance.
//		void SetItemCount(_In_ int nItems);
//
//		// Sets the data (lParam) associated with a particular item.
//		BOOL SetItemData(_In_ int nItem, _In_ DWORD_PTR dwData);
//
//		// Retrieves the data (lParam) associated with a particular item.
////		DWORD_PTR GetItemData(_In_ int nItem) const;
//
//		// Retrieves the number of selected items in the control.
//		UINT GetSelectedCount() const;
//
//		// Retrieves the spacing between items in the control.
//		BOOL GetItemSpacing(_In_ BOOL fSmall, _Out_ int* pnHorzSpacing, _Out_ int* pnVertSpacing) const;
//
//		//BOOL SetColumnOrderArray(_In_ int iCount, _In_ LPINT piArray);
//		//BOOL GetColumnOrderArray(_Out_ LPINT piArray, _In_ int iCount = -1) const;
//
//		//// Sets minimum spacing of items in the icon view.
//		//CSize SetIconSpacing(_In_ CSize size);
//		//CSize SetIconSpacing(_In_ int cx, _In_ int cy);
//
//		//// Retrieves a reference to the header control in
//		//// a report-mode control.
//		//CHeaderCtrl* GetHeaderCtrl() const;
//
//		// Retrieves the cursor used over hot items. (Only valid for
//		// controls with the LVS_EX_TRACKSELECT style.)
//		HCURSOR GetHotCursor() const;
//
//		// Sets cursor to be used over hot items. (Only used in controls
//		// with the LVS_EX_TRACKSELECT style.)
//		HCURSOR SetHotCursor(_In_ HCURSOR hc);
//
//		//BOOL GetSubItemRect(_In_ int iItem, _In_ int iSubItem, _In_ int nArea, _Out_ CRect& ref) const;
//
//		// Retrieves the item currently hot-selected, or -1 if none.
//		int GetHotItem() const;
//
//		// Sets the item to be currently hot-selected.
//		int SetHotItem(_In_ int iIndex);
//
//		// Retrieves the item with the selection mark, or -1 if none.
//		int GetSelectionMark() const;
//
//		// Sets the item with the selection mark.
//		int SetSelectionMark(_In_ int iIndex);
//
//		// Retrieves the control-specific extended style bits.
//		DWORD GetExtendedStyle() const;
//
//		// Sets the control-specific extended style bits.
		//DWORD SetExtendedStyle(_In_ DWORD dwNewStyle);
//
//		// Determines the visual feature of a subitem control under
//		// the specified point.
//		int SubItemHitTest(_In_ LPLVHITTESTINFO pInfo);
//
//		// Sets up virtual work areas within the control.
//		void SetWorkAreas(_In_ int nWorkAreas, _In_ LPRECT lpRect);
//
//		// Updates expected item count for a virtual control.
//		//BOOL SetItemCountEx(_In_ int iCount, _In_ DWORD dwFlags = LVSICF_NOINVALIDATEALL);
//
//		// Calculates the approximate minimum size required to
//		// display the passed number of items.
//		CSize ApproximateViewRect(_In_ CSize sz = CSize(-1, -1),
//			_In_ int iCount = -1) const;
//
//		// Retrieves information about the background image in the control.
//		BOOL GetBkImage(_Out_ LVBKIMAGE* plvbkImage) const;
//
//		// Retrieves the delay (in milliseconds) for the mouse to hover
//		// over an item before it is selected.
//		DWORD GetHoverTime() const;
//
//		// Retrieves the rectangles defining the workareas in the control.
//		void GetWorkAreas(_In_ int nWorkAreas, _Out_ LPRECT prc) const;
//
//		// Sets the image used in the background of the control.
//		//BOOL SetBkImage(_In_ HBITMAP hbm, _In_ BOOL fTile = TRUE,
//		//	_In_ int xOffsetPercent = 0, _In_ int yOffsetPercent = 0);
//		//BOOL SetBkImage(_In_z_ LPTSTR pszUrl, _In_ BOOL fTile = TRUE,
//		//	_In_ int xOffsetPercent = 0, _In_ int yOffsetPercent = 0);
//		BOOL SetBkImage(_In_ LVBKIMAGE* plvbkImage);
//
//		// Sets the delay (in milliseconds) for the mouse to hover
//		// over an item before it is selected.
//		DWORD SetHoverTime(_In_ DWORD dwHoverTime = (DWORD)-1);
//
//		// Returns the number of work areas in the control.
//		UINT GetNumberOfWorkAreas() const;
//
//		// Retrieves the checked state of a particular item. Only useful
//		// on controls with the LVS_EX_CHECKBOXES style.
//		//BOOL GetCheck(_In_ int nItem) const;
//
//		// Sets the checked state of a particular item. Only useful
//		// on controls with the LVS_EX_CHECKBOXES style.
//		//BOOL SetCheck(_In_ int nItem, _In_ BOOL fCheck = TRUE);
//
//		// Operations
//
//		// Adds an item to the control.
//		int InsertItem(_In_ const LVITEM* pItem);
//		int InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem);
//		int InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem, _In_ int nImage);
//
//		// Removes a single item from the control.
//		BOOL DeleteItem(_In_ int nItem);
//
//		// Removes all items from the control.
//		BOOL DeleteAllItems();
//
//		// Finds an item in the control matching the specified criteria.
//		int FindItem(_In_ LVFINDINFO* pFindInfo, _In_ int nStart = -1) const;
//
//		// Determines the visual feature of the control under
//		// the specified point.
//		int HitTest(_In_ LVHITTESTINFO* pHitTestInfo) const;
//		//int HitTest(_In_ CPoint pt, _In_opt_ UINT* pFlags = NULL) const;
//
//		// Causes the control to scroll its content so the specified item
//		// is completely (or at least partially, depending on the
//		// bPartialOK parameter) visible.
//		BOOL EnsureVisible(_In_ int nItem, _In_ BOOL bPartialOK);
//
//		// Forces the control to scroll its client area
//		// by the specified amount.
//		BOOL Scroll(_In_ CSize size);
//
//		// Forces the control to repaint a specific range of items.
//		BOOL RedrawItems(_In_ int nFirst, _In_ int nLast);
//
//		// Causes the control to rearrange items within its client area.
//		BOOL Arrange(_In_ UINT nCode);
//
//		// Causes the control to enter edit mode on the speficied item.
//		CEdit* EditLabel(_In_ int nItem);
//
//		// Inserts a column into a report-mode control.
//		int InsertColumn(_In_ int nCol, _In_ const LVCOLUMN* pColumn);
		int InsertColumn(_In_ int nCol, _In_z_ LPCTSTR lpszColumnHeading,
			_In_ int nFormat = LVCFMT_LEFT, _In_ int nWidth = -1, _In_ int nSubItem = -1);
//
//		// Deletes a column from a report-mode control.
//		BOOL DeleteColumn(_In_ int nCol);
//
//		// Creates a drag-time image from a particular item in the control.
//		//CImageList* CreateDragImage(_In_ int nItem, _In_ LPPOINT lpPoint);
//
//		// Forces the control to repaint a specific item.
//		BOOL Update(_In_ int nItem);
//
//		// Call to sort items using a custom comparison function.
//		BOOL SortItems(_In_ PFNLVCOMPARE pfnCompare, _In_ DWORD_PTR dwData);
//		BOOL SortItemsEx(_In_ PFNLVCOMPARE pfnCompare, _In_ DWORD_PTR dwData);
//
//		// Sets the selected column in a report-mode control.
//		AFX_ANSI_DEPRECATED void SetSelectedColumn(_In_ int iCol);
//
//		AFX_ANSI_DEPRECATED DWORD SetView(_In_ int iView);
//		AFX_ANSI_DEPRECATED DWORD GetView() const;
//
//		// Adds a group to the control.
//		AFX_ANSI_DEPRECATED int InsertGroup(_In_ int index, _In_ PLVGROUP pgrp);
//
//		// Sets information about the specified group (by ID) in the control.
//		AFX_ANSI_DEPRECATED int SetGroupInfo(_In_ int iGroupId, _In_ PLVGROUP pGroup);
//
//		// Retrieves information for the specified group in the control.
//		AFX_ANSI_DEPRECATED int GetGroupInfo(_In_ int iGroupId, _Out_ PLVGROUP pgrp) const;
//
//		// Removes a group from the control.
//		AFX_ANSI_DEPRECATED int RemoveGroup(_In_ int iGroupId);
//
//		// Moves a group in the control.
//		AFX_ANSI_DEPRECATED void MoveGroup(_In_ int iGroupId, _In_ int toIndex);
//
//		// Moves an item to a different group in the control.
//		AFX_ANSI_DEPRECATED void MoveItemToGroup(_In_ int idItemFrom, _In_ int idGroupTo);
//
//		// Sets group metric information to a group in the control.
//		AFX_ANSI_DEPRECATED void SetGroupMetrics(_In_ PLVGROUPMETRICS pGroupMetrics);
//
//		// Retrieves group metric information for a group in the control.
//		AFX_ANSI_DEPRECATED void GetGroupMetrics(_Out_ PLVGROUPMETRICS pGroupMetrics) const;
//
//		// Enables group view in the control.
//		AFX_ANSI_DEPRECATED int EnableGroupView(_In_ BOOL fEnable);
//
//		// Uses an application-defined comparison function to sort groups by ID within the list-view control.
//		AFX_ANSI_DEPRECATED BOOL SortGroups(_In_ PFNLVGROUPCOMPARE _pfnGroupCompare, _In_ LPVOID _plv);
//
//		// Inserts a group into an ordered list of groups in the control.
//		AFX_ANSI_DEPRECATED void InsertGroupSorted(_In_ PLVINSERTGROUPSORTED pStructInsert);
//
//		// Removes all groups from the control.
//		AFX_ANSI_DEPRECATED void RemoveAllGroups();
//
//		// Determines if the control has a group with the specified ID.
//		AFX_ANSI_DEPRECATED BOOL HasGroup(_In_ int iGroupId) const;
//
//		// Sets information that the list-view control uses in tile view.
//		AFX_ANSI_DEPRECATED BOOL SetTileViewInfo(_In_ PLVTILEVIEWINFO ptvi);
//
//		// Retrieves information about the list-view control in tile view.
//		AFX_ANSI_DEPRECATED BOOL GetTileViewInfo(_Out_ PLVTILEVIEWINFO ptvi) const;
//
//		// Sets information for a tile of the list-view control.
//		AFX_ANSI_DEPRECATED BOOL SetTileInfo(_In_ PLVTILEINFO pti);
//
//		// Retrieves information about a tile in the list-view control.
//		AFX_ANSI_DEPRECATED BOOL GetTileInfo(_Out_ PLVTILEINFO pti) const;
//
//		// Sets the insertion point in the control to the defined position.
//		AFX_ANSI_DEPRECATED BOOL SetInsertMark(_In_ LPLVINSERTMARK lvim);
//
//		// Retrieves the position of the insertion point in the control.
//		AFX_ANSI_DEPRECATED BOOL GetInsertMark(_Out_ LPLVINSERTMARK lvim) const;
//
//		// Retrieves the insertion point closest to a specified point.
//		AFX_ANSI_DEPRECATED int InsertMarkHitTest(_In_ LPPOINT pPoint, _In_ LPLVINSERTMARK lvim) const;
//
//		// Retrieves the rectangle that bounds the insertion point in the control.
//		AFX_ANSI_DEPRECATED int GetInsertMarkRect(_Out_ LPRECT pRect) const;
//
//		// Sets the color of the insertion point in the control.
//		AFX_ANSI_DEPRECATED COLORREF SetInsertMarkColor(_In_ COLORREF color);
//
//		// Retrieves the color of the insertion point in the control.
//		AFX_ANSI_DEPRECATED COLORREF GetInsertMarkColor() const;
//
//		// Sets ToolTip text for the control.
//		AFX_ANSI_DEPRECATED BOOL SetInfoTip(_In_ PLVSETINFOTIP plvInfoTip);
//
//		// Retrieves the selected column in a report-mode control.
//		AFX_ANSI_DEPRECATED UINT GetSelectedColumn() const;
//
//		// Determines if group view is enabled for the control.
//		AFX_ANSI_DEPRECATED BOOL IsGroupViewEnabled() const;
//
//		// Retrieves the color of the border of the list-view control.
//		AFX_ANSI_DEPRECATED COLORREF GetOutlineColor() const;
//
//		// Sets the color of the border of the list-view control.
//		AFX_ANSI_DEPRECATED COLORREF SetOutlineColor(_In_ COLORREF color);
//
//		// Cancels an item text editing operation in the control.
//		AFX_ANSI_DEPRECATED void CancelEditLabel();
//
//#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
//		// REVIEW: Retrieves the string displayed when the list-view is empty.
//		CString GetEmptyText() const;
//
//		// REVIEW: Retrieves the rectangle for the specified item in the list-view (by group/item index).
//		BOOL GetItemIndexRect(_In_ PLVITEMINDEX pItemIndex, _In_ int iColumn, _In_ int rectType, _Out_ LPRECT pRect) const;
//
//		// REVIEW: Sets state for the specified item in the list-view (by group/item index).
//		HRESULT SetItemIndexState(_In_ PLVITEMINDEX pItemIndex, _In_ DWORD dwState, _In_ DWORD dwMask);
//
//		// REVIEW: Retrieves the next item in the list-view (by group/item index) based on the flags.
//		BOOL GetNextItemIndex(_In_ PLVITEMINDEX pItemIndex, _In_ int nFlags) const;
//
//		// Retrieves the number of groups in the control.
//		int GetGroupCount() const;
//
//		// REVIEW: Retrieves information about the specified group (by index) in the control.
//		BOOL GetGroupInfoByIndex(_In_ int iIndex, _Out_ PLVGROUP pGroup) const;
//
//		// Sets state information to the specified group in the control.
//		void SetGroupState(_In_ int iGroupId, _In_ DWORD dwMask, _In_ DWORD dwState);
//
//		// Retrieves state information for the specified group in the control.
//		UINT GetGroupState(_In_ int iGroupId, _In_ DWORD dwMask) const;
//
//		// REVIEW: Gets the group that has the focus in the control.
//		int GetFocusedGroup() const;
//
//		// REVIEW: Gets the rectangle for the specified group in the control.
//		BOOL GetGroupRect(_In_ int iGroupId, _Out_ LPRECT lpRect, _In_ int iType = LVGGR_GROUP) const;
//
//		// Maps the index of an item in the control to a unique ID.
//		UINT MapIndexToID(_In_ UINT index) const;
//
//		// Maps the unique ID of an item in the control to an index.
//		UINT MapIDToIndex(_In_ UINT id) const;
//
//		// Indicates if an item in the list-view control is visible.
//		BOOL IsItemVisible(_In_ int index) const;
//
//#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

		// Overridables
		// Override to perform owner draw. Control must
		// have the owner draw style.
		//virtual void DrawItem(_In_ LPDRAWITEMSTRUCT lpDrawItemStruct);

		// Implementation
	public:
		int InsertItem(_In_ UINT nMask, _In_ int nItem, _In_z_ LPCTSTR lpszItem, _In_ UINT nState,
			_In_ UINT nStateMask, _In_ int nImage, _In_ LPARAM lParam);
		//// virtual OK here - ~CWnd is virtual
		//virtual ~CListCtrl();
	//protected:
	//	void RemoveImageList(_In_ int nImageList);
	//	// virtual OK here - CWnd makes this virtual
	//	virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
	//protected:
	//	afx_msg void OnNcDestroy();

	//	DECLARE_MESSAGE_MAP()
		inline CListCtrl::CListCtrl()
		{}

		inline DWORD CListCtrl::SetExtendedStyle(_In_ DWORD dwNewStyle)
		{
			ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, (LPARAM)dwNewStyle);
		}
		inline HCURSOR CListCtrl::SetHotCursor(_In_ HCURSOR hc)
		{
			ASSERT(::IsWindow(m_hWnd) && hc != NULL); return (HCURSOR) ::SendMessage(m_hWnd, LVM_SETHOTCURSOR, 0, (LPARAM)hc);
		}
		inline int CListCtrl::SetHotItem(_In_ int iIndex)
		{
			ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_SETHOTITEM, (WPARAM)iIndex, 0);
		}
		inline void CListCtrl::SetWorkAreas(_In_ int nWorkAreas, _In_ LPRECT lpRect)
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, LVM_SETWORKAREAS, nWorkAreas, (LPARAM)lpRect);
		}
		inline int CListCtrl::SubItemHitTest(_In_ LPLVHITTESTINFO pInfo)
		{
			ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_SUBITEMHITTEST, 0, (LPARAM)pInfo);
		}
		inline HCURSOR CListCtrl::GetHotCursor() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (HCURSOR) ::SendMessage(m_hWnd, LVM_GETHOTCURSOR, 0, 0);
		}
		inline int CListCtrl::GetHotItem() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_GETHOTITEM, 0, 0);
		}
		inline DWORD CListCtrl::GetExtendedStyle() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
		}


		//inline CToolTipCtrl* CListCtrl::GetToolTips() const
		//	{ ASSERT(::IsWindow(m_hWnd)); return (CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, LVM_GETTOOLTIPS, 0, 0L)); }
		//inline CToolTipCtrl* CListCtrl::SetToolTips(_In_ CToolTipCtrl* pTip)
		//	{ ASSERT(::IsWindow(m_hWnd)); return (CToolTipCtrl*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, LVM_SETTOOLTIPS, 0, (LPARAM) pTip->GetSafeHwnd())); }
		inline COLORREF CListCtrl::GetBkColor() const
			{ ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, LVM_GETBKCOLOR, 0, 0L); }

		inline BOOL CListCtrl::SetBkColor(_In_ COLORREF cr)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SETBKCOLOR, 0, cr);
		}
		inline HIMAGELIST CListCtrl::GetImageList(_In_ int nImageList) const
		{
			ASSERT(::IsWindow(m_hWnd)); return (HIMAGELIST) ::SendMessage(m_hWnd, LVM_GETIMAGELIST, nImageList, 0L);
		}
		inline HIMAGELIST CListCtrl::SetImageList(HIMAGELIST hImageList, _In_ int nImageList)
		{
			ASSERT(::IsWindow(m_hWnd)); return (HIMAGELIST)::SendMessage(m_hWnd, LVM_SETIMAGELIST, nImageList, (LPARAM)hImageList);
		}
		inline int CListCtrl::GetItemCount() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_GETITEMCOUNT, 0, 0L);
		}
		inline BOOL CListCtrl::GetItem(_Out_ LVITEM* pItem) const
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_GETITEM, 0, (LPARAM)pItem);
		}
		inline BOOL CListCtrl::SetItem(_In_ const LVITEM* pItem)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SETITEM, 0, (LPARAM)pItem);
		}
		//inline BOOL CListCtrl::SetItemData(_In_ int nItem, _In_ DWORD_PTR dwData)
		//	{ ASSERT(::IsWindow(m_hWnd)); return SetItem(nItem, 0, LVIF_PARAM, NULL, 0, 0, 0, (LPARAM)dwData); }
		inline int CListCtrl::InsertItem(_In_ const LVITEM* pItem)
		{
			ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_INSERTITEM, 0, (LPARAM)pItem);
		}
		inline int CListCtrl::InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem)
			{ ASSERT(::IsWindow(m_hWnd)); return InsertItem(LVIF_TEXT, nItem, lpszItem, 0, 0, 0, 0); }
		inline int CListCtrl::InsertItem(_In_ int nItem, _In_z_ LPCTSTR lpszItem, _In_ int nImage)
			{ ASSERT(::IsWindow(m_hWnd)); return InsertItem(LVIF_TEXT|LVIF_IMAGE, nItem, lpszItem, 0, 0, nImage, 0); }

		inline BOOL CListCtrl::DeleteItem(_In_ int nItem)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_DELETEITEM, nItem, 0L);
		}
		inline BOOL CListCtrl::DeleteAllItems()
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_DELETEALLITEMS, 0, 0L);
		}
		inline UINT CListCtrl::GetCallbackMask() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, LVM_GETCALLBACKMASK, 0, 0);
		}
		inline BOOL CListCtrl::SetCallbackMask(_In_ UINT nMask)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SETCALLBACKMASK, nMask, 0);
		}
		inline int CListCtrl::GetNextItem(_In_ int nItem, _In_ int nFlags) const
		{
			ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_GETNEXTITEM, nItem, MAKELPARAM(nFlags, 0));
		}
		//inline POSITION CListCtrl::GetFirstSelectedItemPosition() const
		//{
		//	ASSERT(::IsWindow(m_hWnd)); return (POSITION)(DWORD_PTR)(1 + GetNextItem(-1, LVIS_SELECTED));
		//}
		//inline int CListCtrl::GetNextSelectedItem(_Inout_ POSITION& pos) const
		//{
		//	ASSERT(::IsWindow(m_hWnd)); DWORD_PTR nOldPos = (DWORD_PTR)pos - 1; pos = (POSITION)(DWORD_PTR)(1 + GetNextItem((UINT)nOldPos, LVIS_SELECTED)); return (UINT)nOldPos;
		//}
		inline int CListCtrl::FindItem(_In_ LVFINDINFO* pFindInfo, _In_ int nStart) const
		{
			ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_FINDITEM, nStart, (LPARAM)pFindInfo);
		}
		inline int CListCtrl::HitTest(_In_ LVHITTESTINFO* pHitTestInfo) const
		{
			ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_HITTEST, 0, (LPARAM)pHitTestInfo);
		}
		inline BOOL CListCtrl::SetItemPosition(_In_ int nItem, _In_ POINT pt)  // LVM_SETITEMPOSITION is not supported, only LVM_SETITEMPOSITION32 is.
		{
			ASSERT(::IsWindow(m_hWnd)); ASSERT((GetStyle() & LVS_OWNERDATA) == 0); return (BOOL) ::SendMessage(m_hWnd, LVM_SETITEMPOSITION32, nItem, (LPARAM)&pt);
		}
		inline BOOL CListCtrl::GetItemPosition(_In_ int nItem, _Out_ LPPOINT lpPoint) const
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_GETITEMPOSITION, nItem, (LPARAM)lpPoint);
		}
		inline int CListCtrl::GetStringWidth(_In_z_ LPCTSTR lpsz) const
		{
			ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_GETSTRINGWIDTH, 0, (LPARAM)lpsz);
		}
		inline BOOL CListCtrl::EnsureVisible(_In_ int nItem, _In_ BOOL bPartialOK)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_ENSUREVISIBLE, nItem, MAKELPARAM(bPartialOK, 0));
		}
		inline BOOL CListCtrl::Scroll(_In_ CSize size)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SCROLL, size.cx, size.cy);
		}
		inline BOOL CListCtrl::RedrawItems(_In_ int nFirst, _In_ int nLast)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_REDRAWITEMS, nFirst, nLast);
		}
		inline BOOL CListCtrl::Arrange(_In_ UINT nCode)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_ARRANGE, nCode, 0L);
		}
		//inline CEdit* CListCtrl::EditLabel(_In_ int nItem)
		//{
		//	ASSERT(::IsWindow(m_hWnd)); return (CEdit*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, LVM_EDITLABEL, nItem, 0L));
		//}
		//inline CEdit* CListCtrl::GetEditControl() const
		//{
		//	ASSERT(::IsWindow(m_hWnd)); return (CEdit*)CWnd::FromHandle((HWND)::SendMessage(m_hWnd, LVM_GETEDITCONTROL, 0, 0L));
		//}
		inline BOOL CListCtrl::GetColumn(_In_ int nCol, _Out_ LVCOLUMN* pColumn) const
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_GETCOLUMN, nCol, (LPARAM)pColumn);
		}
		inline BOOL CListCtrl::SetColumn(_In_ int nCol, _In_ const LVCOLUMN* pColumn)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SETCOLUMN, nCol, (LPARAM)pColumn);
		}
		inline int CListCtrl::InsertColumn(_In_ int nCol, _In_ const LVCOLUMN* pColumn)
		{
			ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_INSERTCOLUMN, nCol, (LPARAM)pColumn);
		}
		inline BOOL CListCtrl::DeleteColumn(_In_ int nCol)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_DELETECOLUMN, nCol, 0);
		}
		inline int CListCtrl::GetColumnWidth(_In_ int nCol) const
		{
			ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_GETCOLUMNWIDTH, nCol, 0);
		}
		inline BOOL CListCtrl::SetColumnWidth(_In_ int nCol, _In_ int cx)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SETCOLUMNWIDTH, nCol, MAKELPARAM(cx, 0));
		}
		inline BOOL CListCtrl::GetViewRect(_Out_ LPRECT lpRect) const
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_GETVIEWRECT, 0, (LPARAM)lpRect);
		}
		inline COLORREF CListCtrl::GetTextColor() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, LVM_GETTEXTCOLOR, 0, 0L);
		}
		inline BOOL CListCtrl::SetTextColor(_In_ COLORREF cr)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SETTEXTCOLOR, 0, cr);
		}
		inline COLORREF CListCtrl::GetTextBkColor() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, LVM_GETTEXTBKCOLOR, 0, 0L);
		}
		inline BOOL CListCtrl::SetTextBkColor(_In_ COLORREF cr)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SETTEXTBKCOLOR, 0, cr);
		}
		inline int CListCtrl::GetTopIndex() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_GETTOPINDEX, 0, 0);
		}
		inline int CListCtrl::GetCountPerPage() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (int) ::SendMessage(m_hWnd, LVM_GETCOUNTPERPAGE, 0, 0);
		}
		inline BOOL CListCtrl::GetOrigin(_Out_ LPPOINT lpPoint) const
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_GETORIGIN, 0, (LPARAM)lpPoint);
		}
		inline BOOL CListCtrl::Update(_In_ int nItem)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_UPDATE, nItem, 0L);
		}
		inline BOOL CListCtrl::SetItemState(_In_ int nItem, _In_ LVITEM* pItem)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, LVM_SETITEMSTATE, nItem, (LPARAM)pItem);
		}
		inline UINT CListCtrl::GetItemState(_In_ int nItem, _In_ UINT nMask) const
		{
			ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, LVM_GETITEMSTATE, nItem, nMask);
		}
		inline void CListCtrl::SetItemCount(_In_ int nItems)
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, LVM_SETITEMCOUNT, nItems, 0);
		}
		inline BOOL CListCtrl::SortItems(_In_ PFNLVCOMPARE pfnCompare, _In_ DWORD_PTR dwData)
		{
			ASSERT(::IsWindow(m_hWnd)); ASSERT((GetStyle() & LVS_OWNERDATA) == 0); return (BOOL) ::SendMessage(m_hWnd, LVM_SORTITEMS, dwData, (LPARAM)pfnCompare);
		}
		inline BOOL CListCtrl::SortItemsEx(_In_ PFNLVCOMPARE pfnCompare, _In_ DWORD_PTR dwData)
		{
			ASSERT(::IsWindow(m_hWnd)); ASSERT((GetStyle() & LVS_OWNERDATA) == 0); return (BOOL) ::SendMessage(m_hWnd, LVM_SORTITEMSEX, dwData, (LPARAM)pfnCompare);
		}
		inline UINT CListCtrl::GetSelectedCount() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (UINT) ::SendMessage(m_hWnd, LVM_GETSELECTEDCOUNT, 0, 0L);
		}
		inline BOOL CListCtrl::GetItemSpacing(_In_ BOOL fSmall, _Out_ int* pnHorzSpacing, _Out_ int* pnVertSpacing) const
		{
			ASSERT(::IsWindow(m_hWnd));
			ASSERT(pnHorzSpacing != NULL);
			ASSERT(pnVertSpacing != NULL);
			if (pnHorzSpacing == NULL || pnVertSpacing == NULL)
			{
				return FALSE;
			}
			DWORD dwSpacing = (DWORD)::SendMessage(m_hWnd, LVM_GETITEMSPACING, (WPARAM)fSmall, 0L);
			*pnHorzSpacing = (int)LOWORD(dwSpacing);
			*pnVertSpacing = (int)HIWORD(dwSpacing);
			return TRUE;
		}

	};


#pragma endregion

#pragma region
	/*============================================================================*/
	// CImageList

	class CImageList
	{
		// Constructors
	public:
		CImageList();
		BOOL Create(int cx, int cy, UINT nFlags, int nInitial, int nGrow);
		//BOOL Create(UINT nBitmapID, int cx, int nGrow, COLORREF crMask);
		//BOOL Create(LPCTSTR lpszBitmapID, int cx, int nGrow, COLORREF crMask);
		//BOOL Create(CImageList& imagelist1, int nImage1, CImageList& imagelist2,
		//	int nImage2, int dx, int dy);
		//BOOL Create(CImageList* pImageList);

		//// Attributes
		HIMAGELIST m_hImageList;            // must be first data member
		//operator HIMAGELIST() const;
		//HIMAGELIST GetSafeHandle() const;

		//static CImageList* PASCAL FromHandle(HIMAGELIST hImageList);
		//static CImageList* PASCAL FromHandlePermanent(HIMAGELIST hImageList);
		//static void PASCAL DeleteTempMap();
		//BOOL Attach(HIMAGELIST hImageList);
		//HIMAGELIST Detach();

		//int GetImageCount() const;
		//COLORREF SetBkColor(COLORREF cr);
		//COLORREF GetBkColor() const;
		//BOOL GetImageInfo(int nImage, IMAGEINFO* pImageInfo) const;

		//// Operations
		BOOL DeleteImageList();
		//BOOL SetImageCount(UINT uNewCount);

		//int Add(CBitmap* pbmImage, CBitmap* pbmMask);
		//int Add(CBitmap* pbmImage, COLORREF crMask);
		//BOOL Remove(int nImage);
		//BOOL Replace(int nImage, CBitmap* pbmImage, CBitmap* pbmMask);
		//int Add(HICON hIcon);
		//int Replace(int nImage, HICON hIcon);
		//HICON ExtractIcon(int nImage);
		//BOOL Draw(CDC* pDC, int nImage, POINT pt, UINT nStyle);
		//BOOL DrawEx(CDC* pDC, int nImage, POINT pt, SIZE sz, COLORREF clrBk, COLORREF clrFg, UINT nStyle);
		//BOOL SetOverlayImage(int nImage, int nOverlay);
		//BOOL Copy(int iDst, int iSrc, UINT uFlags = ILCF_MOVE);
		//BOOL Copy(int iDst, CImageList* pSrc, int iSrc, UINT uFlags = ILCF_MOVE);
		//BOOL DrawIndirect(IMAGELISTDRAWPARAMS* pimldp);
		//BOOL DrawIndirect(CDC* pDC, int nImage, POINT pt, SIZE sz, POINT ptOrigin,
		//	UINT fStyle = ILD_NORMAL, DWORD dwRop = SRCCOPY,
		//	COLORREF rgbBack = CLR_DEFAULT, COLORREF rgbFore = CLR_DEFAULT,
		//	DWORD fState = ILS_NORMAL, DWORD Frame = 0, COLORREF crEffect = CLR_DEFAULT);

		//BOOL Read(CArchive* pArchive);
		//BOOL Write(CArchive* pArchive);

		//// Drag APIs
		//BOOL BeginDrag(int nImage, CPoint ptHotSpot);
		//static void PASCAL EndDrag();
		//static BOOL PASCAL DragMove(CPoint pt);
		//BOOL SetDragCursorImage(int nDrag, CPoint ptHotSpot);
		//static BOOL PASCAL DragShowNolock(BOOL bShow);
		//static CImageList* PASCAL GetDragImage(LPPOINT lpPoint, LPPOINT lpPointHotSpot);
		//static BOOL PASCAL DragEnter(CWnd* pWndLock, CPoint point);
		//static BOOL PASCAL DragLeave(CWnd* pWndLock);

		// Implementation
	public:
		virtual ~CImageList();
#ifdef _DEBUG
		//virtual void Dump(CDumpContext& dc) const;
		//virtual void AssertValid() const;
#endif
		//inline CImageList::operator HIMAGELIST() const
		//{
		//	return m_hImageList;
		//}
		inline HIMAGELIST CImageList::GetSafeHandle() const
		{
			return (this == NULL) ? NULL : m_hImageList;
		}
		inline int CImageList::GetImageCount() const
		{
			ASSERT(m_hImageList != NULL); return ImageList_GetImageCount(m_hImageList);
		}
		//inline int CImageList::Add(CBitmap* pbmImage, CBitmap* pbmMask)
		//{
		//	ASSERT(m_hImageList != NULL); return ImageList_Add(m_hImageList, (HBITMAP)pbmImage->GetSafeHandle(), (HBITMAP)pbmMask->GetSafeHandle());
		//}
		//inline int CImageList::Add(CBitmap* pbmImage, COLORREF crMask)
		//{
		//	ASSERT(m_hImageList != NULL); return ImageList_AddMasked(m_hImageList, (HBITMAP)pbmImage->GetSafeHandle(), crMask);
		//}
		inline BOOL CImageList::Remove(int nImage)
		{
			ASSERT(m_hImageList != NULL); return ImageList_Remove(m_hImageList, nImage);
		}
		//inline BOOL CImageList::Replace(int nImage, CBitmap* pbmImage, CBitmap* pbmMask)
		//{
		//	ASSERT(m_hImageList != NULL); return ImageList_Replace(m_hImageList, nImage, (HBITMAP)pbmImage->GetSafeHandle(), (HBITMAP)pbmMask->GetSafeHandle());
		//}
		inline int CImageList::Add(HICON hIcon)
		{
			ASSERT(m_hImageList != NULL); return ImageList_AddIcon(m_hImageList, hIcon);
		}
		inline int CImageList::Replace(int nImage, HICON hIcon)
		{
			ASSERT(m_hImageList != NULL); return ImageList_ReplaceIcon(m_hImageList, nImage, hIcon);
		}
		inline HICON CImageList::ExtractIcon(int nImage)
		{
			ASSERT(m_hImageList != NULL); return ImageList_ExtractIcon(NULL, m_hImageList, nImage);
		}
		//inline BOOL CImageList::Draw(CDC* pDC, int nImage, POINT pt, UINT nStyle)
		//{
		//	ASSERT(m_hImageList != NULL); ASSERT(pDC != NULL); return ImageList_Draw(m_hImageList, nImage, pDC->GetSafeHdc(), pt.x, pt.y, nStyle);
		//}
		//inline BOOL CImageList::DrawEx(CDC* pDC, int nImage, POINT pt, SIZE sz, COLORREF clrBk, COLORREF clrFg, UINT nStyle)
		//{
		//	ASSERT(m_hImageList != NULL); ASSERT(pDC != NULL); return ImageList_DrawEx(m_hImageList, nImage, pDC->GetSafeHdc(), pt.x, pt.y, sz.cx, sz.cy, clrBk, clrFg, nStyle);
		//}
		inline COLORREF CImageList::SetBkColor(COLORREF cr)
		{
			ASSERT(m_hImageList != NULL); return ImageList_SetBkColor(m_hImageList, cr);
		}
		inline COLORREF CImageList::GetBkColor() const
		{
			ASSERT(m_hImageList != NULL); return ImageList_GetBkColor(m_hImageList);
		}
		inline BOOL CImageList::SetOverlayImage(int nImage, int nOverlay)
		{
			ASSERT(m_hImageList != NULL); return ImageList_SetOverlayImage(m_hImageList, nImage, nOverlay);
		}
		inline BOOL CImageList::GetImageInfo(int nImage, IMAGEINFO* pImageInfo) const
		{
			ASSERT(m_hImageList != NULL); return ImageList_GetImageInfo(m_hImageList, nImage, pImageInfo);
		}
		inline BOOL CImageList::BeginDrag(int nImage, CPoint ptHotSpot)
		{
			ASSERT(m_hImageList != NULL); return ImageList_BeginDrag(m_hImageList, nImage, ptHotSpot.x, ptHotSpot.y);
		}
		inline void PASCAL CImageList::EndDrag()
		{
			ImageList_EndDrag();
		}
		inline BOOL PASCAL CImageList::DragMove(CPoint pt)
		{
			return ImageList_DragMove(pt.x, pt.y);
		}
		inline BOOL CImageList::SetDragCursorImage(int nDrag, CPoint ptHotSpot)
		{
			ASSERT(m_hImageList != NULL); return ImageList_SetDragCursorImage(m_hImageList, nDrag, ptHotSpot.x, ptHotSpot.y);
		}
		inline BOOL PASCAL CImageList::DragShowNolock(BOOL bShow)
		{
			return ImageList_DragShowNolock(bShow);
		}
		//inline CImageList* PASCAL CImageList::GetDragImage(LPPOINT lpPoint, LPPOINT lpPointHotSpot)
		//{
		//	return CImageList::FromHandle(ImageList_GetDragImage(lpPoint, lpPointHotSpot));
		//}
		inline BOOL PASCAL CImageList::DragEnter(CWnd* pWndLock, CPoint point)
		{
			return ImageList_DragEnter(pWndLock->GetSafeHwnd(), point.x, point.y);
		}
		inline BOOL PASCAL CImageList::DragLeave(CWnd* pWndLock)
		{
			return ImageList_DragLeave(pWndLock->GetSafeHwnd());
		}

	};

#pragma endregion

#pragma region
	// Window control functions
	class CStatic : public CWnd
	{

		// Constructors
	public:
		//CStatic();
		////virtual BOOL Create(LPCTSTR lpszText, DWORD dwStyle,
		////	const RECT& rect, CWnd* pParentWnd, UINT nID = 0xffff);

		//// Operations
		//HICON SetIcon(HICON hIcon);
		//HICON GetIcon() const;

		//HENHMETAFILE SetEnhMetaFile(HENHMETAFILE hMetaFile);
		//HENHMETAFILE GetEnhMetaFile() const;
		//HBITMAP SetBitmap(HBITMAP hBitmap);
		//HBITMAP GetBitmap() const;
		//HCURSOR SetCursor(HCURSOR hCursor);
		//HCURSOR GetCursor();

		// Overridables (for owner draw only)
		//virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

		// Implementation
	public:
//		virtual ~CStatic();
		inline CStatic::CStatic()
		{ }
		inline HICON CStatic::SetIcon(HICON hIcon)
		{
			ASSERT(::IsWindow(m_hWnd)); return (HICON)::SendMessage(m_hWnd, STM_SETICON, (WPARAM)hIcon, 0L);
		}
		inline HICON CStatic::GetIcon() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (HICON)::SendMessage(m_hWnd, STM_GETICON, 0, 0L);
		}
		inline HENHMETAFILE CStatic::SetEnhMetaFile(HENHMETAFILE hMetaFile)
		{
			ASSERT(::IsWindow(m_hWnd)); return (HENHMETAFILE)::SendMessage(m_hWnd, STM_SETIMAGE, IMAGE_ENHMETAFILE, (LPARAM)hMetaFile);
		}
		inline HENHMETAFILE CStatic::GetEnhMetaFile() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (HENHMETAFILE)::SendMessage(m_hWnd, STM_GETIMAGE, IMAGE_ENHMETAFILE, 0L);
		}
		inline HBITMAP CStatic::SetBitmap(HBITMAP hBitmap)
		{
			ASSERT(::IsWindow(m_hWnd)); return (HBITMAP)::SendMessage(m_hWnd, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
		}
		inline HBITMAP CStatic::GetBitmap() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (HBITMAP)::SendMessage(m_hWnd, STM_GETIMAGE, IMAGE_BITMAP, 0L);
		}
		inline HCURSOR CStatic::SetCursor(HCURSOR hCursor)
		{
			ASSERT(::IsWindow(m_hWnd)); return (HCURSOR)::SendMessage(m_hWnd, STM_SETIMAGE, IMAGE_CURSOR, (LPARAM)hCursor);
		}
		inline HCURSOR CStatic::GetCursor()
		{
			ASSERT(::IsWindow(m_hWnd)); return (HCURSOR)::SendMessage(m_hWnd, STM_GETIMAGE, IMAGE_CURSOR, 0L);
		}

	protected:
		//virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
	};

	class CButton : public CWnd
	{

		// Constructors
	public:
//		CButton();
//		//virtual BOOL Create(LPCTSTR lpszCaption, DWORD dwStyle,
//		//	const RECT& rect, CWnd* pParentWnd, UINT nID);
//
//		// Attributes
//		UINT GetState() const;
//		void SetState(BOOL bHighlight);
//		int GetCheck() const;
//		void SetCheck(int nCheck);
//		UINT GetButtonStyle() const;
//		void SetButtonStyle(UINT nStyle, BOOL bRedraw = TRUE);
//
//		HICON SetIcon(HICON hIcon);
//		HICON GetIcon() const;
//		HBITMAP SetBitmap(HBITMAP hBitmap);
//		HBITMAP GetBitmap() const;
//		HCURSOR SetCursor(HCURSOR hCursor);
//		HCURSOR GetCursor();
//
//		//AFX_ANSI_DEPRECATED BOOL GetIdealSize(_Out_ LPSIZE psize) const;
//		//AFX_ANSI_DEPRECATED BOOL SetImageList(_In_ PBUTTON_IMAGELIST pbuttonImagelist);
//		//AFX_ANSI_DEPRECATED BOOL GetImageList(_In_ PBUTTON_IMAGELIST pbuttonImagelist) const;
//		//AFX_ANSI_DEPRECATED BOOL SetTextMargin(_In_ LPRECT pmargin);
//		//AFX_ANSI_DEPRECATED BOOL GetTextMargin(_Out_ LPRECT pmargin) const;
//
//#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
//		CString GetNote() const;
//		_Check_return_ BOOL GetNote(_Out_writes_z_(*pcchNote) LPTSTR lpszNote, _Inout_ UINT* pcchNote) const;
//		BOOL SetNote(_In_z_ LPCTSTR lpszNote);
//		UINT GetNoteLength() const;
//		BOOL GetSplitInfo(_Out_ PBUTTON_SPLITINFO pInfo) const;
//		BOOL SetSplitInfo(_In_ PBUTTON_SPLITINFO pInfo);
//		UINT GetSplitStyle() const;
//		BOOL SetSplitStyle(_In_ UINT nStyle);
//		BOOL GetSplitSize(_Out_ LPSIZE pSize) const;
//		BOOL SetSplitSize(_In_ LPSIZE pSize);
//		CImageList* GetSplitImageList() const;
//		BOOL SetSplitImageList(_In_ CImageList* pSplitImageList);
//		TCHAR GetSplitGlyph() const;
//		BOOL SetSplitGlyph(_In_ TCHAR chGlyph);
//		BOOL SetDropDownState(_In_ BOOL fDropDown);
//
//		// Sets whether the action associated with the button requires elevated permissions.
//		// If elevated permissions are required then the button should display an elevated icon.
//		HICON SetShield(_In_ BOOL fElevationRequired);
//#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
//
		// Overridables (for owner draw only)
		//virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

		// Implementation
	public:
		//virtual ~CButton();
	//protected:
		//virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
		inline CButton::CButton()
		{ }
		inline UINT CButton::GetState() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (UINT)::SendMessage(m_hWnd, BM_GETSTATE, 0, 0);
		}
		inline void CButton::SetState(BOOL bHighlight)
		{
			::SendMessage(m_hWnd, BM_SETSTATE, bHighlight, 0);
		}
		inline int CButton::GetCheck() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, BM_GETCHECK, 0, 0);
		}
		inline void CButton::SetCheck(int nCheck)
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, BM_SETCHECK, nCheck, 0);
		}
		inline UINT CButton::GetButtonStyle() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (UINT)GetWindowLong(m_hWnd, GWL_STYLE) & 0xFFFF;
		}
		inline void CButton::SetButtonStyle(UINT nStyle, BOOL bRedraw)
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, BM_SETSTYLE, nStyle, (LPARAM)bRedraw);
		}
		// Win4
		inline HICON CButton::SetIcon(HICON hIcon)
		{
			ASSERT(::IsWindow(m_hWnd)); return (HICON)::SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_ICON, (LPARAM)hIcon);
		}
		inline HICON CButton::GetIcon() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (HICON)::SendMessage(m_hWnd, BM_GETIMAGE, IMAGE_ICON, 0L);
		}
		inline HBITMAP CButton::SetBitmap(HBITMAP hBitmap)
		{
			ASSERT(::IsWindow(m_hWnd)); return (HBITMAP)::SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
		}
		inline HBITMAP CButton::GetBitmap() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (HBITMAP)::SendMessage(m_hWnd, BM_GETIMAGE, IMAGE_BITMAP, 0L);
		}
		inline HCURSOR CButton::SetCursor(HCURSOR hCursor)
		{
			ASSERT(::IsWindow(m_hWnd)); return (HCURSOR)::SendMessage(m_hWnd, BM_SETIMAGE, IMAGE_CURSOR, (LPARAM)hCursor);
		}
		inline HCURSOR CButton::GetCursor()
		{
			ASSERT(::IsWindow(m_hWnd)); return (HCURSOR)::SendMessage(m_hWnd, BM_GETIMAGE, IMAGE_CURSOR, 0L);
		}

	};



//#if defined(UNICODE)
//	inline CSplitButton::CSplitButton()
//		:m_pMenu(NULL), m_nMenuId((UINT)-1), m_nSubMenuId((UINT)-1)
//	{ }
//	inline CSplitButton::CSplitButton(UINT nMenuId, UINT nSubMenuId)
//		: m_pMenu(NULL), m_nMenuId((UINT)nMenuId), m_nSubMenuId((UINT)nSubMenuId)
//	{  }
//	inline CSplitButton::CSplitButton(CMenu* pMenu)
//		: m_pMenu(pMenu), m_nMenuId((UINT)-1), m_nSubMenuId((UINT)-1)
//	{  }
//#endif // defined(UNICODE)

//	inline CListBox::CListBox()
//	{ }
//	inline int CListBox::GetCount() const
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_GETCOUNT, 0, 0);
//	}
//	inline int CListBox::GetCurSel() const
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_GETCURSEL, 0, 0);
//	}
//	inline int CListBox::SetCurSel(int nSelect)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_SETCURSEL, nSelect, 0);
//	}
//	inline int CListBox::GetHorizontalExtent() const
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_GETHORIZONTALEXTENT,
//			0, 0);
//	}
//	inline void CListBox::SetHorizontalExtent(int cxExtent)
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, LB_SETHORIZONTALEXTENT, cxExtent, 0);
//	}
//	inline int CListBox::GetSelCount() const
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_GETSELCOUNT, 0, 0);
//	}
//	inline int CListBox::GetSelItems(int nMaxItems, LPINT rgIndex) const
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_GETSELITEMS, nMaxItems, (LPARAM)rgIndex);
//	}
//	inline int CListBox::GetTopIndex() const
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_GETTOPINDEX, 0, 0);
//	}
//	inline int CListBox::SetTopIndex(int nIndex)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_SETTOPINDEX, nIndex, 0);
//	}
//	inline DWORD_PTR CListBox::GetItemData(int nIndex) const
//	{
//		ASSERT(::IsWindow(m_hWnd)); return ::SendMessage(m_hWnd, LB_GETITEMDATA, nIndex, 0);
//	}
//	inline int CListBox::SetItemData(int nIndex, DWORD_PTR dwItemData)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_SETITEMDATA, nIndex, (LPARAM)dwItemData);
//	}
//	inline void* CListBox::GetItemDataPtr(int nIndex) const
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (LPVOID)::SendMessage(m_hWnd, LB_GETITEMDATA, nIndex, 0);
//	}
//	inline int CListBox::SetItemDataPtr(int nIndex, void* pData)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return SetItemData(nIndex, (DWORD_PTR)(LPVOID)pData);
//	}
//	inline int CListBox::GetItemRect(int nIndex, LPRECT lpRect) const
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_GETITEMRECT, nIndex, (LPARAM)lpRect);
//	}
//	inline int CListBox::GetSel(int nIndex) const
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_GETSEL, nIndex, 0);
//	}
//	inline int CListBox::SetSel(int nIndex, BOOL bSelect)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_SETSEL, bSelect, nIndex);
//	}
//#pragma warning(push)
//#pragma warning(disable: 6001 6054)
//	inline int CListBox::GetText(_In_ int nIndex, _Pre_notnull_ _Post_z_ LPTSTR lpszBuffer) const
//	{
//		ASSERT(::IsWindow(m_hWnd));
//		return (int)::SendMessage(m_hWnd, LB_GETTEXT, nIndex, (LPARAM)lpszBuffer);
//	}
//#pragma warning(pop)
//	inline int CListBox::GetTextLen(int nIndex) const
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_GETTEXTLEN, nIndex, 0);
//	}
//	inline void CListBox::SetColumnWidth(int cxWidth)
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, LB_SETCOLUMNWIDTH, cxWidth, 0);
//	}
//	inline BOOL CListBox::SetTabStops(int nTabStops, LPINT rgTabStops)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, LB_SETTABSTOPS, nTabStops, (LPARAM)rgTabStops);
//	}
//	inline void CListBox::SetTabStops()
//	{
//		ASSERT(::IsWindow(m_hWnd)); VERIFY(::SendMessage(m_hWnd, LB_SETTABSTOPS, 0, 0));
//	}
//	inline BOOL CListBox::SetTabStops(const int& cxEachStop)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, LB_SETTABSTOPS, 1, (LPARAM)(LPINT)&cxEachStop);
//	}
//	inline int CListBox::SetItemHeight(int nIndex, UINT cyItemHeight)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_SETITEMHEIGHT, nIndex, MAKELONG(cyItemHeight, 0));
//	}
//	inline int CListBox::GetItemHeight(int nIndex) const
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_GETITEMHEIGHT, nIndex, 0L);
//	}
//	inline int CListBox::FindStringExact(int nIndexStart, LPCTSTR lpszFind) const
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_FINDSTRINGEXACT, nIndexStart, (LPARAM)lpszFind);
//	}
//	inline int CListBox::GetCaretIndex() const
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_GETCARETINDEX, 0, 0L);
//	}
//	inline int CListBox::SetCaretIndex(int nIndex, BOOL bScroll)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_SETCARETINDEX, nIndex, MAKELONG(bScroll, 0));
//	}
//	inline int CListBox::AddString(LPCTSTR lpszItem)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_ADDSTRING, 0, (LPARAM)lpszItem);
//	}
//	inline int CListBox::DeleteString(UINT nIndex)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_DELETESTRING, nIndex, 0);
//	}
//	inline int CListBox::InsertString(int nIndex, LPCTSTR lpszItem)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_INSERTSTRING, nIndex, (LPARAM)lpszItem);
//	}
//	inline void CListBox::ResetContent()
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, LB_RESETCONTENT, 0, 0);
//	}
//	inline int CListBox::Dir(UINT attr, LPCTSTR lpszWildCard)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_DIR, attr, (LPARAM)lpszWildCard);
//	}
//	inline int CListBox::FindString(int nStartAfter, LPCTSTR lpszItem) const
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_FINDSTRING,
//			nStartAfter, (LPARAM)lpszItem);
//	}
//	inline int CListBox::SelectString(int nStartAfter, LPCTSTR lpszItem)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_SELECTSTRING,
//			nStartAfter, (LPARAM)lpszItem);
//	}
//	inline int CListBox::SelItemRange(BOOL bSelect, int nFirstItem, int nLastItem)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return bSelect ?
//			(int)::SendMessage(m_hWnd, LB_SELITEMRANGEEX, nFirstItem, nLastItem) :
//			(int)::SendMessage(m_hWnd, LB_SELITEMRANGEEX, nLastItem, nFirstItem);
//	}
//	inline void CListBox::SetAnchorIndex(int nIndex)
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, LB_SETANCHORINDEX, nIndex, 0);
//	}
//	inline int CListBox::GetAnchorIndex() const
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_GETANCHORINDEX, 0, 0);
//	}
//	inline LCID CListBox::GetLocale() const
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (LCID)::SendMessage(m_hWnd, LB_GETLOCALE, 0, 0);
//	}
//	inline LCID CListBox::SetLocale(LCID nNewLocale)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (LCID)::SendMessage(m_hWnd, LB_SETLOCALE, (WPARAM)nNewLocale, 0);
//	}
//	inline int CListBox::InitStorage(int nItems, UINT nBytes)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, LB_INITSTORAGE, (WPARAM)nItems, nBytes);
//	}
//
//	inline CCheckListBox::CCheckListBox()
//	{
//		m_cyText = 0; m_nStyle = 0; EnableActiveAccessibility();
//	}
//	inline UINT CCheckListBox::GetCheckStyle()
//	{
//		return m_nStyle;
//	}
#pragma endregion
#pragma region
class CComboBox : public CWnd 
{

// Constructors
public:
//	CComboBox();
//	virtual BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
//
//	// Attributes
//	// for entire combo box
//	int GetCount() const;
//	int GetCurSel() const;
//	int SetCurSel(int nSelect);
//	LCID GetLocale() const;
//	LCID SetLocale(LCID nNewLocale);
//	// Win4
//	int GetTopIndex() const;
//	int SetTopIndex(int nIndex);
//	int InitStorage(int nItems, UINT nBytes);
//	void SetHorizontalExtent(UINT nExtent);
//	UINT GetHorizontalExtent() const;
//	int SetDroppedWidth(UINT nWidth);
//	int GetDroppedWidth() const;
//
//	BOOL GetComboBoxInfo(PCOMBOBOXINFO pcbi) const;
//
//#if defined(UNICODE)
//	// Sets the minimum number of visible items in the drop-down list of the combo box.
//	BOOL SetMinVisibleItems(_In_ int iMinVisible);
//
//	// Retrieves the minimum number of visible items in the drop-down list of the combo box.
//	int GetMinVisible() const;
//
//#ifdef CB_SETCUEBANNER
//	// REVIEW: Sets the cue banner text displayed in the edit control of the combo box.
//	BOOL SetCueBanner(_In_z_ LPCTSTR lpszText);
//
//	// REVIEW: Retrieves the cue banner text displayed in the edit control of the combo box.
//	//CString GetCueBanner() const;
//	//BOOL GetCueBanner(_Out_writes_z_(cchText) LPTSTR lpszText, _In_ int cchText) const;
//#endif  // CB_SETCUEBANNER
//#endif  // defined(UNICODE)
//
//	// for edit control
//	DWORD GetEditSel() const;
//	BOOL LimitText(int nMaxChars);
//	BOOL SetEditSel(_In_ int nStartChar, _In_ int nEndChar);
//
//	// for combobox item
//	DWORD_PTR GetItemData(int nIndex) const;
//	int SetItemData(int nIndex, DWORD_PTR dwItemData);
//	void* GetItemDataPtr(int nIndex) const;
//	int SetItemDataPtr(int nIndex, void* pData);
//	int GetLBText(_In_ int nIndex, _Pre_notnull_ _Post_z_ LPTSTR lpszText) const;
//	//void GetLBText(int nIndex, CString& rString) const;
//	int GetLBTextLen(int nIndex) const;
//
//	int SetItemHeight(int nIndex, UINT cyItemHeight);
//	int GetItemHeight(int nIndex) const;
//	int FindStringExact(int nIndexStart, LPCTSTR lpszFind) const;
//	int SetExtendedUI(BOOL bExtended = TRUE);
//	BOOL GetExtendedUI() const;
//	void GetDroppedControlRect(LPRECT lprect) const;
//	BOOL GetDroppedState() const;
//
//	// Operations
//	// for drop-down combo boxes
//	void ShowDropDown(BOOL bShowIt = TRUE);
//
//	// manipulating listbox items
//	int AddString(LPCTSTR lpszString);
//	int DeleteString(UINT nIndex);
//	int InsertString(_In_ int nIndex, _In_z_ LPCTSTR lpszString);
//	void ResetContent();
//	int Dir(_In_ UINT attr, _In_ LPCTSTR lpszWildCard);
//
//	// selection helpers
//	int FindString(_In_ int nStartAfter, _In_z_ LPCTSTR lpszString) const;
//	int SelectString(int nStartAfter, LPCTSTR lpszString);
//
//	// Clipboard operations
//	void Clear();
//	void Copy();
//	void Cut();
//	void Paste();

	// Overridables (must override draw, measure and compare for owner draw)
	//virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	//virtual int CompareItem(LPCOMPAREITEMSTRUCT lpCompareItemStruct);
	//virtual void DeleteItem(LPDELETEITEMSTRUCT lpDeleteItemStruct);

	// Implementation
public:
	//virtual ~CComboBox();
public:
	//virtual BOOL OnChildNotify(UINT, WPARAM, LPARAM, LRESULT*);
	inline CComboBox::CComboBox()
	{ }
	inline int CComboBox::GetCount() const
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_GETCOUNT, 0, 0);
	}
	inline int CComboBox::GetCurSel() const
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_GETCURSEL, 0, 0);
	}
	inline int CComboBox::SetCurSel(int nSelect)
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_SETCURSEL, nSelect, 0);
	}
	inline DWORD CComboBox::GetEditSel() const
	{
		ASSERT(::IsWindow(m_hWnd)); return DWORD(::SendMessage(m_hWnd, CB_GETEDITSEL, 0, 0));
	}
	inline BOOL CComboBox::LimitText(int nMaxChars)
	{
		ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, CB_LIMITTEXT, nMaxChars, 0);
	}
	inline BOOL CComboBox::SetEditSel(_In_ int nStartChar, _In_ int nEndChar)
	{
		ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, CB_SETEDITSEL, 0, MAKELONG(nStartChar, nEndChar));
	}
	inline DWORD_PTR CComboBox::GetItemData(int nIndex) const
	{
		ASSERT(::IsWindow(m_hWnd)); return ::SendMessage(m_hWnd, CB_GETITEMDATA, nIndex, 0);
	}
	inline int CComboBox::SetItemData(int nIndex, DWORD_PTR dwItemData)
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_SETITEMDATA, nIndex, (LPARAM)dwItemData);
	}
	inline void* CComboBox::GetItemDataPtr(int nIndex) const
	{
		ASSERT(::IsWindow(m_hWnd)); return (LPVOID)GetItemData(nIndex);
	}
	inline int CComboBox::SetItemDataPtr(int nIndex, void* pData)
	{
		ASSERT(::IsWindow(m_hWnd)); return SetItemData(nIndex, (DWORD_PTR)(LPVOID)pData);
	}
#pragma warning(push)
#pragma warning(disable: 6001 6054)
	inline int CComboBox::GetLBText(_In_ int nIndex, _Pre_notnull_ _Post_z_ LPTSTR lpszText) const
	{
		ASSERT(::IsWindow(m_hWnd));
		return (int)::SendMessage(m_hWnd, CB_GETLBTEXT, nIndex, (LPARAM)lpszText);
	}
#pragma warning(pop)
	inline int CComboBox::GetLBTextLen(int nIndex) const
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_GETLBTEXTLEN, nIndex, 0);
	}
	inline void CComboBox::ShowDropDown(BOOL bShowIt)
	{
		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, CB_SHOWDROPDOWN, bShowIt, 0);
	}
	int AddString(LPCTSTR lpszString)
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_ADDSTRING, 0, (LPARAM)lpszString);
	}
	inline int CComboBox::DeleteString(UINT nIndex)
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_DELETESTRING, nIndex, 0);
	}
	inline int CComboBox::InsertString(_In_ int nIndex, _In_z_ LPCTSTR lpszString)
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_INSERTSTRING, nIndex, (LPARAM)lpszString);
	}
	inline void CComboBox::ResetContent()
	{
		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, CB_RESETCONTENT, 0, 0);
	}
	inline int CComboBox::Dir(_In_ UINT attr, _In_ LPCTSTR lpszWildCard)
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_DIR, attr, (LPARAM)lpszWildCard);
	}
	inline int CComboBox::FindString(_In_ int nStartAfter, _In_z_ LPCTSTR lpszString) const
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_FINDSTRING, nStartAfter,
			(LPARAM)lpszString);
	}
	inline int CComboBox::SelectString(int nStartAfter, LPCTSTR lpszString)
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_SELECTSTRING,
			nStartAfter, (LPARAM)lpszString);
	}
	inline void CComboBox::Clear()
	{
		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, WM_CLEAR, 0, 0);
	}
	inline void CComboBox::Copy()
	{
		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, WM_COPY, 0, 0);
	}
	inline void CComboBox::Cut()
	{
		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, WM_CUT, 0, 0);
	}
	inline void CComboBox::Paste()
	{
		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, WM_PASTE, 0, 0);
	}
	inline int CComboBox::SetItemHeight(int nIndex, UINT cyItemHeight)
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_SETITEMHEIGHT, nIndex, MAKELONG(cyItemHeight, 0));
	}
	inline int CComboBox::GetItemHeight(int nIndex) const
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_GETITEMHEIGHT, nIndex, 0L);
	}
	inline int CComboBox::FindStringExact(int nIndexStart, LPCTSTR lpszFind) const
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_FINDSTRINGEXACT, nIndexStart, (LPARAM)lpszFind);
	}
	inline int CComboBox::SetExtendedUI(BOOL bExtended)
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_SETEXTENDEDUI, bExtended, 0L);
	}
	inline BOOL CComboBox::GetExtendedUI() const
	{
		ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, CB_GETEXTENDEDUI, 0, 0L);
	}
	inline void CComboBox::GetDroppedControlRect(LPRECT lprect) const
	{
		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, CB_GETDROPPEDCONTROLRECT, 0, (LPARAM)lprect);
	}
	inline BOOL CComboBox::GetDroppedState() const
	{
		ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, CB_GETDROPPEDSTATE, 0, 0L);
	}
	inline LCID CComboBox::GetLocale() const
	{
		ASSERT(::IsWindow(m_hWnd)); return (LCID)::SendMessage(m_hWnd, CB_GETLOCALE, 0, 0);
	}
	inline LCID CComboBox::SetLocale(LCID nNewLocale)
	{
		ASSERT(::IsWindow(m_hWnd)); return (LCID)::SendMessage(m_hWnd, CB_SETLOCALE, (WPARAM)nNewLocale, 0);
	}
	inline int CComboBox::GetTopIndex() const
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_GETTOPINDEX, 0, 0);
	}
	inline int CComboBox::SetTopIndex(int nIndex)
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_SETTOPINDEX, nIndex, 0);
	}
	inline int CComboBox::InitStorage(int nItems, UINT nBytes)
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_INITSTORAGE, (WPARAM)nItems, nBytes);
	}
	inline void CComboBox::SetHorizontalExtent(UINT nExtent)
	{
		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, CB_SETHORIZONTALEXTENT, nExtent, 0);
	}
	inline UINT CComboBox::GetHorizontalExtent() const
	{
		ASSERT(::IsWindow(m_hWnd)); return (UINT)::SendMessage(m_hWnd, CB_GETHORIZONTALEXTENT, 0, 0);
	}
	inline int CComboBox::SetDroppedWidth(UINT nWidth)
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_SETDROPPEDWIDTH, nWidth, 0);
	}
	inline int CComboBox::GetDroppedWidth() const
	{
		ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, CB_GETDROPPEDWIDTH, 0, 0);
	}

};

#pragma endregion

#pragma region
	class CEdit : public CWnd
	{
//		// DECLARE_DYNAMIC virtual OK - CWnd already has DECLARE_DYNAMIC
//		DECLARE_DYNAMIC(CEdit)
//
//		// Constructors
//	public:
//		CEdit();
//		//BOOL Create(DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID);
//
//		// Attributes
//		BOOL CanUndo() const;
//		int GetLineCount() const;
//		BOOL GetModify() const;
//		void SetModify(BOOL bModified = TRUE);
//		void GetRect(LPRECT lpRect) const;
//		DWORD GetSel() const;
//		void GetSel(int& nStartChar, int& nEndChar) const;
//		HLOCAL GetHandle() const;
//		void SetHandle(HLOCAL hBuffer);
//		void SetMargins(UINT nLeft, UINT nRight);
//		DWORD GetMargins() const;
//		void SetLimitText(UINT nMax);
//		UINT GetLimitText() const;
//		CPoint PosFromChar(UINT nChar) const;
//		int CharFromPos(CPoint pt) const;
//
//		// NOTE: first word in lpszBuffer must contain the size of the buffer!
//		// NOTE: may not return null character
//		int GetLine(_In_ int nIndex, _Out_ LPTSTR lpszBuffer) const;
//		// NOTE: may not return null character
//		int GetLine(_In_ int nIndex, _Out_writes_to_(nMaxLength, return) LPTSTR lpszBuffer, _In_ int nMaxLength) const;
//
//		AFX_ANSI_DEPRECATED BOOL SetCueBanner(_In_z_ LPCWSTR lpszText, _In_ BOOL fDrawIfFocused = FALSE);
//		AFX_ANSI_DEPRECATED BOOL GetCueBanner(_Out_writes_z_(cchText) LPWSTR lpszText, _In_ int cchText) const;
//
//#if defined(UNICODE)
//		CString GetCueBanner() const;
//
//		BOOL ShowBalloonTip(_In_z_ LPCWSTR lpszTitle, _In_z_ LPCWSTR lpszText, _In_ INT ttiIcon = TTI_NONE);
//		BOOL ShowBalloonTip(_In_ PEDITBALLOONTIP pEditBalloonTip);
//		BOOL HideBalloonTip();
//#endif  // (UNICODE)
//
//#if defined(UNICODE)
//		// REVIEW: Sets the characters in the edit control that are highlighted.
//		void SetHighlight(_In_ int ichStart, _In_ int ichEnd);
//
//		// REVIEW: Retrieves the characters in the edit control that are highlighted.
//		BOOL GetHighlight(_Out_ int* pichStart, _Out_ int* pichEnd) const;
//#endif  // defined(UNICODE)
//
//		// Operations
//		void EmptyUndoBuffer();
//		BOOL FmtLines(BOOL bAddEOL);
//
//		void LimitText(int nChars = 0);
//		int LineFromChar(int nIndex = -1) const;
//		int LineIndex(int nLine = -1) const;
//		int LineLength(int nLine = -1) const;
//		void LineScroll(int nLines, int nChars = 0);
//		void ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo = FALSE);
//		void SetPasswordChar(TCHAR ch);
//		void SetRect(LPCRECT lpRect);
//		void SetRectNP(LPCRECT lpRect);
//		void SetSel(DWORD dwSelection, BOOL bNoScroll = FALSE);
//		void SetSel(int nStartChar, int nEndChar, BOOL bNoScroll = FALSE);
//		BOOL SetTabStops(int nTabStops, LPINT rgTabStops);
//		void SetTabStops();
//		BOOL SetTabStops(const int& cxEachStop);    // takes an 'int'
//
//													// Clipboard operations
//		BOOL Undo();
//		void Clear();
//		void Copy();
//		void Cut();
//		void Paste();
//
//		BOOL SetReadOnly(BOOL bReadOnly = TRUE);
//		int GetFirstVisibleLine() const;
//		TCHAR GetPasswordChar() const;

		// Implementation
	public:
		// virtual OK here - ~CWnd already virtual
		//virtual ~CEdit();
		inline CEdit::CEdit()
		{ }
		inline BOOL CEdit::CanUndo() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, EM_CANUNDO, 0, 0);
		}
		inline int CEdit::GetLineCount() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, EM_GETLINECOUNT, 0, 0);
		}
		inline BOOL CEdit::GetModify() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, EM_GETMODIFY, 0, 0);
		}
		inline void CEdit::SetModify(BOOL bModified)
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_SETMODIFY, bModified, 0);
		}
		inline void CEdit::GetRect(LPRECT lpRect) const
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_GETRECT, 0, (LPARAM)lpRect);
		}
		inline void CEdit::GetSel(int& nStartChar, int& nEndChar) const
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_GETSEL, (WPARAM)&nStartChar, (LPARAM)&nEndChar);
		}
		inline DWORD CEdit::GetSel() const
		{
			ASSERT(::IsWindow(m_hWnd)); return DWORD(::SendMessage(m_hWnd, EM_GETSEL, 0, 0));
		}
		inline HLOCAL CEdit::GetHandle() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (HLOCAL)::SendMessage(m_hWnd, EM_GETHANDLE, 0, 0);
		}
		inline void CEdit::SetHandle(HLOCAL hBuffer)
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_SETHANDLE, (WPARAM)hBuffer, 0);
		}
#pragma warning(push)
#pragma warning(disable: 6001 6054)
		inline int CEdit::GetLine(_In_ int nIndex, _Out_ LPTSTR lpszBuffer) const
		{
			ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
		}
#pragma warning(pop)
		inline int CEdit::GetLine(_In_ int nIndex, _Out_writes_to_(nMaxLength, return) LPTSTR lpszBuffer, _In_ int nMaxLength) const
		{
			ASSERT(::IsWindow(m_hWnd));
			*(LPWORD)lpszBuffer = (WORD)nMaxLength;
			return (int)::SendMessage(m_hWnd, EM_GETLINE, nIndex, (LPARAM)lpszBuffer);
		}
		inline void CEdit::EmptyUndoBuffer()
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_EMPTYUNDOBUFFER, 0, 0);
		}
		inline BOOL CEdit::FmtLines(BOOL bAddEOL)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, EM_FMTLINES, bAddEOL, 0);
		}
		inline void CEdit::LimitText(int nChars)
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_LIMITTEXT, nChars, 0);
		}
		inline int CEdit::LineFromChar(int nIndex) const
		{
			ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, EM_LINEFROMCHAR, nIndex, 0);
		}
		inline int CEdit::LineIndex(int nLine) const
		{
			ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, EM_LINEINDEX, nLine, 0);
		}
		inline int CEdit::LineLength(int nLine) const
		{
			ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, EM_LINELENGTH, nLine, 0);
		}
		inline void CEdit::LineScroll(int nLines, int nChars)
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_LINESCROLL, nChars, nLines);
		}
		inline void CEdit::ReplaceSel(LPCTSTR lpszNewText, BOOL bCanUndo)
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_REPLACESEL, (WPARAM)bCanUndo, (LPARAM)lpszNewText);
		}
		inline void CEdit::SetPasswordChar(TCHAR ch)
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_SETPASSWORDCHAR, ch, 0);
		}
		inline void CEdit::SetRect(LPCRECT lpRect)
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_SETRECT, 0, (LPARAM)lpRect);
		}
		inline void CEdit::SetRectNP(LPCRECT lpRect)
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_SETRECTNP, 0, (LPARAM)lpRect);
		}
		inline void CEdit::SetSel(DWORD dwSelection, BOOL bNoScroll)
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_SETSEL,
				LOWORD(dwSelection), HIWORD(dwSelection));
			if (!bNoScroll)
				::SendMessage(m_hWnd, EM_SCROLLCARET, 0, 0);
		}
		inline void CEdit::SetSel(int nStartChar, int nEndChar, BOOL bNoScroll)
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_SETSEL, nStartChar, nEndChar);
			if (!bNoScroll)
				::SendMessage(m_hWnd, EM_SCROLLCARET, 0, 0);
		}
		inline BOOL CEdit::SetTabStops(int nTabStops, LPINT rgTabStops)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, EM_SETTABSTOPS, nTabStops,
				(LPARAM)rgTabStops);
		}
		inline void CEdit::SetTabStops()
		{
			ASSERT(::IsWindow(m_hWnd)); VERIFY(::SendMessage(m_hWnd, EM_SETTABSTOPS, 0, 0));
		}
		inline BOOL CEdit::SetTabStops(const int& cxEachStop)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, EM_SETTABSTOPS,
				1, (LPARAM)(LPINT)&cxEachStop);
		}
		inline BOOL CEdit::Undo()
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, EM_UNDO, 0, 0);
		}
		inline void CEdit::Clear()
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, WM_CLEAR, 0, 0);
		}
		inline void CEdit::Copy()
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, WM_COPY, 0, 0);
		}
		inline void CEdit::Cut()
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, WM_CUT, 0, 0);
		}
		inline void CEdit::Paste()
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, WM_PASTE, 0, 0);
		}
		inline BOOL CEdit::SetReadOnly(BOOL bReadOnly)
		{
			ASSERT(::IsWindow(m_hWnd)); return (BOOL)::SendMessage(m_hWnd, EM_SETREADONLY, bReadOnly, 0L);
		}
		inline int CEdit::GetFirstVisibleLine() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, EM_GETFIRSTVISIBLELINE, 0, 0L);
		}
		inline TCHAR CEdit::GetPasswordChar() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (TCHAR)::SendMessage(m_hWnd, EM_GETPASSWORDCHAR, 0, 0L);
		}
		inline void CEdit::SetMargins(UINT nLeft, UINT nRight)
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_SETMARGINS, EC_LEFTMARGIN | EC_RIGHTMARGIN, MAKELONG(nLeft, nRight));
		}
		inline DWORD CEdit::GetMargins() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (DWORD)::SendMessage(m_hWnd, EM_GETMARGINS, 0, 0);
		}
		inline void CEdit::SetLimitText(UINT nMax)
		{
			ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, EM_SETLIMITTEXT, nMax, 0);
		}
		inline UINT CEdit::GetLimitText() const
		{
			ASSERT(::IsWindow(m_hWnd)); return (UINT)::SendMessage(m_hWnd, EM_GETLIMITTEXT, 0, 0);
		}
		inline CPoint CEdit::PosFromChar(UINT nChar) const
		{
			ASSERT(::IsWindow(m_hWnd)); return CPoint((DWORD)::SendMessage(m_hWnd, EM_POSFROMCHAR, nChar, 0));
		}
		inline int CEdit::CharFromPos(CPoint pt) const
		{
			ASSERT(::IsWindow(m_hWnd)); return (int)::SendMessage(m_hWnd, EM_CHARFROMPOS, 0, MAKELPARAM(pt.x, pt.y));
		}

	};


	//inline CScrollBar::CScrollBar()
	//{ }
	//inline int CScrollBar::GetScrollPos() const
	//{
	//	ASSERT(::IsWindow(m_hWnd)); return ::GetScrollPos(m_hWnd, SB_CTL);
	//}
	//inline int CScrollBar::SetScrollPos(int nPos, BOOL bRedraw)
	//{
	//	ASSERT(::IsWindow(m_hWnd)); return ::SetScrollPos(m_hWnd, SB_CTL, nPos, bRedraw);
	//}
	//inline void CScrollBar::GetScrollRange(LPINT lpMinPos, LPINT lpMaxPos) const
	//{
	//	ASSERT(::IsWindow(m_hWnd)); ::GetScrollRange(m_hWnd, SB_CTL, lpMinPos, lpMaxPos);
	//}
	//inline void CScrollBar::SetScrollRange(int nMinPos, int nMaxPos, BOOL bRedraw)
	//{
	//	ASSERT(::IsWindow(m_hWnd)); ::SetScrollRange(m_hWnd, SB_CTL, nMinPos, nMaxPos, bRedraw);
	//}
	//inline void CScrollBar::ShowScrollBar(BOOL bShow)
	//{
	//	ASSERT(::IsWindow(m_hWnd)); ::ShowScrollBar(m_hWnd, SB_CTL, bShow);
	//}
	//inline BOOL CScrollBar::EnableScrollBar(UINT nArrowFlags)
	//{
	//	ASSERT(::IsWindow(m_hWnd)); return ::EnableScrollBar(m_hWnd, SB_CTL, nArrowFlags);
	//}
	//inline BOOL CScrollBar::SetScrollInfo(LPSCROLLINFO lpScrollInfo, BOOL bRedraw)
	//{
	//	return CWnd::SetScrollInfo(SB_CTL, lpScrollInfo, bRedraw);
	//}
	//inline BOOL CScrollBar::GetScrollInfo(LPSCROLLINFO lpScrollInfo, UINT nMask)
	//{
	//	return CWnd::GetScrollInfo(SB_CTL, lpScrollInfo, nMask);
	//}
	//inline int CScrollBar::GetScrollLimit()
	//{
	//	return CWnd::GetScrollLimit(SB_CTL);
	//}


	// MDI functions
//	inline void CMDIFrameWnd::MDIActivate(CWnd* pWndActivate)
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWndMDIClient, WM_MDIACTIVATE,
//			(WPARAM)pWndActivate->m_hWnd, 0);
//	}
//	inline void CMDIFrameWnd::MDIIconArrange()
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWndMDIClient, WM_MDIICONARRANGE, 0, 0);
//	}
//	inline void CMDIFrameWnd::MDIMaximize(CWnd* pWnd)
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWndMDIClient, WM_MDIMAXIMIZE, (WPARAM)pWnd->m_hWnd, 0);
//	}
//	inline void CMDIFrameWnd::MDIPrev()
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWndMDIClient, WM_MDINEXT, 0, 1);
//	}
//	inline void CMDIFrameWnd::MDINext()
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWndMDIClient, WM_MDINEXT, 0, 0);
//	}
//	inline void CMDIFrameWnd::MDIRestore(CWnd* pWnd)
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWndMDIClient, WM_MDIRESTORE, (WPARAM)pWnd->m_hWnd, 0);
//	}
//	inline CMenu* CMDIFrameWnd::MDISetMenu(CMenu* pFrameMenu, CMenu* pWindowMenu)
//	{
//		ASSERT(::IsWindow(m_hWnd)); return CMenu::FromHandle((HMENU)::SendMessage(
//			m_hWndMDIClient, WM_MDISETMENU, (WPARAM)pFrameMenu->GetSafeHmenu(),
//			(LPARAM)pWindowMenu->GetSafeHmenu()));
//	}
//	inline void CMDIFrameWnd::MDITile()
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWndMDIClient, WM_MDITILE, 0, 0);
//	}
//	inline void CMDIFrameWnd::MDICascade()
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWndMDIClient, WM_MDICASCADE, 0, 0);
//	}
//
//	inline void CMDIFrameWnd::MDICascade(int nType)
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWndMDIClient, WM_MDICASCADE, nType, 0);
//	}
//	inline void CMDIFrameWnd::MDITile(int nType)
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWndMDIClient, WM_MDITILE, nType, 0);
//	}
//	inline void CMDIChildWnd::MDIDestroy()
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(GetParent()->m_hWnd, WM_MDIDESTROY, (WPARAM)m_hWnd, 0L);
//	}
//	inline void CMDIChildWnd::MDIActivate()
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(GetParent()->m_hWnd, WM_MDIACTIVATE, (WPARAM)m_hWnd, 0L);
//	}
//	inline void CMDIChildWnd::MDIMaximize()
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(GetParent()->m_hWnd, WM_MDIMAXIMIZE, (WPARAM)m_hWnd, 0L);
//	}
//	inline void CMDIChildWnd::MDIRestore()
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(GetParent()->m_hWnd, WM_MDIRESTORE, (WPARAM)m_hWnd, 0L);
//	}
//
//	// CView
//	inline CDocument* CView::GetDocument() const
//	{
//		ASSERT(this != NULL); return m_pDocument;
//	}
//	inline CSize CScrollView::GetTotalSize() const
//	{
//		ASSERT(this != NULL); return m_totalLog;
//	}
//
//	// CDocument
//	inline const CString& CDocument::GetTitle() const
//	{
//		ASSERT(this != NULL); return m_strTitle;
//	}
//	inline const CString& CDocument::GetPathName() const
//	{
//		ASSERT(this != NULL); return m_strPathName;
//	}
//	inline CDocTemplate* CDocument::GetDocTemplate() const
//	{
//		ASSERT(this != NULL); return m_pDocTemplate;
//	}
//	inline BOOL CDocument::IsModified()
//	{
//		ASSERT(this != NULL); return m_bModified;
//	}
//	inline void CDocument::SetModifiedFlag(BOOL bModified)
//	{
//		ASSERT(this != NULL); m_bModified = bModified;
//	}
//
//	// CWinThread
//	inline CWinThread::operator HANDLE() const
//	{
//		return this == NULL ? NULL : m_hThread;
//	}
//	inline BOOL CWinThread::SetThreadPriority(int nPriority)
//	{
//		ASSERT(m_hThread != NULL); return ::SetThreadPriority(m_hThread, nPriority);
//	}
//	inline int CWinThread::GetThreadPriority()
//	{
//		ASSERT(m_hThread != NULL); return ::GetThreadPriority(m_hThread);
//	}
//	inline DWORD CWinThread::ResumeThread()
//	{
//		ASSERT(m_hThread != NULL); return ::ResumeThread(m_hThread);
//	}
//	inline DWORD CWinThread::SuspendThread()
//	{
//		ASSERT(m_hThread != NULL); return ::SuspendThread(m_hThread);
//	}
//	inline BOOL CWinThread::PostThreadMessage(UINT message, WPARAM wParam, LPARAM lParam)
//	{
//		ASSERT(m_hThread != NULL); return ::PostThreadMessage(m_nThreadID, message, wParam, lParam);
//	}
//
//	// CWinApp
//	inline HCURSOR CWinApp::LoadCursor(LPCTSTR lpszResourceName) const
//	{
//		return ::LoadCursor(AfxFindResourceHandle(lpszResourceName,
//			ATL_RT_GROUP_CURSOR), lpszResourceName);
//	}
//	inline HCURSOR CWinApp::LoadCursor(UINT nIDResource) const
//	{
//		return ::LoadCursorW(AfxFindResourceHandle(ATL_MAKEINTRESOURCE(nIDResource),
//			ATL_RT_GROUP_CURSOR), ATL_MAKEINTRESOURCEW(nIDResource));
//	}
//	inline HCURSOR CWinApp::LoadStandardCursor(LPCTSTR lpszCursorName) const
//	{
//		return ::LoadCursor(NULL, lpszCursorName);
//	}
//	inline HCURSOR CWinApp::LoadOEMCursor(UINT nIDCursor) const
//	{
//		return ::LoadCursorW(NULL, ATL_MAKEINTRESOURCEW(nIDCursor));
//	}
//	inline HICON CWinApp::LoadIcon(LPCTSTR lpszResourceName) const
//	{
//		return ::LoadIcon(AfxFindResourceHandle(lpszResourceName,
//			ATL_RT_GROUP_ICON), lpszResourceName);
//	}
//	inline HICON CWinApp::LoadIcon(UINT nIDResource) const
//	{
//		return ::LoadIconW(AfxFindResourceHandle(ATL_MAKEINTRESOURCE(nIDResource),
//			ATL_RT_GROUP_ICON), ATL_MAKEINTRESOURCEW(nIDResource));
//	}
//	inline HICON CWinApp::LoadStandardIcon(LPCTSTR lpszIconName) const
//	{
//		return ::LoadIcon(NULL, lpszIconName);
//	}
//	inline HICON CWinApp::LoadOEMIcon(UINT nIDIcon) const
//	{
//		return ::LoadIconW(NULL, ATL_MAKEINTRESOURCEW(nIDIcon));
//	}
//	inline void CWinApp::EnableHtmlHelp()
//	{
//		SetHelpMode(afxHTMLHelp);
//	}
//
//	inline AFX_HELP_TYPE CWinApp::GetHelpMode()
//	{
//		return m_eHelpType;
//	}
//
//	inline void CWinApp::SetHelpMode(AFX_HELP_TYPE eHelpType)
//	{
//		ASSERT(eHelpType == afxHTMLHelp || eHelpType == afxWinHelp);
//		m_eHelpType = eHelpType;
//	}
//
//#pragma warning(push)
//#pragma warning(disable: 4996)
//	inline BOOL CWinApp::Enable3dControls()
//	{
//		return TRUE;
//	}
//#ifndef _AFXDLL
//	inline BOOL CWinApp::Enable3dControlsStatic()
//	{
//		return TRUE;
//	}
//#endif
//	inline void CWinApp::SetDialogBkColor(COLORREF /*clrCtlBk*/, COLORREF /*clrCtlText*/)
//	{
//	}
//#pragma warning(pop)
//
//	inline CWaitCursor::CWaitCursor()
//	{
//		AfxGetApp()->BeginWaitCursor();
//	}
//#pragma warning(push)
//#pragma warning(disable:6271 6273)
//	inline CWaitCursor::~CWaitCursor()
//	{
//		AFX_BEGIN_DESTRUCTOR
//			AfxGetApp()->EndWaitCursor();
//		AFX_END_DESTRUCTOR
//	}
//#pragma warning(pop)
//	inline void CWaitCursor::Restore()
//	{
//		AfxGetApp()->RestoreWaitCursor();
//	}
//
//	/////////////////////////////////////////////////////////////////////////////
//	// Obsolete and non-portable
//
//	inline void CWnd::CloseWindow()
//	{
//		ASSERT(::IsWindow(m_hWnd)); ::CloseWindow(m_hWnd);
//	}
//	inline BOOL CWnd::OpenIcon()
//	{
//		ASSERT(::IsWindow(m_hWnd)); return ::OpenIcon(m_hWnd);
//	}
//
//	/////////////////////////////////////////////////////////////////////////////
#pragma endregion


class CDateTimeCtrl : public CWnd
{
	//DECLARE_DYNAMIC(CDateTimeCtrl)

public:
	// Constructors
//	CDateTimeCtrl();
//	virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);
//
//	// Attributes
//	// Retrieves the color for the specified portion of the calendar within the datetime picker control.
//	COLORREF GetMonthCalColor(_In_ int iColor) const;
//
//	// Sets the color for the specified portion of the calendar within the datetime picker control.
//	COLORREF SetMonthCalColor(_In_ int iColor, _In_ COLORREF ref);
//
//	// Sets the display of the datetime picker control based on the specified format string.
//	BOOL SetFormat(_In_z_ LPCTSTR pstrFormat);
//
//	// Retrieves the datetime picker's child calendar control.
//	CMonthCalCtrl* GetMonthCalCtrl() const;
//
//	// Retrieves the font of the datetime picker control's child calendar control.
//	CFont* GetMonthCalFont() const;
//
//	// Sets the font of the datetime picker control's child calendar control.
//	void SetMonthCalFont(_In_ HFONT hFont, _In_ BOOL bRedraw = TRUE);
//
//	// Sets the minimum and maximum allowable times for the datetime picker control.
//	BOOL SetRange(_In_ const COleDateTime* pMinRange, _In_ const COleDateTime* pMaxRange);
//
//	// Retrieves the current minimum and maximum allowable times for the datetime picker control.
//	DWORD GetRange(_Out_ COleDateTime* pMinRange, _Out_ COleDateTime* pMaxRange) const;
//
//	// Sets the minimum and maximum allowable times for the datetime picker control.
//	BOOL SetRange(_In_ const CTime* pMinRange, _In_ const CTime* pMaxRange);
//
//	// Retrieves the current minimum and maximum allowable times for the datetime picker control.
//	DWORD GetRange(_Out_ CTime* pMinRange, _Out_ CTime* pMaxRange) const;
//
//#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
//	// REVIEW: Sets the style of the datetime picker control's child calendar control.
//	DWORD SetMonthCalStyle(_In_ DWORD dwStyle);
//
//	// REVIEW: Retrieves the style of the datetime picker control's child calendar control.
//	DWORD GetMonthCalStyle() const;
//
//	// Retrieves information from the datetime picker control.
//	BOOL GetDateTimePickerInfo(_Out_ LPDATETIMEPICKERINFO pDateTimePickerInfo) const;
//
//	// Retrieves the ideal size for the control (so that all the text fits).
//	BOOL GetIdealSize(_Out_ LPSIZE pSize) const;
//#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
//
//	// Operations
//	// Sets the time in the datetime picker control.
//	BOOL SetTime(_In_ const CTime* pTimeNew);
//
//	// Retrieves the currently selected time from the datetime picker control.
//	DWORD GetTime(_Out_ CTime& timeDest) const;
//
//	// Sets the time in the datetime picker control.
//	BOOL SetTime(_In_ const COleDateTime& timeNew);
//
//	// Retrieves the currently selected time from the datetime picker control.
//	BOOL GetTime(_Out_ COleDateTime& timeDest) const;
//
//	// Sets the time in the datetime picker control.
	BOOL SetTime(_In_ LPSYSTEMTIME pTimeNew = NULL);
//
//	// Retrieves the currently selected time from the datetime picker control.
//	DWORD GetTime(_Out_ LPSYSTEMTIME pTimeDest) const;
//
//#if defined(UNICODE)
//	// REVIEW: Closes the datetime picker control.
//	void CloseMonthCal();
//#endif // defined(UNICODE)

	// Overridables
//	virtual ~CDateTimeCtrl();
	inline CDateTimeCtrl::CDateTimeCtrl()
	{}

	//inline CFont* CDateTimeCtrl::GetMonthCalFont() const
	//{
	//	ASSERT(::IsWindow(m_hWnd)); return CFont::FromHandle((HFONT) ::SendMessage(m_hWnd, DTM_GETMCFONT, 0, 0));
	//}
	//inline CMonthCalCtrl* CDateTimeCtrl::GetMonthCalCtrl() const
	//{
	//	ASSERT(::IsWindow(m_hWnd)); return (CMonthCalCtrl*)CWnd::FromHandle((HWND) ::SendMessage(m_hWnd, DTM_GETMONTHCAL, 0, 0));
	//}
	inline void CDateTimeCtrl::SetMonthCalFont(_In_ HFONT hFont, _In_ BOOL bRedraw /* = TRUE */)
	{
		ASSERT(::IsWindow(m_hWnd)); ::SendMessage(m_hWnd, DTM_SETMCFONT, (WPARAM)hFont, MAKELONG(bRedraw, 0));
	}
	inline COLORREF CDateTimeCtrl::SetMonthCalColor(_In_ int iColor, _In_ COLORREF ref)
	{
		ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, DTM_SETMCCOLOR, (WPARAM)iColor, (LPARAM)ref);
	}
	inline DWORD CDateTimeCtrl::GetTime(_Out_ LPSYSTEMTIME pTimeDest) const
	{
		ASSERT(::IsWindow(m_hWnd)); ASSERT(pTimeDest != NULL); return (DWORD) ::SendMessage(m_hWnd, DTM_GETSYSTEMTIME, 0, (LPARAM)pTimeDest);
	}
	inline COLORREF CDateTimeCtrl::GetMonthCalColor(_In_ int iColor) const
	{
		ASSERT(::IsWindow(m_hWnd)); return (COLORREF) ::SendMessage(m_hWnd, DTM_GETMCCOLOR, (WPARAM)iColor, 0);
	}
	inline BOOL CDateTimeCtrl::SetFormat(_In_z_ LPCTSTR pstrFormat)
	{
		ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, DTM_SETFORMAT, 0, (LPARAM)pstrFormat);
	}
#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
	inline DWORD CDateTimeCtrl::SetMonthCalStyle(_In_ DWORD dwStyle)
	{
		ASSERT(::IsWindow(m_hWnd)); return (DWORD)DateTime_SetMonthCalStyle(m_hWnd, dwStyle);
	} // DTM_SETMCSTYLE
	inline DWORD CDateTimeCtrl::GetMonthCalStyle() const
	{
		ASSERT(::IsWindow(m_hWnd)); return (DWORD)DateTime_GetMonthCalStyle(m_hWnd);
	} // DTM_GETMCSTYLE
	inline BOOL CDateTimeCtrl::GetDateTimePickerInfo(_Out_ LPDATETIMEPICKERINFO pDateTimePickerInfo) const
	{
		ASSERT(::IsWindow(m_hWnd));
		ASSERT(pDateTimePickerInfo != NULL);
		if (pDateTimePickerInfo == NULL)
			return FALSE;
		pDateTimePickerInfo->cbSize = sizeof(DATETIMEPICKERINFO);
		return (BOOL)DateTime_GetDateTimePickerInfo(m_hWnd, pDateTimePickerInfo); // DTM_GETDATETIMEPICKERINFO
	}
	inline BOOL CDateTimeCtrl::GetIdealSize(_Out_ LPSIZE pSize) const
	{
		ASSERT(::IsWindow(m_hWnd)); return DateTime_GetIdealSize(m_hWnd, pSize);
	} // DTM_GETIDEALSIZE
	inline void CDateTimeCtrl::CloseMonthCal()
	{
		ASSERT(::IsWindow(m_hWnd)); DateTime_CloseMonthCal(m_hWnd);
	} // DTM_CLOSEMONTHCAL
#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

};

/*============================================================================*/
// CMonthCalCtrl
#pragma region
class CMonthCalCtrl : public CWnd
{
	//DECLARE_DYNAMIC(CMonthCalCtrl)

public:
	// Constructors
//	CMonthCalCtrl();
//	virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);
//	virtual BOOL Create(_In_ DWORD dwStyle, _In_ const POINT& pt, _In_ CWnd* pParentWnd, _In_ UINT nID);
//
//	//Attributes
//	// Retrieves the minimum size required to display a full month in the calendar control.
//	BOOL GetMinReqRect(_Out_ RECT* pRect) const;
//
//	// Retrieves the scroll rate for the calendar control.
//	int SetMonthDelta(_In_ int iDelta);
//
//	// Sets the scroll rate for the calendar control.
//	int GetMonthDelta() const;
//
//	// Sets the first day of the week for the calendar control.
//	BOOL SetFirstDayOfWeek(_In_ int iDay, _Out_ int* lpnOld = NULL);
//
//	// Retrieves the first day of the week for the calendar control.
//	int GetFirstDayOfWeek(_Out_ BOOL* pbLocal = NULL) const;
//
//	// Retrieves the color for the specified portion of the calendar control.
//	COLORREF GetColor(_In_ int nRegion) const;
//
//	// Sets the color for the specified portion of the calendar control.
//	COLORREF SetColor(_In_ int nRegion, _In_ COLORREF ref);
//
//	// Determines which portion of the calendar control is at a given point on the screen.
//	DWORD HitTest(_In_ PMCHITTESTINFO pMCHitTest);
//
//	// Retrieves the maximum width of the "today" string in the calendar control, in pixels.
//	DWORD GetMaxTodayWidth() const;
//
//#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
//	// Determines whether the calendar control is in month view.
//	BOOL IsMonthView() const; // REVIEW: do we need this method?
//
//							  // Determines whether the calendar control is in year view.
//	BOOL IsYearView() const; // REVIEW: do we need this method?
//
//							 // Determines whether the calendar control is in decade view.
//	BOOL IsDecadeView() const; // REVIEW: do we need this method?
//
//							   // Determines whether the calendar control is in century view.
//	BOOL IsCenturyView() const; // REVIEW: do we need this method?
//
//								// REVIEW: Retrieves the current view of the calendar control.
//	DWORD GetCurrentView() const;
//
//	// REVIEW: Retrieves the number of calendars currently displayed in the calendar control.
//	int GetCalendarCount() const;
//
//	// REVIEW: Retrieves information about the calendar grid.
//	BOOL GetCalendarGridInfo(_Out_ PMCGRIDINFO pmcGridInfo) const;
//
//	// REVIEW: Retrieves the calendar ID for the calendar control.
//	CALID GetCalID() const;
//
//	// REVIEW: Sets the calendar ID for the calendar control.
//	BOOL SetCalID(_In_ CALID calid);
//
//	// Calculates how many calendars will fit in the given rectangle, and
//	// returns the minimum rectangle that fits that number of calendars.
//	void SizeRectToMin(_Inout_ LPRECT lpRect);
//
//	// REVIEW: Sets the size of the calendar control border, in pixels.
//	void SetCalendarBorder(_In_ int cxyBorder);
//
//	// REVIEW: Resets the size of the calendar control border to the default.
//	void SetCalendarBorderDefault();
//
//	// REVIEW: Retrieves the size of the calendar control border, in pixels.
//	int GetCalendarBorder() const;
//
//	// Puts the calendar control in month view.
//	BOOL SetMonthView(); // REVIEW: do we need this method?
//
//						 // Puts the calendar control in year view.
//	BOOL SetYearView(); // REVIEW: do we need this method?
//
//						// Puts the calendar control in decade view.
//	BOOL SetDecadeView(); // REVIEW: do we need this method?
//
//						  // Puts the calendar control in century view.
//	BOOL SetCenturyView(); // REVIEW: do we need this method?
//
//						   // REVIEW: Sets the current view of the calendar control.
//	BOOL SetCurrentView(_In_ DWORD dwNewView);
//
//#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
//
//	// Operations
//	// Sizes the calendar control to the minimum size that fits a full month.
//	BOOL SizeMinReq(_In_ BOOL bRepaint = TRUE);
//
//	// Sets the "today" selection for the calendar control.
//	void SetToday(_In_ const COleDateTime& refDateTime);
//
//	// Sets the "today" selection for the calendar control.
//	void SetToday(_In_ const CTime* pDateTime);
//
//	// Sets the "today" selection for the calendar control.
//	void SetToday(_In_ const LPSYSTEMTIME pDateTime);
//
//	// Retrieves the date information for the date specified as "today" for the calendar control.
//	BOOL GetToday(_Out_ CTime& refTime) const;
//
//	// Retrieves the date information for the date specified as "today" for the calendar control.
//	BOOL GetToday(_Out_ COleDateTime& refDateTime) const;
//
//	// Retrieves the date information for the date specified as "today" for the calendar control.
//	BOOL GetToday(_Out_ LPSYSTEMTIME pDateTime) const;
//
//	// Retrieves the currently selected date in the calendar control.
//	BOOL GetCurSel(_Out_ LPSYSTEMTIME pDateTime) const;
//
//	// Sets the currently selected date in the calendar control.
//	BOOL SetCurSel(_In_ const LPSYSTEMTIME pDateTime);
//
//	// Sets the currently selected date in the calendar control.
//	BOOL SetCurSel(_In_ const CTime& refDateTime);
//
//	// Retrieves the currently selected date in the calendar control.
//	BOOL GetCurSel(_Out_ CTime& refDateTime) const;
//
//	// Sets the currently selected date in the calendar control.
//	BOOL SetCurSel(_In_ const COleDateTime& refDateTime);
//
//	// Retrieves the currently selected date in the calendar control.
//	BOOL GetCurSel(_Out_ COleDateTime& refDateTime) const;
//
//	// Sets the day states for all months that are currently visible in the calendar control.
//	BOOL SetDayState(_In_ int nMonths, _In_ LPMONTHDAYSTATE pStates);
//
//	// Sets the maximum date range that can be selected in the calendar control.
//	BOOL SetMaxSelCount(_In_ int nMax);
//
//	// Retrieves the maximum date range that can be selected in the calendar control.
//	int GetMaxSelCount() const;
//
//	// Sets the minimum and maximum allowable dates for the calendar control.
//	BOOL SetRange(_In_ const COleDateTime* pMinTime, _In_ const COleDateTime* pMaxTime);
//
//	// Retrieves the minimum and maximum allowable dates set for the calendar control.
//	DWORD GetRange(_Out_ COleDateTime* pMinTime, _Out_ COleDateTime* pMaxTime) const;
//
//	// Sets the minimum and maximum allowable dates for the calendar control.
//	BOOL SetRange(_In_ const CTime* pMinTime, _In_ const CTime* pMaxTime);
//
//	// Retrieves the minimum and maximum allowable dates set for the calendar control.
//	DWORD GetRange(_Out_ CTime* pMinTime, _Out_ CTime* pMaxTime) const;
//
//	// Sets the minimum and maximum allowable dates for the calendar control.
//	BOOL SetRange(_In_ const LPSYSTEMTIME pMinRange, _In_ const LPSYSTEMTIME pMaxRange);
//
//	// Retrieves the minimum and maximum allowable dates set for the calendar control.
//	DWORD GetRange(_Out_ LPSYSTEMTIME pMinRange, _Out_ LPSYSTEMTIME pMaxRange) const;
//
//	// Retrieves date information that represents the limits of the calendar control's display.
//	int GetMonthRange(_Out_ COleDateTime& refMinRange, _Out_ COleDateTime& refMaxRange, _In_ DWORD dwFlags) const;
//
//	// Retrieves date information that represents the limits of the calendar control's display.
//	int GetMonthRange(_Out_ CTime& refMinRange, _Out_ CTime& refMaxRange, _In_ DWORD dwFlags) const;
//
//	// Retrieves date information that represents the limits of the calendar control's display.
//	int GetMonthRange(_Out_ LPSYSTEMTIME pMinRange, _Out_ LPSYSTEMTIME pMaxRange, _In_ DWORD dwFlags) const;
//
//	// Sets the selection for the calendar control to the specified date range.
//	BOOL SetSelRange(_In_ const COleDateTime& pMinRange, _In_ const COleDateTime& pMaxRange);
//
//	// Retrieves date information that represents the date range currently selected in the calendar control.
//	BOOL GetSelRange(_Out_ COleDateTime& refMinRange, _Out_ COleDateTime& refMaxRange) const;
//
//	// Sets the selection for the calendar control to the specified date range.
//	BOOL SetSelRange(_In_ const CTime& pMinRange, _In_ const CTime& pMaxRange);
//
//	// Retrieves date information that represents the date range currently selected in the calendar control.
//	BOOL GetSelRange(_Out_ CTime& refMinRange, _Out_ CTime& refMaxRange) const;
//
//	// Retrieves date information that represents the date range currently selected in the calendar control.
//	BOOL GetSelRange(_Out_ LPSYSTEMTIME pMinRange, _Out_ LPSYSTEMTIME pMaxRange) const;
//
//	// Sets the selection for the calendar control to the specified date range.
//	BOOL SetSelRange(_In_ const LPSYSTEMTIME pMinRange, _In_ const LPSYSTEMTIME pMaxRange);
//
//	// Overridables
//	virtual ~CMonthCalCtrl();
//

	  //CMonthCalCtrl
	inline CMonthCalCtrl::CMonthCalCtrl()
	{ }
	inline DWORD CMonthCalCtrl::HitTest(_In_ PMCHITTESTINFO pMCHitTest)
	{
		ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, MCM_HITTEST, 0, (LPARAM)pMCHitTest);
	}
	inline BOOL CMonthCalCtrl::GetMinReqRect(_Out_ RECT* pRect) const
	{
		ASSERT(m_hWnd != NULL); return (BOOL) ::SendMessage(m_hWnd, MCM_GETMINREQRECT, 0, (LPARAM)pRect);
	}
	inline int CMonthCalCtrl::SetMonthDelta(_In_ int iDelta)
	{
		ASSERT(m_hWnd != NULL); return (int) ::SendMessage(m_hWnd, MCM_SETMONTHDELTA, (WPARAM)iDelta, 0);
	}
	inline int CMonthCalCtrl::GetMonthDelta() const
	{
		ASSERT(m_hWnd != NULL); return (int) ::SendMessage(m_hWnd, MCM_GETMONTHDELTA, 0, 0);
	}
	inline COLORREF CMonthCalCtrl::GetColor(_In_ int nRegion) const
	{
		ASSERT(m_hWnd != NULL); return (COLORREF) ::SendMessage(m_hWnd, MCM_GETCOLOR, (WPARAM)nRegion, 0);
	}
	inline COLORREF CMonthCalCtrl::SetColor(_In_ int nRegion, _In_ COLORREF ref)
	{
		ASSERT(m_hWnd != NULL); return (COLORREF) ::SendMessage(m_hWnd, MCM_SETCOLOR, (WPARAM)nRegion, (LPARAM)ref);
	}
	inline BOOL CMonthCalCtrl::SetMaxSelCount(_In_ int nMax)
	{
		ASSERT(m_hWnd != NULL); return (BOOL) ::SendMessage(m_hWnd, MCM_SETMAXSELCOUNT, nMax, 0);
	}
	inline int CMonthCalCtrl::GetMaxSelCount() const
	{
		ASSERT(m_hWnd != NULL); return (int) ::SendMessage(m_hWnd, MCM_GETMAXSELCOUNT, 0, 0);
	}
	inline void CMonthCalCtrl::SetToday(_In_ const LPSYSTEMTIME pDateTime)
	{
		ASSERT(m_hWnd != NULL); ::SendMessage(m_hWnd, MCM_SETTODAY, 0, (LPARAM)pDateTime);
	}
	inline BOOL CMonthCalCtrl::GetToday(_Out_ LPSYSTEMTIME pDateTime) const
	{
		ASSERT(m_hWnd != NULL); return (BOOL) ::SendMessage(m_hWnd, MCM_GETTODAY, 0, (LPARAM)pDateTime);
	}
	inline BOOL CMonthCalCtrl::SetCurSel(_In_ const LPSYSTEMTIME pDateTime)
	{
		ASSERT(m_hWnd != NULL); return (BOOL) ::SendMessage(m_hWnd, MCM_SETCURSEL, 0, (LPARAM)pDateTime);
	}
	inline BOOL CMonthCalCtrl::GetCurSel(_Out_ LPSYSTEMTIME pDateTime) const
	{
		ASSERT(m_hWnd != NULL); BOOL bRetVal = (BOOL)::SendMessage(m_hWnd, MCM_GETCURSEL, 0, (LPARAM)pDateTime);
		pDateTime->wHour = pDateTime->wMinute = pDateTime->wSecond = pDateTime->wMilliseconds = 0; return bRetVal;
	}
	inline DWORD CMonthCalCtrl::GetMaxTodayWidth() const
	{
		ASSERT(m_hWnd != NULL); return MonthCal_GetMaxTodayWidth(m_hWnd);
	} // MCM_GETMAXTODAYWIDTH
#if (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)
	inline BOOL CMonthCalCtrl::IsMonthView() const // REVIEW: do we need this method?
	{
		ASSERT(m_hWnd != NULL); return (GetCurrentView() == MCMV_MONTH);
	}
	inline BOOL CMonthCalCtrl::IsYearView() const // REVIEW: do we need this method?
	{
		ASSERT(m_hWnd != NULL); return (GetCurrentView() == MCMV_YEAR);
	}
	inline BOOL CMonthCalCtrl::IsDecadeView() const // REVIEW: do we need this method?
	{
		ASSERT(m_hWnd != NULL); return (GetCurrentView() == MCMV_DECADE);
	}
	inline BOOL CMonthCalCtrl::IsCenturyView() const // REVIEW: do we need this method?
	{
		ASSERT(m_hWnd != NULL); return (GetCurrentView() == MCMV_CENTURY);
	}
	inline DWORD CMonthCalCtrl::GetCurrentView() const
	{
		ASSERT(m_hWnd != NULL); return MonthCal_GetCurrentView(m_hWnd);
	} // MCM_GETCURRENTVIEW
	inline int CMonthCalCtrl::GetCalendarCount() const
	{
		ASSERT(m_hWnd != NULL); return (int)MonthCal_GetCalendarCount(m_hWnd);
	} // MCM_GETCALENDARCOUNT
	inline BOOL CMonthCalCtrl::GetCalendarGridInfo(_Out_ PMCGRIDINFO pmcGridInfo) const
	{
		ASSERT(m_hWnd != NULL); return MonthCal_GetCalendarGridInfo(m_hWnd, pmcGridInfo);
	} // MCM_GETCALENDARGRIDINFO
	inline CALID CMonthCalCtrl::GetCalID() const
	{
		ASSERT(m_hWnd != NULL); return MonthCal_GetCALID(m_hWnd);
	} // MCM_GETCALID
	inline BOOL CMonthCalCtrl::SetCalID(_In_ CALID calid)
	{
		ASSERT(m_hWnd != NULL); return (BOOL)MonthCal_SetCALID(m_hWnd, calid);
	} // MCM_SETCALID
	inline void CMonthCalCtrl::SizeRectToMin(_Inout_ LPRECT lpRect)
	{
		ASSERT(m_hWnd != NULL); MonthCal_SizeRectToMin(m_hWnd, lpRect);
	} // MCM_SIZERECTTOMIN
	inline void CMonthCalCtrl::SetCalendarBorder(_In_ int cxyBorder)
	{
		ASSERT(m_hWnd != NULL); MonthCal_SetCalendarBorder(m_hWnd, TRUE, cxyBorder);
	} // MCM_SETCALENDARBORDER
	inline void CMonthCalCtrl::SetCalendarBorderDefault()
	{
		ASSERT(m_hWnd != NULL); MonthCal_SetCalendarBorder(m_hWnd, FALSE, 0);
	} // MCM_SETCALENDARBORDER
	inline int CMonthCalCtrl::GetCalendarBorder() const
	{
		ASSERT(m_hWnd != NULL); return MonthCal_GetCalendarBorder(m_hWnd);
	} // MCM_GETCALENDARBORDER
	inline BOOL CMonthCalCtrl::SetMonthView() // REVIEW: do we need this method?
	{
		ASSERT(m_hWnd != NULL); return (SetCurrentView(MCMV_MONTH));
	}
	inline BOOL CMonthCalCtrl::SetYearView() // REVIEW: do we need this method?
	{
		ASSERT(m_hWnd != NULL); return (SetCurrentView(MCMV_YEAR));
	}
	inline BOOL CMonthCalCtrl::SetDecadeView() // REVIEW: do we need this method?
	{
		ASSERT(m_hWnd != NULL); return (SetCurrentView(MCMV_DECADE));
	}
	inline BOOL CMonthCalCtrl::SetCenturyView() // REVIEW: do we need this method?
	{
		ASSERT(m_hWnd != NULL); return (SetCurrentView(MCMV_CENTURY));
	}
	inline BOOL CMonthCalCtrl::SetCurrentView(_In_ DWORD dwNewView)
	{
		ASSERT(m_hWnd != NULL); return MonthCal_SetCurrentView(m_hWnd, dwNewView);
	} // MCM_SETCURRENTVIEW
#endif // (NTDDI_VERSION >= NTDDI_VISTA) && defined(UNICODE)

};
#pragma endregion

#pragma region
/*============================================================================*/
// CComboBoxEx

class CComboBoxEx : public CComboBox
{

	// Constructors
public:
	//CComboBoxEx();

	//// Generic creator
	//virtual BOOL Create(_In_ DWORD dwStyle, _In_ const RECT& rect, _In_ CWnd* pParentWnd, _In_ UINT nID);

	//// Generic creator allowing extended style bits
	//virtual BOOL CreateEx(_In_ DWORD dwExStyle, _In_ DWORD dwStyle, _In_ const RECT& rect,
	//	_In_ CWnd* pParentWnd, _In_ UINT nID);

	//// Operations
	////using CComboBox::DeleteItem;
	int DeleteItem(_In_ int iIndex);
	BOOL GetItem(_Out_ COMBOBOXEXITEM* pCBItem);
	int InsertItem(_In_ const COMBOBOXEXITEM* pCBItem);
	BOOL SetItem(_In_ const COMBOBOXEXITEM* pCBItem);

	//// Attributes
	//BOOL HasEditChanged();
	//DWORD GetExtendedStyle() const;
	//DWORD SetExtendedStyle(_In_ DWORD dwExMask, _In_ DWORD dwExStyles);
	//CEdit* GetEditCtrl() const;
	//CComboBox* GetComboBoxCtrl() const;
	//CImageList* GetImageList() const;
	//CImageList* SetImageList(_In_ CImageList* pImageList);

	////AFX_ANSI_DEPRECATED HRESULT SetWindowTheme(_In_z_ LPCWSTR pszSubAppName);

	//// These functions are supported by the Windows ComboBox control,
	//// but not supported by the Windows ComboBoxEx control.

	//int Dir(_In_ UINT attr, _In_z_ LPCTSTR lpszWildCard);
	//int FindString(_In_ int nIndexStart, _In_z_ LPCTSTR lpszFind) const;
	//int AddString(_In_z_ LPCTSTR lpszString);
	//BOOL SetEditSel(_In_ int nStartChar, _In_ int nEndChar);
	//int InsertString(_In_ int nIndex, _In_z_ LPCTSTR lpszString);

	// Implementation
public:
	virtual ~CComboBoxEx();

	// While CComboBoxEx derives from CComboBox, there are some
	// CB_messages the underlying ComboBoxEx control doesn't support.

	inline int CComboBoxEx::Dir(_In_ UINT attr, _In_z_ LPCTSTR lpszWildCard)
	{
		//(attr); UNUSED_ALWAYS(lpszWildCard);
		ASSERT(FALSE); return CB_ERR;
	}
	inline int CComboBoxEx::FindString(_In_ int nIndexStart, _In_z_ LPCTSTR lpszFind) const
	{
		//UNUSED_ALWAYS(nIndexStart); UNUSED_ALWAYS(lpszFind);
		ASSERT(FALSE); return CB_ERR;
	}
	inline int CComboBoxEx::AddString(_In_z_ LPCTSTR lpszString)
	{
		//UNUSED_ALWAYS(lpszString); 
		ASSERT(FALSE); return CB_ERR;
	}
	inline BOOL CComboBoxEx::SetEditSel(_In_ int nStartChar, _In_ int nEndChar)
	{
		//UNUSED_ALWAYS(nStartChar); UNUSED_ALWAYS(nEndChar);
		ASSERT(FALSE); return FALSE;
	}
	inline int CComboBoxEx::InsertString(_In_ int nIndex, _In_z_ LPCTSTR lpszString)
	{
		//UNUSED_ALWAYS(nIndex); UNUSED_ALWAYS(lpszString);
		ASSERT(FALSE); return CB_ERR;
	}
	inline CComboBoxEx::CComboBoxEx()
	{ }
	inline DWORD CComboBoxEx::GetExtendedStyle() const
	{
		ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, CBEM_GETEXTENDEDSTYLE, 0, 0);
	}
	inline DWORD CComboBoxEx::SetExtendedStyle(_In_ DWORD dwExMask, _In_ DWORD dwExStyles)
	{
		ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, CBEM_SETEXTENDEDSTYLE, (DWORD)dwExMask, (LPARAM)dwExStyles);
	}
	inline BOOL CComboBoxEx::HasEditChanged()
	{
		ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, CBEM_HASEDITCHANGED, 0, 0);
	}
	//inline CEdit* CComboBoxEx::GetEditCtrl() const
	//{
	//	ASSERT(::IsWindow(m_hWnd)); return (CEdit*)CEdit::FromHandle((HWND) ::SendMessage(m_hWnd, CBEM_GETEDITCONTROL, 0, 0));
	//}
	//inline CComboBox* CComboBoxEx::GetComboBoxCtrl() const
	//{
	//	ASSERT(::IsWindow(m_hWnd)); return (CComboBox*)CComboBox::FromHandle((HWND) ::SendMessage(m_hWnd, CBEM_GETCOMBOCONTROL, 0, 0));
	//}
	inline HIMAGELIST CComboBoxEx::SetImageList(_In_ CImageList* pImageList)
	{
		ASSERT(::IsWindow(m_hWnd)); return (HIMAGELIST) ::SendMessage(m_hWnd, CBEM_SETIMAGELIST, 0, (LPARAM)pImageList->GetSafeHandle());
	}
	//inline CImageList* CComboBoxEx::GetImageList() const
	//{
	//	ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, CBEM_GETIMAGELIST, 0, 0));
	//}


};
#pragma endregion

}