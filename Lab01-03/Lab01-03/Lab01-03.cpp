#include <iostream>
#include "ole2.h"
#include <ExDisp.h> //IWebBrowser2
#include <atlstr.h> //CString 

#pragma region COM 이란

//용어 설명
//CoClass : Component Object Class, COM 객체의 클래스
//COM Object : CoClass가 인스턴스화 되어서 메모리에 생성된것
//CLSID : CoClass의 GUID ( HKEY_CLASSES_ROOT\CLSID 경로에서 확인)
//IID : 인터페이스의 GUID ( HKEY_CLASSES_ROOT\Interface 경로에서 확인)


//COM (Component Object Model)
//COM은 일종의 규약입니다. COM은 C/C++이나 베이직 등의 언어도 아니고 MFC, API 등의 라이브러리도 아닙니다. 
//COM은 컴포넌트와 이를 사용하는 클라이언트 간에 상호 통신할 수 있는 표준화된 방법을 규정한 사양입니다. 
//따라서 COM 컴포넌트는 다음과 같은 조건을 만족해야 합니다.
// 1. COM 컴포넌트는 반드시 하나 이상의 인터페이스를 갖추어야 합니다. 그리고 각 인터페이스는 하나 이상의 Method를 갖추어야 합니다.
// 2. COM 컴포넌트는 기존의 인터페이스를 수정하거나 삭제해서는 안됩니다. 변경사항이 있거나 새로운 기능을 추가해야 할 시에는 새로운 인터페이스를 추가해 줌으로써 계속적으로 진화 되는 형태로 사용되어야 합니다.이는 호환성 및 유지보수에 많은 이점을 줍니다.


//COM 컴포넌트
//COM 규약에 맞게 작성된 DLL, EXE 형태의 단위 소프트웨어 모듈을 말합니다. 
//COM 컴포넌트는 In-Process, Out-Process, DCOM 3가지 모델로 구분되며 하나 이상의 인터페이스와 각 인터페이스 안에 하나 이상의 Method를 갖추어야 합니다. 
//모든 COM 컴포넌트는 윈도우 레지스트리에 등록되며 128비트의 숫자로 이루어진 고유 식별자 GUID를 사용합니다.


//함수와 중요 인터페이스 설명
//GetClassObject() : 원하는 COM 컴포넌트를 윈도우 레지스트리에서 찾아 이를 메모리에 올린 후 클래스 팩토리의 객체를 생성하여 클래스 팩토리 안의 인터페이스 포인터(IClassFactory)를 넘겨주는 역할을 합니다.
//IClassFactory 인터페이스 : COM 컴포넌트의 객체 생성을 담당합니다. IClassFactory ::CreateInstance() 는 COM 컴포넌트의 객체 생성을 한 뒤 객체의 IUnknown 인터페이스 포인터를 반환합니다.
//IUnknown 인터페이스 : 모든 인터페이스의 근간이 되며, COM 컴포넌트 안에 원하는 인터페이스 포인터를 동적으로 얻기 위한 QueryInterface 와 AddRef, Release 메서드를 포함합니다.


#pragma region Out-Process 모델 (COM 컴포넌트의 형태 ".EXE")
//COM 컴포넌트가 사용되는 프로그램과 동일한 프로세스 영역에는 있지 않지만 동일한 시스템내에 서로 다른 프로세스 영역에 존재하여 LPC(Local Procedure Call)를 통해 상호 통신하는 모델을 말합니다.


#pragma endregion Out-Process 모델 (COM 컴포넌트의 형태 ".EXE")

#pragma region In-Process 모델 (COM 컴포넌트의 형태 ".DLL")
//COM 컴포넌트가 사용되는 프로그램과 동일한 프로세스 영역내에 존재하여 상호 통신하는 모델을	말합니다.

//클라이언트 -> COM Library
//CoCreateInstance 함수를 호출해서 COM 컴포넌트 객체(COM Object) 생성을 시도하며 함수 내부에서는 CoGetClassObject 라는 COM Library 함수를 호출한다.
//두 함수는 CLSID(생성시키고자 하는 COM 컴포넌트의 클새스 ID)를 인자로 받아서 실행된다.

