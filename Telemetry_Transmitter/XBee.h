/* File: XBee.h
 * Author: Andrew Riachi
 * Date: 2020-10-15
 * Description: Functions to help send packets using the Digi module
 */
#ifndef XBEE_H
#define XBEE_H

#include <Arduino.h>
#include "MonitoredSerial.h"

struct packet
{
  const uint16_t MAX_BUFFER_SIZE = 1550;
  packet();
  ~packet();
  packet& operator=(const packet& other);
  bool recvd_start;
  uint16_t bytes_recvd;
  uint16_t length;
  uint8_t frameType;
  uint8_t frameID;
  char* frameData;
  bool recvd_checksum;
  uint8_t checksum;
};

struct userPacket
{
  bool operator==(const userPacket& other);
  uint8_t frameType;
  uint8_t frameID;
  const char* frameData;
  uint16_t frameDataLength;
};

const userPacket NULL_USER_PACKET = {0, 0, nullptr, 0};

class XBee
{
  private:
  MonitoredSerial& m_serial;
  packet m_rxBuffer;

  public:
  XBee(MonitoredSerial& serial) : m_serial(serial){}
  bool configure(const String& server);
  void openConnection();
  void closeConnection();
  void sendPacket(const String& message);
  userPacket read(); // Can read one full packet at a time.
  bool waitFor(const String& message, int timeout);
  bool shutdownCommandMode();
  void shutdown(unsigned int timeout);
  void sendFrame(byte frameType, byte frameID, const char frameData[], size_t frameDataLen);
  void sendATCommand(uint8_t frameID, const char command[], const char param[], size_t paramLen);
};

#endif
