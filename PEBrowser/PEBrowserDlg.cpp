// PEBrowserDlg.cpp : implementation file
//
/*-------------------------------------------------------
    ����:   PE �м� �� Section ADD ���� �׽�Ʈ ���α׷�
    ������: �ڼ���(crack90) 
    �ۼ���: 2004-12-21
    ����:   �ϴ� PE ���� �Լ����� �ҽ������� ��Ƴ��Ҵ�.
            ����� ������ ����.

            1) PE �м� / ���� �м�
            2) ���� ã��
            3) ���� ADD & DataArea �߰�
            4) ������ ��ȣȭ �� äũ�� ���

    Ű������ ���Ǵ� ���� ������ ����(�� ���� ������ ���, EXE�� DLL�� ������� ����Ų��).
    AddressOfEntryPoint   -> ���α׷� ���� EIP(������)    : ���� ���α׷��� ���� ���� �ִ�.(�޸� �𵨺� ������ ������)  ASM�� ORG ���
    ResourceDirectorySize -> ���ҽ��� ��ϵ� ���̺� ũ��  : ȭ�� ������, ��ư �̹��������� ���� ���̺��̹Ƿ� ���� ��찡 ���� ����.
    TimeDateStamp         -> ������ ��¥.                 : �����϶����� �ٸ��ŹǷ� ���� ����ũ�ϴ�. 
    SizeOfInitializedData -> �ʱ�ȭ �����Ϳ���            : int i = 0; �� ���� �ʱ�ȭ ���� ����. �̰��� ���� ���� ����⵵ �����.
    ExportDirectorySize   -> Extern �Լ� ����             : external link�� �Լ� ���̺� ũ�� 
-------------------------------------------------------*/
#include "stdafx.h"
#include "PEBrowser.h"
#include "PEBrowserDlg.h"

#include "../common/PEHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// ���� ����
#define AUTH_STRING ".crack90"

// ���� �޽��� ����
#define BUF_SIZE 500
char SEC_DATA[BUF_SIZE] ={0,};

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPEBrowserDlg dialog

CPEBrowserDlg::CPEBrowserDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPEBrowserDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPEBrowserDlg)
	m_edit_message = _T("");
	//}}AFX_DATA_INIT
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CPEBrowserDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPEBrowserDlg)
	DDX_Control(pDX, IDC_EDIT_RELOC, m_edit_reloc);
	DDX_Control(pDX, IDC_EDIT_TIMESTMP, m_edit_timestamp);
	DDX_Control(pDX, IDC_EDIT_PNS, m_edit_pns);
	DDX_Control(pDX, IDC_EDIT_PITR, m_edit_pitr);
	DDX_Control(pDX, IDC_EDIT_PEP, m_edit_pep);
	DDX_Control(pDX, IDC_LIST_SECVIEW, m_listctrl_secview);
	DDX_Control(pDX, IDC_EDIT_FILENAME, m_edit_filename);
	DDX_Text(pDX, IDC_EDIT_MESSAGE, m_edit_message);
	DDV_MaxChars(pDX, m_edit_message, 200);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CPEBrowserDlg, CDialog)
	//{{AFX_MSG_MAP(CPEBrowserDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_OPEN, OnButtonOpen)
	ON_BN_CLICKED(IDC_BTN_ADD_SECTION, OnBtnAddSection)
	ON_BN_CLICKED(IDC_BTN_CHECK, OnBtnCheck)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPEBrowserDlg message handlers

BOOL CPEBrowserDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	Init_ListCtrl();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// ȭ�� ����� 
void CPEBrowserDlg::ClearDisplay()
{
    m_listctrl_secview.DeleteAllItems();
    m_edit_message = "";
    UpdateData(false);         
}

// ����Ʈ ��Ʈ�� �ʱ�ȭ 
void CPEBrowserDlg::Init_ListCtrl()
{
	m_listctrl_secview.InsertColumn(0, "Name");
	m_listctrl_secview.InsertColumn(1, "Vitual Size");
	m_listctrl_secview.InsertColumn(2, "Vitual Offset");
	m_listctrl_secview.InsertColumn(3, "Raw Size");
	m_listctrl_secview.InsertColumn(4, "Raw Offset");
	m_listctrl_secview.InsertColumn(5, "Characteristics");

	m_listctrl_secview.SetColumnWidth(0, 70);
	m_listctrl_secview.SetColumnWidth(1, 90);
	m_listctrl_secview.SetColumnWidth(2, 90);
	m_listctrl_secview.SetColumnWidth(3, 90);
	m_listctrl_secview.SetColumnWidth(4, 90);
	m_listctrl_secview.SetColumnWidth(5, 110);

	m_listctrl_secview.SetExtendedStyle(LVS_EX_FULLROWSELECT);
}

