#include "framework.h"
#include "Lab01-02(GUI).h"
#include <wininet.h>
#include "shellapi.h"

#define MAX_LOADSTRING 100
#define srvName "Malservice"	//서비스 이름
#define mutexName "HGL345"		//뮤넥스 이름

// 전역 변수:
HWND g_hWnd;
HINSTANCE hInst;                                // 현재 인스턴스입니다.
WCHAR szTitle[MAX_LOADSTRING];                  // 제목 표시줄 텍스트입니다.
WCHAR szWindowClass[MAX_LOADSTRING];            // 기본 창 클래스 이름입니다.
CHAR lpOut[2048] = "";

BOOL g_bPause;
DWORD g_NowState;
DWORD g_NowAccept;
SERVICE_STATUS_HANDLE g_hSrv;

// 이 코드 모듈에 포함된 함수의 선언을 전달합니다:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);


BOOL InstallService(LPTSTR srvPath);
BOOL UninstallService(void);
BOOL ServiceStart(void);
void PrintText(LPCSTR str);
void ErrorExit(LPTSTR lpszFunction);
void WINAPI ServiceMain();
void WINAPI StartAddress();
void WINAPI ServiceHandler(DWORD opCode);
void MySetStatus(DWORD dwState, DWORD dwAccept);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	// 전역 문자열을 초기화합니다.
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_LAB0102GUI, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// 애플리케이션 초기화를 수행합니다:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	//사용법 (관리자 권한으로 실행해야함)
	PrintText("사용법 : 서비스 설치(i),  서비스 제거(u),  서비스 실행(s)");


	//서비스 실행구문 (이 프로그램이 서비스로 실행됬을때만 이 구문이 작동한다.)
	SERVICE_TABLE_ENTRY ste[] = {
		{(LPTSTR)srvName, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
		{NULL, NULL}
	};
	StartServiceCtrlDispatcher(ste);


    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_LAB0102GUI));

    MSG msg;

    // 기본 메시지 루프입니다:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_LAB0102GUI));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_LAB0102GUI);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // 인스턴스 핸들을 전역 변수에 저장합니다.

   HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);
   g_hWnd = hWnd;

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // 메뉴 선택을 구문 분석합니다:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
	case WM_CHAR:
		if (wParam == 0x69) { //i
			char Filename[MAX_PATH] = ""; //"" 만으로 전체 초기화

			GetModuleFileName(0, Filename, MAX_PATH);
			PrintText(Filename);

			InstallService(Filename);
		}else if (wParam == 0x75) { //u
			UninstallService();
			PrintText("이제 프로그램을 종료시키면 해당 서비스가 목록에서 완전히 제거됩니다.");
		}else if (wParam == 0x73) { //s
			ServiceStart();
		}

    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            // TODO: 여기에 hdc를 사용하는 그리기 코드를 추가합니다...
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