//COM Library -> DLL
//CoGetClassObject 가 실행되면 COM Library는 레지스터리(HKEY_CLASSES_ROOT\CLSID)에서 CLSID에 해당되는 DLL을 찾는다.
//Dll을 찾으면 COM Library는 해당 Dll의 경로를 검색해서 메모리에 로드시킨다.

//DLL -> Class Factory
//해당 Dll의 Export 함수인 DllGetClassObject 함수를 실행시킨다.(이때 clsid, iid, ppv를 인자로 가진다)
//DllGetClassObject 함수는 Class Factory 객체를 생성시킨다.

//Class Factory -> 클라이언트
//생성된 Class Factory 객체는 객체에 연결되어 있는 IClassFactory 인터페이스의 포인터를 반환한다.
//반환된 포인터는 CoGetClassObject 함수를 거쳐 CoCreateinstance 함수로 다시 반환된다.

//클라이언트 -> Class Factory
//IClassFactory 인터페이스 포인터를 사용해서 Class Factory의 IClassFactory::CreateInstance 함수를 호출한다.
//IClassFactory::CreateInstance 함수는 CoCreateInstance 함수를 사용하면 COM Library가 자동으로 호출해준다.
//IClassFactory::CreateInstance 함수는 IClassFactory 인터페이스의 핵심 멤버 메서드로 다수의 객체(COM Object)를 생성할 수 있게하는 역할을 한다. (그래서 클래스 팩토리라는 이름이 붙는다.)

//Class Factory -> Component 생성
//IClassFactory::CreateInstance 함수는 해당 COM 컴포넌트의 객체(COM Object)를 생성하고 객체의 IUnknown 인터페이스 포인터를 반환한다.
//IUnknown 인터페이스 포인터를 이용하여 IUnknown::QueryInterface 함수를 통해 인터페이스의 포인터를 얻어와 원하는 Method를 호출한다.
//COM 컴포넌트 사용을 마치면 각 인터페이스의 IUnknown::Release 함수를 호출하여 COM 컴포넌트가 자체적으로 제거될 수 있도록 합니다.
#pragma endregion In-Process 모델 (COM 컴포넌트의 형태 ".DLL")

#pragma region DCOM 모델
//COM 컴포넌트가 서로 다른 시스템, 즉 서로 다른 컴퓨터상에서 존재하여 DCOM 네트워크 프로토콜을 통해 상호 통신하는 모델을 말합니다.
#pragma endregion DCOM 모델

#pragma endregion

