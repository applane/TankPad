#include "pch.h"
#define NANOSVG_IMPLEMENTATION
#include "nanosvg.h"
#define NANOSVGRAST_IMPLEMENTATION
#include "nanosvgrast.h"
#include "TankPad.h"
#include "SVGControl.h"
#include "mix.h"

IMPLEMENT_DYNAMIC(CSVGControl, CWnd)

CSVGControl::CSVGControl(ISVGControlNotify* notifyI) : m_bitmapDC(32)
{
    m_notifyI = notifyI;
    m_rasterizer = nsvgCreateRasterizer();
    RegisterWindowClass();
    m_backColor = GetSysColor(COLOR_BTNFACE);
    m_isDirty = true;
}

CSVGControl::~CSVGControl()
{
    Dispose();

    if (m_rasterizer)
        nsvgDeleteRasterizer(m_rasterizer);
}

void CSVGControl::Dispose()
{
    ClearSelection();

    if (m_image)
    {
        nsvgDelete(m_image);
        m_image = nullptr;
    }
}

bool CSVGControl::LoadFile(LPCTSTR filepath, int dpi)
{
    ASSERT(filepath);
    ASSERT(dpi > 10);

    Dispose();

    USES_CONVERSION;

    NSVGimage* image = nsvgParseFromFile(W2A(filepath), "px", (float)dpi);
    if (!image) return false;

    m_image = image;

    if (this->GetSafeHwnd())
        Invalidate();

    return true;
}

bool CSVGControl::LoadResource(int resourceID, int dpi)
{
    int resSize = 0;
    
    LPVOID resPtr = LoadRes(resourceID, L"SVG", &resSize);
    if (!resPtr) return false;

    char* svgBuf = (char*)malloc(resSize+1);
    if (!svgBuf) return false;

    memcpy(svgBuf, resPtr, resSize);
    svgBuf[resSize] = 0;

    NSVGimage* image = nsvgParse(svgBuf, "px", (float)dpi);
    free(svgBuf);
    if (!image) return false;

    m_image = image;

    if (this->GetSafeHwnd())
        Invalidate();

    return true;
}

BOOL CSVGControl::RegisterWindowClass()
{
    WNDCLASS wndcls;
    HINSTANCE hInst = AfxGetInstanceHandle();

    if (!(::GetClassInfo(hInst, CSVGCONTROL_CLASSNAME, &wndcls)))
    {
        // otherwise we need to register a new class
        wndcls.style = CS_DBLCLKS | CS_HREDRAW | CS_VREDRAW;
        wndcls.lpfnWndProc = ::DefWindowProc;
        wndcls.cbClsExtra = wndcls.cbWndExtra = 0;
        wndcls.hInstance = hInst;
        wndcls.hIcon = NULL;
        wndcls.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
        wndcls.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
        wndcls.lpszMenuName = NULL;
        wndcls.lpszClassName = CSVGCONTROL_CLASSNAME;

        if (!AfxRegisterClass(&wndcls))
        {
            ASSERT(FALSE);
            return FALSE;
        }
    }

    return TRUE;
}

BEGIN_MESSAGE_MAP(CSVGControl, CWnd)
    ON_WM_PAINT()
    ON_WM_ERASEBKGND()
    ON_WM_LBUTTONDOWN()
    ON_WM_SETCURSOR()
END_MESSAGE_MAP()

void CSVGControl::OnPaint()
{
    DWORD start = GetTickCount();
    Paint();
    TRACE1("Paint: %ld ms\n", GetTickCount() - start);
}

