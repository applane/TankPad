#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CPadApp:
// See WotPad.cpp for the implementation of this class
//

class CPadApp : public CWinApp
{
public:
	CPadApp();

// Overrides
public:
	virtual BOOL InitInstance();

// Implementation

	DECLARE_MESSAGE_MAP()
};

extern CPadApp theApp;
