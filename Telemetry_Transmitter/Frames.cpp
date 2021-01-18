#include "Frames.h"

frame::frame()
{
  // Helps prevent heap fragmentation (even though it's very possible in the rest of my code :/)
  frameData = new char[MAX_BUFFER_SIZE];
}

frame::~frame()
{
  delete[] frameData;
}



frame& frame::operator=(const frame& other)
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

bool userFrame::operator==(const userFrame& other)
{
  return (
    frameType == other.frameType
    && frameDataLength == other.frameDataLength
    && frameData == other.frameData
  );
}
