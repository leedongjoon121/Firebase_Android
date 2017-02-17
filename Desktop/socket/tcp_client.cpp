#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <WinSock2.h>

#define WIN32_LEAN_AND_MEAN 
#pragma comment(lib, "Ws2_32.lib")
typedef unsigned long long u64;
u64 GetMicroCounter();

#define BUF_SIZE 10000

#define ip_port "192.9.45.167"
#define file_name "abc.mp4"

const int threadSize = 15;
typedef struct SocketInfo {
	int port;
	long fileSeekStart;
	long fileSeekEnd;
}SocketInfo;

DWORD WINAPI FileSender(LPVOID arg)
{
	SocketInfo* si = (SocketInfo *)arg;
	long fileSeekStart = si->fileSeekStart;
	long fileSeekEnd = si->fileSeekEnd;
	int port = si->port;
	WSADATA wsaData;
	struct sockaddr_in server_addr;
	SOCKET s;
	printf("%d 부터 여기까지 %d  포트: %d \n", fileSeekStart, fileSeekEnd, port);
	
	


	WSAStartup(MAKEWORD(2, 2), &wsaData);
	


	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket Creat Error.\n");
		exit(1);
	}

	int optval, optlen;
	optlen = sizeof(optval);

	int bufbuf = getsockopt(s, SOL_SOCKET, SO_RCVBUF, (char *)&optval, &optlen);
	optval *= 20;
	setsockopt(s, SOL_SOCKET, SO_RCVBUF, (char*)&optval, sizeof(optval));

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip_port);
	server_addr.sin_port = htons(port);
	if (connect(s, (SOCKADDR *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		printf("Socket Connection Error.\n");
		exit(1);
	}

	//int sock = socket(PF_INET, SOCK_STREAM, 0);
	//int snd_buf = 1024 * 3;

	//setsockopt(sock, SOL_SOCKET, SO_SNDBUF, (void*)&snd_buf, sizeof(snd_buf));

	printf("File Send Start\n");

	int totalBufferNum;
	int BufferNum;
	int sendBytes;
	long totalSendBytes;
	long file_size;
	char buf[BUF_SIZE];

	FILE *fp;
	fp = fopen(file_name, "rb");
	if (fp == NULL) {
		printf("File not Exist");
		exit(1);
	}

	file_size = fileSeekEnd - fileSeekStart;
	totalBufferNum = file_size / sizeof(buf) + 1;
	fseek(fp, fileSeekStart, SEEK_SET);
	BufferNum = 0;
	totalSendBytes = 0;

	_snprintf(buf, sizeof(buf), "%d", file_size);
	sendBytes = send(s, buf, sizeof(buf), 0);

	while (totalSendBytes<file_size && (sendBytes = fread(buf, sizeof(char), sizeof(buf), fp))>0) {
		//   send(s, buf, sendBytes, 0);
		//BufferNum++;
		totalSendBytes += sendBytes;
		//printf("In progress: %d/%dByte(s) [%d%%]\n", totalSendBytes, file_size, ((BufferNum * 100) / totalBufferNum));
		if (totalSendBytes <= file_size)
			send(s, buf, sendBytes, 0);
		else
		{
			long over = totalSendBytes - file_size;
			//printf("%d\n",);
			send(s, buf, sendBytes - over, 0);
			totalSendBytes = totalSendBytes - sendBytes + sendBytes - over;
	//		printf("여기로 왔네 %d는 오버사이즈 %d는 최종 total 그리고 sendBytes: %d\n", over, totalSendBytes, sendBytes);
			break;
		}
		//printf("%d 에서 %d만큼 데이터보냄\n", port, sendBytes);
	}
	printf("%d 보내기 끝 %d 보낸량 %d 파일사이즈\n", port, totalSendBytes, file_size);
	//closesocket(s);
}

int main(int argc, char **argv) {
	u64 start, end;
	WSADATA wsaData;
	struct sockaddr_in server_addr;
	SOCKET s;



	WSAStartup(MAKEWORD(2, 2), &wsaData);

	if ((s = socket(PF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Socket Creat Error.\n");
		exit(1);
	}

	memset(&server_addr, 0, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr(ip_port);
	server_addr.sin_port = htons(5000);

	if (connect(s, (SOCKADDR *)&server_addr, sizeof(server_addr)) == SOCKET_ERROR) {
		printf("Socket Connection Error.\n");
		exit(1);
	}

	printf("File Send Start");

	int totalBufferNum;
	int BufferNum;
	int sendBytes;
	long totalSendBytes;
	long file_size;
	char buf[BUF_SIZE];

	FILE *fp;
	fp = fopen(file_name, "rb");
	if (fp == NULL) {
		printf("File not Exist");
		exit(1);
	}
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	totalBufferNum = file_size / sizeof(buf) + 1;
	fseek(fp, 0, SEEK_SET);
	BufferNum = 0;
	totalSendBytes = 0;
	printf("%d 총 파일크기", file_size);
	_snprintf(buf, sizeof(buf), "%d", file_size);
	sendBytes = send(s, buf, sizeof(buf), 0);
	start = GetMicroCounter();
	/*

	*/
	long divide = file_size / (threadSize);
	long remain = file_size % (threadSize);
	HANDLE hThread[threadSize];
	//recv(s, buf, BUF_SIZE, 0);
	SocketInfo si[threadSize];
	for (int i = 0; i < threadSize - 1; i++)
	{
		si[i].port = 5001 + i;
		si[i].fileSeekStart = divide * i;
		si[i].fileSeekEnd = divide*(i + 1);
		hThread[i] = CreateThread(NULL, 0, FileSender, (LPVOID)&si[i], 0, NULL);
	}
	si[threadSize - 1].port = 5001 + threadSize - 1;
	si[threadSize - 1].fileSeekStart = divide * (threadSize - 1);
	si[threadSize - 1].fileSeekEnd = divide*(threadSize)+remain;
	hThread[threadSize - 1] = CreateThread(NULL, 0, FileSender, (LPVOID)&si[threadSize - 1], 0, NULL);
	end = GetMicroCounter();
	printf("time: %d second(s)", (end - start) / 1000000);
	closesocket(s);
	getchar();
	WSACleanup();

	return 0;
}

u64 GetMicroCounter()
{
	u64 Counter;

#if defined(_WIN32)
	u64 Frequency;
	QueryPerformanceFrequency((LARGE_INTEGER *)&Frequency);
	QueryPerformanceCounter((LARGE_INTEGER *)&Counter);
	Counter = 1000000 * Counter / Frequency;
#elif defined(__linux__) 
	struct timeval t;
	gettimeofday(&t, 0);
	Counter = 1000000 * t.tv_sec + t.tv_usec;
#endif

	return Counter;
}