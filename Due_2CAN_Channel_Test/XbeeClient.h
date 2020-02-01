#ifndef XBEE_CLIENT_H
#define XBEE_CLIENT_H

#define SUCCESS 1
#define TIMED_OUT -1

#include <Arduino.h>
#include <Client.h>

class XbeeClient : public Client {
 public:
  XbeeClient(size_t baud);
  int connect(IPAddress ip, uint16_t port);
  int connect(const char* host, uint16_t port);
  uint8_t connected();
  size_t write(uint8_t data);
  size_t write(const uint8_t* buf, size_t size);
  int available();
  int read();
  int read(uint8_t* buf, size_t size);
  int peek();
  void flush(); 
  void stop();
  operator bool();
 private:
  const static int RESPONSE_TIMEOUT = 5000;
  int _connect(String address, uint16_t port);
  const char* host;
  uint16_t port;
};

#endif
