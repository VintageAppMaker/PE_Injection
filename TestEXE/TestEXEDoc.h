// TestEXEDoc.h : interface of the CTestEXEDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTEXEDOC_H__5D51C10C_8599_4A35_A922_3F727AE76FF5__INCLUDED_)
#define AFX_TESTEXEDOC_H__5D51C10C_8599_4A35_A922_3F727AE76FF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CTestEXEDoc : public CDocument
{
protected: // create from serialization only
	CTestEXEDoc();
	DECLARE_DYNCREATE(CTestEXEDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTestEXEDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTestEXEDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CTestEXEDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TESTEXEDOC_H__5D51C10C_8599_4A35_A922_3F727AE76FF5__INCLUDED_)
