#include "stdafx.h"
#include "IO.h"
#include "processData.h"

//Buffer chua dl perIoData->dataBuff.buf
//Do dai dl perIoData->dataBuff.len
//Khi thay doi dl tren buffer can cap nhat do dai

int parseArgs(LPPER_IO_OPERATION_DATA perIoData, LPPER_HANDLE_DATA perHandleData) {
	
	strcpy(perIoData->dataBuff.buf, "001");
	perIoData->dataBuff.len = 3;

	sendData(perIoData, perHandleData);

	return 1;
}