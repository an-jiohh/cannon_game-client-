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

class THD {//���������� ���� Ŭ����
public:
	int start_client(); //Ŭ���̾�Ʈ ���� ���� �Լ�
	int join_server(); //���� ���� �Լ�
	virtual int main_func() = 0; //����Լ����� ������ �߻�޼ҵ� ���� 
protected:
	SOCKET mySocket;
};

int THD::start_client() { //Ŭ���̾�Ʈ ���� ���� �Լ�
	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata); // Ŭ���̾�Ʈ ���� ����

	mySocket = socket(PF_INET, SOCK_STREAM, 0); // Ŭ���̾�Ʈ ���� ����
	if (mySocket == -1) {
		cout << "Ŭ���̾�Ʈ ������ �����ϴµ� ���� �߽��ϴ�" << endl;
		return 0;
	}
}

int THD::join_server() { //���� ���� �Լ�
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

class main_CLA :public THD {
public:
	main_CLA();
	~main_CLA();
	int main_func();
	static unsigned __stdcall receiveMessage(void* arg);
	static unsigned __stdcall sendMessage(void* arg);
};
main_CLA::main_CLA() {
	cout << "Ŭ�󸮾�Ʈ�� �����մϴ�." << endl;
}
main_CLA::~main_CLA() {
	closesocket(mySocket); //������ ����;
	cout << "Ŭ���̾�Ʈ�� �����մϴ�." << endl;
}

int main_CLA::main_func() {
	if (start_client() == 0) return 0; //Ŭ���̾�Ʈ �����Լ�(�����ϸ� ����)

	if (join_server() == 0) return 0;//���� ���� �Լ�(�����ϸ� ����)

	unsigned long hReceiveThread = _beginthreadex(NULL, 0, receiveMessage, (void*)mySocket, 0, &receiveThread); //�޽��� ���� ������
	unsigned long hSendThread = _beginthreadex(NULL, 0, sendMessage, (void*)mySocket, 0, &receiveThread); //�Է°� ���� ������

	WaitForSingleObject((HANDLE)hReceiveThread, INFINITE); //���� �����带 ��ٸ�
}

unsigned __stdcall main_CLA::receiveMessage(void* arg) {
	int mySocket = (int)arg;
	int xy = 0;

	int myposition[1]; //�ڽ��� �÷��̾� ����
	recv(mySocket, (char*)myposition, sizeof(myposition), 0);
	myposition[0] = htonl(myposition[0]);


	int mylifeX, mylifeY;
	while (1) {
		cout << "����� " << myposition[0] << "��° �÷��̾� �Դϴ�." << endl;
		int myMessage[101];
		int num[10][10];
		mylifeX = -1, mylifeY = -1;

		int readlen = recv(mySocket, (char*)myMessage, sizeof(myMessage), 0); //�� ����
		system("cls");

		for (int i = 0; i < 10; i++) { //���� �޽����� 2���� �迭�� ��ȯ
			for (int j = 0; j < 10; j++) {
				num[i][j] = htonl(myMessage[i * 10 + j]);
			}
		}
		myMessage[100] = htonl(myMessage[100]); //�Ȱ���� Ȯ��

		if (myMessage[100] == 1) { //�Ȱ������ ��� ���� ��ũ�� ����
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) {
					if (num[i][j] / 100 == myposition[0]) {
						mylifeX = j, mylifeY = i; //���� ��ũ��ġ ����(������ -1)
						num[i][j] = num[i][j] - (num[i][j] / 100) * 100;
						if (num[i][j] / 10 == 1) {
							for (int k = 1; k < 6; k++) { //�� 5ĭ
								if (i - k >= 0) {
									if (num[i - k][j] / 100 != 0) {
										num[i - k][j] = num[i - k][j] - (num[i - k][j] / 100) * 100;
									}
									if (num[i - k][j] == 6) num[i - k][j] = 60;
								}
								else break;
							}
						}
						else if (num[i][j] / 10 == 2) { //�Ʒ� 5ĭ
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
						else if (num[i][j] / 10 == 3) { //���� 5ĭ
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
						else if (num[i][j] / 10 == 4) { //������ 5ĭ
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
						break; //���� ��ũ�� ã�� ���Ŀ��� ����
					}
				}
			}
		}
		else { //�Ȱ���尡 �ƴҰ��
			for (int i = 0; i < 10; i++) {
				for (int j = 0; j < 10; j++) {
					if (num[i][j] / 100 != 0) {
						if (num[i][j] / 100 == myposition[0]) mylifeX = j, mylifeY = i; //���� ��ũ��ġ ����(������ -1)
						num[i][j] = num[i][j] - (num[i][j] / 100) * 100;
					}
					if (num[i][j] == 6) num[i][j] = 60;
				}
			}
		}

		for (int i = 0; i < 10; i++) { //���� ǥ���Ѵ�.
			for (int j = 0; j < 10; j++) {
				if (j == mylifeX && i == mylifeY) {
					if (num[i][j] / 10 == 1) cout << "��";
					else if (num[i][j] / 10 == 2) cout << "��";
					else if (num[i][j] / 10 == 3) cout << "��";
					else if (num[i][j] / 10 == 4) cout << "��";
				}
				else if (num[i][j] == 5) cout << "��";
				else if (num[i][j] / 10 == 6) cout << "��";
				else if (num[i][j] / 10 == 1) cout << "��";
				else if (num[i][j] / 10 == 2) cout << "��";
				else if (num[i][j] / 10 == 3) cout << "��";
				else if (num[i][j] / 10 == 4) cout << "��";
				else cout << "��";
			}
			cout << endl;
		}
		cout << endl;
		if (mylifeX == -1) cout << "��ũ�� �ı��Ǿ����ϴ�." << endl; //�ڽ��� ��ũ�� �ı��Ǹ� ���������� ǥ������ �ʴ´�.
		else { //�ڽ��� ��ũ�� ��������� ����
			for (int i = 0; i < 10; i++) {
				if (i == mylifeX) {
					if (num[mylifeY][i] / 10 == 1) cout << "��";
					else if (num[mylifeY][i] / 10 == 2) cout << "��";
					else if (num[mylifeY][i] / 10 == 3) cout << "��";
					else if (num[mylifeY][i] / 10 == 4) cout << "��";
				}
				else if (num[mylifeY][i] == 5) cout << "��";
				else if (num[mylifeY][i] / 10 == 6) cout << "��";
				else if (num[mylifeY][i] / 10 == 1) cout << "��";
				else if (num[mylifeY][i] / 10 == 2) cout << "��";
				else if (num[mylifeY][i] / 10 == 3) cout << "��";
				else if (num[mylifeY][i] / 10 == 4) cout << "��";
				else cout << "  ";
			}
			//����, �� ���, �÷��̾� ��Ȳ����
			cout << endl << "�����������" << endl;
			num[mylifeY][mylifeX] = num[mylifeY][mylifeX] - (num[mylifeY][mylifeX] / 10) * 10;
			cout << "���� : SPACE  ����/�̵� : S" << endl;
			cout << "�÷��̾� ������� : ";
			if (num[mylifeY][mylifeX] == 0) cout << "�̵�" << endl;
			else  cout << "����" << endl;
			cout << "���� ���� ";
			if (myMessage[100] == 1) cout << "�Ȱ���� �Դϴ�." << endl;
			else cout << "��Ȱ���� �Դϴ�." << endl;
		}
		if (readlen < 0) break;
	}
	if (mylifeX == -1) cout << "���ӿ��� �й��Ͽ����ϴ�." << endl;
	else cout << "����� ����Ͽ����ϴ�." << "(" << myposition[0] << "��° �÷��̾�)" << endl;

	cout << "������ ����Ǿ����ϴ�." << endl;
	return 0;
}

unsigned __stdcall main_CLA::sendMessage(void* arg) { //Ű���� �Է°��� ������ �����Ѵ�.
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