#pragma once
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <iostream>
#include <winsock2.h>
#include <thread>
#include <vector>
using namespace std;

#pragma comment(lib,"ws2_32.lib")
#define PACKET_SIZE 1024