#include "DueCANLayer.h"

void setup()
{
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
        Serial.print("High Temperature: ");
        Serial.print(cRxData[4]);
        Serial.print("\n\r");
      }
    } // end if
  }
}
