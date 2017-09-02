// PEBrowserDlg.cpp : implementation file
//
/*-------------------------------------------------------
    목적:   PE 분석 및 Section ADD 관련 테스트 프로그램
    수정자: 박성완(crack90) 
    작성일: 2004-12-21
    참고:   일단 PE 관련 함수들을 소스레벨로 모아놓았다.
            기능은 다음과 같다.

            1) PE 분석 / 섹션 분석
            2) 섹션 찾기
            3) 섹션 ADD & DataArea 추가
            4) 간단한 암호화 및 채크섬 기능

    키값으로 사용되는 것은 다음과 같다(이 값이 수정될 경우, EXE나 DLL은 실행오류 일으킨다).
    AddressOfEntryPoint   -> 프로그램 실행 EIP(진입점)    : 같은 프로그램이 있을 수는 있다.(메모리 모델별 유형을 가진다)  ASM의 ORG 명령
    ResourceDirectorySize -> 리소스로 등록된 테이블 크기  : 화면 모양부터, 버튼 이미지까지에 대한 테이블이므로 같은 경우가 거의 없다.
    TimeDateStamp         -> 생성된 날짜.                 : 컴파일때마다 다른거므로 정말 유니크하다. 
    SizeOfInitializedData -> 초기화 데이터영역            : int i = 0; 과 같은 초기화 변수 영역. 이것을 같은 경우로 만들기도 힘들다.
    ExportDirectorySize   -> Extern 함수 영역             : external link된 함수 테이블 크기 
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

// 인증 섹션
#define AUTH_STRING ".crack90"

// 인증 메시지 버퍼
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

// 화면 지우기 
void CPEBrowserDlg::ClearDisplay()
{
    m_listctrl_secview.DeleteAllItems();
    m_edit_message = "";
    UpdateData(false);         
}

// 리스트 컨트롤 초기화 
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

// 리스트에 값 넣기 
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
// Section 정보를 보여주는 콜백함수 
void print_section_list(SECTION_HEADER SEC)
{
    CPEBrowserDlg* dlg = (CPEBrowserDlg*)theApp.m_pMainWnd;
    dlg->Insert_SectionToLIstctrl(SECTION_Info);	
}

// PE 정보 읽어오기 
void CPEBrowserDlg::OnButtonOpen() 
{
   // 화면지우기 
   ClearDisplay();

   char szFilters[]="All Files (*.exe)";
   CFileDialog fileDlg (TRUE, "*.exe", "*.exe",OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);
   
   if( fileDlg.DoModal ()==IDOK )
   {
      CString pathName = fileDlg.GetPathName();
      CString fileName = fileDlg.GetFileTitle();
	  m_edit_filename.SetWindowText(pathName);
	  
      // PE 정보 가져오기 
      OpenPEFile( pathName.GetBuffer( 0 ), true );
      LONGLONG offset = GetPEInfoOfFile();
      GetSectionInfofFile( offset, print_section_list);      
      ClosePEFile();      

      // section 정보 가져오기 
      OpenPEFile( pathName.GetBuffer( 0 ), true );
      offset   = GetPEInfoOfFile();
      DWORD msg_offset  = FindSection(AUTH_STRING);
      
      if( msg_offset != 0){
          if( !ReadSectionMessage(SEC_DATA, msg_offset, MAX_PATH) ){
              AfxMessageBox("인증키의 채크섬값이 틀립니다.");
          };

          m_edit_message = &SEC_DATA[1];
          UpdateData(false);
      }      
      ClosePEFile();	  
      
      // 기본정보 화면에 보여주기   
      Set_ValueToEditctrl(&m_edit_pep,  PE_info.AddressOfEntryPoint);
      Set_ValueToEditctrl(&m_edit_pitr, PE_info.SizeOfInitializedData);
      Set_ValueToEditctrl(&m_edit_pns,  PE_info.ResourceDirectorySize);
      Set_ValueToEditctrl(&m_edit_timestamp,  PE_info.TimeDateStamp);
      Set_ValueToEditctrl(&m_edit_reloc,  PE_info.ExportDirectorySize);

   }

}

// Edit 컨트롤에 값넣기 
void CPEBrowserDlg::Set_ValueToEditctrl(CEdit *l_edit, DWORD addr)
{
	CString strText;
	strText.Format("%.8X",addr);
	l_edit->SetWindowText(strText);
}

// Section 정보 입력 하기 
void CPEBrowserDlg::OnBtnAddSection() 
{
   // 화면지우기 
   ClearDisplay();
   
   char szFilters[]="All Files (*.exe)";
   CFileDialog fileDlg (TRUE, "*.exe", "*.exe",OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);
   
   if( fileDlg.DoModal ()==IDOK )
   {
      CString pathName = fileDlg.GetPathName();
      CString fileName = fileDlg.GetFileTitle();

	  m_edit_filename.SetWindowText(pathName);

      // 섹션정보 저장하기  
      OpenPEFile( pathName.GetBuffer( 0 ) );
      LONGLONG offset = GetPEInfoOfFile();
      
      // Section의 데이터 입력하기 
      CString msg;
      msg.Format(
          "%X_AUTH_AH%X_DJ%X_CK%X_Q%X",
          PE_info.AddressOfEntryPoint,
          PE_info.ResourceDirectorySize,
          PE_info.TimeDateStamp,
          PE_info.SizeOfInitializedData,
          PE_info.ExportDirectorySize
      );
      
      // 버퍼의 0번 인덱스는 BCC
      strcpy((char*)&SEC_DATA[1], msg.GetBuffer( 0 ));
      
      AddSection(offset, AUTH_STRING, SEC_DATA , BUF_SIZE );   
      AfxMessageBox("인증키가 생성되었습니다");
      
      m_edit_message = msg;
      UpdateData(false);

      ClosePEFile();	  
   }	
}

/*
   해당 코드 구조를 DLL내의 Entry 함수에 
   서 사용하면 된다. 

*/
void CPEBrowserDlg::OnBtnCheck() 
{
   AfxMessageBox(
       "File 열어서 Section(.pswauth)영역의 Data를 \r\n"
       "Hex Editing 했으면 \r\nchecksum 처리로 인해서 인증이 Fail 됩니다. "
       "\r\n나중에 확인하십시오 "
   );

   // 화면지우기 
   ClearDisplay();

   char szFilters[]="All Files (*.exe)";
   CFileDialog fileDlg (TRUE, "*.exe", "*.exe",OFN_FILEMUSTEXIST| OFN_HIDEREADONLY, szFilters, this);
   
   if( fileDlg.DoModal ()==IDOK )
   {
      CString pathName = fileDlg.GetPathName();
      CString fileName = fileDlg.GetFileTitle();
	  m_edit_filename.SetWindowText(pathName);
	  
      // PE 정보 가져오기       
      OpenPEFile( pathName.GetBuffer( 0 ), true );
      LONGLONG offset = GetPEInfoOfFile();
      GetSectionInfofFile( offset, print_section_list);      
      ClosePEFile();      

      // section 정보 가져오기 
      OpenPEFile( pathName.GetBuffer( 0 ), true );
      offset   = GetPEInfoOfFile();
      DWORD msg_offset  = FindSection(AUTH_STRING);
      
      if( msg_offset != 0){
          if( !ReadSectionMessage(SEC_DATA, msg_offset, MAX_PATH) ){
              AfxMessageBox("인증키의 채크섬값이 틀립니다.");
          };

          m_edit_message = &SEC_DATA[1];
          UpdateData(false);
      } else {
          AfxMessageBox("인증키가 없습니다.");
      }
            
      ClosePEFile();
      
      // PE 정보와 섹션 정보가 맞는지 비교해보자구나...
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
          AfxMessageBox("인증정보가 틀립니다.");
      } 
      
      // 기본정보 화면에 보여주기   
      Set_ValueToEditctrl(&m_edit_pep,  PE_info.AddressOfEntryPoint);
      Set_ValueToEditctrl(&m_edit_pitr, PE_info.SizeOfInitializedData);
      Set_ValueToEditctrl(&m_edit_pns,  PE_info.NumberofSections);
      Set_ValueToEditctrl(&m_edit_timestamp,  PE_info.TimeDateStamp);
      Set_ValueToEditctrl(&m_edit_reloc,  PE_info.ExportDirectorySize);
   }
}
