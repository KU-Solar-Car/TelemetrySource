/**
 * Obtained from https://copperhilltech.com/blog/arduino-due-dual-can-port-test-sketch/
 */

#ifndef _DUECANLAYER_
#define _DUECANLAYER_

#include "due_can.h"

#define CAN_OK               1
#define CAN_ERROR            0

#define ENABLE_PIN_CAN0      62
#define ENABLE_PIN_CAN1      65

// CAN Layer functions
byte canInit(byte cPort, long lBaudRate);
byte canTx(byte cPort, long lMsgID, bool bExtendedFormat, byte* cData, byte cDataLen);
byte canRx(byte cPort, long* lMsgID, bool* bExtendedFormat, byte* cData, byte* cDataLen);

#endif
