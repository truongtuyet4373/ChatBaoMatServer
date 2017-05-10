#include "stdafx.h"
#include "IO.h"

//file nay yc giu nguyen, ko duoc sua vao

int sendData(LPPER_IO_OPERATION_DATA perIoData, LPPER_HANDLE_DATA perHandleData) {
	
	DWORD transferredBytes;
	perIoData->sentBytes = 0;
	perIoData->operation = SEND;

	if (WSASend(perHandleData->socket,
		&(perIoData->dataBuff),
		1,
		&transferredBytes,
		0,
		&(perIoData->overlapped),
		NULL) == SOCKET_ERROR) {
		if (WSAGetLastError() != ERROR_IO_PENDING) {
			printf("WSASend() failed with error %d\n", WSAGetLastError());
			return 0;
		}
	}

	return 1;
}

int recvData(LPPER_IO_OPERATION_DATA perIoData, LPPER_HANDLE_DATA perHandleData) {

	DWORD transferredBytes;
	DWORD flags = 0;

	ZeroMemory(&(perIoData->overlapped), sizeof(OVERLAPPED));
		perIoData->sentBytes = 0;
		perIoData->recvBytes = 0;
		perIoData->dataBuff.len = DATA_BUFSIZE;
		perIoData->dataBuff.buf = perIoData->buffer;
		perIoData->operation = RECEIVE;

	if (WSARecv(perHandleData->socket,&(perIoData->dataBuff),
		1,&transferredBytes,&flags,
		&(perIoData->overlapped), NULL) == SOCKET_ERROR) {
		if (WSAGetLastError() != ERROR_IO_PENDING) {
			printf("WSARecv() failed with error %d\n", WSAGetLastError());
			return 0;
		}
	}

	return 1;
}