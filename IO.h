#pragma once
#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <process.h>

#define DATA_BUFSIZE 8192
#define SEND 0
#define RECEIVE 1

typedef struct {
	WSAOVERLAPPED overlapped;
	WSABUF dataBuff;
	CHAR buffer[DATA_BUFSIZE];
	int bufLen;
	int recvBytes;
	int sentBytes;
	int operation;
} PER_IO_OPERATION_DATA, *LPPER_IO_OPERATION_DATA;

typedef struct {
	SOCKET socket;
} PER_HANDLE_DATA, *LPPER_HANDLE_DATA;

int sendData(LPPER_IO_OPERATION_DATA, LPPER_HANDLE_DATA);

int recvData(LPPER_IO_OPERATION_DATA, LPPER_HANDLE_DATA);