// ����Ʈ�� �� �ֱ� 
void CPEBrowserDlg::Insert_SectionToLIstctrl(SECTION_HEADER SEC)
{
	LVITEM li;
		
	li.mask      = LVIF_TEXT|LVIF_IMAGE;
	li.state     = 0;
	li.stateMask = 0;
	li.iSubItem  = 0;
	li.iItem     = 0;
	li.iImage    = 0;
	li.pszText   = (LPSTR)(LPCSTR)SECTION_Info.Name;

	CString strAddr;
	
	m_listctrl_secview.InsertItem(&li);
	strAddr.Format("%.8X",SEC.PhysicalAddress);
	m_listctrl_secview.SetItemText(0,1,strAddr);

	strAddr.Format("%.8X",SEC.VirtualAddress);
	m_listctrl_secview.SetItemText(0,2,strAddr);

	strAddr.Format("%.8X",SEC.SizeOfRawData);
	m_listctrl_secview.SetItemText(0,3,strAddr);

	strAddr.Format("%.8X",SEC.PointerToRawData);
	m_listctrl_secview.SetItemText(0,4,strAddr);

	strAddr.Format("%.8X",SEC.Characteristics);
	m_listctrl_secview.SetItemText(0,5,strAddr);
}

void CPEBrowserDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPEBrowserDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR CPEBrowserDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//--------------------------------------
// Section ������ �����ִ� �ݹ��Լ� 
void print_section_list(SECTION_HEADER SEC)
{
    CPEBrowserDlg* dlg = (CPEBrowserDlg*)theApp.m_pMainWnd;
    dlg->Insert_SectionToLIstctrl(SECTION_Info);	
}

// PE ���� �о���� 
void CPEBrowserDlg::OnButtonOpen() 
{
   // ȭ������� 
   ClearDisplay();

   char szFilters[]="All Files (*.exe)";
   CFileDialog fileDlg (TRUE, "*.exe", "*.exe",OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);
   
   if( fileDlg.DoModal ()==IDOK )
   {
      CString pathName = fileDlg.GetPathName();
      CString fileName = fileDlg.GetFileTitle();
	  m_edit_filename.SetWindowText(pathName);
	  
      // PE ���� �������� 
      OpenPEFile( pathName.GetBuffer( 0 ), true );
      LONGLONG offset = GetPEInfoOfFile();
      GetSectionInfofFile( offset, print_section_list);      
      ClosePEFile();      

      // section ���� �������� 
      OpenPEFile( pathName.GetBuffer( 0 ), true );
      offset   = GetPEInfoOfFile();
      DWORD msg_offset  = FindSection(AUTH_STRING);
      
      if( msg_offset != 0){
          if( !ReadSectionMessage(SEC_DATA, msg_offset, MAX_PATH) ){
              AfxMessageBox("����Ű�� äũ������ Ʋ���ϴ�.");
          };

          m_edit_message = &SEC_DATA[1];
          UpdateData(false);
      }      
      ClosePEFile();	  
      
      // �⺻���� ȭ�鿡 �����ֱ�   
      Set_ValueToEditctrl(&m_edit_pep,  PE_info.AddressOfEntryPoint);
      Set_ValueToEditctrl(&m_edit_pitr, PE_info.SizeOfInitializedData);
      Set_ValueToEditctrl(&m_edit_pns,  PE_info.ResourceDirectorySize);
      Set_ValueToEditctrl(&m_edit_timestamp,  PE_info.TimeDateStamp);
      Set_ValueToEditctrl(&m_edit_reloc,  PE_info.ExportDirectorySize);

   }

}

// Edit ��Ʈ�ѿ� ���ֱ� 
void CPEBrowserDlg::Set_ValueToEditctrl(CEdit *l_edit, DWORD addr)
{
	CString strText;
	strText.Format("%.8X",addr);
	l_edit->SetWindowText(strText);
}

