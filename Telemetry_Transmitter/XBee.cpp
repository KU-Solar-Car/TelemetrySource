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

  m_serial.write(start);
  m_serial.write(len_msb);
  m_serial.write(len_lsb);
  m_serial.write(frameType);
  m_serial.write(frameData, frameDataLen);
  m_serial.write(checksum);
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
  m_serial.setTimeout(30000);
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
  for (int recvd = m_serial.read(); recvd != -1 && (m_rxBuffer.bytes_recvd < m_rxBuffer.length()); recvd = m_serial.read())
  {
    if (m_rxBuffer.bytes_recvd == 1)
    {
      m_rxBuffer.frameLength = recvd << 8;
    }
    else if (m_rxBuffer.bytes_recvd == 2)
    {
      m_rxBuffer.frameLength += recvd;
    }
    else if (m_rxBuffer.bytes_recvd == 3)
    {
      m_rxBuffer.frameType = recvd;
    }
    else if (m_rxBuffer.frameDataRecvd() < m_rxBuffer.frameDataLength())
    {
      m_rxBuffer.frameData[m_rxBuffer.frameDataRecvd()] = (char) recvd; // use a char here because that is probably what append is defined for
    }
    else
    {
      m_rxBuffer.checksum = recvd;
    }
    if ((m_rxBuffer.bytes_recvd == 0 && recvd == 0x7E) || m_rxBuffer.bytes_recvd > 0)
      m_rxBuffer.bytes_recvd++;
  }

  if (m_rxBuffer.bytes_recvd == m_rxBuffer.length())
  {
    uint8_t verify = m_rxBuffer.frameType;
    for (int i = 0; i < m_rxBuffer.frameDataLength(); i++)
    {
      verify += m_rxBuffer.frameData[i];
    }
    verify += m_rxBuffer.checksum;

    userFrame ret;
    if (verify == 0xFF)
    {
      Serial.print("Frame is verified\n");
      ret = {m_rxBuffer.frameType, m_rxBuffer.frameData, m_rxBuffer.frameDataLength()};
    }
    else
    { // poo poo frame, return nothing to our poor user :(
      Serial.print("Failed to verify frame. verify=" + String(verify, HEX) + " bytes_recvd=" + String(m_rxBuffer.bytes_recvd) + "\n");
      ret = NULL_USER_FRAME;
    }
    
    m_rxBuffer.bytes_recvd = 0; // reset the bytes received so that we will look for a frame next time
    return ret;
  }
  else
  {
    return NULL_USER_FRAME;
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
