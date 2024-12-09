#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <stdio.h>
#include <WinSock2.h>
#include <Windows.h>
#include <iostream>
#include <string>
#include "RCoord.h"

#pragma comment(lib, "ws2_32")

#define doosanIP "192.168.137.100"
#define serverIP "192.168.137.50"
#define PORT 200
#define PACKET_SIZE 1024

std::string RecvMessage(SOCKET hClient, char* cBuffer, std::string& RecvString);
int SendMessage(SOCKET hClient, std::string& SendMsg);

int main() {
	RCOORD Coord1(0,1,2,3,4,5);
	std::cout << Coord1.getCoord1String();
	double Tcoord[6] = {9,8,7,6,5,4};
	Coord1.setCoord2(Tcoord);
	std::cout << Coord1.getCoord2String();
	double Acoord1[6] = { 0 };
	double Acoord2[6] = { 0 };
	Coord1.getCoord1(Acoord1);
	Coord1.getCoord2(Acoord2);
	std::cout << std::to_string(Acoord1[1]) << std::endl;
	std::cout << std::to_string(Acoord2[1]) << std::endl;
	return 0;

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
	std::string RecvMsg = "";
	std::string& RecvString = RecvMsg;
	std::string cMsg = "";
	std::string& SendMsg = cMsg;

	RecvMessage(hClient, cBuffer, RecvString);

	while (1) {
		SendMessage(hClient, SendMsg);
		if (cMsg.compare("end") == 0) {
			break;
		}

		system("cls");
		RecvMessage(hClient, cBuffer, RecvString);
		if (RecvMsg.compare("Ready") == -1) {
			std::cout << "Task Failure" << std::endl;
			break;
		}

		std::cout << std::endl;
	}

	RecvMessage(hClient, cBuffer, RecvString);

	closesocket(hClient);
	closesocket(hListen);

	WSACleanup();
	return 0;
}

std::string RecvMessage(SOCKET hClient, char* cBuffer, std::string& RecvString) {
	recv(hClient, cBuffer, PACKET_SIZE, 0);
	RecvString = cBuffer;
	memset(cBuffer, 0, sizeof(char) * PACKET_SIZE);
	std::cout << "RecvMsg : " << RecvString << std::endl;
	RecvString = "";
	return RecvString;
}

int SendMessage(SOCKET hClient, std::string& SendMsg) {
	std::cout << "SendMsg : ";
	std::cin >> SendMsg;
	send(hClient, SendMsg.c_str(), strlen(SendMsg.c_str()), 0);
	return 1;
}