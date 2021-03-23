/* File: XBee.cpp
 * Author: Andrew Riachi
 * Date: 2020-10-15
 * Description: Implementation of the XBee class
 */
#include "XBee.h"
#include "Frames.h"
#include "IPAddress.h"
#include <Arduino.h>


bool XBee::configure()
{
  m_serial.write("+++");
  delay(1500);
  String ok = m_serial.readStringUntil('\r');
  if (ok != "OK")
    return false;
  m_serial.write("ATAP 1\r");
  delay(2000);
  ok = "";
  ok = m_serial.readStringUntil('\r');
  if (ok != "OK")
    return false;
  m_serial.write("CN\r");
  return true;
}

void XBee::sendFrame(const byte& frameType, const char frameData[], size_t frameDataLen)
{
  uint16_t checksum = frameType;
  uint16_t frameLength = frameDataLen + 1; // length of frameData + length of frameType
  for (uint16_t i = 0; i < frameDataLen; i++)
    checksum += frameData[i];
  checksum = 0xFF - checksum;

  const uint8_t start = 0x7E;
  byte len_msb = (uint8_t) (frameLength >> 8);
  byte len_lsb = (uint8_t) frameLength;

  Serial.print("TX: ");
  m_serial.write(start);
  m_serial.write(len_msb);
  m_serial.write(len_lsb);
  m_serial.write(frameType);
  m_serial.write(frameData, frameDataLen);
  m_serial.write(checksum);
  Serial.println("");
}

void XBee::sendATCommand(uint8_t frameID, const char command[], const char param[], size_t paramLen)
{
  char* frameData = new char[3 + paramLen];
  frameData[0] = frameID;
  memcpy(frameData+1, command, 2);
  memcpy(frameData+3, param, paramLen);
  sendFrame(0x08, frameData, 3+paramLen);
  delete[] frameData;
}

void XBee::shutdown(unsigned int timeout)
{
  sendATCommand(1, "SD", (char) 0x00, 1);
  unsigned int startTime = millis();
  userFrame response;
  do
  {
    response = read();
  } while (response.frameData[0] != 1 && millis() < (startTime + timeout));
  if (!(response == NULL_USER_FRAME))
  {
    Serial.println("Got response:");
    Serial.println("Frame type: " + String(response.frameType, HEX));
    Serial.println("Frame ID: " + String(response.frameData[0], HEX));
    Serial.println("Command: " + String(response.frameData[1]) + String(response.frameData[2]));
    Serial.println("Status: " + String((uint8_t)response.frameData[3]));
  }
  else
  {
    Serial.println("Timed Out.");
  }
  
}

bool XBee::shutdownCommandMode()
{
  m_serial.write("+++");
  delay(1500);
  String ok = m_serial.readStringUntil('\r');
  if (ok != "OK")
    return false;
  m_serial.write("ATSD 0\r");
  m_serial.setTimeout(120000);
  ok = "";
  ok = m_serial.readStringUntil('\r');
  m_serial.setTimeout(1000);
  if (ok != "OK")
    return false;
  m_serial.write("CN\r");
  return true;
}

