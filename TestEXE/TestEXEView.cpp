// TestEXEView.cpp : implementation of the CTestEXEView class
//

#include "stdafx.h"
#include "TestEXE.h"

#include "TestEXEDoc.h"
#include "TestEXEView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTestEXEView

IMPLEMENT_DYNCREATE(CTestEXEView, CView)

BEGIN_MESSAGE_MAP(CTestEXEView, CView)
	//{{AFX_MSG_MAP(CTestEXEView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTestEXEView construction/destruction

CTestEXEView::CTestEXEView()
{
	// TODO: add construction code here

}

CTestEXEView::~CTestEXEView()
{
}

BOOL CTestEXEView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CTestEXEView drawing

void CTestEXEView::OnDraw(CDC* pDC)
{
	CTestEXEDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CTestEXEView printing

BOOL CTestEXEView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CTestEXEView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CTestEXEView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CTestEXEView diagnostics

#ifdef _DEBUG
void CTestEXEView::AssertValid() const
{
	CView::AssertValid();
}

void CTestEXEView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CTestEXEDoc* CTestEXEView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CTestEXEDoc)));
	return (CTestEXEDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTestEXEView message handlers
