#include "DueCANLayer.h"
#include "FirebaseCert.h"
#include "XbeeClient.h"
#include "HttpClient.h"
#include "DataFrame.h"

// CAN Layer functions
extern byte canInit(byte cPort, long lBaudRate);
extern byte canTx(byte cPort, long lMsgID, bool bExtendedFormat, byte* cData, byte cDataLen);
extern byte canRx(byte cPort, long* lMsgID, bool* bExtendedFormat, byte* cData, byte* cDataLen);

FirebaseCert cert;
HttpClient* httpClient;

void setup()
{
  // Set the serial interface baud rate
  SerialUSB.begin(115200);
  
  if(canInit(1, CAN_BPS_500K) == CAN_OK)
    SerialUSB.print("CAN1: Initialized Successfully.\n\r");
  else
    SerialUSB.print("CAN1: Initialization Failed.\n\r");

  Client* client = new XbeeClient(9600);
  httpClient = new HttpClient(*client, "34.70.170.128", 80);
  
}// end setup

double celsiusToFahrenheit(double celsius) {
  return celsius * 1.8 + 32;
}

void loop()
{
  // Declarations
  byte cTxData0[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
  byte cTxData1[] = {0x0F, 0x0E, 0x0D, 0x0C, 0x0C, 0x0B, 0x0A, 0x08};
  int nTimer0 = 0;
  int nTimer1 = 0;
  
  while(1)
  {
    delay(1);
    // Check for received message
    long lMsgID;
    bool bExtendedFormat;
    byte cRxData[8];
    byte cDataLen;

    if(canRx(1, &lMsgID, &bExtendedFormat, &cRxData[0], &cDataLen) == CAN_OK)
    {
       if (lMsgID == 0x6b1) {
          // I'm not really sure what to do with this right now
          SerialUSB.println("High: " + String(celsiusToFahrenheit(cRxData[4])) + " Low: " + String(celsiusToFahrenheit(cRxData[5])));
       } else {
          int voltage = 0;
          voltage = ((voltage + cRxData[0]) << 8) + cRxData[1];
          int current = 0;
          current = ((current + cRxData[2]) << 8) + cRxData[3];
          double soc = cRxData[4] / 2;
          
          DataFrame frame(voltage, current, soc);
          httpClient->beginRequest();
          
          int result = httpClient->post("/", "application/json", frame.toJson());
          if (result != 0) {
            SerialUSB.println("Posting data failed");
          }
       }
    }
  }
}
