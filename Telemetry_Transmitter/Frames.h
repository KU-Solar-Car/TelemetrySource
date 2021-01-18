#ifndef FRAMES_H
#define FRAMES_H

#include <Arduino.h>

struct frame
{
  const uint16_t MAX_BUFFER_SIZE = 1550;
  frame();
  ~frame();
  frame& operator=(const frame& other);

  uint16_t frameDataRecvd(); // the number of octets of frameData (without frameType) we have received
  uint16_t frameDataLength(); // the length of the frameData array, without frameType
  uint16_t length(); // the total length of the frame

  uint16_t bytes_recvd;
  uint16_t frameLength; // the length octets we receive from the packet
  uint8_t frameType;
  char* frameData;
  uint8_t checksum;
};

inline uint16_t frame::frameDataRecvd() // I don't know if inlining this one will optimize it, but maybe the compiler is smart
{
  if (bytes_recvd <= 4)
    return 0;
  else if ((bytes_recvd - 4) > frameDataLength())
    return frameDataLength();
  else
    return (bytes_recvd - 4);
}

inline uint16_t frame::frameDataLength()
{
  return (bytes_recvd >= 3 ? frameLength - 1 : 0);
}

inline uint16_t frame::length()
{
  return (bytes_recvd >= 3 ? frameLength + 4 : 4);
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
