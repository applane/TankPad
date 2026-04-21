#pragma once

void SetWindowText(CWnd* wnd, LPCTSTR text, double value);
void SetWindowText(CWnd* wnd, LPCTSTR text, double value1, double value2);

LPVOID LoadRes(int ID, LPCTSTR resType, int* pResSize);

class CStyledFont : CFont
{
	int m_weight; bool m_isItalic; int m_size;
public:
	CStyledFont(int weight = FW_NORMAL, int size = 0, bool italic = false) 
		: m_weight(weight), m_size(size), m_isItalic(italic) {}
	void Apply(CWnd* wnd);
};

// config
void InitAppConfigPath();
bool WriteConfigInt(LPCTSTR name, int value);
int ReadConfigInt(LPCTSTR name, int default);

bool WriteConfigBool(LPCTSTR name, bool value);
bool ReadConfigBool(LPCTSTR name, bool default);

bool WriteConfigString(LPCTSTR name, LPCTSTR value);
CString ReadConfigString(LPCTSTR name, LPCTSTR default);


