#ifndef FRAMES_H
#define FRAMES_H

#include <Arduino.h>

struct packet
{
  const uint16_t MAX_BUFFER_SIZE = 1550;
  packet();
  ~packet();
  packet& operator=(const packet& other);

  uint16_t bytes_recvd;
  uint16_t length;
  uint8_t frameType;
  char* frameData;
  uint8_t checksum;
};

struct userPacket
{
  bool operator==(const userPacket& other);
  
  uint8_t frameType;
  const char* frameData;
  uint16_t frameDataLength;
};

const userPacket NULL_USER_PACKET = {0, nullptr, 0};

#endif
