#include "XbeeClient.h"

XbeeClient::XbeeClient(size_t baud) {
  Serial.begin(baud);
  Serial.setTimeout(RESPONSE_TIMEOUT);
}

int XbeeClient::connect(IPAddress ip, uint16_t port) {
  String ipString = String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]);
  return _connect(ipString, port);
}

int XbeeClient::connect(const char* host, uint16_t port) {
  return _connect(String(host), port);
}

uint8_t XbeeClient::connected() {
  Serial.write("+++");
  String response = Serial.readStringUntil('\r');
  if (!response.equals("OK")) {
    SerialUSB.println(response);
    return 0;
  }

  Serial.write("ATOD\r");
  response = Serial.readStringUntil('\r');
  SerialUSB.println(response);
  return response != "0.0.0.0";
}

size_t XbeeClient::write(uint8_t data) {
  return Serial.write(data);
}

size_t XbeeClient::write(const uint8_t* buf, size_t size) {
  return Serial.write(buf, size);
}

int XbeeClient::available() {
  return Serial.available();
}

int XbeeClient::read() {
  return Serial.read();
}

int XbeeClient::read(uint8_t* buf, size_t size) {
  return Serial.readBytes(buf, size);
}

int XbeeClient::peek() {
  return Serial.peek();
}

void XbeeClient::flush() {
  Serial.flush();
}

void XbeeClient::stop() {
  Serial.end();
}

XbeeClient::operator bool() {
  return connected();
}

int XbeeClient::_connect(String address, uint16_t port) {
  Serial.write("+++"); // put Xbee into command mode
  String response1 = Serial.readStringUntil('\r');
  if (!response1.equals("OK")) {
    return -5;
  }

  Serial.print("ATDL" + address + "\r");
  String response2 = Serial.readStringUntil('\r');
  if (!response2.equals("OK")) {
    return -2;
  }

  Serial.print("ATDE" + String(port, 16) + "\r");
  String response3 = Serial.readStringUntil('\r');
  if (!response3.equals("OK")) {
    return -3;
  }

  Serial.print("ATCN\r");
  String response4 = Serial.readStringUntil('\r');
  if (!response4.equals("OK")) {
    return -4;
  }

  return SUCCESS;
}
