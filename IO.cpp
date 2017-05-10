#include "stdafx.h"
#include "IO.h"

int sendData(LPPER_IO_OPERATION_DATA perIoData, LPPER_HANDLE_DATA perHandleData) {
	
	DWORD transferredBytes;
	
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
	if (WSARecv(perHandleData->socket,
		&(perIoData->dataBuff),
		1,
		&transferredBytes,
		&flags,
		&(perIoData->overlapped), NULL) == SOCKET_ERROR) {
		if (WSAGetLastError() != ERROR_IO_PENDING) {
			printf("WSARecv() failed with error %d\n", WSAGetLastError());
			return 0;
		}
	}

	return 1;
}