#pragma region Lab01-03 분석내용
/*
Lab01-03 은 FSG 패커로 패킹되어 있다.
FSG 패커는 Ibsen Software 에서 제공하는 plib.lib 라이브러리를 사용해서 "코드부분"은 암호화시키고
"IMPORT TABLE"은 INT의 NAME을 빈섹션에(정확히는 2160) 하드코딩시킨뒤 기존의 IMPORT 관련코드를 전부지우고 KERNEL32.DLL 의 LoadLibrary와 GetProcAddress 함수만 참조하도록 한다.
실행파일의 복호화코드 뒷부분에는 LoadLibrary와 GetProcAddress을 이용한 루프문이 있으며, 이 루프문은 INT의 NAME을 참조하여 직접 Dll과 함수를 로드시키는 코드이다.
복호화코드를 전부 거치게 되면 앞의 RVA값으로

1000은 Main함수
1090은 OEP: Microsoft Visual C++ 6.0의 시작부분
2000은 GetProcAddress 함수의 리턴값으로 로드된 DLL안의 함수들의 시작주소
2059~2068 CoCreateInstance의 rclsid {0002DF01-0000-0000-C000-000000000046} (C:\Program Files\Internet Explorer\IEXPLORE.EXE)
2068~2069 CoCreateInstance의 riid {D30C1661-CDAF-11D0-8A3E-00C04FC9E26E} (IWebBrowser2 Interface)
207A~2084 의미불명
2160은 LoadLibrary와 GetProcAddress 함수의 인자값으로 넣을 NAME 값들
3010은 인터넷 주소값

으로 복호화된다.
4000, 5000은 암호화된 코드가 있는 섹션으로 복호화시킨뒤 덤프를 한 상태라면 지워도 무방하다.

디버거 내부에서 SFX 설정이나 OEP에 하드브레이크를 걸고 실행시킨후 dump 시키면 코드는 복호화 되었지만 OEP가 앞서 말한 루프문보다 앞에 있음으로 실행이 안된다.
실행되기 위해서는 크게 2가지방법이 있다.
1. 기존의 OEP를 앞의 루프문 시작부분으로 바꾼다.
2. ImpREC 툴을 이용해 덤프에 IMPORT 테이블을 만들어준다.
이중 ImpREC 툴을 사용하는 방법에 대해 설명해보겠다.

ImpREC 툴은 현재 실행중인 프로그램에 로드돼있는 Dll과 함수를 탐지해서 직접 IAT를 만들어주는 툴이다.
실행중인 프로그램을 Attach 시키고 OEP 코드를 집어고 IAT 값까지 확인한뒤(기본으로 세팅되는 IAT는 섹션안에 Dll의 함수주소가 하드코딩되있는곳을 찾으면 그곳이 IAT라고 분별하는것 같음)
"Get Imports" 버튼을 누르면 Imported된 함수들이 화면에 뜨고 "Fix Dump" 버튼을 눌러서 기존의 Dump에 덮어씌우면 된다.
새로바뀐 Dump는 옵션에 따라 파일헤더의 섹션 수가 하나더 늘고 옵션헤더의 이미지크기가 증가하며 섹션이 추가된다.
또한 세팅된 IAT RVA에 IAT가 생기고, 추가된 섹션안에 IDT와 Name 값이 생긴다
*/
#pragma endregion


//기존코드
#if 0
BOOL myGUIDFromString(LPCTSTR psz, LPGUID pguid);

int main()
{
	char strCLSID[] = "{0002DF01-0000-0000-C000-000000000046}"; //"C:\Program Files\Internet Explorer\IEXPLORE.EXE"
	char strIID[] = "{D30C1661-CDAF-11D0-8A3E-00C04FC9E26E}";  //IWebBrowser2 Interface //IID_IWebBrowser2
	GUID myCLSID;
	IID myIID;
	
	myGUIDFromString(strCLSID, &myCLSID);
	myGUIDFromString(strIID, &myIID);

	REFCLSID rclsid = myCLSID;	
	REFIID riid = myIID;		
	IWebBrowser2 *pWebBrowser; //VARIANTARG pvarg; //VARIANT 타입은 어떤 타입이든 포함할 수 있는 타입 
	

	//OLE객체에 접근하기 위해서 하는 사전 정비 작업 같은 함수. OLE객체를 사용하겠다는 뜻을 시스템에 이야기 하고 허가
	if (OleInitialize(NULL) >= 0)
	{
		HRESULT hr = CoCreateInstance(rclsid, NULL, CLSCTX_LOCAL_SERVER, riid, (void**)&pWebBrowser);

		if (FAILED(hr) || !pWebBrowser)
			return 0;

		//VariantInit(&pvarg); //필요없음

		OLECHAR *olechar = SysAllocString(L"https://www.kitribob.kr/");

		VARIANT vtFlags, vtTarget, vtPostData, vtHeader;
		::VariantInit(&vtFlags);
		::VariantInit(&vtTarget);
		::VariantInit(&vtPostData);
		::VariantInit(&vtHeader);

		pWebBrowser->put_Visible(VARIANT_TRUE);
		pWebBrowser->Navigate(olechar, &vtFlags, &vtTarget, &vtPostData, &vtHeader);

		SysFreeString(olechar);
		OleUninitialize();
	}

	return 0;
}

#ifdef UNICODE
#define MAKEINTRESOURCEA_T(a, u) MAKEINTRESOURCEA(u)
#else
#define MAKEINTRESOURCEA_T(a, u) MAKEINTRESOURCEA(a)
#endif