//서비스 함수
void WINAPI ServiceMain()
{
	//###서비스 핸들러를 등록한다.###
	g_hSrv = RegisterServiceCtrlHandler(srvName, (LPHANDLER_FUNCTION)ServiceHandler);
	if (g_hSrv == 0) {
		ErrorExit((LPTSTR)("RegisterServiceCtrlHandler"));
		return;
	}

	//###서비스가 시작중임을 알린다.###
	MySetStatus(SERVICE_START_PENDING, 3);

	
	//OpenMutex로 뮤텍스를 검사해서 만약 "HGL345" 뮤텍스가 이미 있다면 종료
	if (OpenMutex(MUTEX_ALL_ACCESS, 0, mutexName))
		ExitProcess(0);

	//뮤텍스를 생성 (동기화 보다는 중복을 막음)
	CreateMutex(NULL, FALSE, mutexName);


	//타이머 기반 동기화 : 정해진 시간이 되면 Signaled 상태가 된다.
	SYSTEMTIME SystemTime;
	FILETIME FileTime;

	//타이머 세팅
	SystemTime.wYear = 0;
	SystemTime.wDayOfWeek = 0;
	SystemTime.wHour = 0;
	SystemTime.wSecond = 0;
	SystemTime.wYear = 2100;
	
	//기존실행
	//SystemTimeToFileTime(&SystemTime, &FileTime);

	//빠른실행 (현재시간에서 3초후 실행으로 바꾼다.)
	GetLocalTime(&SystemTime);
	SystemTimeToFileTime(&SystemTime, &FileTime);
	DWORDLONG dwDateTime = ((DWORDLONG)FileTime.dwHighDateTime << 32) + FileTime.dwLowDateTime;
	dwDateTime = dwDateTime + (DWORDLONG)10000000 * 3;
	FileTime.dwHighDateTime = (DWORDLONG)(dwDateTime >> 32);
	FileTime.dwLowDateTime = (DWORDLONG)(dwDateTime & 0xFFFFFFFF);

	//SetWaitableTimer 의 2번째 인자에 (LARGE_INTEGER *)&FileTime 와 같이 FILETIME 구조체를 넣어도 실행이 안되서 결국 LARGE_INTEGER.QuadPart 을 이용한 상대시간으로 해결했음.
	//설정된 절대시간에서 현재시간을 빼 상대시간으로 만들고 LARGE_INTEGER 구조체의 QuadPart 에 마이너스 값으로 집어넣음.
	SYSTEMTIME SystemTimeNOW;
	FILETIME FileTimeNOW;
	GetLocalTime(&SystemTimeNOW);
	SystemTimeToFileTime(&SystemTimeNOW, &FileTimeNOW);
	DWORDLONG dueTime = (((DWORDLONG)FileTime.dwHighDateTime << 32) + FileTime.dwLowDateTime) - (((DWORDLONG)FileTimeNOW.dwHighDateTime << 32) + FileTimeNOW.dwLowDateTime);
	LARGE_INTEGER liDue;
	liDue.QuadPart = -1 * dueTime;

	//타이머 오브젝트 생성 
	//2번째 인자는 수동 리셋(TRUE)인지, 자동 리셋(FALSE)인지의 여부. 3번째 인자는 타임오브젝트 이름.
	HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, 0); 

	//알람 시간 설정 (2번째 인자는 알람이 울리는 시간, 3번째 인자는 설정시 주기적으로 Signaled 상태가 된다.)
	//정해진 시간이 되면 이 핸들(hTimer)은 Signaled 상태가 된다.
	SetWaitableTimer(hTimer, &liDue, 0, NULL, NULL, FALSE);

	//###서비스가 시작되었음을 알린다.###
	MySetStatus(SERVICE_RUNNING, 3);

	//(타이머)오브젝트가 Signaled 상태가 되기를 기다린다.
	if (WaitForSingleObject(hTimer, INFINITE)) //
	{
		//WAIT_FAILED
		return;
	}
	else
	{
		int count = 1; //코드에는 20

		do
		{
			HANDLE hThred = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)StartAddress, NULL, 0, NULL);
			if (hThred != NULL)
				WaitForSingleObject(hThred, INFINITE); //스레드가 끝날때 까지 대기

			--count;
		} while (count);
	}

	//###서비스가 멈추었음을 알린다.###
	MySetStatus(SERVICE_STOPPED, 3);

	return;
}

