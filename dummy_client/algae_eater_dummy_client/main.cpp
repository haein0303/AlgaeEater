

#include <stdio.h>
#include <stdlib.h>
#include <winsock2.h>
#include <Windows.h>
#include <iostream>

#include "..\..\Game\Server_work\protocol.h"

#define IP "127.0.0.1"
#define PORT 4000


void ErrorHandling(char* message);


#pragma comment(lib, "ws2_32.lib")
#pragma warning (disable : 4996)

int main()
{
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)
        ErrorHandling((char*)"WSAStartup() error!");

    SOCKET hSocket = WSASocket(PF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED);
    if (hSocket == INVALID_SOCKET)
        ErrorHandling((char*)"socket() error");

    SOCKADDR_IN recvAddr;
    memset(&recvAddr, 0, sizeof(recvAddr));
    recvAddr.sin_family = AF_INET;
    recvAddr.sin_addr.s_addr = inet_addr(IP);
    recvAddr.sin_port = htons(PORT);

    if (connect(hSocket, (SOCKADDR*)&recvAddr, sizeof(recvAddr)) == SOCKET_ERROR)
        ErrorHandling((char*)"connect() error!");

    WSAEVENT event = WSACreateEvent();

    WSAOVERLAPPED overlapped;
    memset(&overlapped, 0, sizeof(overlapped));

    overlapped.hEvent = event;

    WSABUF dataBuf;
    char message[1024] = { 0, };
    int sendBytes = 0;
    int recvBytes = 0;
    int flags = 0;

    while (true)
    {
        flags = 0;
        printf("�����ҵ�����(���Ḧ���ҽ�exit)\n");

        //scanf("%s", message);

        if (!strcmp(message, "exit")) break;

        dataBuf.len = strlen(message);
        dataBuf.buf = message;

        if (WSASend(hSocket, &dataBuf, 1, (LPDWORD)&sendBytes, 0, &overlapped, NULL) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
                ErrorHandling((char*)"WSASend() error");
        }

        WSAWaitForMultipleEvents(1, &event, TRUE, WSA_INFINITE, FALSE);

        WSAGetOverlappedResult(hSocket, &overlapped, (LPDWORD)&sendBytes, FALSE, NULL);

        printf("���۵ȹ���Ʈ��: %d \n", sendBytes);

        if (WSARecv(hSocket, &dataBuf, 1, (LPDWORD)&recvBytes, (LPDWORD)&flags, &overlapped, NULL) == SOCKET_ERROR)
        {
            if (WSAGetLastError() != WSA_IO_PENDING)
                ErrorHandling((char*)"WSASend() error");
        }

        printf("Recv[%s]\n", dataBuf.buf);
    }

    closesocket(hSocket);

    WSACleanup();

    return 0;
}

void ErrorHandling(char* message)
{
    fputs(message, stderr);
    fputc('\n', stderr);

    exit(1);
}