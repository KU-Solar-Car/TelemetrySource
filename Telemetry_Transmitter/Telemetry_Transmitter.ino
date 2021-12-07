#define DEBUG_BUILD // Remove when using with DriverHUD

// Debug print macro. Do while loop ensures a semicolon is used.
#ifdef DEBUG_BUILD
  #define DEBUG(x) do { Serial.println(x); } while (0)
#else
  #define DEBUG(x) do {} while (0)
#endif

#include "DueCANLayer.h"
#include "XBee.h"
#include "MonitoredSerial.h"
#include "Stats.h"
#include "IPAddress.h"
#include "Frames.h"
#include "GPSFormatter.h"
#include "auth_key.h"
#include <MemoryFree.h>   // https://github.com/mpflaga/Arduino-MemoryFree
#include <pgmStrToRAM.h>
#include <DueTimer.h>     // https://github.com/ivanseidel/DueTimer/releases

// Number of milliseconds to wait between transmitting data packets
const int XBEE_TX_INTERVAL = 2000;

// Number of milliseconds to wait between checking for commands from DriverHUD
const int DRIVERHUD_CHECK_INTERVAL = 33; // 30 fps

const byte BYTE_MIN = -128;
byte maxTemp;

unsigned long nextTimeWeSendFrame;

// MonitoredSerial mySerial(Serial2, Serial);

XBee xbee(Serial2);
GPSFormatter gpsFormatter(&Serial1); // 19rx, 18tx

const size_t DATA_BUFFER_SIZE = 550;
char dataBuffer[DATA_BUFFER_SIZE]; // Buffer for XBee data
char serialDataBuffer[DATA_BUFFER_SIZE]; // Buffer for DriverHUD data (this is separate because of interrupts)

const size_t REQUEST_BUFFER_SIZE = 600;
char requestBuffer[REQUEST_BUFFER_SIZE];

volatile TelemetryData testStats;

volatile bool resetButtonPressed = false;
volatile bool shutdownButtonPressed = false;

volatile bool resetButtonMaybePressed = false;
volatile bool shutdownButtonMaybePressed = false;

volatile bool checkShutdown = false;

const int SHUTDOWN_PIN = 2;
const int RESET_PIN = 3;
const unsigned BUTTON_DEBOUNCE_MICROS = 10000;


void setup()
{
  Serial.begin(115200); // Interface with DriverHUD and/or debugging
  Serial2.begin(9600); // Interface with XBee
  Serial1.begin(4800); // Interface with GPS
  
  // Suppress the serial
  // mySerial.suppress();

  /* --------------------------------
   * Set the XBee in API Mode
   * =================================*/
  if(xbee.configure())
    DEBUG("XBee: Configuration successful");
  else
    DEBUG("XBee: Configuration failed");
    
  /* =================================
   * Initialize CAN board
   * =================================*/
  if(canInit(0, CAN_BPS_250K) == CAN_OK)
    DEBUG("CAN0: Initialized Successfully.\n\r");
  else
    DEBUG("CAN0: Initialization Failed.\n\r");

  /* =================================
   * Set the CAN interrupt
   * =================================*/
   Can0.setGeneralCallback(CANCallback);
   
  /* =================================
   * Wait for modem to associate before starting 
   * =================================*/
  userFrame status;
//  DEBUG("Waiting for network to associate...");
//  do
//  {
//    status = xbee.read();
//  } while(!(status.frameType == 0x8A && status.frameData[0] == 2));
//  DEBUG("XBee: Network associated.");
  
  /* =================================
   * Initialize variables that track stuff
   * =================================*/
  maxTemp = -128;
  nextTimeWeSendFrame = 0;

  // Shutdown and reset pins
  pinMode(SHUTDOWN_PIN, INPUT_PULLUP);
  pinMode(RESET_PIN, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(SHUTDOWN_PIN), shutdown_debounce_interrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(RESET_PIN), reset_debounce_interrupt, FALLING);

  // Debounce ISRs
  Timer0.attachInterrupt(shutdown_interrupt);
  Timer1.attachInterrupt(reset_interrupt);

  // Check for commands from the DriverHUD frequently, even if doing something else
  Timer2.attachInterrupt(serial_commands_interrupt).setPeriod(DRIVERHUD_CHECK_INTERVAL*1000).start();
}

