#ifndef __IMAGEDC_H_
#define __IMAGEDC_H_

#include "dibapi.h"

#define RGBA(r,g,b,a)  ((COLORREF)(((BYTE)(r)|((WORD)(g)<<8))|(((DWORD)(BYTE)(b))<<16)|(((DWORD)(BYTE)(a))<<24)))

class	CTrueColorMemoryDC
{
	HDC			m_hDC;
	HBITMAP		m_hBitmap;
	HBITMAP		m_hOldBitmap;
	BITMAPINFO	m_BmpInfo;
	void*		m_pBits;	

	HPALETTE	m_hPal;
	SIZE		m_Size;
	int			m_nBitCount;

public:
	HDC		Create(int	cx,	int	cy, int nBitCount = 24);
	void*	GetBits();
	void		Clear();
	SIZE		GetSize() { return m_Size;}
	int		GetBitCount() { return m_nBitCount; }
	HDC		GetHDC() { return m_hDC; }

	CTrueColorMemoryDC();
	~CTrueColorMemoryDC();

protected:

};

class CBitmapDC
{
public:
	CBitmapDC(int nBitCount = 24);
	~CBitmapDC(){}

	// Inits & resizes the image, run first
	BOOL Resize(long nWidth, long nHeight);

	// image size
	long Width() { return m_nWidth; }
	long Height() { return m_nHeight; }
	
	// image dc
	HDC GetDC() { return m_hDC; }

	void Fill(COLORREF cColor);
	void Fill(COLORREF rgb, BYTE alpha);

	void FillTransparentAlphaBackround();
	void UpdateAlphaOpacity(double opacity, COLORREF* transparent = NULL);
	void RestoreAlphaBackroundFromGdiZeros();

	void ReleaseResources() { m_DibDC.Clear(); m_hDC = NULL; }

	void AlphaBlend(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int hHeightDest, int nOpacity);

	void AlphaBlend(HDC hdcDest, int nXOriginDest, int nYOriginDest, int nWidthDest, int hHeightDest);

	INT32 ScanlineSize() { return WIDTHBYTES(Width() * GetBitCount()); }

	BYTE*	GetBits() { return (BYTE*)m_DibDC.GetBits(); }

	BYTE*	GetScanline(long row) { return (BYTE*)m_DibDC.GetBits() + ScanlineSize() * (m_nHeight - row - 1); }
	
	void	SetBitCount(int nBitCount) { m_nBitCount = nBitCount; }
	int		GetBitCount() { return m_DibDC.GetBitCount(); }
	
	void Dispose();

	BOOL IsValid() { return m_hDC != NULL; }

	void PremultiplyAlpha();

private:
	long	 m_nWidth;
	long	 m_nHeight;
	CTrueColorMemoryDC m_DibDC;
	HDC		 m_hDC;
	int		 m_nBitCount;
};

#endif //__IMAGEDC_H_