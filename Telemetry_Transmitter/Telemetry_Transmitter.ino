#include "DueCANLayer.h"
#include "XBee.h"

const byte BYTE_MIN = -128;
const unsigned long DELAY = 5000;
byte maxTemp;
unsigned long nextTimeWeSendPacket;
MonitoredSerial mySerial(Serial1, Serial);
XBee xbee(mySerial);

void setup()
{
  Serial.begin(9600);
  Serial1.begin(9600);
  
  delay(5000);
  if(xbee.configure(""))
    Serial.println("Configuration successful");
  else
    Serial.println("Configuration failed");
  maxTemp = -128;
  nextTimeWeSendPacket = 0;
  // Set the serial interface baud rate
    // Initialize both CAN controllers
  Serial.println("Test");  
  if(canInit(0, CAN_BPS_250K) == CAN_OK)
    Serial.print("CAN0: Initialized Successfully.\n\r");
  else
    Serial.print("CAN0: Initialization Failed.\n\r");
}

void loop()
{
  // sendMaxTempEveryFiveSeconds();
  printReceivedPacket();
  shutdownOnCommand();
}

void sendMaxTempEveryFiveSeconds()
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

  if (millis() >= nextTimeWeSendPacket)
  {
    nextTimeWeSendPacket += DELAY;
    printTemperature(maxTemp);
    maxTemp = BYTE_MIN;
  }
}

void printReceivedPacket()
{
  // xbee.read();
}

void shutdownOnCommand()
{
  if (Serial.read() == 's')
  {
    Serial.println("Shutting down, please wait about 30 seconds...");
    if (Serial.read() != 'c')
      xbee.shutdown(15000);
    else
    {
      if (xbee.shutdownCommandMode())
        Serial.println("Shutdown successful");
      else
        Serial.println("Shutdown failed");
    }
  }
}

void printTemperature(byte temp)
{
  Serial.print("High Temperature: ");
  Serial.print(temp);
  Serial.print("\n\r");
}
