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
	WSAStartup(MAKEWORD(2, 2), &wsadata); // 클라이언트 소켓 생성

	SOCKET mySocket = socket(PF_INET, SOCK_STREAM, 0); // 클라이언트 소켓 생성
	if (mySocket == -1) {
		cout << "클라이언트 소켓을 생성하는데 실패 했습니다" << endl;
		return 0;
	}

	sockaddr_in serverAddress; //서버 구조체 선언
	ZeroMemory(&serverAddress, sizeof(serverAddress)); //서버 구조체 초기화

	serverAddress.sin_addr.s_addr = inet_addr("127.0.0.1");//localhost 입력
	serverAddress.sin_family = AF_INET; // TCP/IP선택
	serverAddress.sin_port = htons(PORT); //포트번호 

	/*서버에 접속*/
	if (connect(mySocket, (sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
		cout << "서버와 연결하는데 실패했습니다" << endl;
		return 0;
	}

	char greetMessage[BUFFERSIZE];  //메시지 저장 배열 선언
	recv(mySocket, greetMessage, sizeof(greetMessage), 0); //메시지 수령
	cout << greetMessage << endl;

	unsigned long hReceiveThread = _beginthreadex(NULL, 0, receiveMessage, (void*)mySocket, 0, &receiveThread);
	unsigned long hSendThread = _beginthreadex(NULL, 0, sendMessage, (void*)mySocket, 0, &receiveThread);

	WaitForSingleObject((HANDLE)hReceiveThread, INFINITE);
	WaitForSingleObject((HANDLE)hSendThread, INFINITE);

}

unsigned __stdcall THD::receiveMessage(void* arg) {
	int mySocket = (int)arg;
	int xy = 0;

	int myposition[1]; //자신의 플레이어 순번
	recv(mySocket, (char*)myposition, sizeof(myposition), 0);
	myposition[0] = htonl(myposition[0]);

	while (1) {
		cout << myposition[0] << "번째 플레이어 입니다." << endl;
		int myMessage[100];


		recv(mySocket, (char*)myMessage, sizeof(myMessage), 0); //맵 수령
		system("cls");

		int mylife = 0;
		for (int i = 0; i < 100; i++) {
			myMessage[i] = htonl(myMessage[i]);
			if (i % 10 == 0) cout << endl;
			if (myMessage[i] == 0) cout << "□";
			else if (myMessage[i] == 5) cout << "■";
			else if (myMessage[i] == 6) cout << "▣";
			else {
				if (myMessage[i] / 100 == myposition[0]) {
					mylife = i; //나의 탱크가 살아있는지 구분
					myMessage[i] = myMessage[i] - (myMessage[i] / 100) * 100;
					if (myMessage[i] / 10 == 1) cout << "▲";
					else if (myMessage[i] / 10 == 2) cout << "▼";
					else if (myMessage[i] / 10 == 3) cout << "◀";
					else if (myMessage[i] / 10 == 4) cout << "▶";
				}
				else {
					myMessage[i] = myMessage[i] - (myMessage[i] / 100) * 100;
					if (myMessage[i] / 10 == 1) cout << "△";
					else if (myMessage[i] / 10 == 2) cout << "▽";
					else if (myMessage[i] / 10 == 3) cout << "◁";
					else if (myMessage[i] / 10 == 4) cout << "▷";
				}
			}
		}
		cout << endl;
		int temp = (mylife / 10) * 10;
		for (int i = temp; i < temp + 10; i++) {
			if (myMessage[i] == 0) cout << "  ";
			else if (myMessage[i] == 5) cout << "■";
			else if (myMessage[i] == 6) cout << "▣";
			if (i == mylife) {
				if (myMessage[i] / 10 == 1) cout << "▲";
				else if (myMessage[i] / 10 == 2) cout << "▼";
				else if (myMessage[i] / 10 == 3) cout << "◀";
				else if (myMessage[i] / 10 == 4) cout << "▶";
			}
			else {
				if (myMessage[i] / 10 == 1) cout << "△";
				else if (myMessage[i] / 10 == 2) cout << "▽";
				else if (myMessage[i] / 10 == 3) cout << "◁";
				else if (myMessage[i] / 10 == 4) cout << "▷";
			}
		}
		cout << endl << "□□□□□□□□□□" << endl;
		if (mylife == 0) cout << "탱크가 파괴되었습니다." << endl;
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
