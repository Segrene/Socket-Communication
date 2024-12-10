#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <stdexcept>
#include <thread>
#include "RCoord.h"

#pragma comment(lib, "ws2_32")

#define doosanIP "192.168.137.100"
#define serverIP "192.168.137.50"
#define PORT 200
#define PACKET_SIZE 1024

using namespace std;

int CoordTest();
int Menu();
string RecvMessage(SOCKET hClient, char* cBuffer, string& RecvString);
string GetString();
int GetInt();
int SendMessage(SOCKET hClient, string& SendMsg);
int NotReady(string& RecvString);
int AutoMode(SOCKET hClient, char* cBuffer, string& RecvString, string& SendMsg);
int HoldMode(SOCKET hClient, char* cBuffer, string& RecvString, string& SendMsg, long long HoldTime); //HoldTime�� -1�� ��� �ð� ���� ����
void EndTask(SOCKET hClient, char* cBuffer, string& RecvString, string& SendMsg);
void MoveBegin(RCOORD& coord, char* cBuffer, string& RecvString, string& SendMsg);
void MoveEnd(RCOORD& coord, char* cBuffer, string& RecvString, string& SendMsg);
int getCommand();

int key;

int main() {
	//CoordTest();

	WSADATA wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	SOCKET hListen;
	hListen = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);

	SOCKADDR_IN tListenAddr = {};
	tListenAddr.sin_family = AF_INET;
	tListenAddr.sin_port = htons(PORT);
	tListenAddr.sin_addr.s_addr = htonl(INADDR_ANY);

	bind(hListen, (SOCKADDR*)&tListenAddr, sizeof(tListenAddr));
	listen(hListen, SOMAXCONN);

	SOCKADDR_IN tCIntAddr = {};
	int iCIntSize = sizeof(tCIntAddr);
	SOCKET hClient = accept(hListen, (SOCKADDR*)&tCIntAddr, &iCIntSize);

	char cBuffer[PACKET_SIZE] = {};
	string RecvMsg = "";
	string& RecvString = RecvMsg;
	string cMsg = "";
	string& SendMsg = cMsg;
	int state = 0; // -1 : ��������, 0 : ��������, 1 : Ư����Ȳ

	RecvMessage(hClient, cBuffer, RecvString);

	while (1) {
		switch (Menu()) {
		case 1: {
			state = AutoMode(hClient, cBuffer, RecvString, SendMsg);
			continue;
		}
		case 2: {
			state = HoldMode(hClient, cBuffer, RecvString, SendMsg, 0);
			continue;
		}
		case 3: {
			state = HoldMode(hClient, cBuffer, RecvString, SendMsg, -1);
			continue;
		}
		case 0: {
			EndTask(hClient, cBuffer, RecvString, SendMsg);
			break;
		}
		default: continue;
		}

		break;
	}

	cout << "RecvMsg : " << RecvMessage(hClient, cBuffer, RecvString);

	closesocket(hClient);
	closesocket(hListen);

	WSACleanup();
	return 0;
}

int Menu() {
	cout << "���� ����" << endl;
	cout << "1. Auto" << endl << "2. Hold" << endl << "3. Shot" << endl << "0. End" << endl << "Select : ";
	return GetInt();
}

string GetString() {
	string gstr;
	cin >> gstr;
	cout << endl;
	return gstr;
}

int GetInt() {
	int gint;
	cin >> gint;
	cout << endl;
	return gint;
}

string RecvMessage(SOCKET hClient, char* cBuffer, string& RecvString) {
	RecvString = ""; //���� �޼��� �ʱ�ȭ
	recv(hClient, cBuffer, PACKET_SIZE, 0); //�޼��� ����
	RecvString = cBuffer; //char ���� string����
	memset(cBuffer, 0, sizeof(char) * PACKET_SIZE); //char ���� �ʱ�ȭ
	cout << "RecvMsg : " << RecvString << endl; //���� �޼��� ���
	return RecvString;
}

int NotReady(string& RecvString) {
	if (RecvString.compare("Ready\r\n") != 0) { //Ready�� �ƴ� �޼����� ���� ��� ����
		cout << "Task Failure" << endl;
		return 1;
	}
	return 0;
}

int SendMessage(SOCKET hClient, string& SendMsg) {
	send(hClient, SendMsg.c_str(), strlen(SendMsg.c_str()), 0);
	return 1;
}

int AutoMode(SOCKET hClient, char* cBuffer, string& RecvString, string& SendMsg) {
	RCOORD Coord1; //��ǥ Ŭ���� ����
	cout << "��ǥ 1 : ";
	Coord1.setCoord(GetString()); //���� ��ǥ ����
	string c;
	int count = 0;
	while (1) {
		cout << "��ǥ " << Coord1.getPoint() + 2 << " : ";
		c = GetString();
		if (c.compare("start") == 0) {
			break;
		}
		Coord1.addCoord(GetString()); //��ǥ �߰�
	}
	if (Coord1.getPoint() < 1) { cout << "��ǥ ����" << endl; return 0; } //��ǥ�� 1�� ������ ��� ���� �ź�
	while (1) {
		for (int i = 0; i <= Coord1.getPoint(); i++) { //i+1���� ��ǥ�� ��ȸ��
			SendMsg = Coord1.getCoordString(i);
			SendMessage(hClient, SendMsg); //��ǥ i ����
			RecvMessage(hClient, cBuffer, RecvString); //Ready���
			if (NotReady(RecvString)) {
				return -1; //Ready�� �ƴ� ��� ���� ����
			}
		}
		cout << "\r" << "Loop Count : " << count;
		if (getCommand() != -1) { //�ѹ��� ��ȸ�� ���� �ڿ��� ���� ����
			return 0;
		}
	}
}
int HoldMode(SOCKET hClient, char* cBuffer, string& RecvString, string& SendMsg, long long HoldTime) {
	RCOORD Coord1; //��ǥ Ŭ���� ����
	cout << "��ǥ : ";
	Coord1.setCoord(GetString()); //��� ��ǥ ����
	if (HoldTime == -1) { //HoldTime�Ű������� -1�� ��� �ð� ���� ����
		cout << "��� �ð�(ms) : ";
		cin >> HoldTime;
	}
	if (HoldTime < 0) { HoldTime = 0; } //HoldTime�� ������ ��� 0���� ����
	SendMsg = Coord1.getCoordString(1);
	SendMessage(hClient, SendMsg);
	RecvMessage(hClient, cBuffer, RecvString); //Ready���
	if (NotReady(RecvString)) { return -1; }
	Sleep(HoldTime);
	SendMsg = "0, 0, 0, 0, 0, 0";
	SendMessage(hClient, SendMsg);
	RecvMessage(hClient, cBuffer, RecvString); //Ready���
	if (NotReady(RecvString)) { return -1; }
	return 0;
}
void EndTask(SOCKET hClient, char* cBuffer, string& RecvString, string& SendMsg) {
	SendMsg = "end";
	SendMessage(hClient, SendMsg);
}

void MoveBegin(RCOORD& coord, char* cBuffer, string& RecvString, string& SendMsg) { //������
	SendMsg = coord.getCoordString(0);
}
void MoveEnd(RCOORD& coord, char* cBuffer, string& RecvString, string& SendMsg) {
	SendMsg = coord.getCoordString(0);
}

int getCommand() { //�ǽð����� Ű �Է��� �޴� �Լ�
	if (_kbhit()) {
		return _getch();
	}
	return -1;
}