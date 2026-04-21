
// WotPad.cpp : Defines the class behaviors for the application.
//

#include "pch.h"
#include "framework.h"
#include "TankPad.h"
#include "PadDlg.h"
#include "mix.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CPadApp

BEGIN_MESSAGE_MAP(CPadApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CPadApp construction

CPadApp::CPadApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CPadApp object

CPadApp theApp;


// CPadApp initialization

BOOL CPadApp::InitInstance()
{
	CWinApp::InitInstance();

	InitAppConfigPath();

	// Activate "Windows Native" visual manager for enabling themes in MFC controls
	// CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerWindows));
	CMFCVisualManager::SetDefaultManager(RUNTIME_CLASS(CMFCVisualManagerOffice2007));

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	// SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CPadDlg dlg;
	
	m_pMainWnd = &dlg;
	
	INT_PTR nResponse = dlg.DoModal();

#if !defined(_AFXDLL) && !defined(_AFX_NO_MFC_CONTROLS_IN_DIALOGS)
	ControlBarCleanUp();
#endif

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

