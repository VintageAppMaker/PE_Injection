/*-------------------------------------------------------
    목적:   PE 정보 가공을 위한 함수 모음
    수정자: 박성완(crack90) 
    작성일: 2004-12-21
    참고:   귀찮다. 
            MFC 관련 함수로 작성
-------------------------------------------------------*/

#include "stdafx.h"
#include "PEHeader.h"

//-------------------------------------------------------
// PE 정보 변수
//-------------------------------------------------------
DOS_HEADER           DOS_info;
PE_HEADER            PE_info;
SECTION_HEADER       SECTION_Info;
IMPORT_DIRECTORY     IMPORT_info;	
IMAGE_THUNK          IMAGE_THUNK_info;


//-------------------------------------------------------
// 내부모듈 변수 
//-------------------------------------------------------
static CFile* f_pe = NULL;

//-------------------------------------------------------
// FileOpen
// 파일을 연다.
bool OpenPEFile(char* filename, bool ReadOnly)
{   
    bool rst = false;

    f_pe = new CFile();

    DWORD mode = (ReadOnly) ? CFile::modeRead : CFile::modeReadWrite;
    if(f_pe->Open(filename, mode ,NULL)) rst = true;
    return rst;         
}

//-------------------------------------------------------
// FileClose . 
// 파일을 닫는다.
void ClosePEFile()
{
    f_pe->Close();
    delete f_pe;
}


//-------------------------------------------------------
// Section 정보 가져오기 
// 해당 섹션이 있는지 검사한다.
// 리턴값:  
DWORD FindSection(char* section)
{
	
    // DOS 헤더정보 가져오기
	f_pe->Seek( 0, CFile::begin );
	f_pe->Read(&DOS_info,sizeof(DOS_HEADER));

	LONGLONG  lOffset = DOS_info.e_lfanew;

	// PE 헤더정보 가져오기
	f_pe->Seek( lOffset, CFile::begin );
	f_pe->Read(&PE_info,sizeof(PE_HEADER));

	if( PE_info.Signature != 0x4550)
        return 0;
	
    bool rst = false;
	for(int i  = 0; i < PE_info.NumberofSections; i++)	{
        f_pe->Read(&SECTION_Info, sizeof(SECTION_HEADER));
        if ( strcmp((char*)SECTION_Info.Name, section) == 0) rst = true;
	}
    
    return (rst == true) ? SECTION_Info.PointerToRawData : 0 ;      
}


//-------------------------------------------------------
// PE 정보 저장하기 
void SavePEHeader()
{
    f_pe->Seek(DOS_info.e_lfanew, CFile::begin );
	f_pe->Write(&PE_info,sizeof(PE_HEADER));
}

//-------------------------------------------------------
// read
int ReadSectionMessage(char* pBuf, int offset, int size)
{
    f_pe->Seek(offset, CFile::begin );
	f_pe->Read(pBuf, size);
    
    // Description 디코딩
    return DecodeData((BYTE*)pBuf, size);
}


//-------------------------------------------------------
// PE 정보 가져오기 
LONGLONG GetPEInfoOfFile()
{
    
	// DOS 헤더정보 가져오기
	f_pe->Seek( 0, CFile::begin );
	f_pe->Read(&DOS_info,sizeof(DOS_HEADER));

	LONGLONG  lOffset = DOS_info.e_lfanew;

	// PE 헤더정보 가져오기
	f_pe->Seek( lOffset, CFile::begin );
	f_pe->Read(&PE_info,sizeof(PE_HEADER));

	if( PE_info.Signature != 0x4550){
        f_pe->Close();
        return -1;
    }
    return ( lOffset + sizeof(PE_HEADER) );
}


//-------------------------------------------------------
// Section 정보 가져오기 
int GetSectionInfofFile(LONGLONG offset, cbSECTIONINFO cbPrint)
{
    f_pe->Seek( offset, CFile::begin );
    
	int SecCount;
	for(SecCount = 0;SecCount<PE_info.NumberofSections;SecCount++){
		f_pe->Read(&SECTION_Info,sizeof(SECTION_HEADER));
	    // Listctrl 삽입.
		if(cbPrint != NULL) cbPrint(SECTION_Info);		
	}

    return SecCount;
}

//-------------------------------------------------------
// Section 정보 저장하기 
int AddSection(LONGLONG offset, char* sec_name, char* description, int size)
{
    f_pe->Seek( offset, CFile::begin );
    
    UINT idata_rawOffset;
	int SecCount;
	for(SecCount = 0;SecCount<PE_info.NumberofSections;SecCount++){
		f_pe->Read(&SECTION_Info,sizeof(SECTION_HEADER));
    	
		// IMPORT_DESCRIPTOR 의 Raw Offset 구하기 
		if( PE_info.ImportDirectoryVA >= SECTION_Info.VirtualAddress)
			idata_rawOffset = (PE_info.ImportDirectoryVA - SECTION_Info.VirtualAddress) + SECTION_Info.PointerToRawData;
	}

	if(SecCount > 13)		
		return -1;
	
	// DLL 섹션의 dll 로딩갯수 구하기 ( import decsriptor 수 )
	int dllcount;
	f_pe->Seek(idata_rawOffset,CFile::begin);
	for(dllcount=0; dllcount<30; dllcount++) {
		f_pe->Read(&IMPORT_info,sizeof(IMPORT_DIRECTORY));
		if( IMPORT_info.FirstThunk == 0 && IMPORT_info.OriginalFirstThunk == 0) break;
	}	
    
	// DLL 로딩가능하도록 남기기 위한 section을 추가한다.
	InsertSection(sec_name ,0x100);	

    // Description 암호화
    EncodeData((BYTE*)description, size);
    
    // 섹션값이 자장된 FileOffset으로 이동 해서 값을 찍는다.
    f_pe->Seek (SECTION_Info.PointerToRawData,CFile::begin); 
	f_pe->Write(description,  size);   
    
}

