#include "stdafx.h"
#include "IO.h"
#include "processData.h"

int parseArgs(LPPER_IO_OPERATION_DATA perIoData, LPPER_HANDLE_DATA perHandleData) {
	
	strcpy(perIoData->dataBuff.buf, "001");
	perIoData->dataBuff.len = 3;
	perIoData->sentBytes = 0;
	perIoData->operation = SEND;

	sendData(perIoData, perHandleData);

	return 1;
}