BOOL myGUIDFromString(LPCTSTR psz, LPGUID pguid)
{
	BOOL bRet = FALSE;

	typedef BOOL(WINAPI *LPFN_GUIDFromString)(LPCTSTR, LPGUID);
	LPFN_GUIDFromString pGUIDFromString = NULL;

	HINSTANCE hInst = LoadLibrary(TEXT("shell32.dll"));
	if (hInst)
	{
		pGUIDFromString = (LPFN_GUIDFromString)GetProcAddress(hInst, MAKEINTRESOURCEA_T(703, 704));
		if (pGUIDFromString)
			bRet = pGUIDFromString(psz, pguid);
		FreeLibrary(hInst);
	}

	if (!pGUIDFromString)
	{
		hInst = LoadLibrary(TEXT("Shlwapi.dll"));
		if (hInst)
		{
			pGUIDFromString = (LPFN_GUIDFromString)GetProcAddress(hInst, MAKEINTRESOURCEA_T(269, 270));
			if (pGUIDFromString)
				bRet = pGUIDFromString(psz, pguid);
			FreeLibrary(hInst);
		}
	}

	return bRet;
}

#endif

//변경된 코드
//OLECHAR 대신 CString 을 사용해서 SysAllocString(), SysFreeString() 함수 제거
//ExDisp.h 헤더에 있는 GUID 상수 값으로 rclsid, riid를 생성과 동시에 초기화 (덕분에 GUIDFromString()를 불러오는 함수코드제거)
//CoCreateInstance 의 인자중에서 "riid" 는 "얻고자하는 컨퍼넌트의 인터페이스" 이고, "마지막 인자" 는 "인터페이스 포인터를 받는 변수" 이다.
//기존코드의 riid(얻고자하는 컨퍼넌트의 인터페이스)는 IID_IWebBrowser2 인데 그 포인터값을 "IWebBrowser2 인터페이스 포인터 변수" 에 담았다. (문제는 없지만 IUnknown 인터페이스의 QueryInterface 메서드를 통해 IWebBrowser2 인터페이스 포인터를 받아오는 작동방식을 배울 수 없다.)
//변경된 코드는 riid(얻고자하는 컨퍼넌트의 인터페이스)를 IID_IUnknown 으로 설정하고, "인터페이스 포인터를 받는 변수" 는 "IUnknown 인터페이스 포인터 변수" 로 설정
//따라서 IUnknown 인터페이스의 QueryInterface 메서드를 통해 IWebBrowser2 인터페이스 포인터를 얻는 과정을 추가
#if 0
int main()
{
	REFCLSID rclsid = CLSID_InternetExplorer;
	REFIID riid = IID_IUnknown;
	IUnknown *pI;
	IWebBrowser2 *pWebBrowser;

	if (OleInitialize(NULL) >= 0)
	{
		HRESULT hr = CoCreateInstance(rclsid, NULL, CLSCTX_LOCAL_SERVER, riid, (void**)&pI);

		if (FAILED(hr) || !pI)
			return 0;

		CString strAddress = "https://www.kitribob.kr/";
		
		hr = pI->QueryInterface(IID_IWebBrowser2, (void**)&pWebBrowser);
		if (FAILED(hr))
			return 0;

		VARIANT vtFlags, vtTarget, vtPostData, vtHeader;
		VariantInit(&vtFlags);
		VariantInit(&vtTarget);
		VariantInit(&vtPostData);
		VariantInit(&vtHeader);

		pWebBrowser->put_Visible(VARIANT_TRUE);
		pWebBrowser->Navigate(strAddress.AllocSysString(), &vtFlags, &vtTarget, &vtPostData, &vtHeader);

		pWebBrowser->Release();
		OleUninitialize();
	}

	return 0;
}
#endif

