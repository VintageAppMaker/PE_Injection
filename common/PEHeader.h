/*-------------------------------------------------------
    ����:   PE ���� ������ ���� �Լ� ����
    ������: �ڼ���(crack90) 
    �ۼ���: 2004-12-21
    ����:   ������. 
            MFC ���� �Լ��� �ۼ�
-------------------------------------------------------*/
#ifndef __PSW__SNAKE_PEINFO
#define __PSW__SNAKE_PEINFO

// PE ���� �ش� ���� ��Ʈ����
// �������� �ҽ����� �� �����ؼ� 
// ����ϴ� ����.

#pragma pack(push, 1)

// DOS Header
typedef struct _DOS_HEADER {
	WORD e_magic;
	WORD e_cblp;
	WORD e_cp;
	WORD e_crlc;
	WORD e_cparhdr ;
	WORD e_minalloc;
	WORD e_maxalloc;
	WORD e_ss;
	WORD e_sp;
	WORD e_csum;
	WORD e_ip;
	WORD e_cs;
	WORD e_lfarlc;
	WORD e_ovno;
	WORD e_res[4];
	WORD e_oemid;
	WORD e_oeminfo ; 
	WORD e_res2[10];
	DWORD e_lfanew;
}DOS_HEADER;

// PE �����ش� 
typedef struct _PE_HEADER {
	DWORD Signature; 
	WORD Machine;  
	WORD NumberofSections;  
	DWORD TimeDateStamp;  
	DWORD PointerToSymbolTable;  
	DWORD NumberOfSymbols;  
	WORD SizeOfOptionalHeader;  
	WORD Characteristics;

	WORD Magic;
	BYTE MajorLinkerVersion;
	BYTE MinorLinkerVersion;
	DWORD SizeOfCode;
	DWORD SizeOfInitializedData;
	DWORD SizeOfUnitializedData;
	DWORD AddressOfEntryPoint;
	DWORD BaseOfCode;
	DWORD BaseOfData;
	DWORD ImageBase;
	DWORD SectionAlignment;
	DWORD FileAlignment;
	WORD MajorOperatingSystemVersion;
	WORD MinorOperatingSystemVersion;
	WORD MajorImageVersion;
	WORD MinorImageVersion;
	WORD MajorSubsystemVersion;
	WORD MinorSubsystemVersion;
	DWORD Reserved1;
	DWORD SizeOfImage;
	DWORD SizeOfHeaders;
	DWORD CheckSum;
	WORD Subsystem;
	WORD DllCharacteristics;
	DWORD SizeOfStackReserve;
	DWORD SizeOfStackCommit;
	DWORD SizeOFHeapReserve;
	DWORD SizeOfHeapCommit;
	DWORD LoaderFlags;
	DWORD NumberOfRvaAndSizes;
	DWORD ExportDirectoryVA;
	DWORD ExportDirectorySize;
	DWORD ImportDirectoryVA;
	DWORD ImportDirectorySize;
	DWORD ResourceDirectoryVA;
	DWORD ResourceDirectorySize;
	DWORD ExceptionDirectoryVA;
	DWORD ExceptionDirectorySize;
	DWORD SecurityDirectoryVA;
	DWORD SecurityDirectorySize;
	DWORD BaseRelocationTableVA;
	DWORD BaseRelocationTableSize;
	DWORD DebugDirectoryVA;
	DWORD DebugDirectorySize;
	DWORD ArchitectureSpecificDataVA;
	DWORD ArchitectureSpecificDataSize;
	DWORD RVAofGPVA;
	DWORD RVAofGPSize;
	DWORD TLSDirectoryVA;
	DWORD TLSDirectorySize;
	DWORD LoadConfigurationDirectoryVA;
	DWORD LoadConfigurationDirectorySize;
	DWORD BoundImportDirectoryinheadersVA;
	DWORD BoundImportDirectoryinheadersSize; 
	DWORD ImportAddressTableVA; 
	DWORD ImportAddressTableSize;
	DWORD DelayLoadImportDescriptorsVA;
	DWORD DelayLoadImportDescriptorsSize;
	DWORD COMRuntimedescriptorVA;
	DWORD COMRuntimedescriptorSize;
	DWORD e1;
	DWORD e2;

}PE_HEADER;

// Section ����
typedef struct _SECTION_HEADER {
	BYTE Name[8];  
	DWORD PhysicalAddress;  
	DWORD VirtualAddress;  
	DWORD SizeOfRawData;  
	DWORD PointerToRawData;  
	DWORD PointerToRelocations;  
	DWORD PointerToLineNumbers;  
	WORD NumberOfRelocations;  
	WORD NumberOfLineNumbers;  
	DWORD Characteristics;
}SECTION_HEADER;

// .idata ������ ����Ʈ �Լ� ���� 
typedef struct _IMPORT_DIRECTORY {
	DWORD OriginalFirstThunk  ;
	DWORD TimeDateStamp ; 
	DWORD ForwarderChain;  
	DWORD Name ; 
	DWORD FirstThunk ;
}IMPORT_DIRECTORY;

// .image ûũ ���� 
typedef struct _IMAGE_THUNK { 
   DWORD funcname;
   DWORD etc;
} IMAGE_THUNK;

#pragma pack(pop)

//-------------------------------------------------------
// PE ���� ����
//-------------------------------------------------------
extern DOS_HEADER           DOS_info;
extern PE_HEADER            PE_info;
extern SECTION_HEADER       SECTION_Info;
extern IMPORT_DIRECTORY     IMPORT_info;	
extern IMAGE_THUNK          IMAGE_THUNK_info;

// GetSectionInfo ���� callback �Լ� - ������� 
typedef void (*cbSECTIONINFO)(SECTION_HEADER);

extern "C" {
    int  GetSectionInfofFile(LONGLONG offset, cbSECTIONINFO cbPrint);
    int  AddSection(LONGLONG offset, char* sec_name, char* description, int size);
    void InsertSection(char* Name, UINT size);
    void SavePEHeader();
    LONGLONG  GetPEInfoOfFile();
    DWORD FindSection(char* section);
    void ClosePEFile();
    bool OpenPEFile(char* filename, bool ReadOnly = false);
    int  ReadSectionMessage(char* pBuf, int offset, int size);

    int EncodeData(BYTE* pData, int nSize);
    int DecodeData(BYTE* pData, int nSize);
 
}
 
#endif