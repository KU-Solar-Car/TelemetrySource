#include "MonitoredSerial.h"

int MonitoredSerial::read(void)
{
  int temp = m_toMonitor.read();
  if (temp != -1 && !m_suppress)
    printByte(temp);
  return temp;
}

size_t MonitoredSerial::write(uint8_t n)
{
  m_toMonitor.write(n);
  if (!m_suppress)
    printByte(n);
  return 1;
}

int MonitoredSerial::peek()
{
  return m_toMonitor.peek();
}

int MonitoredSerial::available()
{
  return m_toMonitor.available();
}

void MonitoredSerial::flush()
{
  return m_toMonitor.flush();
}

void MonitoredSerial::suppress()
{
  m_suppress = true;
}

void MonitoredSerial::unsuppress()
{
  m_suppress = false;
}

void MonitoredSerial::printByte(uint8_t n)
{
  static char toPrint[3];
  sprintf(toPrint, "%02X", n);
  m_listener.print(toPrint);
  m_listener.print(" ");
}