//개선된 코드
//OleInitialize() 대신에 CoInitialize() 를 사용해봄.
//rclsid, riid 변수자체를 제거시키고 인자에 ExDisp.h 헤더에 있는 GUID 상수 값을 바로 넣음
//IUnknown 인터페이스 제거. CoCreateInstance 의 "얻고자하는 컨퍼넌트의 인터페이스" 를 "IID_IWebBrowser2" 으로 설정하고, "인터페이스 포인터를 받는 변수" 는 "IWebBrowser2 인터페이스 포인터 변수" 로 설정
//따라서 IUnknown 인터페이스의 QueryInterface 메서드를 통해 IWebBrowser2 인터페이스 포인터를 얻는 과정도 자연스럽게 생략
#if 1
int main()
{
	IWebBrowser2 *pWebBrowser;

	if (CoInitialize(NULL) == S_OK)
	{
		HRESULT hr = CoCreateInstance(CLSID_InternetExplorer, NULL, CLSCTX_LOCAL_SERVER, IID_IWebBrowser2, (void**)&pWebBrowser);

		if (FAILED(hr) || !pWebBrowser)
			return 0;

		CString strAddress = "https://www.kitribob.kr/";

		VARIANT vtFlags, vtTarget, vtPostData, vtHeader;
		VariantInit(&vtFlags);
		VariantInit(&vtTarget);
		VariantInit(&vtPostData);
		VariantInit(&vtHeader);

		pWebBrowser->put_Visible(VARIANT_TRUE);
		pWebBrowser->Navigate(strAddress.AllocSysString(), &vtFlags, &vtTarget, &vtPostData, &vtHeader);

		pWebBrowser->Release();
		CoUninitialize();
	}

	return 0;
}
#endif



/* 
plib.lib 복호화 코드 분석 (도중 포기)
DL = 80

DL : 80			HIT	00:83

LOOP START
DL : 80 -> 00		DL	01:1C
DL : 1C -> 39		HIT	02:EC
DL : 39 -> 72		HIT	03:24
DL : 72 -> E4		HIT	04:6A
DL : E4 -> C8		LP END
DL : C8 -> 90 [1 CALL]	NO JMP
DL : 90 -> 20 [2 CALL]	NO JMP
DL : 20 -> 40 [AL  10]	AL LOOP
DL : 40 -> 80 [AL  20]	AL LOOP
DL : 80 -> 00 [AL  40]	DL	05:01
DL : 01 -> 03
DL : 03 -> 06 [AL  80]	AL LOOP
			  [AL 100]	STOS BYTE	ECX:1


LOOP START
DL : 06 -> 0C		HIT	06:FF
DL : 0C -> 18		HIT	07:15
DL : 18 -> 30		HIT	08:48
DL : 30 -> 60		HIT	09:20
DL : 60 -> CO		HIT	0A:40
DL : 60 -> 80		LP END
DL : 80 -> 00 [1 CALL]	DL	0B:C0
DL : C0 -> 81		NO JMP
DL : 81 -> 02 [2 CALL]	NO JMP

DL : 02 -> 04 [AL  10]	AL LOOP
DL : 04 -> 08 [AL  20]	AL LOOP
DL : 08 -> 10 [AL  40]	AL LOOP
DL : 10 -> 20 [AL  80]	AL LOOP
			  [AL 100]	STOS BYTE	ECX:1


LOOP START
DL : 20 -> 40		HIT	0C:85
DL : 40 -> 80		HIT	0D:C0
DL : 80 -> 00		DL	0E:OF
DL : 0F -> 1F		HIT	0F:7C
DL : 1F -> 3E		HIT	10:76
DL : 3E -> 7C		HIT	11:8D
DL : 7C -> F8		HIT	12:44
DL : 78 -> F0		LP END
DL : F0 -> E0 [1 CALL]	NO JMP
DL : E0 -> C0 [2 CALL]	NO JMP
DL : C0 -> 80 [AL  10]	AL LOOP
DL : 80 -> 00 [AL  21]	DL	13:FC
DL : FC -> F9
DL : F9 -> F2 [AL  43]	AL LOOP
DL : F2 -> E4 [AL  87]	AL LOOP
			  [AL 10F]	JMP
*/