void CSVGControl::Paint()
{
    CPaintDC dc(this);

    CRect clientRect;
    GetClientRect(&clientRect);

    if (IsImageLoaded())
    {
        if (NeedResizeBitmap(clientRect.Width(), clientRect.Height()))
        {
            if (!m_bitmapDC.Resize(clientRect.Width(), clientRect.Height()))
            {
                dc.TextOutW(1, 1, L"Not enough memory to create bitmap.");
                return;
            }

            SetDirty();
        }

        if (m_isDirty)
        {
            m_bitmapDC.Fill(GetSysColor(COLOR_3DFACE), 255);
         
            CalcScale(clientRect.Width(), clientRect.Height());

            DWORD start = GetTickCount();
            // rasterize
            nsvgRasterize(m_rasterizer, m_image, m_offsX, m_offsY, m_scale, m_bitmapDC.GetBits(),
                clientRect.Width(), clientRect.Height(), m_bitmapDC.ScanlineSize());

            CString s; s.Format(L"nsvgRasterize: %ld ms\n", GetTickCount() - start);
            OutputDebugStringW(s);

            m_isDirty = false;
        }

        BLENDFUNCTION blendfunc;
        blendfunc.BlendOp = AC_SRC_OVER;
        blendfunc.BlendFlags = 0;
        blendfunc.SourceConstantAlpha = 255;
        blendfunc.AlphaFormat = AC_SRC_ALPHA;

        //::AlphaBlend(dc.GetSafeHdc(), 0, 0, clientRect.Width(), clientRect.Height(),
          //  m_bitmapDC.GetDC(), 0, 0, clientRect.Width(), clientRect.Height(), blendfunc);

        ::BitBlt(dc.GetSafeHdc(), 0, 0, clientRect.Width(), clientRect.Height(),
              m_bitmapDC.GetDC(), 0, 0, SRCCOPY);
    }

    //CBrush brush; brush.CreateStockObject(DKGRAY_BRUSH);
    //dc.FrameRect(&clientRect, &brush);

    //dc.DrawIcon(testPoint, LoadIcon(NULL, MAKEINTRESOURCE(IDI_ASTERISK)));
}

BOOL CSVGControl::OnEraseBkgnd(CDC* pDC)
{
    if (!m_image) return CWnd::OnEraseBkgnd(pDC);

    return TRUE;
}

bool CSVGControl::NeedResizeBitmap(int width, int height)
{
    return (!m_bitmapDC.IsValid() || 
            m_bitmapDC.Width() != width || 
            m_bitmapDC.Height() != height);
}

void CSVGControl::CalcScale(int width, int height)
{
    if (!m_image) return;

    float scaleX = (float)width / m_image->width;
    float scaleY = (float)height / m_image->height;

    if (scaleX > scaleY)
    {
        m_offsX = (width - (int)(m_image->width * scaleY)) / 2.f;
        m_offsY = 0;
        m_scale = scaleY;
    }
    else
    {
        m_offsX = 0;
        m_offsY = (height - (int)(m_image->height * scaleX)) / 2.f;
        m_scale = scaleX;
    }
}

NSVGshape* FindSvgShape(NSVGimage* const image, LPCSTR name)
{
    for (NSVGshape* shape = image->shapes; shape; shape = shape->next)
    {
        if (strncmp(shape->id, name, strlen(name)) == 0)
        {
            return shape;
        }
    }

    return nullptr;
}

bool IsInsideBoundBox(float* bounds, float x, float y)
{
    return (x >= bounds[0] && x <= bounds[2] &&
        y >= bounds[1] && y <= bounds[3]);
}

bool FindSvgShapes(NSVGimage* const image, float x, float y, CStringArray& shapeIds)
{
    for (NSVGshape* shape = image->shapes; shape; shape = shape->next)
    {
        if (IsInsideBoundBox(shape->bounds, x, y))
        {
            if (shape->id && shape->id[0] != '\0')
            {
                USES_CONVERSION;
                shapeIds.Add(A2W(shape->id));
            }
        }
    }

    return shapeIds.GetCount() > 0;
}


bool CSVGControl::SelectShape(LPCTSTR shapeId, COLORREF fillRGBA)
{
    ASSERT(shapeId);
    if (!m_image) return false;

    RestoreSelectedShape();

    USES_CONVERSION;

    NSVGshape* shape = FindSvgShape(m_image, W2A(shapeId));
    if (!shape) return false;

    m_selectedShape = shape;
    m_savedShape = *shape;

    m_selectedShape->fill.type = NSVG_PAINT_COLOR;
    m_selectedShape->fill.color = fillRGBA;

    Refresh();

    return true;
}

void CSVGControl::ClearSelection()
{
    RestoreSelectedShape();
    Refresh();
}

void CSVGControl::RestoreSelectedShape()
{
    if (!m_selectedShape) return;

    *m_selectedShape = m_savedShape;

    m_selectedShape = nullptr;
}

void CSVGControl::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (!m_image) return; 

    CWnd::OnLButtonDown(nFlags, point);

    testPoint.x = (LONG)((point.x - m_offsX) / m_scale);
    testPoint.y = (LONG)((point.y - m_offsY) / m_scale);

    CStringArray shapeIds;

    if (!FindSvgShapes(m_image, (float)testPoint.x, (float)testPoint.y, shapeIds))
        return;

    if (m_notifyI) m_notifyI->ShapeClicked(shapeIds);
}

BOOL CSVGControl::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    if (m_cursor)
    {
        HCURSOR hCursor = ::LoadCursor(NULL,m_cursor);
        ::SetCursor(hCursor);
        return FALSE;
    }

    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}
