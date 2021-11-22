#include <iostream>
#include <conio.h>
#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <stdlib.h>
#include <time.h>
#include <cstring>

#pragma comment (lib, "ws2_32.lib")
#pragma warning(disable:4996)

using namespace std;

#define PORT	5500
#define BUFFERSIZE 100
#define NAMESIZE 30

unsigned  sendThread, receiveThread;

class THD {//서버가동을 위한 클래스
public:
	int start_client(); //클라이언트 소켓 생성 함수
	int join_server(); //서버 연결 함수
	virtual int main_func() = 0; //통신함수들을 구동할 추상메소드 지정 
protected:
	SOCKET mySocket;
};

int THD::start_client() { //클라이언트 소켓 생성 함수
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata); // 클라이언트 소켓 생성

	mySocket = socket(PF_INET, SOCK_STREAM, 0); // 클라이언트 소켓 생성
	if (mySocket == -1) {
		cout << "클라이언트 소켓을 생성하는데 실패 했습니다" << endl;
		return 0;
	}
}

int THD::join_server() { //서버 연결 함수
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

class main_CLA :public THD {
public:
	main_CLA();
	~main_CLA();
	int main_func();
	static unsigned __stdcall receiveMessage(void* arg);
	static unsigned __stdcall sendMessage(void* arg);
};
main_CLA::main_CLA() {
	cout << "클라리언트를 시작합니다." << endl;
}
main_CLA::~main_CLA() {
	closesocket(mySocket); //소켓을 종료;
	cout << "클라이언트를 종료합니다." << endl;
}

int main_CLA::main_func() {
	if (start_client() == 0) return 0; //클라이언트 시작함수(실패하면 종료)

	if (join_server() == 0) return 0;//서버 연결 함수(실패하면 종료)

	unsigned long hReceiveThread = _beginthreadex(NULL, 0, receiveMessage, (void*)mySocket, 0, &receiveThread); //메시지 수령 스레드
	unsigned long hSendThread = _beginthreadex(NULL, 0, sendMessage, (void*)mySocket, 0, &receiveThread); //입력값 전송 스레드

	WaitForSingleObject((HANDLE)hReceiveThread, INFINITE); //수령 스레드를 기다림
}

unsigned __stdcall main_CLA::receiveMessage(void* arg) {
	int mySocket = (int)arg;
	int xy = 0;

	int myposition[1]; //자신의 플레이어 순번
	recv(mySocket, (char*)myposition, sizeof(myposition), 0);
	myposition[0] = htonl(myposition[0]);


	int mylifeX, mylifeY;
	while (1) {
		cout << "당신은 " << myposition[0] << "번째 플레이어 입니다." << endl;
		int myMessage[101];
		int num[10][10];
		mylifeX = -1, mylifeY = -1;

		int readlen = recv(mySocket, (char*)myMessage, sizeof(myMessage), 0); //맵 수령
		system("cls");

		for (int i = 0; i < 10; i++) { //받은 메시지를 2차원 배열로 변환
			for (int j = 0; j < 10; j++) {
				num[i][j] = htonl(myMessage[i * 10 + j]);
			}
		}
		myMessage[100] = htonl(myMessage[100]); //안개모드 확인

		if (myMessage[100] == 1) { //안개모드일 경우 맵의 탱크를 조절
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) {
					if (num[i][j] / 100 == myposition[0]) {
						mylifeX = j, mylifeY = i; //나의 탱크위치 저장(없으면 -1)
						num[i][j] = num[i][j] - (num[i][j] / 100) * 100;
						if (num[i][j] / 10 == 1) {
							for (int k = 1; k < 6; k++) { //위 5칸
								if (i - k >= 0) {
									if (num[i - k][j] / 100 != 0) {
										num[i - k][j] = num[i - k][j] - (num[i - k][j] / 100) * 100;
									}
									if (num[i - k][j] == 6) num[i - k][j] = 60;
								}
								else break;
							}
						}
						else if (num[i][j] / 10 == 2) { //아래 5칸
							for (int k = 1; k < 6; k++) {
								if (i + k <= 9) {
									if (num[i + k][j] / 100 != 0) {
										num[i + k][j] = num[i + k][j] - (num[i + k][j] / 100) * 100;
									}
									if (num[i + k][j] == 6) num[i + k][j] = 60;
								}
								else break;
							}
						}
						else if (num[i][j] / 10 == 3) { //왼쪽 5칸
							for (int k = 1; k < 6; k++) {
								if (j - k >= 0) {
									if (num[i][j - k] / 100 != 0) {
										num[i][j - k] = num[i][j - k] - (num[i][j - k] / 100) * 100;
									}
									if (num[i][j - k] == 6) num[i][j - k] = 60;
								}
								else break;
							}
						}
						else if (num[i][j] / 10 == 4) { //오른쪽 5칸
							for (int k = 1; k < 6; k++) {
								if (j + k <= 9) {
									if (num[i][j + k] / 100 != 0) {
										num[i][j + k] = num[i][j + k] - (num[i][j + k] / 100) * 100;
									}
									if (num[i][j + k] == 6) num[i][j + k] = 60;
								}
								else break;
							}
						}
						break; //나의 탱크를 찾은 이후에는 종료
					}
				}
			}
		}
		else { //안개모드가 아닐경우
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) {
					if (num[i][j] / 100 != 0) {
						if (num[i][j] / 100 == myposition[0]) mylifeX = j, mylifeY = i; //나의 탱크위치 저장(없으면 -1)
						num[i][j] = num[i][j] - (num[i][j] / 100) * 100;
					}
					if (num[i][j] == 6) num[i][j] = 60;
				}
			}
		}

		for (int i = 0; i < 10; i++) { //맵을 표시한다.
			for (int j = 0; j < 10; j++) {
				if (j == mylifeX && i == mylifeY) {
					if (num[i][j] / 10 == 1) cout << "▲";
					else if (num[i][j] / 10 == 2) cout << "▼";
					else if (num[i][j] / 10 == 3) cout << "◀";
					else if (num[i][j] / 10 == 4) cout << "▶";
				}
				else if (num[i][j] == 5) cout << "■";
				else if (num[i][j] / 10 == 6) cout << "▣";
				else if (num[i][j] / 10 == 1) cout << "△";
				else if (num[i][j] / 10 == 2) cout << "▽";
				else if (num[i][j] / 10 == 3) cout << "◁";
				else if (num[i][j] / 10 == 4) cout << "▷";
				else cout << "□";
			}
			cout << endl;
		}
		cout << endl;
		if (mylifeX == -1) cout << "탱크가 파괴되었습니다." << endl; //자신의 탱크가 파괴되면 수평지도를 표시하지 않는다.
		else { //자신의 탱크가 살아있으면 진행
			for (int i = 0; i < 10; i++) {
				if (i == mylifeX) {
					if (num[mylifeY][i] / 10 == 1) cout << "▲";
					else if (num[mylifeY][i] / 10 == 2) cout << "▼";
					else if (num[mylifeY][i] / 10 == 3) cout << "◀";
					else if (num[mylifeY][i] / 10 == 4) cout << "▶";
				}
				else if (num[mylifeY][i] == 5) cout << "■";
				else if (num[mylifeY][i] / 10 == 6) cout << "▣";
				else if (num[mylifeY][i] / 10 == 1) cout << "△";
				else if (num[mylifeY][i] / 10 == 2) cout << "▽";
				else if (num[mylifeY][i] / 10 == 3) cout << "◁";
				else if (num[mylifeY][i] / 10 == 4) cout << "▷";
				else cout << "  ";
			}
			//사용법, 맵 모드, 플레이어 상황제공
			cout << endl << "□□□□□□□□□□" << endl;
			num[mylifeY][mylifeX] = num[mylifeY][mylifeX] - (num[mylifeY][mylifeX] / 10) * 10;
			cout << "공격 : SPACE  멈춤/이동 : S" << endl;
			cout << "플레이어 현재상태 : ";
			if (num[mylifeY][mylifeX] == 0) cout << "이동" << endl;
			else  cout << "멈춤" << endl;
			cout << "현재 맵은 ";
			if (myMessage[100] == 1) cout << "안개모드 입니다." << endl;
			else cout << "비안개모드 입니다." << endl;
		}
		if (readlen < 0) break;
	}
	if (mylifeX == -1) cout << "게임에서 패배하였습니다." << endl;
	else cout << "당신이 우승하였습니다." << "(" << myposition[0] << "번째 플레이어)" << endl;

	cout << "서버가 종료되었습니다." << endl;
	return 0;
}

unsigned __stdcall main_CLA::sendMessage(void* arg) { //키보드 입력값을 서버로 전송한다.
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
	main_CLA cli;

	cli.main_func();
}