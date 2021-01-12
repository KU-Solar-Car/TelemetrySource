/* File: XBee.h
 * Author: Andrew Riachi
 * Date: 2020-10-15
 * Description: Functions to help send packets using the Digi module
 */
#ifndef XBEE_H
#define XBEE_H

#include <Arduino.h>
#include "MonitoredSerial.h"
#include "Frames.h"



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
  userPacket read(unsigned timeout); // Can read one full packet at a time.
  bool waitFor(const String& message, int timeout);
  bool shutdownCommandMode();
  void shutdown(unsigned int timeout);
  void sendFrame(const byte& frameType, const char frameData[], size_t frameDataLen);
  void sendATCommand(uint8_t frameID, const char command[], const char param[], size_t paramLen);
};

#endif
