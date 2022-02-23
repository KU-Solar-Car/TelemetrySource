#include "debug.h"
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
#include <DueTimer.h>
#include <Scheduler.h>

// Number of milliseconds to wait between transmitting data packets
const int XBEE_TX_INTERVAL = 2000;

const byte BYTE_MIN = -128;
byte maxTemp;

unsigned long nextTimeWeSendFrame;

// Monitored serial for debugging XBee
//MonitoredSerial mySerial(Serial2, Serial);
//XBee xbee(mySerial);

XBee xbee(Serial2);

GPSFormatter gpsFormatter(&Serial1); // 19rx, 18tx

const size_t DATA_BUFFER_SIZE = 550;
char dataBuffer[DATA_BUFFER_SIZE]; // Buffer for XBee data

const size_t REQUEST_BUFFER_SIZE = 600;
char requestBuffer[REQUEST_BUFFER_SIZE];

// Separate stats for XBee and DriverHUD since they get cleared after sending
volatile TelemetryData xbeeStats;
volatile TelemetryData serialStats;

volatile bool resetButtonPressed = false;
volatile bool shutdownButtonPressed = false;

volatile bool resetButtonMaybePressed = false;
volatile bool shutdownButtonMaybePressed = false;

volatile bool checkShutdown = false;
volatile bool checkConnected = false;

const int SHUTDOWN_PIN = 2;
const int RESET_PIN = 3;
const unsigned BUTTON_DEBOUNCE_MICROS = 10000;

// Setup and loop =============================================================

void setup()
{
  Serial.begin(115200); // Interface with DriverHUD and/or debugging
  Serial2.begin(9600); // Interface with XBee
  Serial1.begin(4800); // Interface with GPS
  
  /* =================================
   * Initialize CAN board
   * =================================*/
  if(canInit(0, CAN_BPS_250K) == CAN_OK)
    DEBUG("CAN0: Initialized successfully");
  else
    DEBUG("CAN0: Initialization failed");

  /* =================================
   * Set the CAN interrupt
   * =================================*/
   Can0.setGeneralCallback(CANCallback);

  // Suppress the serial monitoring
  // mySerial.suppress();

  /* =================================
   * Set the XBee in API Mode
   * =================================*/
  if(xbee.configure())
    DEBUG("XBee: Configuration successful");
  else
    DEBUG("XBee: Configuration failed"); // This happens sometimes - maybe should do multiple attempts
   
  /* =================================
   * Wait for modem to associate before starting 
   * =================================*/
  userFrame status;
  // TODO: This doesn't seem reliable. Maybe use isConnected
  // Could also move this to xbeeLoop so other tasks don't wait for it
  DEBUG("XBee: Waiting for network to associate. Send \"!\" to skip.");
  do
  {
    status = xbee.read();
    printReceivedFrame(status);
  } while(!(status.frameType == 0x8A && status.frameData[0] == 2) && Serial.read() != '!');
  DEBUG("XBee: Network associated");
  
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

  // Setup multiple loops for the hardware scheduler to run
  // If a loop is doing something that takes a long time (e.g. busy waiting
  // or serial with long timeout), it should call yield() on a regular basis
  // to allow other loops to run. The built-in delay function works as well.
  Scheduler.startLoop(xbeeLoop);
  Scheduler.startLoop(gpsLoop);
  Scheduler.startLoop(commandLoop);

  // Maybe this should be Pi-readable format
  DEBUG("Setup complete");
}

// Separate loops for different tasks =========================================

// CAN data loop
void loop()
{
  // Read most data from CAN bus
  // setMaxTemp();
  yield();
}

void xbeeLoop() {
  sendStatsPeriodically(XBEE_TX_INTERVAL);

  // Send stats to the XBee
  /*if (millis() >= nextTimeWeSendFrame)
  {
    nextTimeWeSendFrame += XBEE_TX_INTERVAL;
    sendStats(xbeeStats);
  }*/
  yield();
}

void gpsLoop() {
  //DEBUG("GPS loop");
  // Read GPS values
  gpsFormatter.readSerial(10); // Doesn't need a long timeout
  gpsFormatter.writeToData(xbeeStats);
  gpsFormatter.writeToData(serialStats);
  yield();
}

void commandLoop() {
  // Check for serial commands and shutdown/reset buttons
  checkSerialCommands();
  shutdownOnCommand();
  yield();
}

// Shutdown and reset button interrupts =======================================

