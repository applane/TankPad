#include "pch.h"
#include "dibapi.h"
#include "bitmapdc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CBitmapDC::CBitmapDC(int nBitCount)
{
	Dispose();
	
	ASSERT(nBitCount >= 24);
	m_nBitCount = nBitCount;
}

BOOL CBitmapDC::Resize(long nWidth, long nHeight)
{
	if (m_nWidth == nWidth && m_nHeight == nHeight)
		return TRUE;

	Dispose();

	HDC hDC = NULL;

	if (nWidth > 0 &&
		nHeight > 0 &&
		(hDC = m_DibDC.Create(nWidth, nHeight, m_nBitCount)) != NULL)
	{
		m_nWidth = nWidth;
		m_nHeight = nHeight;
		m_hDC = hDC;
		return TRUE;
	}

	return FALSE;
}

void CBitmapDC::Fill(COLORREF cColor)
{
	if (m_hDC == NULL) return;

	CDC dc;

	dc.Attach(m_hDC);

	COLORREF bg = dc.GetBkColor();

	dc.FillSolidRect(0, 0, m_nWidth, m_nHeight, cColor);

	dc.SetBkColor(bg);

	dc.Detach();
}

void CBitmapDC::Fill(COLORREF color, BYTE alpha)
{
	if (m_hDC == NULL) return;

	if (m_nBitCount < 32)
		return;

	DWORD fillPixel = RGB(GetBValue(color),GetGValue(color),GetRValue(color)) 
							| alpha << 24;

	BYTE* startBits = (BYTE*)m_DibDC.GetBits();
	ASSERT(startBits);
		
	for(int i = 0; i < m_nHeight; i++)
	{
		int width = WIDTHBYTES(m_nWidth * (DWORD)32);

		DWORD* pixels = (DWORD*)((BYTE*)startBits + i * WIDTHBYTES(m_nWidth * (DWORD)32));
		
		for (int j = 0; j < m_nWidth; j++)
		{
			pixels[j] = fillPixel;
		}
	}
}

void CBitmapDC::FillTransparentAlphaBackround()
{
	if (m_hDC == NULL) return;

	if (m_nBitCount < 32)
		return;

	DWORD transparentPixel = RGB(0, 0, 255) | 255 << 24;

	BYTE* startBits = (BYTE*)m_DibDC.GetBits();
	ASSERT(startBits);

	for (int i = 0; i < m_nHeight; i++)
	{
		int width = WIDTHBYTES(m_nWidth * (DWORD)32);

		DWORD* pixels = (DWORD*)((BYTE*)startBits + i * WIDTHBYTES(m_nWidth * (DWORD)32));

		for (int j = 0; j < m_nWidth; j++)
		{
			pixels[j] = transparentPixel;
		}
	}
}

void CBitmapDC::RestoreAlphaBackroundFromGdiZeros()
{
	if (m_hDC == NULL) return;

	if (m_nBitCount < 32)
		return;

	BYTE* startBits = (BYTE*)m_DibDC.GetBits();
	ASSERT(startBits);

	for(int i = 0; i < m_nHeight; i++)
	{		
		BYTE* pixels = ((BYTE*)startBits + i * WIDTHBYTES(m_nWidth * (DWORD)32));

		for (int j = 0; j < m_nWidth; j++)
		{
			if ((pixels[j * 4] != 255 || 
				pixels[j * 4 + 1] != 255 || 
				pixels[j * 4 + 2] != 254) 	&&
				pixels[j * 4 + 3] == 0)		
			{
				pixels[j * 4 + 3] = 255;
			}
		}
	}
}

void CBitmapDC::UpdateAlphaOpacity(double opacity, COLORREF* transparent)
{
	if (m_hDC == NULL) return;

	if (m_nBitCount < 32)
		return;

	BYTE* startBits = (BYTE*)m_DibDC.GetBits();
	ASSERT(startBits);

	for(int i = 0; i < m_nHeight; i++)
	{		
		BYTE* pixels = ((BYTE*)startBits + i * WIDTHBYTES(m_nWidth * (DWORD)32));

		for (int j = 0; j < m_nWidth; j++)
		{	
			if (transparent &&  
				pixels[j * 4] == GetBValue(*transparent) &&
				pixels[j * 4 + 1] == GetGValue(*transparent) && 
				pixels[j * 4 + 2] == GetRValue(*transparent)	)
			{
				pixels[j * 4 + 3] = 0;
			}
			else
			{
				pixels[j * 4 + 3] = (BYTE)(255.0 * opacity);
			}
		}
	}
}

