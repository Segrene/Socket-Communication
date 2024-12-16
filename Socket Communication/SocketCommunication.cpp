#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <zmq.hpp>
#include <conio.h>
#include <WinSock2.h>
#include "SerialClass.h"
#include <cstdlib>
#include <iostream>
#include <string>
#include <stdexcept>
#include <thread>
#include <vector>
#include "RCoord.h"

#pragma comment(lib, "ws2_32")

#define doosanIP "192.168.137.100"
#define serverIP "192.168.137.50"
#define PORT 200
#define PACKET_SIZE 1024

using namespace std;

int Menu(); //�޴� �Լ�
string RecvMessage(SOCKET hClient, char* cBuffer, string& RecvString); //�޼��� ���� �Լ�
string GetString();
int GetInt();
int SendMessage(SOCKET hClient, string& SendMsg); //�޼��� �߽� �Լ�
int NotReady(string& RecvString); //�̻� ���� �޼��� Ȯ�� �Լ�
int AutoMode(SOCKET hClient, char* cBuffer, string& RecvString, string& SendMsg, int inputMode);
int HoldMode(SOCKET hClient, char* cBuffer, string& RecvString, string& SendMsg, long long HoldTime, int inputMode); //HoldTime�� -1�� ��� �ð� ���� ����
void EndTask(SOCKET hClient, char* cBuffer, string& RecvString, string& SendMsg);
void Move(RCOORD& coord, char* cBuffer, string& RecvString, string& SendMsg);
int getCommand(); //�ǽð� Ű �Է� Ȯ�ο� �Լ�
string ZMQGetMessage(zmq::socket_t& zmqsocket);
void ZMQGetCoord(RCOORD& Coord);
void ManualGetCoord(RCOORD& Coord);

int key;