void loop()
{
  // Read most data from CAN bus
  // setMaxTemp();

  // Read GPS values
  gpsFormatter.readSerial();
  gpsFormatter.writeToData(testStats);

  // Send stats to XBee and DriverHUD
  // sendStatsPeriodically(XBEE_TX_INTERVAL);
  sendStats(testStats);

  /*
  unsigned long myTime = millis();
  while(myTime + XBEE_TX_INTERVAL > millis()) {
    continue;
  }
  */

  // Check for serial commands and shutdown/reset buttons
  shutdownOnCommand();
}

void shutdown_interrupt()
{
  if (digitalRead(digitalPinToInterrupt(SHUTDOWN_PIN)) == LOW)
    shutdownButtonPressed = true;
  shutdownButtonMaybePressed = false;
  Timer0.stop();
}
void reset_interrupt()
{
  if (digitalRead(digitalPinToInterrupt(RESET_PIN)) == LOW)
    resetButtonPressed = true;
  resetButtonMaybePressed = false;
  Timer1.stop();
}

void shutdown_debounce_interrupt()
{
  // first, check that we're not already debouncing.
  if (!shutdownButtonMaybePressed)
    shutdownButtonMaybePressed = true;
    Timer0.start(BUTTON_DEBOUNCE_MICROS);
}
void reset_debounce_interrupt()
{
  if (!resetButtonMaybePressed)
    resetButtonMaybePressed = true;
    Timer1.start(BUTTON_DEBOUNCE_MICROS);
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
    #ifdef DEBUG_BUILD
      Serial.println("Got frame:");
      Serial.println("Frame type: " + String(recvd.frameType, HEX));
      Serial.print("Frame data: ");
      Serial.write(recvd.frameData, recvd.frameDataLength);
      Serial.println("");
    #endif
  }
  // else
    // DEBUG("Got here nothing :(");
}

void serial_commands_interrupt() {
  char cmd = Serial.read();
  if (cmd == 's') shutdownButtonPressed = true;
  else if (cmd == 'r') resetButtonPressed = true;
  else if (cmd == 'd') sendStatsSerial(testStats); // DriverHUD requesting data
  else if (cmd == 'x') checkShutdown = true;
}

// TODO: Format the output of shutdown/reset in a way the DriverHUD can understand
void shutdownOnCommand()
{
  if (shutdownButtonPressed)
  {
    Serial.println("Shutting down, please wait up to 2 minutes...");
    if (Serial.read() != 'c')
      xbee.shutdown(120000, false);
    else
    {
      if (xbee.shutdownCommandMode())
        Serial.println("Shutdown successful");
      else
        Serial.println("Shutdown failed");
    }
    shutdownButtonPressed = false;
    resetButtonPressed = false;
  }
  else if (resetButtonPressed)
  {
    Serial.println("Resetting, please wait up to 4 minutes...");
    xbee.safeReset(120000);
    resetButtonPressed = false;
    shutdownButtonPressed = false;
  }
  else if (checkShutdown)
  {
    Serial.println("Checking if XBee is shutdown, please wait up to 1 minute...");
    bool isShutdown = xbee.isShutDown(60000);
    if (isShutdown) Serial.println("YES!");
    else Serial.println("No :(");
    checkShutdown = false;
  }
}

void sendStatsPeriodically(int period)
{
  unsigned long myTime = millis();
  if (myTime >= nextTimeWeSendFrame)
  {
    nextTimeWeSendFrame = myTime + period;
    DEBUG("Free memory: " + String(freeMemory()) + "\0");
    // randomizeData();
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
        DEBUG("Request timed out.");
    }
    else
      DEBUG("Modem is not connected, skipping this time.");
    DEBUG("");
  }
}