void shutdown_interrupt()
{
  if (digitalRead(digitalPinToInterrupt(SHUTDOWN_PIN)) == LOW)
  {
    shutdownButtonPressed = true;
  }
  shutdownButtonMaybePressed = false;
  Timer0.stop();
}
void reset_interrupt()
{
  if (digitalRead(digitalPinToInterrupt(RESET_PIN)) == LOW)
  {
    resetButtonPressed = true;
  }
  resetButtonMaybePressed = false;
  Timer1.stop();
}

void shutdown_debounce_interrupt()
{
  // first, check that we're not already debouncing.
  if (!shutdownButtonMaybePressed)
  {
    shutdownButtonMaybePressed = true;
    Timer0.start(BUTTON_DEBOUNCE_MICROS);
  }
}
void reset_debounce_interrupt()
{
  if (!resetButtonMaybePressed)
  {
    resetButtonMaybePressed = true;
    Timer1.start(BUTTON_DEBOUNCE_MICROS);
  }
}

// Run commands ===============================================================

void checkSerialCommands() {
  char cmd = Serial.read();
  if (cmd == 's') shutdownButtonPressed = true;
  else if (cmd == 'r') resetButtonPressed = true;
  else if (cmd == 'd') sendStatsSerial(serialStats); // DriverHUD requesting data
  else if (cmd == 'x') checkShutdown = true;
  else if (cmd == '?') checkConnected = true;
}

// TODO: Format the output of shutdown/reset in a way the DriverHUD can understand
void shutdownOnCommand()
{
  if (shutdownButtonPressed)
  {
    Serial.println("Shutting down, please wait up to 2 minutes...");
    if (Serial.read() != 'c')
    {
      xbee.shutdown(120000, false);
    }
    else
    {
      if (xbee.shutdownCommandMode())
      {
        Serial.println("Shutdown successful");
      }
      else
      {
        Serial.println("Shutdown failed");
      }
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
    Serial.println("Checking if XBee is shutdown, please wait up to 5 seconds...");
    bool isShutdown = xbee.isShutDown(5000);
    if (isShutdown) Serial.println("isShutDown: YES!");
    else Serial.println("isShutDown: No :(");
    checkShutdown = false;
  }
  else if (checkConnected)
  {
    Serial.println("Checking if XBee is connected, please wait up to 5 seconds...");
    bool isConnected = xbee.isConnected(5000);
    if (isConnected) Serial.println("isConnected: YES!");
    else Serial.println("isConnected: No :(");
    checkConnected = false;
  }
}

// Format and send telemetry data =============================================

void sendStatsPeriodically(int period)
{
  unsigned long myTime = millis();
  if (myTime >= nextTimeWeSendFrame)
  {
    nextTimeWeSendFrame = myTime + period;
    DEBUG("Free memory: " + String(freeMemory()) + "\0");
    // randomizeData(xbeeStats);
    if (xbee.isConnected(5000))
    {
      userFrame resp;
      sendStats(xbeeStats);
      const unsigned long myTime = millis();
      const unsigned timeout = 10000;
      xbeeStats.clear(); // TODO: This should only happen if sent successfully
      do
      {
        resp = xbee.read();
      } while (millis() < myTime+timeout && resp.frameType != 0xB0);
      if (resp.frameType != 0xB0)
      {
        DEBUG("Request timed out.");
      }
    }
    else
    {
      DEBUG("Modem is not connected, skipping this time.");
    }
    DEBUG("");
  }
}

// Create a JSON string containing the latest sensor values from stats and store it in buf
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
  {
    buf[strlen(buf)-1] = '}';
  }
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
  DEBUG("sendStats");
  int bodyLength = fillDataBuffer(dataBuffer, stats);
  char bodyLengthStr[4]; 
  sprintf(bodyLengthStr, "%03u", bodyLength);
  
  strcpy(requestBuffer, "POST /car HTTP/1.1\r\nContent-Length: ");
  strcat(requestBuffer, bodyLengthStr);
  strcat(requestBuffer, "\r\nHost: ku-solar-car-b87af.appspot.com\r\nContent-Type: application/json\r\nAuthentication: ");
  strcat(requestBuffer, AUTH_KEY);
  strcat(requestBuffer, "\r\n\r\n");
  strcat(requestBuffer, dataBuffer);
  
  DEBUG(dataBuffer);
  // DEBUG(requestBuffer);
  xbee.sendTCP(IPAddress(142, 250, 190, 84), PORT_HTTPS, 0, PROTOCOL_TLS, 0, requestBuffer, strlen(requestBuffer));
}

