#ifndef FRAMES_H
#define FRAMES_H

#include <Arduino.h>

struct frame
{
  const uint16_t MAX_BUFFER_SIZE = 1550;
  frame();
  ~frame();
  frame& operator=(const frame& other);
  uint16_t frameDataLength();

  uint16_t bytes_recvd;
  uint16_t frameLength; // the length octets we receive from the packet
  uint8_t frameType;
  char* frameData;
  uint8_t checksum;
};

inline uint16_t frame::frameDataLength()
{
  return frameLength - 1;
}


struct userFrame
{
  bool operator==(const userFrame& other);
  
  uint8_t frameType;
  const char* frameData;
  uint16_t frameDataLength;
};

const userFrame NULL_USER_FRAME = {0, nullptr, 0};

#endif
