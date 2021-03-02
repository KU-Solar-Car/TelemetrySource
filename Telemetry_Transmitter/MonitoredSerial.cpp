#include "MonitoredSerial.h"

int MonitoredSerial::read(void)
{
  int temp = m_toMonitor.read();
  if (temp != -1 && !m_suppress)
    m_listener.print(String(temp, HEX) + " ");
  return temp;
}

size_t MonitoredSerial::write(uint8_t n)
{
  m_toMonitor.write(n);
  if (!m_suppress)
    m_listener.print(String(n, HEX) + " ");
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
