// TestEXEDoc.cpp : implementation of the CTestEXEDoc class
//

#include "stdafx.h"
#include "TestEXE.h"

#include "TestEXEDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestEXEDoc

IMPLEMENT_DYNCREATE(CTestEXEDoc, CDocument)

BEGIN_MESSAGE_MAP(CTestEXEDoc, CDocument)
	//{{AFX_MSG_MAP(CTestEXEDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestEXEDoc construction/destruction

CTestEXEDoc::CTestEXEDoc()
{
	// TODO: add one-time construction code here

}

CTestEXEDoc::~CTestEXEDoc()
{
}

BOOL CTestEXEDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CTestEXEDoc serialization

void CTestEXEDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CTestEXEDoc diagnostics

#ifdef _DEBUG
void CTestEXEDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CTestEXEDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTestEXEDoc commands
