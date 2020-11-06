/* File: XBee.h
 * Author: Andrew Riachi
 * Date: 2020-10-15
 * Description: Functions to help send packets using the Digi module
 */
#ifndef XBEE_H
#define XBEE_H

#include <Arduino.h>
#include <string>

struct packet
{
  packet();
  bool recvd_start;
  bool recvd_len_msb;
  bool recvd_len_lsb;
  bool recvd_frameType;
  bool recvd_checksum;
  uint16_t length;
  uint8_t frameType;
  String frameData;
  uint8_t checksum;
};

struct userPacket
{
  const uint8_t& frameType;
  const String& frameData;
};

const userPacket NULL_USER_PACKET = {0, ""};

class XBee
{
  private:
  HardwareSerial& m_serial;
  packet m_rxBuffer;

  public:
  XBee(MonitoredSerial &serial) : m_serial(serial) {}
  bool configure(const String& server);
  void openConnection();
  void closeConnection();
  void sendPacket(const String& message);
  userPacket read(); // Can read one full packet at a time.
  bool waitFor(const String& message, int timeout);
  bool shutdownCommandMode();
  void shutdown();
  void sendFrame(byte frameType, const String& frameData);
  void sendATCommand(const String& command, const String& param);
};

#endif