int main() {
	//�ø��� ���� ����
	//Serial* SP = new Serial("\\\\.\\com3");
	//if (SP->IsConnected()) {
	//	cout << "���� ������ �����" << endl;
	//}

	//���� ���� ����
	WSADATA wsadata;
	if (WSAStartup(MAKEWORD(2, 2), &wsadata) != 0) {
		std::cout << "WSAStartup failed" << endl;
		return 1;
	}

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
	//���� ���� ��

	char cBuffer[PACKET_SIZE] = {};
	string RecvMsg = "";
	string& RecvString = RecvMsg; //RecvMsg ���۷���
	string cMsg = "";
	string& SendMsg = cMsg; //cMsg ���۷���
	int state = 0; // -1 : ��������, 0 : ��������, 1 : Ư����Ȳ
	int inputMode = 0; // 0 : �ڵ�, 1 : ����

	cout << "M0609 : " << RecvMessage(hClient, cBuffer, RecvString); //���� ��Ȳ Ȯ�ο�

	Sleep(1000);

	while (1) {
		if (state == -1) {
			cout << "�̻� ���� ������" << endl;
			break;
		}
		system("cls");
		cout << "��ǥ �Է� ��� : ";
		switch (inputMode) {
		case 0: cout << "�ڵ�" << endl; break;
		case 1: cout << "����" << endl; break;
		}
		switch (Menu()) { //�޴� ����
		case 1: {
			state = AutoMode(hClient, cBuffer, RecvString, SendMsg, inputMode);
			continue;
		}
		case 2: {
			state = HoldMode(hClient, cBuffer, RecvString, SendMsg, -1, inputMode);
			continue;
		}
		case 3: {
			state = HoldMode(hClient, cBuffer, RecvString, SendMsg, 0, inputMode);
			continue;
		}
		case 4: {
			cout << "��ǥ �Է� ���(0:�ڵ�, 1:����) : ";
			cin >> inputMode;
			if (inputMode != 0 && inputMode != 1) {
				inputMode = 1;
			}
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

	cout << "RecvMsg : " << RecvMessage(hClient, cBuffer, RecvString); //���� ��Ȳ Ȯ��

	closesocket(hClient);
	closesocket(hListen);

	WSACleanup();
	return 0;
}

int Menu() {
	cout << "���� ����" << endl;
	cout << "1. �ڵ� ���" << endl << "2. ��� ���" << endl << "3. ��� ���" << endl << "4. ��ǥ �Է� ��� ����" << endl << "0. ����" << endl << "Select : ";
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

int AutoMode(SOCKET hClient, char* cBuffer, string& RecvString, string& SendMsg, int inputMode) {
	RCOORD Coord1; //��ǥ Ŭ���� ����
	switch (inputMode) { //��ǥ �Է� ��� ����
	case 0: ZMQGetCoord(Coord1); break; //�������� �ڵ� �Է�
	case 1: ManualGetCoord(Coord1); break; //����� �Է�
	default: ManualGetCoord(Coord1); break;
	}
	if (Coord1.getPointCount() < 1) { cout << "��ǥ ����" << endl; return 0; } //��ǥ�� 1�� ������ ��� ���� �ź�
	string sl;
	cout << "���ø���(y/n) : ";
	cin >> sl;
	if (sl.compare("y") == 0 || sl.compare("Y") == 0) {
		Coord1.makeLifting();
	}
	cout << "\r" << "Loop Count : 0";
	int count = 0;
	while (1) {
		for (int i = 0; i <= Coord1.getPointCount(); i++) { //i+1���� ��ǥ�� ��ȸ��
			SendMsg = Coord1.getPointString(i);
			SendMessage(hClient, SendMsg); //��ǥ i ����
			RecvMessage(hClient, cBuffer, RecvString); //Ready���
			if (NotReady(RecvString)) {
				return -1; //Ready�� �ƴ� ��� ���� ����
			}
		}
		cout << "\r" << "Loop Count : " << ++count;
		if (getCommand() != -1) { //Ű �Է��� ������ ��� ����, �ѹ��� ��ȸ�� ���� �ڿ��� ���� ����
			return 0;
		}
	}
}
int HoldMode(SOCKET hClient, char* cBuffer, string& RecvString, string& SendMsg, long long HoldTime, int inputMode) {
	RCOORD Coord1; //��ǥ Ŭ���� ����
	cout << "��ǥ : ";
	switch (inputMode) { //��ǥ �Է� ��� ����
	case 0: ZMQGetCoord(Coord1); break; //�������� �ڵ� �Է�
	case 1: ManualGetCoord(Coord1); break; //����� �Է�
	default: ManualGetCoord(Coord1); break;
	}
	cout << Coord1.getPointString(0) << endl;
	if (HoldTime == -1) { //HoldTime�Ű������� -1�� ��� �ð� ���� ����
		cout << "��� �ð�(ms) : ";
		cin >> HoldTime;
	}
	if (HoldTime < 0) { HoldTime = 0; } //HoldTime�� ������ ��� 0���� ����
	SendMsg = Coord1.getPointString(0);
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
	SendMsg = "end"; //�κ��� ���� ��� ����
	SendMessage(hClient, SendMsg);
}

int getCommand() { //�ǽð����� Ű �Է��� �޴� �Լ�
	if (_kbhit()) {
		return _getch();
	}
	return -1;
}

void ManualGetCoord(RCOORD& Coord) {
	cout << "��ǥ ���� �Է�" << endl;
	Coord.Clear();
	string point;
	cout << "��ǥ 1 : ";
	cin >> point;
	Coord.setOrigin(point);
	int count = 2;
	while (true) {
		cout << "��ǥ " << count << " : ";
		cin >> point;
		if (point.compare("start") == 0) {
			cout << "��ǥ �Է� ����" << endl;
			break;
		}
		Coord.addPoint(point);
		count++;
	}
}

void ZMQGetCoord(RCOORD& Coord) {
	zmq::context_t context{ 1 };
	zmq::socket_t zmqsocket(context, ZMQ_REQ);
	zmqsocket.connect("tcp://localhost:5555");
	zmq::socket_t& zmqskt = zmqsocket;
	Coord.Clear();
	string RawCoord = ZMQGetMessage(zmqskt);
	stringstream ss(RawCoord);
	vector<string> Points;
	string Point;
	int scount = 0;
	// ��Ʈ���� �� �پ� �о�, ���� ������ �и��� ��, ��� �迭�� ����
	while (getline(ss, Point, ' ')) {
		Points.push_back(Point);
		cout << "��ǥ " << ++scount << " : " << Point << endl;
	}
	Coord.setOrigin(Points[0]);
	for (int i = 1; i < Points.size(); i++) {
		Coord.addPoint(Points[i]);
	}
	zmqsocket.close();
	context.close();
}

string ZMQGetMessage(zmq::socket_t& zmqsocket) {

	cout << "RequestPoint " << endl;
	zmqsocket.send(zmq::buffer("Send Point Data"), zmq::send_flags::none);

	//  Get the reply.
	zmq::message_t reply;
	zmqsocket.recv(reply, zmq::recv_flags::none);
	string Received = reply.to_string();
	std::cout << "Received : " << Received << endl;

	return Received;
}