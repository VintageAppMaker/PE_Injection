// PEDll.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "PEDll.h"

#include "../common/PEHeader.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
//	Note!
//
//		If this DLL is dynamically linked against the MFC
//		DLLs, any functions exported from this DLL which
//		call into MFC must have the AFX_MANAGE_STATE macro
//		added at the very beginning of the function.
//
//		For example:
//
//		extern "C" BOOL PASCAL EXPORT ExportedFunction()
//		{
//			AFX_MANAGE_STATE(AfxGetStaticModuleState());
//			// normal function body here
//		}
//
//		It is very important that this macro appear in each
//		function, prior to any calls into MFC.  This means that
//		it must appear as the first statement within the 
//		function, even before any object variable declarations
//		as their constructors may generate calls into the MFC
//		DLL.
//
//		Please see MFC Technical Notes 33 and 58 for additional
//		details.
//

/////////////////////////////////////////////////////////////////////////////
// CPEDllApp

BEGIN_MESSAGE_MAP(CPEDllApp, CWinApp)
	//{{AFX_MSG_MAP(CPEDllApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*---------------------------------------------------------------------------
    ����:   EXE ��ȿ�� ���� �� DLL Hex Editing ��� ���
    ������: �ڼ���(crack90) 
    �ۼ���: 2004-12-22
    ����:   HEX Editing�� �����ϱ� ���ؼ� 
            äũ�Լ��� OPCODE �迭ó�� ��, ��ȣȭ �Ѵ�.
            
            - GetProcAdress()�� �̿��Ͽ� API�� ����ó���Ѵ�. ->API B.P ó�� ���             

            MFC�� GOTO��, ��ȣȭ�� ����� ����:
            - ����� �����̶�� MFC �Լ� ��ü�ε� ������� ���� �ɸ���.
            - GOTO ������ �����ٸ� �����ϱ� �����.
            - Ʈ���̽� ��, ���Ͽ��� Hex Editing�� �Ϸ��� �ص� OPCODE�� ã�� �� ����.
              (�޸� �������� ������ �ϳ�... �׷��ڸ� Application ���ߵ� ���������.)

---------------------------------------------------------------------------*/

// �޽��� ��� �Լ� �迭(OPCODE)
// 99������� 19ī��Ʈ�� NOP�� ä��� �޽��� ����� ���� �� �ִ�.

// ��ȣȭ ��Ų ���� OPCODE �迭 --> ��κ� ��ȣȭ�Ѵ�. 
BYTE ENCODING_DISPLAY_MESSAGE[] =
{
    0x55, 0x8B, 0xEC, 0x83, 0xEC, 0x58, 0x53, 0x56, 0x57, 0x8D, 0x7D, 0xA8, 0xB9, 0x16, 0x00,
	0x00, 0x00, 0xB8, 0xCC, 0xCC, 0xCC, 0xCC, 0xF3, 0xAB, 0xC6, 0x45, 0xF4, 0x41, 0xC6, 0x45,
	0xF5, 0x50, 0xC6, 0x45, 0xF6, 0x49, 0xC6, 0x45, 0xF7, 0x20, 0xC6, 0x45, 0xF8, 0x54, 0xC6,
	0x45, 0xF9, 0x45, 0xC6, 0x45, 0xFA, 0x53, 0xC6, 0x45, 0xFB, 0x54, 0xC6, 0x45, 0xFC, 0x00,
	0xC6, 0x45, 0xEC, 0x43, 0xC6, 0x45, 0xED, 0x41, 0xC6, 0x45, 0xEE, 0x50, 0xC6, 0x45, 0xEF,
	0x54, 0xC6, 0x45, 0xF0, 0x49, 0xC6, 0x45, 0xF1, 0x4F, 0xC6, 0x45, 0xF2, 0x4E, 0xC6, 0x45,
	0xF3, 0x00, 0x8B, 0x45, 0x08, 0x89, 0x45, 0xE8, 0x8B, 0xF4, 0x6A, 0x00, 0x8D, 0x4D, 0xEC,
	0x51, 0x8D, 0x55, 0xF4, 0x52, 0x6A, 0x00, 0xFF, 0x55, 0xE8, 0x3B, 0xF4, 
	0x90, 0x5F, 0x5E, 0x5B, 0x83, 0xC4, 0x58, 0x3B, 0xEC, 
	0x8B, 0xE5, 0x5D, 0xC3
};

