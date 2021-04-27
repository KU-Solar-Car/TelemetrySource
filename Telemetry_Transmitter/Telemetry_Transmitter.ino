#include "DueCANLayer.h"
#include "XBee.h"
#include "MonitoredSerial.h"
#include "Stats.h"
#include "IPAddress.h"
#include "Frames.h"
#include <MemoryFree.h>
#include <pgmStrToRAM.h>

const byte BYTE_MIN = -128;
byte maxTemp;

const unsigned long DELAY = 5000;
unsigned long nextTimeWeSendFrame;
// MonitoredSerial mySerial(Serial1, Serial);
XBee xbee(Serial1);

const size_t REQUEST_BUFFER_SIZE = 600;
char requestBuffer[REQUEST_BUFFER_SIZE];

TelemetryData testStats;


void setup()
{
  // Set the serial interface baud rate
  Serial.begin(115200);
  Serial1.begin(9600);

  // Suppress the serial
  // mySerial.suppress();

  /* --------------------------------
   * Set the XBee in API Mode
   * =================================*/
  if(xbee.configure())
    Serial.println("Configuration successful");
  else
    Serial.println("Configuration failed");
    
  /* =================================
   * Initialize CAN board
   * =================================*/
  if(canInit(0, CAN_BPS_250K) == CAN_OK)
    Serial.print("CAN0: Initialized Successfully.\n\r");
  else
    Serial.print("CAN0: Initialization Failed.\n\r");
   
  /* =================================
   * Wait for modem to associate before starting 
   * =================================*/
  userFrame status;
//  Serial.println("Waiting for network to associate...");
//  do
//  {
//    status = xbee.read();
//  } while(!(status.frameType == 0x8A && status.frameData[0] == 2));
//  Serial.println("Network associated.");
  
  /* =================================
   * Initialize variables that track stuff
   * =================================*/
  maxTemp = -128;
  nextTimeWeSendFrame = 0;
}

void loop()
{
  // sendMaxTempEveryFiveSeconds();
  sendStatsEveryFiveSeconds();
  shutdownOnCommand();
}


void randomizeData()
{
   /* =================================
   * Set TelemetryData
   * =================================*/

  for(int i = 0; i < TelemetryData::Key::_LAST; i++)
  {
    
    if(i == TelemetryData::Key::BMS_FAULT)
    {
      testStats.setBool(i, static_cast<bool>(random(0, 2))); // Excludes the max :(
    }
    else if(i == TelemetryData::Key::GPS_TIME)
    {
      testStats.setUInt(i, static_cast<unsigned int>(random(5001)));
    }
    else
    {
      testStats.setDouble(i, random(0, 8000) / static_cast<double>(random(1, 100)));
    }
  }
}

void printReceivedFrame()
{
  userFrame recvd = xbee.read();
  if (!(recvd == NULL_USER_FRAME))
  {
    Serial.println("Got frame:");
    Serial.println("Frame type: " + String(recvd.frameType, HEX));
    Serial.print("Frame data: ");
    Serial.write(recvd.frameData, recvd.frameDataLength);
    Serial.println("");
  }
  // else
    // Serial.println("Got here nothing :(");
}

void shutdownOnCommand()
{
  if (Serial.read() == 's')
  {
    Serial.println("Shutting down, please wait up to 2 minutes...");
    if (Serial.read() != 'c')
      xbee.shutdown(120000);
    else
    {
      if (xbee.shutdownCommandMode())
        Serial.println("Shutdown successful");
      else
        Serial.println("Shutdown failed");
    }
  }
}


void setContentLengthHeader(char* dest, int len)
{
  char* contentLength = strstr(dest, "Content-Length: ") + 16;
  char tmpBuffer[4]; 
  sprintf(tmpBuffer, "%03u", len);
  strncpy(contentLength, tmpBuffer, 3);
}

