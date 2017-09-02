// PEBrowser.h : main header file for the PEBROWSER application
//

#if !defined(AFX_CODEINJECTION_H__17421D4F_B09B_475E_BD3A_C4929314AB50__INCLUDED_)
#define AFX_CODEINJECTION_H__17421D4F_B09B_475E_BD3A_C4929314AB50__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPEBrowserApp:
// See PEBrowser.cpp for the implementation of this class
//

class CPEBrowserApp : public CWinApp
{
public:
	CPEBrowserApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPEBrowserApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CPEBrowserApp)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

extern CPEBrowserApp theApp;
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CODEINJECTION_H__17421D4F_B09B_475E_BD3A_C4929314AB50__INCLUDED_)
