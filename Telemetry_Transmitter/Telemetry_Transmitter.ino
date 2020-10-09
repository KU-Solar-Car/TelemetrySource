#include "DueCANLayer.h"
const byte BYTE_MIN = -128;
const unsigned long DELAY = 5000;

byte maxTemp;
unsigned long nextTimeWeSendPacket;
void setup()
{
  maxTemp = -128;
  nextTimeWeSendPacket = 0;
  // Set the serial interface baud rate
  Serial.begin(115200);
    // Initialize both CAN controllers
  if(canInit(0, CAN_BPS_250K) == CAN_OK)
    Serial.print("CAN0: Initialized Successfully.\n\r");
  else
    Serial.print("CAN0: Initialization Failed.\n\r");
}

void loop()
{
  while(true)
  {
    // Check for received message
    long lMsgID;
    bool bExtendedFormat;
    byte cRxData[8];
    byte cDataLen;
    if(canRx(0, &lMsgID, &bExtendedFormat, &cRxData[0], &cDataLen) == CAN_OK)
    {
      if (lMsgID == 0x6B1) {
        if (cRxData[4] > maxTemp)
          maxTemp = cRxData[4];
      }
    } // end if
  }
  if (millis() >= nextTimeWeSendPacket)
  {
    nextTimeWeSendPacket += DELAY;
    printTemperature(maxTemp);
    maxTemp = BYTE_MIN;
  }
}

void printTemperature(byte temp)
{
  Serial.print("High Temperature: ");
  Serial.print(temp);
  Serial.print("\n\r");
}