void sendStatsEveryFiveSeconds()
{
  unsigned long myTime = millis();
  if (myTime >= nextTimeWeSendFrame)
  {
    nextTimeWeSendFrame = myTime + DELAY;
    Serial.println("Free memory: " + String(freeMemory()) + "\0");
    randomizeData();
    if (xbee.isConnected(5000))
    {
      userFrame resp;
      sendStats(testStats);
      const unsigned long myTime = millis();
      const unsigned timeout = 10000;
      testStats.clear();
      do
      {
        resp = xbee.read();
      } while (millis() < myTime+timeout && resp.frameType != 0xB0);
      if (resp.frameType != 0xB0)
        Serial.println("Request timed out.");
    }
    else
      Serial.println("Modem is not connected, skipping this time.");
    Serial.println("");
  }
}

void sendStats(TelemetryData stats)
{
  
  strcpy(requestBuffer, "POST /car HTTP/1.1\r\nContent-Length: 000\r\nHost: ku-solar-car-b87af.appspot.com\r\nContent-Type: application/json\r\nAuthentication: eiw932FekWERiajEFIAjej94302Fajde\r\n\r\n");
  strcat(requestBuffer, "{");
  int bodyLength = 1; // the open bracket
  for (int k = 0; k < TelemetryData::Key::_LAST; k++)
  {
    if (stats.isPresent(k))
    {
      bodyLength += toKeyValuePair(requestBuffer + strlen(requestBuffer), k, stats) + 1; // append the key-value pair, plus the trailing comma
      strcat(requestBuffer, ",");
    }
  }
  requestBuffer[strlen(requestBuffer)-1] = '}';
  setContentLengthHeader(requestBuffer, bodyLength);

  xbee.sendTCP(IPAddress(216, 58, 192, 212), PORT_HTTPS, 0, PROTOCOL_TLS, 0, requestBuffer, strlen(requestBuffer));
  Serial.println(requestBuffer);

  // strcpy(requestBuffer, "GET /get HTTP/1.1\r\nHost: httpbin.org\r\n");
  // xbee.sendTCP(IPAddress(54, 166, 163, 67), 443, 0, 0, requestBuffer, strlen(requestBuffer));
}

int toKeyValuePair(char* dest, int key, TelemetryData data)
{
  switch(key)
  {
    case TelemetryData::Key::BATT_VOLTAGE: return sprintf(dest, "\"battery_voltage\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::BATT_CURRENT: return sprintf(dest, "\"battery_current\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::BATT_TEMP: return sprintf(dest, "\"battery_temperature\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::BMS_FAULT: return sprintf(dest, "\"bms_fault\":%d", data.getBool(key)); break;
    case TelemetryData::Key::GPS_TIME: return sprintf(dest, "\"gps_time\":%u", data.getUInt(key)); break;
    case TelemetryData::Key::GPS_LAT: return sprintf(dest, "\"gps_lat\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::GPS_LON: return sprintf(dest, "\"gps_lon\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::GPS_VEL_EAST: return sprintf(dest, "\"gps_velocity_east\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::GPS_VEL_NOR: return sprintf(dest, "\"gps_velocity_north\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::GPS_VEL_UP: return sprintf(dest, "\"gps_velocity_up\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::GPS_SPD: return sprintf(dest, "\"gps_speed\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::SOLAR_VOLTAGE: return sprintf(dest, "\"solar_voltage\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::SOLAR_CURRENT: return sprintf(dest, "\"solar_current\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::MOTOR_SPD: return sprintf(dest, "\"motor_speed\":%6f", data.getDouble(key)); break;
  }
}

/* =================================
 * Temporarily not being used
 * =================================*/
//void sendMaxTempEveryFiveSeconds()
//{
//  // Check for received message
//  long lMsgID;
//  bool bExtendedFormat;
//  byte cRxData[8];
//  byte cDataLen;
//  if(canRx(0, &lMsgID, &bExtendedFormat, &cRxData[0], &cDataLen) == CAN_OK)
//  {
//    if (lMsgID == 0x6B1) {
//      if (cRxData[4] > maxTemp)
//        maxTemp = cRxData[4];
//    }
//  } // end if
//  if (millis() >= nextTimeWeSendFrame)
//  {
//    nextTimeWeSendFrame += DELAY;
//    printTemperature(maxTemp);
//    maxTemp = BYTE_MIN;
//  }
//}

//void printTemperature(byte temp)
//{
//  Serial.print("High Temperature: ");
//  Serial.print(temp);
//  Serial.print("\n\r");
//}
