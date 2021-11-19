#include <iostream>
#include <conio.h>
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
	static unsigned __stdcall receiveMessage(void* arg);
	static unsigned __stdcall sendMessage(void* arg);
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

	unsigned long hReceiveThread = _beginthreadex(NULL, 0, receiveMessage, (void*)mySocket, 0, &receiveThread);
	unsigned long hSendThread = _beginthreadex(NULL, 0, sendMessage, (void*)mySocket, 0, &receiveThread);

	WaitForSingleObject((HANDLE)hReceiveThread, INFINITE);
	WaitForSingleObject((HANDLE)hSendThread, INFINITE);

}

unsigned __stdcall THD::receiveMessage(void* arg) {
	int mySocket = (int)arg;
	int xy = 0;

	int myposition[1]; //�ڽ��� �÷��̾� ����
	recv(mySocket, (char*)myposition, sizeof(myposition), 0);
	myposition[0] = htonl(myposition[0]);

	while (1) {
		cout << myposition[0] << "��° �÷��̾� �Դϴ�." << endl;
		int myMessage[100];


		recv(mySocket, (char*)myMessage, sizeof(myMessage), 0); //�� ����
		system("cls");

		int mylife = 0;
		for (int i = 0; i < 100; i++) {
			myMessage[i] = htonl(myMessage[i]);
			if (i % 10 == 0) cout << endl;
			if (myMessage[i] == 0) cout << "��";
			else if (myMessage[i] == 5) cout << "��";
			else if (myMessage[i] == 6) cout << "��";
			else {
				if (myMessage[i] / 100 == myposition[0]) {
					mylife = i; //���� ��ũ�� ����ִ��� ����
					myMessage[i] = myMessage[i] - (myMessage[i] / 100) * 100;
					if (myMessage[i] / 10 == 1) cout << "��";
					else if (myMessage[i] / 10 == 2) cout << "��";
					else if (myMessage[i] / 10 == 3) cout << "��";
					else if (myMessage[i] / 10 == 4) cout << "��";
				}
				else {
					myMessage[i] = myMessage[i] - (myMessage[i] / 100) * 100;
					if (myMessage[i] / 10 == 1) cout << "��";
					else if (myMessage[i] / 10 == 2) cout << "��";
					else if (myMessage[i] / 10 == 3) cout << "��";
					else if (myMessage[i] / 10 == 4) cout << "��";
				}
			}
		}
		cout << endl;
		int temp = (mylife / 10) * 10;
		for (int i = temp; i < temp + 10; i++) {
			if (myMessage[i] == 0) cout << "  ";
			else if (myMessage[i] == 5) cout << "��";
			else if (myMessage[i] == 6) cout << "��";
			if (i == mylife) {
				if (myMessage[i] / 10 == 1) cout << "��";
				else if (myMessage[i] / 10 == 2) cout << "��";
				else if (myMessage[i] / 10 == 3) cout << "��";
				else if (myMessage[i] / 10 == 4) cout << "��";
			}
			else {
				if (myMessage[i] / 10 == 1) cout << "��";
				else if (myMessage[i] / 10 == 2) cout << "��";
				else if (myMessage[i] / 10 == 3) cout << "��";
				else if (myMessage[i] / 10 == 4) cout << "��";
			}
		}
		cout << endl << "�����������" << endl;
		if (mylife == 0) cout << "��ũ�� �ı��Ǿ����ϴ�." << endl;
	}
}

unsigned __stdcall THD::sendMessage(void* arg) {
	int mySocket = (int)arg;
	while (1) {
		int in[1];
		in[0] = _getch();
		if (in[0] == 224) in[0] = _getch();
		in[0] = htonl(in[0]);
		send(mySocket, (char*)in, sizeof(in), 0);
	}
}

int main()
{
	THD cli;

	cli.Soc_Cli();
}
