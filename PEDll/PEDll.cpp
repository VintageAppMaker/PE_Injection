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
    목적:   EXE 유효성 검증 및 DLL Hex Editing 방어 모듈
    수정자: 박성완(crack90) 
    작성일: 2004-12-22
    참고:   HEX Editing을 방지하기 위해서 
            채크함수를 OPCODE 배열처리 후, 암호화 한다.
            
            - GetProcAdress()를 이용하여 API를 변수처리한다. ->API B.P 처리 방어             

            MFC와 GOTO문, 암호화를 사용한 이유:
            - 어셈블리 수준이라면 MFC 함수 자체로도 디버깅이 한참 걸린다.
            - GOTO 문으로 점프뛰면 추적하기 힘들다.
            - 트레이싱 후, 파일에서 Hex Editing을 하려고 해도 OPCODE를 찾을 수 없다.
              (메모리 에디팅이 가능은 하나... 그러자면 Application 개발도 힘들어진다.)

---------------------------------------------------------------------------*/

// 메시지 출력 함수 배열(OPCODE)
// 99번재부터 19카운트를 NOP로 채우면 메시지 출력을 없앨 수 있다.

// 암호화 시킨 않은 OPCODE 배열 --> 대부분 암호화한다. 
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
// 인증 섹션
//---------------------------------------------------------------------------
#define AUTH_STRING ".crack90"
#define BUF_SIZE 500
char SEC_DATA[BUF_SIZE] = {0,};
DWORD HiddenAPIFunc     = NULL;

//---------------------------------------------------------------------------
// DecodeFunctionBuffer() 함수 
// 목적: XOR한 OPCODE를 디코딩한다.
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
// CheckAuthAPIAndExeute() 함수 
// 목적: 트레이싱을 방해하기 위해서 (크랙방지) 사용한다.
//       
//---------------------------------------------------------------------------
void CheckAuthAPIAndExeute()
{
   
    // GetModuleHandle과 GetModuleFileName에 대한 이해!!
    // 1) GetModuleHandle은 자신을 호출한 스택정보에 있는 모듈 핸들을 찾는다.
    //    즉.. 실행되는 모듈에서 참조되거나 Loading된 모듈핸들만 찾는다.(EX:Kernel.dll) 
    // 2) GetModuleFileName은 자신 모듈의 호출 스택정보를 이용하여 파일명을 가지고 올 수 있다.
    // 1) 2) 모두 같은 CONTEXT 내의 프로세스 스택에서만 가능한 것이다.
    
    // 확장자가 붙은 "파일명"만 정의한다.
    // 파일이 있을 경우, 해당 파일의 절대경로명을 가져올 것이다.
    // 없을 경우는 NULL이 되므로 최종 EXE의 이름을 가져올 것이다.

    // API에  B.P를 걸어 추적할 수 있으므로 
    // 원하는 API의 주소값을 읽어와 변수 처리한다.
    HiddenAPIFunc = (DWORD)GetProcAddress( GetModuleHandle( "USER32.dll" ), "MessageBox");     
    
    int result = 1;
    char FILENAME[MAX_PATH] = {0,};
	BYTE DUMMY1[1024] ={ 0xCC,};
        
    // 핸들값을 가져오면 NULL 대신 핸들값을 입력
    GetModuleFileName(NULL, FILENAME, MAX_PATH);   

    // 선언
    CString pathName = FILENAME;
    CString m_edit_message;
    CString msg;
      
    // PE 정보 가져오기 
    OpenPEFile( pathName.GetBuffer( 0 ), true );
    LONGLONG offset = GetPEInfoOfFile();
    GetSectionInfofFile( offset, NULL);      
    ClosePEFile();      

    // section 정보 가져오기 
    OpenPEFile( pathName.GetBuffer( 0 ), true );
    offset   = GetPEInfoOfFile();
    DWORD msg_offset  = FindSection(AUTH_STRING);
      
    if( msg_offset != 0){
        if( !ReadSectionMessage(SEC_DATA, msg_offset, MAX_PATH) ){
            AfxMessageBox("인증키의 채크섬값이 틀립니다.");
            goto EXIT;
        }
        m_edit_message = &SEC_DATA[1];
        
    } else {
        AfxMessageBox("인증키가 없습니다.");
        goto EXIT;
    }
      
    // PE 정보와 섹션 정보가 맞는지 비교해보자구나...
    msg.Format(
       "%X_AUTH_AH%X_DJ%X_CK%X_Q%X",
        PE_info.AddressOfEntryPoint,
        PE_info.ResourceDirectorySize,
        PE_info.TimeDateStamp,
        PE_info.SizeOfInitializedData,
        PE_info.ExportDirectorySize
    );	  
    
    
	// 결과값은 여기서 저장
	result = strcmp( m_edit_message.GetBuffer(0), msg.GetBuffer(0)) ;
    
// Tracing하다가 지치라고 쓰잘데 없는 코드 입력
// 아래 코드 따라가다 보면 result 변수가 뭐하는 놈인지 
// 확신이 안가게 된다.
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
// 실제 OK처리 
	if(!result) {
        // NOP로 메시지 OPCODE 삭제 
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
// AlwayPrintMessage() 함수 
// 목적: 함수배열을 출력한다.
//       인증처리가 되었다면 호출하기전 어디선가 OPCODE
//       를 치환했을 것이다.
//---------------------------------------------------------------------------
void AlwayPrintMessage()
{
    // 무조건 메시지 출력하기 
    void (*fnNoty)(DWORD);
    fnNoty = ( void (__cdecl*)(DWORD) )&ENCODING_DISPLAY_MESSAGE;
    
    // fnNoty((DWORD)MessageBox);
    // MessageBox로 B.P를 걸면 쉽게 잡히므로 API의 주소를 읽어와서 변수 처리한다.
    fnNoty(HiddenAPIFunc);
}
//---------------------------------------------------------------------------


/////////////////////////////////////////////////////////////////////////////
// CPEDllApp construction
CPEDllApp::CPEDllApp()
{    
    // DLL Entry에서 그대로 사용해주십시오
    
	// 현업에서는 배열로 암호화 처리한다.

	//DecodeFunctionBuffer();
    CheckAuthAPIAndExeute();
	AlwayPrintMessage();

}

/////////////////////////////////////////////////////////////////////////////
// The one and only CPEDllApp object

CPEDllApp theApp;