void CBitmapDC::AlphaBlend(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int hHeightDest, int nOpacity)
{
	ASSERT(hdcDest);

	if (!m_hDC) return;

	BLENDFUNCTION blendfunc;
	memset(&blendfunc, '\0', sizeof(BLENDFUNCTION));
	blendfunc.BlendOp = AC_SRC_OVER;
	blendfunc.SourceConstantAlpha = nOpacity;

	::AlphaBlend(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, hHeightDest,
				m_hDC, 0, 0, m_nWidth, m_nHeight, blendfunc);
}

void CBitmapDC::AlphaBlend(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int hHeightDest)
{
	ASSERT(hdcDest);

	if (!m_hDC) return;

	BLENDFUNCTION blendfunc;
    blendfunc.BlendOp = AC_SRC_OVER;
    blendfunc.BlendFlags = 0;
    blendfunc.SourceConstantAlpha = 255;
    blendfunc.AlphaFormat = AC_SRC_ALPHA;

	::AlphaBlend(hdcDest, nXOriginDest, nYOriginDest, nWidthDest, hHeightDest,
				m_hDC, 0, 0, m_nWidth, m_nHeight, blendfunc);
}

void CBitmapDC::PremultiplyAlpha()
{
	if (m_hDC == NULL) return;

	if (m_nBitCount <= 24)
		return;

	BYTE* startBits = (BYTE*)m_DibDC.GetBits();
	ASSERT(startBits);

	for (int i = 0; i < m_nHeight; i++)
	{		
		BYTE* pixels = ((BYTE*)startBits + i * WIDTHBYTES(m_nWidth * (DWORD)32));

		for (int j = 0; j < m_nWidth; j++)
		{
			int offs = j * 4;

			pixels[offs] =  pixels[offs] * pixels[offs+3] / 255;
			pixels[offs + 1] = pixels[offs+1] *  pixels[offs+3] / 255; 
			pixels[offs + 2] = pixels[offs + 2] * pixels[offs+3] / 255;
		}
	}
}

void CBitmapDC::Dispose()
{
	m_DibDC.Clear();
	m_hDC = NULL;
	m_nWidth = 0;
	m_nHeight = 0;
}

//////////////////////////////////////////////////////////////////////////////////////////////
// CTrueColorMemoryDC

CTrueColorMemoryDC::CTrueColorMemoryDC()
{
	m_nBitCount = 24;
	m_hDC = NULL;
	m_hBitmap = NULL;
	m_hOldBitmap = NULL;
	m_pBits = NULL;

	m_hPal = NULL;

	m_Size.cx = m_Size.cy = 0;
}

CTrueColorMemoryDC::~CTrueColorMemoryDC()
{
	Clear();
}

HDC  CTrueColorMemoryDC::Create(int	cx,	int	cy, int nBitCount)
{
	ASSERT(nBitCount >= 24);

	m_nBitCount = nBitCount;

	m_pBits = NULL;
	
	Clear();

	HDC	hScreenDC = ::CreateDC(_T("DISPLAY"), NULL, NULL, NULL);

	if (!hScreenDC)
	{
		return NULL;
	}
	
	m_hDC = CreateCompatibleDC(hScreenDC);
	
	VERIFY( DeleteDC(hScreenDC) );

	if (!m_hDC)
	{
		return NULL;
	}

	memset(&m_BmpInfo, sizeof(m_BmpInfo), '\0');

	m_BmpInfo.bmiHeader.biSize			=	sizeof(BITMAPINFOHEADER);
	m_BmpInfo.bmiHeader.biWidth			=	cx;
    m_BmpInfo.bmiHeader.biHeight		=	-cy;
	m_BmpInfo.bmiHeader.biPlanes		=	1;
	m_BmpInfo.bmiHeader.biBitCount		=	m_nBitCount;
	m_BmpInfo.bmiHeader.biCompression	=	BI_RGB;
	
	m_hBitmap = CreateDIBSection(m_hDC, &m_BmpInfo, DIB_RGB_COLORS, &m_pBits, NULL, 0);
	
	if	(!m_hBitmap)
	{
		return NULL;
	}

	ASSERT(m_pBits != NULL);

	m_hOldBitmap = (HBITMAP)::SelectObject(m_hDC, m_hBitmap);
	VERIFY( m_hOldBitmap );

	m_Size.cx = cx;
	m_Size.cy = cy;

	return m_hDC;
}

void CTrueColorMemoryDC::Clear()
{
	if (m_hOldBitmap && m_hDC)
	{
		VERIFY( ::SelectObject(m_hDC, m_hOldBitmap) );
		m_hOldBitmap = NULL;
	}
	
	if (m_hDC)
	{
		VERIFY( DeleteDC(m_hDC) );
		m_hDC = NULL;
	}

	if (m_hBitmap)
	{
		VERIFY( ::DeleteObject(m_hBitmap) );
		m_hBitmap = NULL;
	}
}

void*	CTrueColorMemoryDC::GetBits()
{
	GdiFlush();

	return m_pBits;
}
