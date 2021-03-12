#ifndef MONITOREDSERIAL_H
#define MONITOREDSERIAL_H

#include <Arduino.h>

class MonitoredSerial: public Stream
{
  public:
  MonitoredSerial(Stream& toMonitor, Stream& listener) : 
   m_toMonitor(toMonitor), m_listener(listener) {}
  int read(void);
  size_t write(uint8_t);
  inline size_t write(unsigned long n) { return write((uint8_t)n); }
  inline size_t write(long n) { return write((uint8_t)n); }
  inline size_t write(unsigned int n) { return write((uint8_t)n); }
  inline size_t write(int n) { return write((uint8_t)n); }
  using Print::write; // pull in write(str) and write(buf, size) from Print
  int available();
  int peek();
  void flush();
  void suppress();
  void unsuppress();
  void printByte(uint8_t n);
  
  private:
  Stream& m_toMonitor;
  Stream& m_listener;
  bool m_suppress;
};

#endif
