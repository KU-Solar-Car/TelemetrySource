#include "Frames.h"

packet::packet()
{
  // Helps prevent heap fragmentation (even though it's very possible in the rest of my code :/)
  frameData = new char[MAX_BUFFER_SIZE];
}

packet::~packet()
{
  delete[] frameData;
}

packet& packet::operator=(const packet& other)
{
  bytes_recvd = other.bytes_recvd;
  length = other.length;
  frameType = other.frameType;
  for (unsigned i = 0; i < (length-1); i++)
  {
    frameData[i] = other.frameData[i]; // copy the new frame data
  }
  for (unsigned i = length-1; i < MAX_BUFFER_SIZE; i++)
  {
    frameData[i] = '\0'; // clear the remaining bytes
  }

  checksum = other.checksum;
}

bool userPacket::operator==(const userPacket& other)
{
  return (
    frameType == other.frameType
    && frameDataLength == other.frameDataLength
    && frameData == other.frameData
  );
}
