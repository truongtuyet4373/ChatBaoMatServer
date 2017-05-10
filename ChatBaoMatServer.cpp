// IOCPServerv2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <stdio.h>
#include <process.h>
#include "IO.h"

#define PORT 5500

#pragma comment(lib, "Ws2_32.lib")

unsigned __stdcall serverWorkerThread(LPVOID CompletionPortID);

int _tmain(int argc, _TCHAR* argv[])
{
	SOCKADDR_IN internetAddr;
	SOCKET listenSock, acceptSock;
	HANDLE completionPort;
	SYSTEM_INFO systemInfo;
	LPPER_HANDLE_DATA perHandleData;
	LPPER_IO_OPERATION_DATA perIoData;
	WSADATA wsaData;


	if (WSAStartup((2, 2), &wsaData) != 0) {
		printf("WSAStartup() failed with error %d\n", GetLastError());
		return 1;
	}

	// Setup an I/O completion port
	if ((completionPort = CreateIoCompletionPort(INVALID_HANDLE_VALUE, NULL, 0, 0)) == NULL) {
		printf("CreateIoCompletionPort() failed with error %d\n", GetLastError());
		return 1;
	}

	// Determine how many processors are on the system
	GetSystemInfo(&systemInfo);

	// Create worker threads based on the number of processors available on the
	// system. Create two worker threads for each processor	
	for (int i = 0; i < (int)systemInfo.dwNumberOfProcessors * 2; i++) {
		// Create a server worker thread and pass the completion port to the thread
		if (_beginthreadex(0, 0, serverWorkerThread, (void*)completionPort, 0, 0) == 0) {
			printf("Create thread failed with error %d\n", GetLastError());
			return 1;
		}
	}
	// Create a listening socket
	if ((listenSock = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, WSA_FLAG_OVERLAPPED)) == INVALID_SOCKET) {
		printf("WSASocket() failed with error %d\n", WSAGetLastError());
		return 1;
	}
	internetAddr.sin_family = AF_INET;
	internetAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	internetAddr.sin_port = htons(PORT);
	if (bind(listenSock, (PSOCKADDR)&internetAddr, sizeof(internetAddr)) == SOCKET_ERROR) {
		printf("bind() failed with error %d\n", WSAGetLastError());
		return 1;
	}

	// Prepare socket for listening
	if (listen(listenSock, 5) == SOCKET_ERROR) {
		printf("listen() failed with error %d\n", WSAGetLastError());
		return 1;
	}

	// Accept connections and assign to the completion port
	while (TRUE) {
		if ((acceptSock = WSAAccept(listenSock, NULL, NULL, NULL, 0)) == SOCKET_ERROR) {
			printf("WSAAccept() failed with error %d\n", WSAGetLastError());
			return 1;
		}
		// Create a socket information structure to associate with the socket
		if ((perHandleData = (LPPER_HANDLE_DATA)GlobalAlloc(GPTR, sizeof(PER_HANDLE_DATA))) == NULL) {
			printf("GlobalAlloc() failed with error %d\n", GetLastError());
			return 1;
		}

		// Associate the accepted socket with the original completion port
		printf("\nSocket number %d got connected...\n", acceptSock);
		perHandleData->socket = acceptSock;
		if (CreateIoCompletionPort((HANDLE)acceptSock, completionPort, (DWORD)perHandleData, 0) == NULL) {
			printf("CreateIoCompletionPort() failed with error %d\n", GetLastError());
			return 1;
		}
		// Create per I/O socket information structure to associate with the WSARecv call below
		if ((perIoData = (LPPER_IO_OPERATION_DATA)GlobalAlloc(GPTR, sizeof(PER_IO_OPERATION_DATA))) == NULL) {
			printf("GlobalAlloc() failed with error %d\n", GetLastError());
			return 1;
		}
		ZeroMemory(&(perIoData->overlapped), sizeof(OVERLAPPED));
		perIoData->sentBytes = 0;
		perIoData->recvBytes = 0;
		perIoData->dataBuff.len = DATA_BUFSIZE;
		perIoData->dataBuff.buf = perIoData->buffer;
		perIoData->operation = RECEIVE;

		recvData(perIoData, perHandleData);
	}
	return 0;
}

