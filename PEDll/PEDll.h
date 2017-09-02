// PEDll.h : main header file for the PEDLL DLL
//

#if !defined(AFX_PEDLL_H__94A479E1_4F69_4344_9F77_559AB9479AF2__INCLUDED_)
#define AFX_PEDLL_H__94A479E1_4F69_4344_9F77_559AB9479AF2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CPEDllApp
// See PEDll.cpp for the implementation of this class
//

class CPEDllApp : public CWinApp
{
public:
	CPEDllApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPEDllApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CPEDllApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PEDLL_H__94A479E1_4F69_4344_9F77_559AB9479AF2__INCLUDED_)
