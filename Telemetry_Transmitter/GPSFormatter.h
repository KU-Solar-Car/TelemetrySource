#ifndef GPSFORMATTER
#define GPSFORMATTER

#include <TinyGPS.h> // https://github.com/mikalhart/TinyGPS
#include "Stats.h"

struct GPSFormatter
{
private:
  TinyGPS gps;
  Stream* serial;

public:
  GPSFormatter(Stream* serialport);
  void readSerial();
  void writeToData(volatile TelemetryData& stats);
};

GPSFormatter::GPSFormatter(Stream* serialport) {
  serial = serialport;
}

void GPSFormatter::readSerial() {
  int delay = 5000;
  unsigned long start = millis();
  do 
  {
    while (serial->available()) {
      char red = serial->read();
      // Serial.print(red);
      gps.encode(red);
    }
  } while (millis() - start < delay);
}

void GPSFormatter::writeToData(volatile TelemetryData& stats) {
  
  // Variables in question.
  float flat, flon;
  unsigned long presDate, presTime;
  unsigned long fixAge, presAge;
  
  gps.get_datetime(&presDate, &presTime, &presAge);
  
  if (!(presAge == TinyGPS::GPS_INVALID_AGE)) {
    stats.setUInt(TelemetryData::Key::GPS_DATE, presDate);
    stats.setUInt(TelemetryData::Key::GPS_TIME, presTime);
    Serial.print("Wrote date: ");
    Serial.print(presDate);
    Serial.print("\r\n");
  }

  gps.f_get_position(&flat, &flon, &fixAge);
  if(!(flat == TinyGPS::GPS_INVALID_F_ANGLE || flon == TinyGPS::GPS_INVALID_F_ANGLE)) {
    stats.setDouble(TelemetryData::Key::GPS_LAT, flat);
    stats.setDouble(TelemetryData::Key::GPS_LON, flon);

    Serial.print("Wrote lon: ");
    Serial.print(flon);
    Serial.print("\r\n");
  }
}

#endif