unsigned __stdcall serverWorkerThread(LPVOID completionPortID)
{
	HANDLE completionPort = (HANDLE)completionPortID;
	DWORD transferredBytes;
	LPPER_HANDLE_DATA perHandleData;
	LPPER_IO_OPERATION_DATA perIoData;
	DWORD flags;
	while (TRUE) {

		//Get Queued Status
		if (GetQueuedCompletionStatus(completionPort,
			&transferredBytes,
			(LPDWORD)&perHandleData,
			(LPOVERLAPPED *)&perIoData,
			INFINITE) == 0) {
			printf("GetQueuedCompletionStatus() failed with error %d\n", GetLastError());
			return 0;
		}

		printf("\n\nAfter get: ");
		printf("\nTransferredBytes = %d", transferredBytes);
		printf("\nDataBuff.buf: %s\nDataBuf.len = %d", perIoData->dataBuff.buf, perIoData->dataBuff.len);
		printf("\nPerIoData: bufLen = %d, recvBytes = %d, sentBytes = %d, operation = %d",
			perIoData->bufLen, perIoData->recvBytes, perIoData->sentBytes, perIoData->operation);


		// Check to see if an error has occurred on the socket and if so
		// then close the socket and cleanup the SOCKET_INFORMATION structure
		// associated with the socket
		if (transferredBytes == 0 && (perIoData->operation == SEND || perIoData->operation == RECEIVE)) {
			printf("Closing socket %d\n", perHandleData->socket);
			if (closesocket(perHandleData->socket) == SOCKET_ERROR) {
				printf("closesocket() failed with error %d\n", WSAGetLastError());
				return 0;
			}
			GlobalFree(perHandleData);
			GlobalFree(perIoData);

			//Delete user from usr_online
			//Delate socket from chatting

			continue;
		}

		// Check to see if the operation field equals RECEIVE. If this is so, then
		// this means a WSARecv call just completed so update the recvBytes field
		// with the transferredBytes value from the completed WSARecv() call
		if (perIoData->operation == RECEIVE) {
			//Process Data here
			perIoData->bufLen = transferredBytes;
			perIoData->dataBuff.len = transferredBytes;
			perIoData->sentBytes = 0;
			perIoData->operation = SEND;

			printf("\n\nif (perIoData->operation == RECEIVE)");
			printf("\nTransferredBytes = %d", transferredBytes);
			printf("\nDataBuff.buf: %s\nDataBuf.len = %d", perIoData->dataBuff.buf, perIoData->dataBuff.len);
			printf("\nPerIoData: bufLen = %d, recvBytes = %d, sentBytes = %d, operation = %d",
				perIoData->bufLen, perIoData->recvBytes, perIoData->sentBytes, perIoData->operation);
			
			sendData(perIoData, perHandleData);
		}
		else if (perIoData->operation == SEND) {
			perIoData->sentBytes += transferredBytes;

			printf("\n\nif (perIoData->operation == SEND)");
			printf("\nTransferredBytes = %d", transferredBytes);
			printf("\nDataBuff.buf: %s\nDataBuf.len = %d", perIoData->dataBuff.buf, perIoData->dataBuff.len);
			printf("\nPerIoData: bufLen = %d, recvBytes = %d, sentBytes = %d, operation = %d",
				perIoData->bufLen, perIoData->recvBytes, perIoData->sentBytes, perIoData->operation);


			if (perIoData->bufLen > perIoData->sentBytes) {
				// Post another WSASend() request.
				// Since WSASend() is not guaranteed to send all of the bytes requested,
				// continue posting WSASend() calls until all bytes are sent.
				ZeroMemory(&(perIoData->overlapped), sizeof(OVERLAPPED));
				perIoData->dataBuff.buf = perIoData->buffer + perIoData->sentBytes;
				perIoData->dataBuff.len = perIoData->bufLen - perIoData->sentBytes;

				printf("\n\nif (perIoData->bufLen > perIoData->sentBytes)");
				printf("\nTransferredBytes = %d", transferredBytes);
				printf("\nDataBuff.buf: %s\nDataBuf.len = %d", perIoData->dataBuff.buf, perIoData->dataBuff.len);
				printf("\nPerIoData: bufLen = %d, recvBytes = %d, sentBytes = %d, operation = %d",
					perIoData->bufLen, perIoData->recvBytes, perIoData->sentBytes, perIoData->operation);

				sendData(perIoData, perHandleData);
			}
			else {
				// No more bytes to send post another WSARecv() request
				perIoData->recvBytes = 0;
				perIoData->operation = RECEIVE;
				ZeroMemory(&(perIoData->overlapped), sizeof(OVERLAPPED));
				perIoData->dataBuff.len = DATA_BUFSIZE;
				perIoData->dataBuff.buf = perIoData->buffer;
				
				printf("\n\nNo more bytes to send post another WSARecv() request");
				printf("\nTransferredBytes = %d", transferredBytes);
				printf("\nDataBuff.buf: %s\nDataBuf.len = %d", perIoData->dataBuff.buf, perIoData->dataBuff.len);
				printf("\nPerIoData: bufLen = %d, recvBytes = %d, sentBytes = %d, operation = %d",
					perIoData->bufLen, perIoData->recvBytes, perIoData->sentBytes, perIoData->operation);


				recvData(perIoData, perHandleData);
			}
		}

		
	}
}
