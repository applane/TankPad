#include "pch.h"
#include "svg.h"

LPVOID LoadSvgRes(UINT ID, UINT* pResSize);

#define ROUND_VALUE 0.5

CSvgDraw::CSvgDraw(void)
{
	m_scale = 1;
	m_nsvg = nullptr;
	m_oldPen = nullptr;
}

CSvgDraw::~CSvgDraw(void)
{
	Clear();
	ASSERT(!m_oldPen);
	ASSERT(!m_oldBrush);
}

void CSvgDraw::Draw(CGdiDC* drawDC, CRect& rect)
{
	ASSERT(drawDC);
	ASSERT(rect.Size().cx > 0 && rect.Size().cy > 0);
	ASSERT(m_nsvg);

	if (!m_nsvg) return;

	double scaleX = (double)rect.Size().cx / (double)m_nsvg->width;
	double scaleY = (double)rect.Size().cy / (double)m_nsvg->height;

	if (scaleX < scaleY)
		m_scale = scaleX;
	else
		m_scale = scaleY;

	m_x = rect.left;
	m_y = rect.top;

	NSVGshape* shape;	

	for (shape = m_nsvg->shapes; shape != NULL; shape = shape->next) 
	{
		DrawShape(drawDC, shape);
	}
}

void CSvgDraw::DrawShape(CGdiDC* drawDC, NSVGshape* shape)
{
	ASSERT(drawDC);
	ASSERT(shape);

	if (memcmp(shape->id, "path1329", 8) == 0) // ignore std. SVG symbol background
		return;

	if (!shape->paths)
		return;

	if (!SetupStyle(drawDC, shape))
		return;

	drawDC->BeginPath();

	for (NSVGpath* path = shape->paths; path != NULL; path = path->next) 
	{
		DrawPath(drawDC, path);
	}	
	
	drawDC->DrawPath();

	ResetStyle(drawDC);
}

void CSvgDraw::DrawPath(CGdiDC* drawDC, NSVGpath* path)
{
	ASSERT(drawDC);
	ASSERT(path);

	if (path->npts == 0) return;	

	MoveTo(drawDC, CGeoPoint(path->pts[0],path->pts[1]));
	
	for (int i = 1; i < path->npts-1; i += 3) 
	{
		float* p = &path->pts[i*2];
		CubicBezierTo(drawDC, CGeoPoint(p[0],p[1]), CGeoPoint(p[2],p[3]), CGeoPoint(p[4],p[5]));
	}

	if (path->closed) 
		LineTo(drawDC, CGeoPoint(path->pts[0],path->pts[1]));		
}

void CSvgDraw::MoveTo(CGdiDC* drawDC, CGeoPoint& p)
{
	ASSERT(drawDC);

	ScalePoint(p);
	drawDC->PathMoveTo(p.x, p.y);
}

void CSvgDraw::LineTo(CGdiDC* drawDC, CGeoPoint& p)
{
	ASSERT(drawDC);
	ScalePoint(p);
	drawDC->PathLineTo(p.x, p.y);
}

void CSvgDraw::CubicBezierTo(CGdiDC* drawDC, CGeoPoint& ctrl1, CGeoPoint& ctrl2, CGeoPoint& to)
{
	ASSERT(drawDC);
	
	ScalePoint(ctrl1);
	ScalePoint(ctrl2);
	ScalePoint(to);

	drawDC->CubicBezierTo(ctrl1, ctrl2, to);
}

void CSvgDraw::ScalePoint(CGeoPoint& p)
{
	if (m_scale != 1.0)
	{
		p.x = p.x * m_scale;
		p.y = p.y * m_scale;
	}

	p.x = m_x + p.x;
	p.y = m_y + p.y;
}