/*



004011A0 55                   push        ebp
004011A1 8B EC                mov         ebp,esp
004011A3 83 EC 58             sub         esp,58h
004011A6 53                   push        ebx
004011A7 56                   push        esi
004011A8 57                   push        edi
004011A9 8D 7D A8             lea         edi,[ebp-58h]
004011AC B9 16 00 00 00       mov         ecx,16h
004011B1 B8 CC CC CC CC       mov         eax,0CCCCCCCCh
004011B6 F3 AB                rep stos    dword ptr [edi]
171:      char MSG[] = {'A', 'P', 'I', ' ', 'T', 'E', 'S', 'T', '\0'};
004011B8 C6 45 F4 41          mov         byte ptr [ebp-0Ch],41h
004011BC C6 45 F5 50          mov         byte ptr [ebp-0Bh],50h
004011C0 C6 45 F6 49          mov         byte ptr [ebp-0Ah],49h
004011C4 C6 45 F7 20          mov         byte ptr [ebp-9],20h
004011C8 C6 45 F8 54          mov         byte ptr [ebp-8],54h
004011CC C6 45 F9 45          mov         byte ptr [ebp-7],45h
004011D0 C6 45 FA 53          mov         byte ptr [ebp-6],53h
004011D4 C6 45 FB 54          mov         byte ptr [ebp-5],54h
004011D8 C6 45 FC 00          mov         byte ptr [ebp-4],0
172:      char CAP[] = {'C', 'A', 'P', 'T', 'I', 'O', 'N', '\0'};
004011DC C6 45 EC 43          mov         byte ptr [ebp-14h],43h
004011E0 C6 45 ED 41          mov         byte ptr [ebp-13h],41h
004011E4 C6 45 EE 50          mov         byte ptr [ebp-12h],50h
004011E8 C6 45 EF 54          mov         byte ptr [ebp-11h],54h
004011EC C6 45 F0 49          mov         byte ptr [ebp-10h],49h
004011F0 C6 45 F1 4F          mov         byte ptr [ebp-0Fh],4Fh
004011F4 C6 45 F2 4E          mov         byte ptr [ebp-0Eh],4Eh
004011F8 C6 45 F3 00          mov         byte ptr [ebp-0Dh],0
173:      typedef int (WINAPI *APIFUNC)(HWND, LPCSTR, LPCSTR, UINT);
174:
175:      APIFUNC api = (APIFUNC)pAPIAdress;
004011FC 8B 45 08             mov         eax,dword ptr [ebp+8]
004011FF 89 45 E8             mov         dword ptr [ebp-18h],eax
176:      api(NULL, MSG, CAP, MB_OK);
00401202 8B F4                mov         esi,esp
00401204 6A 00                push        0
00401206 8D 4D EC             lea         ecx,[ebp-14h]
00401209 51                   push        ecx
0040120A 8D 55 F4             lea         edx,[ebp-0Ch]
0040120D 52                   push        edx
0040120E 6A 00                push        0
00401210 FF 55 E8             call        dword ptr [ebp-18h]
00401213 3B F4                cmp         esi,esp
00401215 E8 36 02 00 00       call        __chkesp (00401450)
177:  }
0040121A 5F                   pop         edi
0040121B 5E                   pop         esi
0040121C 5B                   pop         ebx
0040121D 83 C4 58             add         esp,58h
00401220 3B EC                cmp         ebp,esp
00401222 E8 29 02 00 00       call        __chkesp (00401450)
00401227 8B E5                mov         esp,ebp
00401229 5D                   pop         ebp
0040122A C3                   ret

*/


//---------------------------------------------------------------------------
// ���� ����
//---------------------------------------------------------------------------
#define AUTH_STRING ".crack90"
#define BUF_SIZE 500
char SEC_DATA[BUF_SIZE] = {0,};
DWORD HiddenAPIFunc     = NULL;

//---------------------------------------------------------------------------
// DecodeFunctionBuffer() �Լ� 
// ����: XOR�� OPCODE�� ���ڵ��Ѵ�.
//
//---------------------------------------------------------------------------
void DecodeFunctionBuffer()
{
	/*
    int nSize = sizeof( ENCODING_DISPLAY_MESSAGE ) / sizeof( ENCODING_DISPLAY_MESSAGE[0] );
	for(int i = 0 ; i < nSize; i++){
        ENCODING_DISPLAY_MESSAGE[i] ^= ??;
	}
	*/
    
}

