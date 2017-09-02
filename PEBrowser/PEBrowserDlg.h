// PEBrowserDlg.h : header file
//

#if !defined(AFX_CODEINJECTIONDLG_H__005DCF17_8B84_429B_AC2D_AD53038BA6C0__INCLUDED_)
#define AFX_CODEINJECTIONDLG_H__005DCF17_8B84_429B_AC2D_AD53038BA6C0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "../common/PEHeader.h"
/////////////////////////////////////////////////////////////////////////////
// CPEBrowserDlg dialog

class CPEBrowserDlg : public CDialog
{
// Construction
public:
	void Set_ValueToEditctrl(CEdit* l_edit,DWORD addr);
	void Insert_SectionToLIstctrl(SECTION_HEADER SEC);
	void Init_ListCtrl();
    void ClearDisplay();

	CPEBrowserDlg(CWnd* pParent = NULL);	// standard constructor


// Dialog Data
	//{{AFX_DATA(CPEBrowserDlg)
	enum { IDD = IDD_PEBROWSER_DIALOG };
	CEdit	m_edit_reloc;
	CEdit	m_edit_timestamp;
	CEdit	m_edit_pns;
	CEdit	m_edit_pitr;
	CEdit	m_edit_pep;
	CListCtrl	m_listctrl_secview;
	CEdit	m_edit_filename;
	CString	m_edit_message;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPEBrowserDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPEBrowserDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonInsertmsg();
	afx_msg void OnButtonOpen();
	afx_msg void OnBtnAddSection();
	afx_msg void OnBtnCheck();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CODEINJECTIONDLG_H__005DCF17_8B84_429B_AC2D_AD53038BA6C0__INCLUDED_)