int fillDataBuffer(char buf[], volatile TelemetryData& stats)
{
  strcpy(buf, "{");
  
  int bodyLength = 1; // the open bracket
  for (int k = 0; k < TelemetryData::Key::_LAST; k++)
  {
    if (stats.isPresent(k))
    {
      bodyLength += toKeyValuePair(buf + strlen(buf), k, stats) + 1; // append the key-value pair, plus the trailing comma
      strcat(buf, ",");
    }
  }
  // Here we are checking if we have data. If so, we need to replace the last trailing comma with a } to close the json body.
  // If not, we need to append a }, and also add 1 to the content length.
  if (buf[strlen(buf)-1] == ',')
    buf[strlen(buf)-1] = '}';
  else if (buf[strlen(buf)-1] == '{')
  {
    strcat(buf, "}");
    bodyLength++;
  }
  return bodyLength;
}

// Send stats to the cloud via Xbee
void sendStats(volatile TelemetryData& stats)
{
  int bodyLength = fillDataBuffer(dataBuffer, stats);
  char bodyLengthStr[4]; 
  sprintf(bodyLengthStr, "%03u", bodyLength);
  
  strcpy(requestBuffer, "POST /car HTTP/1.1\r\nContent-Length: ");
  strcat(requestBuffer, bodyLengthStr);
  strcat(requestBuffer, "\r\nHost: ku-solar-car-b87af.appspot.com\r\nContent-Type: application/json\r\nAuthentication: ");
  strcat(requestBuffer, AUTH_KEY);
  strcat(requestBuffer, "\r\n\r\n");
  strcat(requestBuffer, dataBuffer);
  
  //xbee.sendTCP(IPAddress(142, 250, 190, 84), PORT_HTTPS, 0, PROTOCOL_TLS, 0, requestBuffer, strlen(requestBuffer));
  DEBUG(requestBuffer);
}

// Send stats to DriverHUD over serial
void sendStatsSerial(volatile TelemetryData& stats)
{
  fillDataBuffer(serialDataBuffer, stats);
  Serial.println(serialDataBuffer);
}

int toKeyValuePair(char* dest, int key, volatile TelemetryData& data)
{
  switch(key)
  {
    case TelemetryData::Key::BATT_VOLTAGE: return sprintf(dest, "\"battery_voltage\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::BATT_CURRENT: return sprintf(dest, "\"battery_current\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::BATT_TEMP: return sprintf(dest, "\"battery_temperature\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::BMS_FAULT: return sprintf(dest, "\"bms_fault\":%d", data.getBool(key)); break;
    case TelemetryData::Key::GPS_TIME: return sprintf(dest, "\"gps_time\":%06u", data.getUInt(key)); break;
    case TelemetryData::Key::GPS_DATE: return sprintf(dest, "\"gps_date\":%06u", data.getUInt(key)); break;
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

// TODO: implement rest of these?
void CANCallback(CAN_FRAME* frame)
{
  // if msg id == 0x6B1, let maxTempCallBack handle it
  if (frame->id == 0x6B1)
  {
    maxTempCallback(frame);
  }
  
}

void maxTempCallback(CAN_FRAME* frame) // assume we have a temperature frame
{
  double newTemp = frame->data.bytes[4];
  if (!testStats.isPresent(TelemetryData::Key::BATT_TEMP) || testStats.getDouble(TelemetryData::Key::BATT_TEMP) < newTemp)
  {
    testStats.setDouble(TelemetryData::Key::BATT_TEMP, newTemp);
  }
}

/* =================================
 * Temporarily not being used
 * =================================*/
//void setMaxTemp()
//{
//  // Check for received message
//  long lMsgID;
//  bool bExtendedFormat;
//  byte cRxData[8];
//  byte cDataLen;
//  if(canRx(0, &lMsgID, &bExtendedFormat, &cRxData[0], &cDataLen) == CAN_OK)
//  {
//    if (lMsgID == 0x6B1) {
//      if (cRxData[4] > testStats.getDouble(TelemetryData::Key::BATT_TEMP))
//        testStats.setDouble(TelemetryData::Key::BATT_TEMP, cRxData[4]);
//    }
//  } // end if
//}

//void httpsTest()
//{
//  strcpy(requestBuffer, "GET /get HTTP/1.1\r\nHost: httpbin.org\r\n");
//  xbee.sendTCP(IPAddress(54, 166, 163, 67), 443, 0, 0, requestBuffer, strlen(requestBuffer));
//}
