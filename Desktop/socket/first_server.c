#include <WinSock2.h>
#include <Windows.h>
#include <stdio.h>


void ErrorHandling(char *Message){//���� ó�� �ϴ� �Լ�
	fputs(Message, stderr);
	fputc('\n', stderr);
	exit(1);	//������ �߻������� �״�� ���α׷� ����
}

int main(int argc, char **argv){
	WSADATA wsaData;
	SOCKET listen_sock, connect_sock;
	SOCKADDR_IN listen_addr, connect_addr;
	int sockaddr_in_size;
	char message[100];
	if (argc != 2){
		printf("������ ���� ����ϼ��� : %s <PORT��ȣ>\n", argv[0]);
		exit(1);
	}

	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0)	//���� 2.2���� ����
		ErrorHandling("WSAStartup() error");	//��� ��ġ���� ������ ������ Ȯ�� �� ���α׷� ����.

	listen_sock = socket(PF_INET, SOCK_STREAM, 0);//���ο� ���ϻ����� ����					
	if (listen_sock == INVALID_SOCKET)
		ErrorHandling("socket() error");

	memset(&listen_addr, 0, sizeof(listen_addr));//�ּ� ���尡���� ����ü�� �ּ� ������
	listen_addr.sin_family = AF_INET;
	listen_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	listen_addr.sin_port = htons(atoi(argv[1]));//////////////�������///////////////

	if (bind(listen_sock, (SOCKADDR*)&listen_addr, sizeof(listen_addr)) == SOCKET_ERROR)
		ErrorHandling("bind() error");	//������ �ּҸ� bind�Լ��� ���Ͽ� �Ҵ�!

	if (listen(listen_sock, 5) == SOCKET_ERROR)	//���⼭ Ŭ���̾�Ʈ ���ӿ�û������ ��� ��ٸ���!
		ErrorHandling("listen() error");

	sockaddr_in_size = sizeof(connect_addr);
	connect_sock = accept(listen_sock, (SOCKADDR*)&connect_addr, &sockaddr_in_size);
	if (connect_sock == INVALID_SOCKET)
		ErrorHandling("accept() error");
	/************�����͸� ���� �ְ���� �� �ִ�***************/
	//~source~//
	recv(connect_sock, message, 12, 0);
	printf("�����κ��� �޼��� : %s\n", message);
	/****************************************************/
	closesocket(connect_sock);	//���ϵ��� �������ְ�
	closesocket(listen_sock);
	WSACleanup();	//���� �ʱ�ȭ
	return 0;
}