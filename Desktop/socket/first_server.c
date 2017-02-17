#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>


void ErrorHandling(char *Message){//에러 처리 하는 함수
	fputs(Message, stderr);
	fputc('\n', stderr);
	exit(1);	//오류가 발생했으니 그대로 프로그램 종료
}

int main(int argc, char **argv){
	WSADATA wsaData;
	SOCKET listen_sock, connect_sock;
	SOCKADDR_IN listen_addr, connect_addr;
	int sockaddr_in_size;
	char message[100];
	if (argc != 2){
		printf("다음과 같이 사용하세요 : %s <PORT번호>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)	//윈속 2.2버전 시작
		ErrorHandling("WSAStartup() error");	//어느 위치에서 오류가 났는지 확인 및 프로그램 종료.

	listen_sock = socket(PF_INET, SOCK_STREAM, 0);//새로운 소켓생성후 대입					
	if (listen_sock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&listen_addr, 0, sizeof(listen_addr));//주소 저장가능한 구조체에 주소 설정중
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	listen_addr.sin_port = htons(atoi(argv[1]));//////////////여기까지///////////////

	if (bind(listen_sock, (SOCKADDR*)&listen_addr, sizeof(listen_addr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");	//설정된 주소를 bind함수로 소켓에 할당!

	if (listen(listen_sock, 5) == SOCKET_ERROR)	//여기서 클라이언트 접속요청때까지 계속 기다린다!
		ErrorHandling("listen() error");

	sockaddr_in_size = sizeof(connect_addr);
	connect_sock = accept(listen_sock, (SOCKADDR*)&connect_addr, &sockaddr_in_size);
	if (connect_sock == INVALID_SOCKET)
		ErrorHandling("accept() error");
	/************데이터를 이제 주고받을 수 있다***************/
	//~source~//
	recv(connect_sock, message, 12, 0);
	printf("서버로부터 메세지 : %s\n", message);
	/****************************************************/
	closesocket(connect_sock);	//소켓들을 정리해주고
	closesocket(listen_sock);
	WSACleanup();	//윈속 초기화
	return 0;
}