// Send stats to DriverHUD over serial
void sendStatsSerial(volatile TelemetryData& stats)
{
  DEBUG("sendStatsSerial");
  fillDataBuffer(dataBuffer, stats);
  Serial.println(dataBuffer);
  stats.clear();
}

int toKeyValuePair(char* dest, int key, volatile TelemetryData& data)
{
  switch(key)
  {
    case TelemetryData::Key::BATT_VOLTAGE: return sprintf(dest, "\"battery_voltage\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::BATT_CURRENT: return sprintf(dest, "\"battery_current\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::BATT_TEMP: return sprintf(dest, "\"battery_temperature\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::BMS_FAULT: return sprintf(dest, "\"bms_fault\":%d", data.getBool(key)); break;
    case TelemetryData::Key::GPS_COURSE: return sprintf(dest, "\"gps_course\":%6f", data.getDouble(key)); break;
    case TelemetryData::Key::GPS_TIME: return sprintf(dest, "\"gps_time\":\"%08u\"", data.getUInt(key)); break;
    case TelemetryData::Key::GPS_DATE: return sprintf(dest, "\"gps_date\":\"%06u\"", data.getUInt(key)); break;
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

// Read data from CAN bus =====================================================

void CANCallback(CAN_FRAME* frame)
{
  #ifdef DEBUG_BUILD
    printCanFrame(frame);
  #endif
  processCanFrame(frame, xbeeStats);
  processCanFrame(frame, serialStats);
}

void processCanFrame(CAN_FRAME* frame, volatile TelemetryData& stats) {
  switch (frame->id)
  {
  case 0x36:
    break;
  case 0x6b0:
    break;
  case 0x6b1: // Battery temperature - keep the max value
    keepMaxStat(stats, TelemetryData::Key::BATT_TEMP, frame->data.bytes[4]);
    break;
  case 0x6b2:
    break;
  case 0x6b3:
    break;
  case 0x6b4:
    break;
  case 0x6b5:
    break;
  default:
    break;
  }
}

void keepMaxStat(volatile TelemetryData& stats, int key, double newValue)
{
  if (!stats.isPresent(key) || stats.getDouble(key) < newValue)
  {
    stats.setDouble(key, newValue);
  }
}

// For testing, set some sensors to random values
void randomizeData(volatile TelemetryData& stats)
{
  for (int i = 0; i < TelemetryData::Key::_LAST; i++)
  {
    
    if (i == TelemetryData::Key::BMS_FAULT)
    {
      stats.setBool(i, static_cast<bool>(random(0, 2))); // Excludes the max
    }
    else if (i == TelemetryData::Key::GPS_TIME)
    {
      stats.setUInt(i, static_cast<unsigned int>(random(5001)));
    }
    else
    {
      stats.setDouble(i, random(0, 8000) / static_cast<double>(random(1, 100)));
    }
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
//  if (canRx(0, &lMsgID, &bExtendedFormat, &cRxData[0], &cDataLen) == CAN_OK)
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

void printReceivedFrame(userFrame& recvd)
{
  if (!(recvd == NULL_USER_FRAME))
  {
    #ifdef DEBUG_BUILD
      Serial.println("[BEGIN XBee frame]:");
      Serial.println("Frame type: " + String(recvd.frameType, HEX));
      Serial.print("Frame data: ");
      Serial.write(recvd.frameData, recvd.frameDataLength);
      Serial.println("[END XBee frame]");
    #endif
  }
  // else
    // DEBUG("Got here nothing :(");
}

void printCanFrame(CAN_FRAME* frame)
{
  Serial.println("[BEGIN CAN frame]:");
  Serial.println("ID (EID/SID): 0x" + String(frame->id, HEX));
  Serial.println("Family ID: 0x" + String(frame->fid, HEX));
  Serial.println("Extended ID: 0x" + String(frame->extended, HEX));
  Serial.println("Length: " + String(frame->length));
  //Serial.println("Data (in hex): " + String(frame->data.value, HEX)); // Can't do uint64_t
  String data = "Data (in hex): " ;
  for (int i = 0; i < frame->length; i++) {
    data += String(frame->data.bytes[i], HEX) + " ";
  }
  Serial.println(data);
  Serial.println("[END CAN frame]");
}