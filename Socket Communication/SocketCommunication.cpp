#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include <thread>
#include "RCoord.h"

#pragma comment(lib, "ws2_32")

#define doosanIP "192.168.137.100"
#define serverIP "192.168.137.50"
#define PORT 200
#define PACKET_SIZE 1024

using namespace std;

int RecvMessage(SOCKET hClient, char* cBuffer, string& RecvString);
string GetString();
int SendMessage(SOCKET hClient, string& SendMsg);
int NotReady(string& RecvString);
int AutoMode(SOCKET hClient, char* cBuffer, string& RecvString, string& SendMsg);
void MoveBegin(RCOORD& coord, char* cBuffer, string& RecvString, string& SendMsg);
void MoveEnd(RCOORD& coord, char* cBuffer, string& RecvString, string& SendMsg);
int getCommand();

int key;

int main() {
	/*RCOORD Coord1(10,20,30,20,10,0);
	cout << Coord1.getCoord1String();
	double Tcoord[6] = {9,8,7,6,5,4};
	Coord1.setCoord2(Tcoord);
	cout << Coord1.getCoord2String();
	double Acoord1[6] = { 0 };
	double Acoord2[6] = { 0 };
	Coord1.getCoord1(Acoord1);
	Coord1.getCoord2(Acoord2);
	cout << to_string(Acoord1[1]) << endl;
	cout << to_string(Acoord2[1]) << endl;*/

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
	int A = 0;

	RecvMessage(hClient, cBuffer, RecvString);

	while (1) {
		if (A == 1) {
			A = AutoMode(hClient, cBuffer, RecvString, SendMsg); //�ڵ� ��� ����
		}
		if (A == -1) {
			break;
		}

		cMsg = GetString();
		if (cMsg.compare("end") == 0) {
			SendMessage(hClient, SendMsg);
			break;
		} else if (cMsg.compare("manual") == 0) {
			A = 0;
			continue;
		} else if (cMsg.compare("auto") == 0) {
			A = 1;
			continue;
		}
		SendMessage(hClient, SendMsg);

		//system("cls");
		RecvMessage(hClient, cBuffer, RecvString);
		if (NotReady(RecvString)) {
			break;
		}

		cout << endl;
	}

	RecvMessage(hClient, cBuffer, RecvString);

	closesocket(hClient);
	closesocket(hListen);

	WSACleanup();
	return 0;
}

string GetString() {
	string gstr;
	cout << "SendMsg : ";
	cin >> gstr;
	return gstr;
}

int RecvMessage(SOCKET hClient, char* cBuffer, string& RecvString) {
	RecvString = ""; //���� �޼��� �ʱ�ȭ
	recv(hClient, cBuffer, PACKET_SIZE, 0); //�޼��� ����
	RecvString = cBuffer; //char ���� string����
	memset(cBuffer, 0, sizeof(char) * PACKET_SIZE); //char ���� �ʱ�ȭ
	cout << "RecvMsg : " << RecvString << endl; //���� �޼��� ���
	return 1;
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
	//RCOORD Coord1(0,-30,-20,0,0,0); //�ӽ÷� ���� ��ǥ ����. ���߿� �и��ؼ� �Ű������� ���� ����
	//double c2[] = { 0,30,-20,0,0,0 };
	//Coord1.setCoord2(c2);
	RCOORD Coord1;
	string c1 = GetString();
	string c2 = GetString();
	Coord1.setCoord1(c1);
	Coord1.setCoord2(c2);
	while (1) {
		SendMsg = Coord1.getCoord1String();
		SendMessage(hClient, SendMsg); //��ǥ 1 ����
		RecvMessage(hClient, cBuffer, RecvString); //Ready���
		if (NotReady(RecvString)) {
			return -1;
		}
		SendMsg = Coord1.getCoord2String(); //��ǥ 2 ����
		SendMessage(hClient, SendMsg);
		RecvMessage(hClient, cBuffer, RecvString);
		if (NotReady(RecvString)) {
			return -1;
		}
		if (getCommand() != -1) {
			return 0;
		}
	}
}

void MoveBegin(RCOORD& coord, char* cBuffer, string& RecvString, string& SendMsg) {
	SendMsg = coord.getCoord1String();
}
void MoveEnd(RCOORD& coord, char* cBuffer, string& RecvString, string& SendMsg) {
	SendMsg = coord.getCoord2String();
}

int getCommand() { //�ǽð����� Ű �Է��� �޴� �Լ�
	if (_kbhit()) {
		return _getch();
	}
	return -1;
}