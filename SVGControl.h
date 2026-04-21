#pragma once
#include "bitmapdc.h"

class ISVGControlNotify
{
public:
	virtual void ShapeClicked(const CStringArray& shapeIds) = 0;
};

#define CSVGCONTROL_CLASSNAME    _T("CSVGControl")
// CSVGControl

#include "nanosvg.h"
#include "nanosvgrast.h"

class CSVGControl : public CWnd
{
	DECLARE_DYNAMIC(CSVGControl)
public:
	CSVGControl(ISVGControlNotify* notify = nullptr);
	virtual ~CSVGControl();

	bool LoadFile(LPCTSTR filepath, int dpi = 96);
	bool LoadResource(int resourceID, int dpi = 96);

	bool SelectShape(LPCTSTR shapeId, COLORREF fillRGBA);
	void ClearSelection();

	void SetCursor(LPCTSTR stdCursorId) { m_cursor = stdCursorId; }

protected:
	ISVGControlNotify* m_notifyI = nullptr;
	bool m_isDirty = true;
	NSVGimage* m_image = nullptr;
	NSVGrasterizer* m_rasterizer = nullptr;
	CBitmapDC m_bitmapDC;
	COLORREF m_backColor;
	LPCTSTR m_cursor = nullptr;
	
	float m_scale = 1, m_offsX = 0, m_offsY = 0;
	POINT testPoint;

	NSVGshape* m_selectedShape = nullptr;
	NSVGshape m_savedShape;
	void RestoreSelectedShape();

	void Dispose();
	BOOL RegisterWindowClass();
	bool IsImageLoaded() { return m_image != nullptr; }
	bool NeedResizeBitmap(int width, int height);
	void CalcScale(int width, int height);

	void Paint();

	void SetDirty() { m_isDirty = true; }
	void Refresh() { SetDirty(); if (GetSafeHwnd()) Invalidate(); }
	void SetBackColor(COLORREF color) { m_backColor = color; }

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
};


