#pragma once

#ifdef LAB01-01DLL_EXPORTS
#define MYDLL __declspec(dllexport)
#else
#define MYDLL __declspec(dllimport)
#endif

#ifdef __cplusplus
extern "C" {
#endif // __cplusplus


	MYDLL int add(int x, int y);
	//__deslspec(dllexport) int add(int x, int y)
	//__declspec(dllimport) int add(int x, int y) 


#ifdef __cplusplus
}
#endif // __cplusplus



//VS 에서는 프로젝트가 DLL 형식이면 컴파일되면서 "프로젝트이름(대문자)_EXPORTS" 가 자동적으로 정의된다.

//여기서는 프로젝트이름이 Lab01-01dll 이니까 대문자로 바뀌어 LAB01-01DLL_EXPORTS 가 정의된다.

//그래서 Lab01-01dll 프로젝트를 컴파일할때 MYDLL은 __declspec(dllexport) 로 바뀐다.