//스레드로 실행되는 함수
//엄청중요한 포인트!!! : "WINAPI" 안붙이면 ServiceMain 에서 함수 사용이 안됨!!! 스레드에서도 작동안함!!!
void WINAPI StartAddress()
{
	char szAgent[] = "Mozilla/5.0";
	char szUrl[] = "https://www.kitribob.kr/";

	HINTERNET hInternet = InternetOpen(szAgent, INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0);
	HINTERNET hURL = InternetOpenUrl(hInternet, szUrl, NULL, 0, INTERNET_FLAG_RELOAD, 0); //Forces a download of the requested file, object, or directory listing from the origin server, not from the cache.
	if (hURL == NULL) {
		InternetCloseHandle(hInternet);
		return;
	}

	char fileName[MAX_PATH] = "C:\\result.html"; //디렉토리 하드코딩
	//서비스에서 프로그램 동작시 현재 디렉토리는 "C:\Windows\System32" 으로 나온다... 그래서 GetCurrentDirectory 함수 대신 하드코딩을 했다.


	HANDLE hFile = CreateFile(fileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return;

	int size = 2048;
	DWORD *pBuffer = new DWORD[size]; //배열로 선언 하니까 오버플로우가 치명적임. 그래서 동적할당했음.
	DWORD dwReadAvailable = 0;	//서버가 한번에 보내줄 수 있는 크기
	DWORD dwReadedByte = 0;		//실제로 받아들인 데이터의 크기
	DWORD dwWritten = 0;		//실제로 기록된 바이트 크기

	do {
		InternetQueryDataAvailable(hURL, &dwReadAvailable, 0, 0);

		//오버플로우 방지 (dwReadAvailable 크기가 버퍼크기를 넘어가면 재할당)
		if (size < dwReadAvailable)
		{
			size = dwReadAvailable;
			pBuffer = new DWORD[size];
		}

		InternetReadFile(hURL, pBuffer, dwReadAvailable, &dwReadedByte);
		WriteFile(hFile, pBuffer, dwReadedByte, &dwWritten, NULL);
	} while (dwReadedByte != 0);

	InternetCloseHandle(hURL);
	CloseHandle(hFile);


	return;
}

//서비스의 현재 상태를 변경하는 함수
void MySetStatus(DWORD dwState, DWORD dwAccept)
{
	SERVICE_STATUS ss;
	ss.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	ss.dwCurrentState = dwState;
	ss.dwControlsAccepted = dwAccept;
	ss.dwWin32ExitCode = 0;
	ss.dwServiceSpecificExitCode = 0;
	ss.dwCheckPoint = 0;
	ss.dwWaitHint = 0;

	//현재 상태를 보관해 둔다.
	g_NowState = dwState;
	g_NowAccept = dwAccept;
	SetServiceStatus(g_hSrv, &ss);
}

//서비스 핸들러
void WINAPI ServiceHandler(DWORD fdwControl)
{
	if (fdwControl == g_NowState)
		return;

	switch (fdwControl)
	{
	case SERVICE_CONTROL_PAUSE:
		MySetStatus(SERVICE_PAUSE_PENDING, 0);
		g_bPause = TRUE;
		//to do
		MySetStatus(SERVICE_PAUSED, 3);
		break;

	case SERVICE_CONTROL_CONTINUE:
		MySetStatus(SERVICE_CONTINUE_PENDING, 0);
		g_bPause = FALSE;
		//to do
		MySetStatus(SERVICE_RUNNING, 3);
		break;

	case SERVICE_CONTROL_STOP:
		MySetStatus(SERVICE_STOP_PENDING, 0);
		//to do
		MySetStatus(SERVICE_STOPPED, 0);
		break;

	case SERVICE_CONTROL_INTERROGATE:
	default:
		MySetStatus(g_NowState, 3);
		break;
	}
}

//서비스 컨트롤 함수
BOOL InstallService(LPTSTR srvPath)
{
	PrintText("InstallService");

	SC_HANDLE hScm;
	SC_HANDLE hSrv;

	hScm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); //SC_MANAGER_CREATE_SERVICE
	if (hScm == FALSE)
	{
		ErrorExit((LPTSTR)("OpenSCManager"));
		return FALSE;
	}

	//서비스 설치 = 레지스터리에 서비스에 대한 정보를 써넣는다.
	hSrv = CreateService(
		hScm,
		srvName, //서비스 고유 이름  
		srvName, //서비스 외부 표시 이름
		SERVICE_ALL_ACCESS, // SERVICE_PAUSE_CONTINUE | SERVICE_CHANGE_CONFIG //SERVICE_ALL_ACCESS SERVICE_START
		SERVICE_WIN32_OWN_PROCESS, //SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS 
		SERVICE_AUTO_START, //SERVICE_DEMAND_START
		SERVICE_ERROR_IGNORE,
		srvPath,
		NULL, NULL, NULL, NULL, NULL);

	if (hSrv == FALSE)
	{
		ErrorExit((LPTSTR)("CreateService"));
		CloseServiceHandle(hSrv);
		return FALSE;
	}

	//CloseServiceHandle(hSrv);
	//CloseServiceHandle(hScm);

	return TRUE;
}
BOOL UninstallService(void)
{
	PrintText("UninstallService");
	SC_HANDLE hScm;
	SC_HANDLE hSrv;
	SERVICE_STATUS ss;

	hScm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); // full access rights 
	if (hScm == FALSE)
	{
		ErrorExit((LPTSTR)("OpenSCManager"));
		return FALSE;
	}

	hSrv = OpenService(hScm, srvName, SERVICE_ALL_ACCESS); //DELETE
	if (hSrv == FALSE)
	{
		if (GetLastError() != 1060)
			ErrorExit((LPTSTR)("OpenService"));
		CloseServiceHandle(hScm);
		return FALSE;
	}

	if (QueryServiceStatus(hSrv, &ss) == FALSE)
	{
		ErrorExit((LPTSTR)("QueryServiceStatus"));
		DeleteService(hSrv);
		CloseServiceHandle(hSrv);
		CloseServiceHandle(hScm);
		return FALSE;
	}

	if (ss.dwCurrentState != SERVICE_STOPPED)
	{
		ControlService(hSrv, SERVICE_CONTROL_STOP, &ss);
		Sleep(500);
	}

	if (DeleteService(hSrv) == FALSE)
	{
		ErrorExit((LPTSTR)("DeleteService"));
		CloseServiceHandle(hSrv);
		CloseServiceHandle(hScm);
		return FALSE;
	}

	CloseServiceHandle(hSrv);
	CloseServiceHandle(hScm);

	return TRUE;
}
BOOL ServiceStart(void)
{
	PrintText("ServiceStart");
	SC_HANDLE hScm;
	SC_HANDLE hSrv;

	hScm = OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS); // SC_MANAGER_CREATE_SERVICE
	if (hScm == FALSE)
	{
		ErrorExit((LPTSTR)("OpenSCManager"));
		return FALSE;
	}

	hSrv = OpenService(hScm, srvName, SERVICE_ALL_ACCESS); //SERVICE_START | SERVICE_QUERY_STATUS
	if (hSrv == FALSE)
	{
		ErrorExit((LPTSTR)("OpenService"));
		CloseServiceHandle(hScm);
		return FALSE;
	}

	SERVICE_STATUS ss;
	if (QueryServiceStatus(hSrv, &ss) == FALSE)
	{
		ErrorExit((LPTSTR)("OpenService"));
		CloseServiceHandle(hSrv);
		CloseServiceHandle(hScm);
		return FALSE;
	}

	if (StartService(hSrv, 0, NULL) == FALSE)
	{
		ErrorExit((LPTSTR)("StartService"));
		CloseServiceHandle(hSrv);
		CloseServiceHandle(hScm);
		return FALSE;
	}

	CloseServiceHandle(hSrv);
	CloseServiceHandle(hScm);
	return TRUE;
}

//텍스트 출력 함수
static int line = 10;
void ErrorExit(LPTSTR lpszFunction)
{
	TCHAR errmsg[1024];
	DWORD dw = GetLastError();

	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, dw, 0, errmsg, 1024, NULL);

	HDC hdc = GetDC(g_hWnd);
	wsprintf(lpOut, "%s failed with error %d : %s", lpszFunction, dw, errmsg);
	TextOut(hdc, 10, line, lpOut, lstrlen(lpOut));
	ReleaseDC(g_hWnd, hdc);

	line += 15;
}
void PrintText(LPCSTR str)
{
	HDC hdc = GetDC(g_hWnd);
	wsprintf(lpOut, "%s", str);
	TextOut(hdc, 10, line, lpOut, lstrlen(lpOut));
	ReleaseDC(g_hWnd, hdc);

	line += 15;
}

//TMI 외부에서 서비스 제어 가능하게 하기 //https://sisiblog.tistory.com/165