bool CSvgDraw::SetupStyle(CGdiDC* drawDC, NSVGshape* shape)
{
	ASSERT(drawDC);
	ASSERT(shape);
	ASSERT(!m_oldPen);
	ASSERT(!m_oldBrush);

	int style = PS_SOLID;
	int endCap = 0;
	int join = 0;
	float width = 0;
	COLORREF penColor, fillColor;
	
	switch (shape->strokeLineCap)
	{
	case NSVG_CAP_BUTT: endCap = PS_ENDCAP_FLAT; break;
	case NSVG_CAP_ROUND: endCap = PS_ENDCAP_ROUND; break;
	case NSVG_CAP_SQUARE: endCap = PS_ENDCAP_SQUARE; break;
	}

	switch (shape->strokeLineJoin)
	{
	case NSVG_JOIN_MITER: join = PS_JOIN_MITER; break;
	case NSVG_JOIN_ROUND: join = PS_JOIN_ROUND; break;
	case NSVG_JOIN_BEVEL: join = PS_JOIN_BEVEL; break;
	}

	width = shape->strokeWidth;

	if (width <= 0)
		style |= PS_NULL;
	else
	{
		width = Round(width * m_scale);
		if (width <= 0) width = 1;
	}

	if (shape->stroke.type == NSVG_PAINT_COLOR) 
		penColor = GetStrokeColor(shape->stroke.color & 0x00ffffff);
	else if (shape->stroke.type == NSVG_PAINT_NONE) 
	{
		penColor = -1;
		style |= PS_NULL;
		width = 0;
	}
	else
		penColor = m_defStroke;
	

	if (shape->fill.type == NSVG_PAINT_COLOR) 
		fillColor = GetFillColor(shape->fill.color & 0x00ffffff);
	else if (shape->fill.type == NSVG_PAINT_NONE) 
		fillColor = -1;
	else
		fillColor = m_defFill;

	if (!m_pen.CreatePen(style | endCap | join, (int)width, penColor))
		return false;

	if (fillColor == -1)
	{
		if (!m_brush.CreateStockObject(NULL_BRUSH))
		{
			m_pen.DeleteObject();
			return false;
		}
	}
	else if (!m_brush.CreateSolidBrush(fillColor))
	{
		m_pen.DeleteObject();
		return false;
	}
	
	m_oldPen = drawDC->SelectObject(&m_pen);
	m_oldBrush = drawDC->SelectObject(&m_brush);

	ASSERT(m_oldPen);
	ASSERT(m_oldBrush);

	return true;
}

COLORREF CSvgDraw::GetStrokeColor(COLORREF color)
{
	if (m_oldStroke == -1 || m_oldStroke != color) return color;

	return m_newStroke;
}

COLORREF CSvgDraw::GetFillColor(COLORREF color)
{
	if (m_oldFill == -1 || m_oldFill != color) return color;

	return m_newFill;
}

void CSvgDraw::ResetStyle(CGdiDC* drawDC)
{
	ASSERT(m_oldPen);
	ASSERT(m_oldBrush);
	
	VERIFY( drawDC->SelectObject(m_oldPen) );
	VERIFY( drawDC->SelectObject(m_oldBrush) );

	m_oldPen = NULL;
	m_oldBrush = NULL;

	VERIFY( m_pen.DeleteObject() );
	VERIFY( m_brush.DeleteObject() );
}

bool CSvgDraw::Load(LPCTSTR filename)
{
	ASSERT(filename);

	try
	{
		CSysFile file;
		file.Open(filename, CFile::modeRead | CFile::shareDenyWrite);
		
		if (file.GetLength() == 0)
			return false;

		buf.AllocateBuffer(file.GetLength() + 1);

		VRF_FORMAT( file.Read(buf.GetData(), file.GetLength()) == file.GetLength());

		((char*)buf.GetData())[file.GetLength()] = '\0';
		
		NSVGimage* nsvg = nsvgParse((char*)buf.GetData(), "px", NSVG_DPI);
		if (!nsvg) 	return false;

		Clear();

		m_nsvg = nsvg;

		return true;
	}
	catch(CException*){}

	return false;
}

bool CSvgDraw::Load(UINT svgResID)
{
try
{
	NSVGimage* nsvg = LoadNsvg(svgResID);
	
	if (!nsvg) 	return false;

	Clear();

	m_nsvg = nsvg;
}
catch(CException*){}

	return true;
}

NSVGimage* LoadNsvg(UINT resourceID)
{
	UINT resSize;

try
{
	char* svgFile = (char*)LoadSvgRes(resourceID, &resSize);
	if (!svgFile)
		return false;

	CStaticMem buf;
	buf.AllocateBuffer(resSize + 1);

	memcpy(buf.GetData(), svgFile, resSize);

	((char*)buf.GetData())[resSize] = '\0';

	NSVGimage* nsvg = nsvgParse((char*)buf.GetData(), "px", NSVG_DPI);
	
	if (!nsvg) 	return nullptr;

	return nsvg;
}
catch(CException*){}	

return nullptr;
}

void CSvgDraw::Clear()
{
	if (m_nsvg && !m_isTransitNsvg)
	{
		nsvgDelete(m_nsvg);
		m_nsvg = nullptr;
	}
	m_scale = 1;
}

LPVOID LoadSvgRes(UINT ID, UINT* pResSize)
{
	HRSRC hResInfo = FindResource(AfxGetInstanceHandle(),
					MAKEINTRESOURCE(ID),
					_T("SVG"));

	if (!hResInfo)
	{
		TRACE("Cant find resource");
		return NULL;
	}
	
	HGLOBAL hRes = LoadResource(AfxGetInstanceHandle(),
								hResInfo);

	if (!hRes) return NULL;

	*pResSize = SizeofResource(  AfxGetInstanceHandle(), hResInfo);

	return LockResource(hRes);
}
