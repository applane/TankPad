#include "pch.h"
#include "Mix.h"

LPCTSTR g_configMainSection = _T("Settings");

void SetWindowText(CWnd* wnd, LPCTSTR text, double value)
{
	CString str;
	str.Format(text, value);
	wnd->SetWindowText(str);
}

void SetWindowText(CWnd* wnd, LPCTSTR text, double value1, double value2)
{
	CString str;
	str.Format(text, value1, value2);
	wnd->SetWindowText(str);
}

void CStyledFont::Apply(CWnd* wnd)
{
	ASSERT(wnd);

	if (this->GetSafeHandle())
	{
		wnd->SetFont(this);
		return;
	}

	CFont* font = wnd->GetFont();
	if (!font) return;

	LOGFONT lf;

	if (font->GetLogFont(&lf))
	{
		if (m_size != 0)
		{
			HDC hdc = ::GetDC(wnd->GetSafeHwnd());
			lf.lfHeight = -MulDiv(m_size, GetDeviceCaps(hdc, LOGPIXELSY), 96);
			::ReleaseDC(wnd->GetSafeHwnd(), hdc);
		}

		lf.lfWeight = m_weight;
		lf.lfItalic = (BYTE)m_isItalic;

		if (CreateFontIndirect(&lf))
		{
			wnd->SetFont(this);
		}
	}
}

void InitAppConfigPath()
{
	TCHAR szPath[MAX_PATH];
	if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE,
						NULL, 0, szPath)))
	{
		wcscat_s(szPath, _T("\\"));
		wcscat_s(szPath, AfxGetAppName());
		CreateDirectory(szPath, NULL);
		wcscat_s(szPath, _T("\\"));
		wcscat_s(szPath, AfxGetAppName());
		wcscat_s(szPath, _T(".ini"));
		AfxGetApp()->m_pszProfileName = _tcsdup(szPath);
	}
}

bool WriteConfigInt(LPCTSTR name, int value)
{
	return (bool)AfxGetApp()->WriteProfileInt(g_configMainSection, name, value);
}

int ReadConfigInt(LPCTSTR name, int default)
{
	return AfxGetApp()->GetProfileInt(g_configMainSection, name, default);
}

bool WriteConfigBool(LPCTSTR name, bool value)
{
	return (bool)AfxGetApp()->WriteProfileInt(g_configMainSection, name, (int)value);
}

bool ReadConfigBool(LPCTSTR name, bool default)
{
	return (bool)AfxGetApp()->GetProfileInt(g_configMainSection, name, (int)default);
}

bool WriteConfigString(LPCTSTR name, LPCTSTR value)
{
	return (bool)AfxGetApp()->WriteProfileString(g_configMainSection, name, value);
}

CString ReadConfigString(LPCTSTR name, LPCTSTR default)
{
	return AfxGetApp()->GetProfileString(g_configMainSection, name, default);
}

LPVOID LoadRes(int ID, LPCTSTR resType, int* pResSize)
{
	HRSRC hResInfo = FindResource(AfxGetInstanceHandle(),
		MAKEINTRESOURCE(ID),
		resType);

	if (!hResInfo)
	{
		TRACE("Cant find resource");
		return NULL;
	}

	HGLOBAL hRes = LoadResource(AfxGetInstanceHandle(),
		hResInfo);

	if (!hRes) return NULL;

	*pResSize = SizeofResource(AfxGetInstanceHandle(), hResInfo);

	return LockResource(hRes);
}