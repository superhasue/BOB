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



//VS ������ ������Ʈ�� DLL �����̸� �����ϵǸ鼭 "������Ʈ�̸�(�빮��)_EXPORTS" �� �ڵ������� ���ǵȴ�.

//���⼭�� ������Ʈ�̸��� Lab01-01dll �̴ϱ� �빮�ڷ� �ٲ�� LAB01-01DLL_EXPORTS �� ���ǵȴ�.

//�׷��� Lab01-01dll ������Ʈ�� �������Ҷ� MYDLL�� __declspec(dllexport) �� �ٲ��.





