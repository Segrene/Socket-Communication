#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <zmq.hpp>
#include <stdio.h>
#include <conio.h>
#include <WinSock2.h>
#include <Windows.h>
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

int Menu(); //메뉴 함수
string RecvMessage(SOCKET hClient, char* cBuffer, string& RecvString); //메세지 수신 함수
string GetString();
int GetInt();
int SendMessage(SOCKET hClient, string& SendMsg); //메세지 발신 함수
int NotReady(string& RecvString); //이상 동작 메세지 확인 함수
int AutoMode(SOCKET hClient, char* cBuffer, string& RecvString, string& SendMsg);
int HoldMode(SOCKET hClient, char* cBuffer, string& RecvString, string& SendMsg, long long HoldTime); //HoldTime이 -1인 경우 시간 수동 설정
void EndTask(SOCKET hClient, char* cBuffer, string& RecvString, string& SendMsg);
void Move(RCOORD& coord, char* cBuffer, string& RecvString, string& SendMsg);
int getCommand(); //실시간 키 입력 확인용 함수
string ZMQGetMessage(zmq::socket_t& zmqsocket);
void ZMQGetCoord(RCOORD& Coord, zmq::socket_t& zmqsocket);

int key;

int main() {

	zmq::context_t context{ 1 };
	zmq::socket_t zmqsocket(context, ZMQ_REQ);
	zmqsocket.connect("tcp://localhost:5555");
	zmq::socket_t& zmqskt = zmqsocket;

	while (true) {
		ZMQGetMessage(zmqskt);
	}

	//소켓 구성 시작
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
	//소켓 구성 끝

	char cBuffer[PACKET_SIZE] = {};
	string RecvMsg = "";
	string& RecvString = RecvMsg; //RecvMsg 레퍼런스
	string cMsg = "";
	string& SendMsg = cMsg; //cMsg 레퍼런스
	int state = 0; // -1 : 강제종료, 0 : 정상종료, 1 : 특수상황

	cout << RecvMessage(hClient, cBuffer, RecvString); //연결 상황 확인용

	while (1) {
		system("cls");
		switch (Menu()) { //메뉴 선택
		case 1: {
			state = AutoMode(hClient, cBuffer, RecvString, SendMsg);
			continue;
		}
		case 2: {
			state = HoldMode(hClient, cBuffer, RecvString, SendMsg, -1);
			continue;
		}
		case 3: {
			state = HoldMode(hClient, cBuffer, RecvString, SendMsg, 0);
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

	cout << "RecvMsg : " << RecvMessage(hClient, cBuffer, RecvString); //종료 상황 확인

	closesocket(hClient);
	closesocket(hListen);

	zmqsocket.close();
	context.close();

	WSACleanup();
	return 0;
}

int Menu() {
	cout << "동작 선택" << endl;
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
	RecvString = ""; //받은 메세지 초기화
	recv(hClient, cBuffer, PACKET_SIZE, 0); //메세지 수신
	RecvString = cBuffer; //char 에서 string으로
	memset(cBuffer, 0, sizeof(char) * PACKET_SIZE); //char 버퍼 초기화
	return RecvString;
}

int NotReady(string& RecvString) {
	if (RecvString.compare("Ready\r\n") != 0) { //Ready가 아닌 메세지를 받을 경우 종료
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
	RCOORD Coord1; //좌표 클래스 생성
	cout << "좌표 1 : ";
	Coord1.setOrigin(GetString()); //시작 좌표 설정
	string c;
	int count = 0;
	while (1) {
		cout << "좌표 " << Coord1.getPointCount() + 2 << " : ";
		c = GetString();
		if (c.compare("start") == 0) {
			break;
		}
		Coord1.addPoint(c); //좌표 추가
	}
	if (Coord1.getPointCount() < 1) { cout << "좌표 부족" << endl; return 0; } //좌표가 1개 이하인 경우 실행 거부
	cout << "\r" << "Loop Count : 0";
	while (1) {
		for (int i = 0; i <= Coord1.getPointCount(); i++) { //i+1개의 자표를 순회함
			SendMsg = Coord1.getPointString(i);
			SendMessage(hClient, SendMsg); //좌표 i 전송
			RecvMessage(hClient, cBuffer, RecvString); //Ready대기
			if (NotReady(RecvString)) {
				return -1; //Ready가 아닌 경우 강제 종료
			}
		}
		cout << "\r" << "Loop Count : " << ++count;
		if (getCommand() != -1) { //키 입력이 감지될 경우 종료, 한번의 순회가 끝난 뒤에만 종료 가능
			return 0;
		}
	}
}
int HoldMode(SOCKET hClient, char* cBuffer, string& RecvString, string& SendMsg, long long HoldTime) {
	RCOORD Coord1; //좌표 클래스 생성
	cout << "좌표 : ";
	Coord1.setOrigin(GetString()); //대기 좌표 설정
	if (HoldTime == -1) { //HoldTime매개변수가 -1인 경우 시간 수동 설정
		cout << "대기 시간(ms) : ";
		cin >> HoldTime;
	}
	if (HoldTime < 0) { HoldTime = 0; } //HoldTime이 음수인 경우 0으로 변경
	SendMsg = Coord1.getPointString(0);
	SendMessage(hClient, SendMsg);
	RecvMessage(hClient, cBuffer, RecvString); //Ready대기
	if (NotReady(RecvString)) { return -1; }
	Sleep(HoldTime);
	SendMsg = "0, 0, 0, 0, 0, 0";
	SendMessage(hClient, SendMsg);
	RecvMessage(hClient, cBuffer, RecvString); //Ready대기
	if (NotReady(RecvString)) { return -1; }
	return 0;
}
void EndTask(SOCKET hClient, char* cBuffer, string& RecvString, string& SendMsg) {
	SendMsg = "end"; //로봇에 종료 명령 전달
	SendMessage(hClient, SendMsg);
}

int getCommand() { //실시간으로 키 입력을 받는 함수
	if (_kbhit()) {
		return _getch();
	}
	return -1;
}

void ZMQGetCoord(RCOORD& Coord, zmq::socket_t& zmqsocket) {
	Coord.Clear();
	string RawCoord = ZMQGetMessage(zmqsocket);
	stringstream ss(RawCoord);
	vector<string> Points;
	string Point;
	// 스트림을 한 줄씩 읽어, 공백 단위로 분리한 뒤, 결과 배열에 저장
	while (getline(ss, Point, ' ')) {
		Points.push_back(Point);
	}
	Coord.setOrigin(Points[0]);
	for (int i = 1; i <= Points.size(); i++) {
		Coord.addPoint(Points[i]);
	}
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