userFrame XBee::read()
{
  int recvd;
  unsigned long lastRead = millis();
  const unsigned timeout = 7000;
  do
  {
    recvd = m_serial.read();
    if (recvd == -1) 
    {
      // Serial.println("Start delimiter not found.");
      return NULL_USER_FRAME; 
    }
  } while (recvd != 0x7E);

  m_rxBuffer.bytes_recvd = 1;
  Serial.println("Got start delimiter 0x7E");

  while (true)
  {
    while (m_rxBuffer.bytes_recvd < 3 || m_rxBuffer.bytes_recvd < m_rxBuffer.frameLength + 4)
    {
      recvd = m_serial.read();
      if (recvd != -1)
      {
        lastRead = millis();
        m_rxBuffer.bytes_recvd++;
        if (m_rxBuffer.bytes_recvd == 3 || m_rxBuffer.bytes_recvd == 4 || m_rxBuffer.bytes_recvd == (m_rxBuffer.frameLength + 3))
        {
          char bytes_recvd_s[5];
          sprintf(bytes_recvd_s, "%d", m_rxBuffer.bytes_recvd);
          Serial.print("bytes_recvd=");
          Serial.print(bytes_recvd_s);
          Serial.print(" ");
        }
        break;
      }
      else if (millis() > lastRead + timeout)
      {
        char recvd_debug[2];
        sprintf(recvd_debug, "%d", recvd);
        Serial.print(" recvd=");
        Serial.print(recvd_debug);
        Serial.println("Timed out in the middle of receiving a packet.");
        return NULL_USER_FRAME;
      }
    }
    
    if (m_rxBuffer.bytes_recvd == 2)
    {
      m_rxBuffer.frameLength = recvd << 8;
    }
    else if (m_rxBuffer.bytes_recvd == 3)
    {
      m_rxBuffer.frameLength += recvd;
      char lengthField[5];
      sprintf(lengthField, "%d", m_rxBuffer.frameLength);
      Serial.print("Got length field: ");
      Serial.println(lengthField);
    }
    else if (m_rxBuffer.bytes_recvd == 4)
    {
      m_rxBuffer.frameType = recvd;
      char frameType[2];
      sprintf(frameType, "%02X", recvd);
      Serial.print("Got frame type: ");
      Serial.println(frameType);
    }
    else if ((m_rxBuffer.bytes_recvd - 4) <= m_rxBuffer.frameDataLength())
    {
      m_rxBuffer.frameData[m_rxBuffer.bytes_recvd - 5] = (char) recvd;
    }
    else
    {
      m_rxBuffer.checksum = recvd;
      char checksum[2];
      char lastData[2];
      sprintf(checksum, "%02X", recvd);
      sprintf(lastData, "%02X", m_rxBuffer.frameData[m_rxBuffer.frameDataLength() - 1]);
      Serial.print("End frame. Last data: ");
      Serial.print(lastData);
      Serial.print("; Checksum: ");
      Serial.println(checksum);

      uint8_t verify = m_rxBuffer.frameType;
      for (int i = 0; i < m_rxBuffer.frameDataLength(); i++)
      {
        verify += m_rxBuffer.frameData[i];
      }
      verify += m_rxBuffer.checksum;

      userFrame ret;
      if (verify == 0xFF)
      {
        Serial.println("Frame is verified (0xFF).");
        ret = {m_rxBuffer.frameType, m_rxBuffer.frameData, m_rxBuffer.frameDataLength()};
      }
      else
      { // poo poo frame, return nothing to our poor user :(
        char verify_s[2];
        sprintf(verify_s, "%02X", verify);
        Serial.print("Failed to verify frame. verify=");
        Serial.println(verify_s);
        ret = NULL_USER_FRAME;
      }
      
      Serial.println("END RX");
      m_rxBuffer.bytes_recvd = 0;
      return ret;
    }
  }
}

void XBee::sendTCP(IPAddress address, uint16_t destPort, uint16_t sourcePort, uint8_t options, const char payload[], size_t payloadLength)
{
  size_t bufSize = 11 + payloadLength;
  char* buf = new char[bufSize];
  buf[0] = 0; // frameID = 0
  uint32_t addr_array = address;
  memcpy(buf+1, &addr_array, 4);
  buf[5] = (char) (destPort >> 8); // The arduino is little-endian but tht digi wants big-endian here
  buf[6] = (char) (destPort);
  buf[7] = (char) (sourcePort >> 8);
  buf[8] = (char) (sourcePort);
  buf[9] = 0x04;
  buf[10] = options;
  memcpy(buf+11, payload, payloadLength);

  sendFrame(0x20, buf, bufSize);

  delete[] buf;
}

bool XBee::isConnected(unsigned timeout)
{
  sendATCommand(1, "AI", nullptr, 0);
  userFrame resp;
  const unsigned long myTime = millis();
  do
  {
    resp = read();
    if (resp.frameType == 0x88 && strncmp(resp.frameData+1, "AI", 2) == 0)
    {
      // Serial.print("Got here: ");
      // Serial.write(resp.frameData+1, 2);
      return (resp.frameData[4] == 0x00);
    }
  } while (millis() < myTime+timeout);
  Serial.println("Connectivity check timed out");
  return false;
}