// Section ���� �Է� �ϱ� 
void CPEBrowserDlg::OnBtnAddSection() 
{
   // ȭ������� 
   ClearDisplay();
   
   char szFilters[]="All Files (*.exe)";
   CFileDialog fileDlg (TRUE, "*.exe", "*.exe",OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);
   
   if( fileDlg.DoModal ()==IDOK )
   {
      CString pathName = fileDlg.GetPathName();
      CString fileName = fileDlg.GetFileTitle();

	  m_edit_filename.SetWindowText(pathName);

      // �������� �����ϱ�  
      OpenPEFile( pathName.GetBuffer( 0 ) );
      LONGLONG offset = GetPEInfoOfFile();
      
      // Section�� ������ �Է��ϱ� 
      CString msg;
      msg.Format(
          "%X_AUTH_AH%X_DJ%X_CK%X_Q%X",
          PE_info.AddressOfEntryPoint,
          PE_info.ResourceDirectorySize,
          PE_info.TimeDateStamp,
          PE_info.SizeOfInitializedData,
          PE_info.ExportDirectorySize
      );
      
      // ������ 0�� �ε����� BCC
      strcpy((char*)&SEC_DATA[1], msg.GetBuffer( 0 ));
      
      AddSection(offset, AUTH_STRING, SEC_DATA , BUF_SIZE );   
      AfxMessageBox("����Ű�� �����Ǿ����ϴ�");
      
      m_edit_message = msg;
      UpdateData(false);

      ClosePEFile();	  
   }	
}

/*
   �ش� �ڵ� ������ DLL���� Entry �Լ��� 
   �� ����ϸ� �ȴ�. 

*/
void CPEBrowserDlg::OnBtnCheck() 
{
   AfxMessageBox(
       "File ��� Section(.pswauth)������ Data�� \r\n"
       "Hex Editing ������ \r\nchecksum ó���� ���ؼ� ������ Fail �˴ϴ�. "
       "\r\n���߿� Ȯ���Ͻʽÿ� "
   );

   // ȭ������� 
   ClearDisplay();

   char szFilters[]="All Files (*.exe)";
   CFileDialog fileDlg (TRUE, "*.exe", "*.exe",OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);
   
   if( fileDlg.DoModal ()==IDOK )
   {
      CString pathName = fileDlg.GetPathName();
      CString fileName = fileDlg.GetFileTitle();
	  m_edit_filename.SetWindowText(pathName);
	  
      // PE ���� ��������       
      OpenPEFile( pathName.GetBuffer( 0 ), true );
      LONGLONG offset = GetPEInfoOfFile();
      GetSectionInfofFile( offset, print_section_list);      
      ClosePEFile();      

      // section ���� �������� 
      OpenPEFile( pathName.GetBuffer( 0 ), true );
      offset   = GetPEInfoOfFile();
      DWORD msg_offset  = FindSection(AUTH_STRING);
      
      if( msg_offset != 0){
          if( !ReadSectionMessage(SEC_DATA, msg_offset, MAX_PATH) ){
              AfxMessageBox("����Ű�� äũ������ Ʋ���ϴ�.");
          };

          m_edit_message = &SEC_DATA[1];
          UpdateData(false);
      } else {
          AfxMessageBox("����Ű�� �����ϴ�.");
      }
            
      ClosePEFile();
      
      // PE ������ ���� ������ �´��� ���غ��ڱ���...
      CString msg;
      msg.Format(
         "%X_AUTH_AH%X_DJ%X_CK%X_Q%X",
          PE_info.AddressOfEntryPoint,
          PE_info.ResourceDirectorySize,
          PE_info.TimeDateStamp,
          PE_info.SizeOfInitializedData,
          PE_info.ExportDirectorySize
      );	  
      
      if( strcmp( m_edit_message.GetBuffer(0), msg.GetBuffer(0) ) ){
          AfxMessageBox("���������� Ʋ���ϴ�.");
      } 
      
      // �⺻���� ȭ�鿡 �����ֱ�   
      Set_ValueToEditctrl(&m_edit_pep,  PE_info.AddressOfEntryPoint);
      Set_ValueToEditctrl(&m_edit_pitr, PE_info.SizeOfInitializedData);
      Set_ValueToEditctrl(&m_edit_pns,  PE_info.NumberofSections);
      Set_ValueToEditctrl(&m_edit_timestamp,  PE_info.TimeDateStamp);
      Set_ValueToEditctrl(&m_edit_reloc,  PE_info.ExportDirectorySize);
   }
}
