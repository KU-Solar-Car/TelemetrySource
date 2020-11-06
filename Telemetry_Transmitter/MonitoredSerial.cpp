int MonitoredSerial::read(void)
{
  int temp = m_toMonitor.read();
  m_listener.write(temp);
  return temp;
}

size_t MonitoredSerial::write(uint8_t n)
{
  m_toMonitor.write(n);
  m_listener.write(n);
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
