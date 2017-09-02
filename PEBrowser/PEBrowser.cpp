// PEBrowser.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "PEBrowser.h"
#include "PEBrowserDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPEBrowserApp

BEGIN_MESSAGE_MAP(CPEBrowserApp, CWinApp)
	//{{AFX_MSG_MAP(CPEBrowserApp)
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPEBrowserApp construction

CPEBrowserApp::CPEBrowserApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPEBrowserApp object

CPEBrowserApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CPEBrowserApp initialization

BOOL CPEBrowserApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	CPEBrowserDlg dlg;
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
	}
	else if (nResponse == IDCANCEL)
	{
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
