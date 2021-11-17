#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <stdlib.h>
#include <time.h>

#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable:4996)

using namespace std;

#define PORT	5500
#define BUFFERSIZE 100
#define NAMESIZE 30

unsigned  sendThread, receiveThread;

class THD {
public:
	int Soc_Cli();
};

int THD::Soc_Cli()
{
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata); // Ŭ���̾�Ʈ ���� ����

	SOCKET mySocket = socket(PF_INET, SOCK_STREAM, 0); // Ŭ���̾�Ʈ ���� ����
	if (mySocket == -1) {
		cout << "Ŭ���̾�Ʈ ������ �����ϴµ� ���� �߽��ϴ�" << endl;
		return 0;
	}

	sockaddr_in serverAddress; //���� ����ü ����
	ZeroMemory(&serverAddress, sizeof(serverAddress)); //���� ����ü �ʱ�ȭ

	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");//localhost �Է�
	serverAddress.sin_family = AF_INET; // TCP/IP����
	serverAddress.sin_port = htons(PORT); //��Ʈ��ȣ 

	/*������ ����*/
	if (connect(mySocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		cout << "������ �����ϴµ� �����߽��ϴ�" << endl;
		return 0;
	}

	char greetMessage[BUFFERSIZE];  //�޽��� ���� �迭 ����

	recv(mySocket, greetMessage, sizeof(greetMessage), 0); //�޽��� ����
	cout << greetMessage << endl;
}

int main()
{
	THD cli;

	cli.Soc_Cli();
}
