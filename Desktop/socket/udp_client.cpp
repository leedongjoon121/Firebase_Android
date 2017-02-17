#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable:C4996)
#define BUF_SIZE 5000


int main(int argc, char **argv) {
	WSADATA wsaData;
	struct sockaddr_in server_addr;
	SOCKET s;

	WSAStartup(MAKEWORD(2, 2), &wsaData);

	if ((s = socket(PF_INET, SOCK_DGRAM, 0)) < 0) {
		printf("Socket Creat Error.\n");
		exit(1);
	}

	int optval, optlen;
	optlen = sizeof(optval);
	int bufSize = getsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&optval, &optlen);
	setsockopt(s, SOL_SOCKET, SO_SNDBUF, (char*)&optval, sizeof(optval * 2));

	printf("File Send Start");

	int totalBufferNum;
	int BufferNum;
	int sendBytes;
	int file_size;  // total file size
	int totalSendBytes;  // received file size

	char buf[BUF_SIZE];
	char buf2[50];
	FILE *fp;
	gets(buf2);
	fp = fopen(buf2, "rb");
	if (fp == NULL) {
		printf("File not Exist");
		exit(1);
	}
	gets(buf2);
	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(buf2);
	server_addr.sin_port = htons(5000);

	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	totalBufferNum = file_size / BUF_SIZE + 1;
	fseek(fp, 0, SEEK_SET);
	BufferNum = 0;
	totalSendBytes = 0;

	_snprintf(buf, sizeof(buf), "%d", file_size);
	sendBytes = sendto(s, buf, sizeof(char) * 1024, 0, (SOCKADDR *)&server_addr, sizeof(server_addr));
	
	int rrr = sizeof(char);
	while ((sendBytes = fread(buf, rrr, BUF_SIZE, fp))>0) {
		sendto(s, buf, sendBytes, 0, (SOCKADDR *)&server_addr, sizeof(server_addr));
	}
	
	closesocket(s);
	printf("Àü¼Û³¡");

	getchar();
	WSACleanup();
	getchar();
	return 0;
}
