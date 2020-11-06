#include <Arduino>

class MonitoredSerial: public Stream
{
  public:
  MonitoredSerial(HardwareSerial toMonitor, HardwareSerial listener) : 
   {m_toMonitor(toMonitor), m_listener(listener)}
  int read(void);
  size_t write(uint8_t);
  int available();
  int peek();
  
  private:
  HardwareSerial m_toMonitor;
  HardwareSerial m_listener;
}