//---------------------------------------------------------------------------
// CheckAuthAPIAndExeute() �Լ� 
// ����: Ʈ���̽��� �����ϱ� ���ؼ� (ũ������) ����Ѵ�.
//       
//---------------------------------------------------------------------------
void CheckAuthAPIAndExeute()
{
   
    // GetModuleHandle�� GetModuleFileName�� ���� ����!!
    // 1) GetModuleHandle�� �ڽ��� ȣ���� ���������� �ִ� ��� �ڵ��� ã�´�.
    //    ��.. ����Ǵ� ��⿡�� �����ǰų� Loading�� ����ڵ鸸 ã�´�.(EX:Kernel.dll) 
    // 2) GetModuleFileName�� �ڽ� ����� ȣ�� ���������� �̿��Ͽ� ���ϸ��� ������ �� �� �ִ�.
    // 1) 2) ��� ���� CONTEXT ���� ���μ��� ���ÿ����� ������ ���̴�.
    
    // Ȯ���ڰ� ���� "���ϸ�"�� �����Ѵ�.
    // ������ ���� ���, �ش� ������ �����θ��� ������ ���̴�.
    // ���� ���� NULL�� �ǹǷ� ���� EXE�� �̸��� ������ ���̴�.

    // API��  B.P�� �ɾ� ������ �� �����Ƿ� 
    // ���ϴ� API�� �ּҰ��� �о�� ���� ó���Ѵ�.
    HiddenAPIFunc = (DWORD)GetProcAddress( GetModuleHandle( "USER32.dll" ), "MessageBox");     
    
    int result = 1;
    char FILENAME[MAX_PATH] = {0,};
	BYTE DUMMY1[1024] ={ 0xCC,};
        
    // �ڵ鰪�� �������� NULL ��� �ڵ鰪�� �Է�
    GetModuleFileName(NULL, FILENAME, MAX_PATH);   

    // ����
    CString pathName = FILENAME;
    CString m_edit_message;
    CString msg;
      
    // PE ���� �������� 
    OpenPEFile( pathName.GetBuffer( 0 ), true );
    LONGLONG offset = GetPEInfoOfFile();
    GetSectionInfofFile( offset, NULL);      
    ClosePEFile();      

    // section ���� �������� 
    OpenPEFile( pathName.GetBuffer( 0 ), true );
    offset   = GetPEInfoOfFile();
    DWORD msg_offset  = FindSection(AUTH_STRING);
      
    if( msg_offset != 0){
        if( !ReadSectionMessage(SEC_DATA, msg_offset, MAX_PATH) ){
            AfxMessageBox("����Ű�� äũ������ Ʋ���ϴ�.");
            goto EXIT;
        }
        m_edit_message = &SEC_DATA[1];
        
    } else {
        AfxMessageBox("����Ű�� �����ϴ�.");
        goto EXIT;
    }
      
    // PE ������ ���� ������ �´��� ���غ��ڱ���...
    msg.Format(
       "%X_AUTH_AH%X_DJ%X_CK%X_Q%X",
        PE_info.AddressOfEntryPoint,
        PE_info.ResourceDirectorySize,
        PE_info.TimeDateStamp,
        PE_info.SizeOfInitializedData,
        PE_info.ExportDirectorySize
    );	  
    
    
	// ������� ���⼭ ����
	result = strcmp( m_edit_message.GetBuffer(0), msg.GetBuffer(0)) ;
    
// Tracing�ϴٰ� ��ġ��� ���ߵ� ���� �ڵ� �Է�
// �Ʒ� �ڵ� ���󰡴� ���� result ������ ���ϴ� ������ 
// Ȯ���� �Ȱ��� �ȴ�.
STARTFAKE:

	result++;
	if(!result) {
        memset(DUMMY1, 0x00, 1024);
		for(int ii = result; ii < 1020; ii++){
		    DUMMY1[ii] = ii^0x30;
		}
	} else {
        memcpy(DUMMY1, ENCODING_DISPLAY_MESSAGE, result);
	}

	if(DUMMY1[result] > 0x10){
		for(int i = result; i < result + 130; i++){
		    DUMMY1[i] = i^0x30;
		}
	} else {
		for(int j = result; j < result + 230; j++){
		    DUMMY1[j] = j^0x30;
		}
	}
    result--;
	DUMMY1[result] = result;

REALCODE:
//-------------------------------------------------
// ���� OKó�� 
	if(!result) {
        // NOP�� �޽��� OPCODE ���� 
        memset(ENCODING_DISPLAY_MESSAGE + 98, 0x90, 19);
    }
//-------------------------------------------------
EXIT: 
ENDFAKE:
	
	for(int j = result; j < result + 30; j++){
        DUMMY1[j] = j^0x30;
	};
}


//---------------------------------------------------------------------------
// AlwayPrintMessage() �Լ� 
// ����: �Լ��迭�� ����Ѵ�.
//       ����ó���� �Ǿ��ٸ� ȣ���ϱ��� ��𼱰� OPCODE
//       �� ġȯ���� ���̴�.
//---------------------------------------------------------------------------
void AlwayPrintMessage()
{
    // ������ �޽��� ����ϱ� 
    void (*fnNoty)(DWORD);
    fnNoty = ( void (__cdecl*)(DWORD) )&ENCODING_DISPLAY_MESSAGE;
    
    // fnNoty((DWORD)MessageBox);
    // MessageBox�� B.P�� �ɸ� ���� �����Ƿ� API�� �ּҸ� �о�ͼ� ���� ó���Ѵ�.
    fnNoty(HiddenAPIFunc);
}
//---------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////
// CPEDllApp construction
CPEDllApp::CPEDllApp()
{    
    // DLL Entry���� �״�� ������ֽʽÿ�
    
	// ���������� �迭�� ��ȣȭ ó���Ѵ�.

	//DecodeFunctionBuffer();
    CheckAuthAPIAndExeute();
	AlwayPrintMessage();

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPEDllApp object

CPEDllApp theApp;