//-------------------------------------------------------
// Section 정보 저장하기 
void InsertSection(char* Name, UINT size)
{
	char setSec = 0x00;
	
	//섹션수 및 이미지사이즈 수정 
	PE_info.NumberofSections += 1;
	PE_info.SizeOfImage += 0x1000;
    SavePEHeader();
	
	// 이전섹션에 대한 경계값까지 채우기
	UINT secRawOffset = SECTION_Info.SizeOfRawData+SECTION_Info.PointerToRawData;
	UINT endOffset    = f_pe->SeekToEnd();

	if(secRawOffset < SECTION_Info.PointerToRawData+PE_info.FileAlignment)
		secRawOffset = SECTION_Info.PointerToRawData+PE_info.FileAlignment;
	
	if( endOffset < secRawOffset ) { // 생성될 섹션까지의 여분 채우기 
		for(endOffset;endOffset <= secRawOffset ; endOffset++) {
			f_pe->Write(&setSec,sizeof(setSec));
		}
	}

	// 섹션공간 확보 (파일끝에 첨가)
	f_pe->Seek( secRawOffset , CFile::begin);
	for(int size_count=0;size_count < size ; size_count++)
		f_pe->Write(&setSec,sizeof(setSec));

    // 섹션테이블 삽입 
	UINT rva_cal = (UINT)(SECTION_Info.PhysicalAddress+SECTION_Info.VirtualAddress)%0x1000;
	UINT one_plus = 1;
	if( rva_cal == 0 ) 	one_plus = 0;
	
	memcpy(SECTION_Info.Name,Name,8);
	SECTION_Info.VirtualAddress = (((UINT)(SECTION_Info.PhysicalAddress+SECTION_Info.VirtualAddress)/0x1000)+one_plus)*0x1000;
	SECTION_Info.PhysicalAddress = size;
	SECTION_Info.SizeOfRawData = size;
	SECTION_Info.PointerToRawData = secRawOffset;
	SECTION_Info.NumberOfLineNumbers = 0;
	SECTION_Info.NumberOfRelocations = 0;
	SECTION_Info.PointerToLineNumbers = 0;
	SECTION_Info.PointerToRelocations = 0;
	SECTION_Info.Characteristics = 0xE0000060;
    f_pe->Seek(DOS_info.e_lfanew+sizeof(PE_HEADER)+(sizeof(SECTION_HEADER)*(PE_info.NumberofSections-1))
		, CFile::begin );
	f_pe->Write(&SECTION_Info,sizeof(SECTION_HEADER));	
}

//------------------------------------------------------
// 암호표 - lookup table
static int DICTIONARY[40] = {
    0x02,0xAC,0xF9,0x0C,0xA2,0x28,0x07,0x0A,0xF0,0x7B,
    0x81,0xAF,0xF3,0x13,0xAB,0xB1,0x40,0x79,0xE6,0x7C,
    0xF2,0xEC,0x29,0x9C,0xC2,0x28,0x67,0x8A,0x20,0x7B,
	0x81,0xDF,0x03,0x13,0xBB,0xB1,0x40,0x79,0xE6,0x7C
};

/*------------------------------------------------------
    목적: Data 인코딩
    In>  pData(Header + 데이터), nSize(인코딩할 크기)  
    Out> 관행상 0 리턴
------------------------------------------------------*/
int EncodeData(BYTE* pData, int nSize)
{
    BYTE bTemp = 0x00;
    
    //pData[0]   = 0x00;
    *pData = 0x00;
    
    for (int i = 0; i < nSize; i++)
        bTemp ^= pData[i];

    bTemp |= 0x40;
    *pData = bTemp; 

    // Data Encoding
    BYTE SEED = 0x2C;
    for (int j = 0; j < nSize; j++){ 
        pData[j] ^= SEED + DICTIONARY[j % 40]; 
        SEED++;        
    } 
    	
    return 0;	
}


/*------------------------------------------------------
    목적: Data 디코딩
    In>  pData(Header + 데이터), nSize(인코딩할 크기)  
    Out> 채크섬 코드의 유효성(TRUE 1: FALSE:0)
------------------------------------------------------*/
int DecodeData(BYTE* pData, int nSize)
{

    // Data Decoding
    BYTE SEED = 0x2C;
    for (int j = 0; j < nSize; j++){ 
        pData[j] ^= SEED + DICTIONARY[j % 40]; 
        SEED++;
    } 
    
    //pData[0]   = 0x00;
    BYTE bCheckSUm  = *pData;
    
    *pData = 0x00;
    BYTE bTemp = 0x00;
    
    for (int i = 0; i < nSize; i++)
        bTemp ^= pData[i];
     
    bTemp |= 0x40;
    
    return (bCheckSUm == bTemp);	
}