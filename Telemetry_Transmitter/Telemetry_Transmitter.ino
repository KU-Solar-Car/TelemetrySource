#include "DueCANLayer.h"
#include "XBee.h"

const byte BYTE_MIN = -128;
const unsigned long DELAY = 5000;
byte maxTemp;
unsigned long nextTimeWeSendPacket;
MonitoredSerial mySerial(Serial, SerialUSB);
XBee xbee(mySerial);

void setup()
{
  Serial.begin(9600);
  SerialUSB.begin(9600);
  
  delay(5000);
  if(xbee.configure(""))
    SerialUSB.println("Configuration successful");
  else
    SerialUSB.println("Configuration failed");
  maxTemp = -128;
  nextTimeWeSendPacket = 0;
  // Set the serial interface baud rate
    // Initialize both CAN controllers
  SerialUSB.println("Test");  
  if(canInit(0, CAN_BPS_250K) == CAN_OK)
    SerialUSB.print("CAN0: Initialized Successfully.\n\r");
  else
    SerialUSB.print("CAN0: Initialization Failed.\n\r");
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
  const String& data = xbee.read().frameData;
}

void shutdownOnCommand()
{
  if (SerialUSB.read() == 's')
  {
    SerialUSB.println("Shutting down, please wait about 30 seconds...");
    xbee.shutdown();
    /*
    if (xbee.shutdownCommandMode())
      SerialUSB.println("Shutdown successful");
    else
      SerialUSB.println("Shutdown failed");
    */
    
  }
}

void printTemperature(byte temp)
{
  SerialUSB.print("High Temperature: ");
  SerialUSB.print(temp);
  SerialUSB.print("\n\r");
}
