#include "DueCANLayer.h"
#include "FirebaseCert.h"
#include "OAuth.h"
#include "XbeeClient.h"
#include "HttpClient.h"

// CAN Layer functions
extern byte canInit(byte cPort, long lBaudRate);
extern byte canTx(byte cPort, long lMsgID, bool bExtendedFormat, byte* cData, byte cDataLen);
extern byte canRx(byte cPort, long* lMsgID, bool* bExtendedFormat, byte* cData, byte* cDataLen);

FirebaseCert cert;
Client* client;

void setup()
{
  // Set the serial interface baud rate
  SerialUSB.begin(115200);
  
  if(canInit(1, CAN_BPS_500K) == CAN_OK)
    SerialUSB.print("CAN1: Initialized Successfully.\n\r");
  else
    SerialUSB.print("CAN1: Initialization Failed.\n\r");

  client = new XbeeClient(9600);
}// end setup

void loop()
{
  // Declarations
  byte cTxData0[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  byte cTxData1[] = {0x0F, 0x0E, 0x0D, 0x0C, 0x0C, 0x0B, 0x0A, 0x08};
  int nTimer0 = 0;
  int nTimer1 = 0;
  int result = client->connect("52.44.173.164", 80);
  String data = "GET /get?foo1=bar1&foo2=bar2 HTTP/1.1\r\nHost: 52.44.173.164\r\n\r\n";
  
  int written = client->write((const uint8_t*) data.c_str(), data.length());
  
  while(1)
  {
    delay(1);
    if (client->available() > 0) {
      SerialUSB.write(client->read());
    }
    // Check for received message
    long lMsgID;
    bool bExtendedFormat;
    byte cRxData[8];
    byte cDataLen;

    if(canRx(1, &lMsgID, &bExtendedFormat, &cRxData[0], &cDataLen) == CAN_OK)
    {
       
    }
  }
}
