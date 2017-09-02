/*-------------------------------------------------------
    ����:   PE ���� ������ ���� �Լ� ����
    ������: �ڼ���(crack90) 
    �ۼ���: 2004-12-21
    ����:   ������. 
            MFC ���� �Լ��� �ۼ�
-------------------------------------------------------*/

#include "stdafx.h"
#include "PEHeader.h"

//-------------------------------------------------------
// PE ���� ����
//-------------------------------------------------------
DOS_HEADER           DOS_info;
PE_HEADER            PE_info;
SECTION_HEADER       SECTION_Info;
IMPORT_DIRECTORY     IMPORT_info;	
IMAGE_THUNK          IMAGE_THUNK_info;


//-------------------------------------------------------
// ���θ�� ���� 
//-------------------------------------------------------
static CFile* f_pe = NULL;

//-------------------------------------------------------
// FileOpen
// ������ ����.
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
// ������ �ݴ´�.
void ClosePEFile()
{
    f_pe->Close();
    delete f_pe;
}


//-------------------------------------------------------
// Section ���� �������� 
// �ش� ������ �ִ��� �˻��Ѵ�.
// ���ϰ�:  
DWORD FindSection(char* section)
{
	
    // DOS ������� ��������
	f_pe->Seek( 0, CFile::begin );
	f_pe->Read(&DOS_info,sizeof(DOS_HEADER));

	LONGLONG  lOffset = DOS_info.e_lfanew;

	// PE ������� ��������
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
// PE ���� �����ϱ� 
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
    
    // Description ���ڵ�
    return DecodeData((BYTE*)pBuf, size);
}


//-------------------------------------------------------
// PE ���� �������� 
LONGLONG GetPEInfoOfFile()
{
    
	// DOS ������� ��������
	f_pe->Seek( 0, CFile::begin );
	f_pe->Read(&DOS_info,sizeof(DOS_HEADER));

	LONGLONG  lOffset = DOS_info.e_lfanew;

	// PE ������� ��������
	f_pe->Seek( lOffset, CFile::begin );
	f_pe->Read(&PE_info,sizeof(PE_HEADER));

	if( PE_info.Signature != 0x4550){
        f_pe->Close();
        return -1;
    }
    return ( lOffset + sizeof(PE_HEADER) );
}


//-------------------------------------------------------
// Section ���� �������� 
int GetSectionInfofFile(LONGLONG offset, cbSECTIONINFO cbPrint)
{
    f_pe->Seek( offset, CFile::begin );
    
	int SecCount;
	for(SecCount = 0;SecCount<PE_info.NumberofSections;SecCount++){
		f_pe->Read(&SECTION_Info,sizeof(SECTION_HEADER));
	    // Listctrl ����.
		if(cbPrint != NULL) cbPrint(SECTION_Info);		
	}

    return SecCount;
}

//-------------------------------------------------------
// Section ���� �����ϱ� 
int AddSection(LONGLONG offset, char* sec_name, char* description, int size)
{
    f_pe->Seek( offset, CFile::begin );
    
    UINT idata_rawOffset;
	int SecCount;
	for(SecCount = 0;SecCount<PE_info.NumberofSections;SecCount++){
		f_pe->Read(&SECTION_Info,sizeof(SECTION_HEADER));
    	
		// IMPORT_DESCRIPTOR �� Raw Offset ���ϱ� 
		if( PE_info.ImportDirectoryVA >= SECTION_Info.VirtualAddress)
			idata_rawOffset = (PE_info.ImportDirectoryVA - SECTION_Info.VirtualAddress) + SECTION_Info.PointerToRawData;
	}

	if(SecCount > 13)		
		return -1;
	
	// DLL ������ dll �ε����� ���ϱ� ( import decsriptor �� )
	int dllcount;
	f_pe->Seek(idata_rawOffset,CFile::begin);
	for(dllcount=0; dllcount<30; dllcount++) {
		f_pe->Read(&IMPORT_info,sizeof(IMPORT_DIRECTORY));
		if( IMPORT_info.FirstThunk == 0 && IMPORT_info.OriginalFirstThunk == 0) break;
	}	
    
	// DLL �ε������ϵ��� ����� ���� section�� �߰��Ѵ�.
	InsertSection(sec_name ,0x100);	

    // Description ��ȣȭ
    EncodeData((BYTE*)description, size);
    
    // ���ǰ��� ����� FileOffset���� �̵� �ؼ� ���� ��´�.
    f_pe->Seek (SECTION_Info.PointerToRawData,CFile::begin); 
	f_pe->Write(description,  size);   
    
}

//-------------------------------------------------------
// Section ���� �����ϱ� 
void InsertSection(char* Name, UINT size)
{
	char setSec = 0x00;
	
	//���Ǽ� �� �̹��������� ���� 
	PE_info.NumberofSections += 1;
	PE_info.SizeOfImage += 0x1000;
    SavePEHeader();
	
	// �������ǿ� ���� ��谪���� ä���
	UINT secRawOffset = SECTION_Info.SizeOfRawData+SECTION_Info.PointerToRawData;
	UINT endOffset    = f_pe->SeekToEnd();

	if(secRawOffset < SECTION_Info.PointerToRawData+PE_info.FileAlignment)
		secRawOffset = SECTION_Info.PointerToRawData+PE_info.FileAlignment;
	
	if( endOffset < secRawOffset ) { // ������ ���Ǳ����� ���� ä��� 
		for(endOffset;endOffset <= secRawOffset ; endOffset++) {
			f_pe->Write(&setSec,sizeof(setSec));
		}
	}

	// ���ǰ��� Ȯ�� (���ϳ��� ÷��)
	f_pe->Seek( secRawOffset , CFile::begin);
	for(int size_count=0;size_count < size ; size_count++)
		f_pe->Write(&setSec,sizeof(setSec));

    // �������̺� ���� 
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
// ��ȣǥ - lookup table
static int DICTIONARY[40] = {
    0x02,0xAC,0xF9,0x0C,0xA2,0x28,0x07,0x0A,0xF0,0x7B,
    0x81,0xAF,0xF3,0x13,0xAB,0xB1,0x40,0x79,0xE6,0x7C,
    0xF2,0xEC,0x29,0x9C,0xC2,0x28,0x67,0x8A,0x20,0x7B,
	0x81,0xDF,0x03,0x13,0xBB,0xB1,0x40,0x79,0xE6,0x7C
};

/*------------------------------------------------------
    ����: Data ���ڵ�
    In>  pData(Header + ������), nSize(���ڵ��� ũ��)  
    Out> ����� 0 ����
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
    ����: Data ���ڵ�
    In>  pData(Header + ������), nSize(���ڵ��� ũ��)  
    Out> äũ�� �ڵ��� ��ȿ��(TRUE 1: FALSE:0)
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