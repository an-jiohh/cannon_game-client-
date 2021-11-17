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
}

int main()
{
	THD cli;

	cli.Soc_Cli();
}
