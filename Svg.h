#pragma once

#include "nanosvg.h
#include <drwint.h>

#define NSVG_DPI 96

NSVGimage* LoadNsvg(UINT resourceID);

class CSvgDraw
{
public:
	CSvgDraw(void);
	~CSvgDraw(void);

	bool Load(LPCTSTR filename);
	bool Load(UINT resourceID);
	
	void Draw(CGdiDC* drawDC, CRect& rect);
	
	void SetTransitNsvg(NSVGimage* nsvg);

	void Clear();

private:
	void DrawPath(CGdiDC* drawDC, NSVGpath* path);	
	void DrawShape(CGdiDC* drawDC, NSVGshape* shape);

	void MoveTo(CGdiDC* drawDC, CGeoPoint& p);
	void LineTo(CGdiDC* drawDC, CGeoPoint& p);
	void CubicBezierTo(CGdiDC* drawDC, CGeoPoint& ctrl1, CGeoPoint& ctrl2, CGeoPoint& to);

	void ScalePoint(CGeoPoint& p);

	bool SetupStyle(CGdiDC* drawDC, NSVGshape* shape);
	void ResetStyle(CGdiDC* drawDC);

	COLORREF GetStrokeColor(COLORREF color);
	COLORREF GetFillColor(COLORREF color);

	CPen		m_pen;
	CBrush	m_brush;
	CPen*	m_oldPen;
	CBrush*	m_oldBrush;

	double m_scale;
	int m_x;
	int m_y;

	NSVGimage* m_nsvg;		
};

