#ifndef GPSFORMATTER_H
#define GPSFORMATTER_H

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

GPSFormatter::GPSFormatter(Stream* serialport)
{
  serial = serialport;
}

void GPSFormatter::readSerial()
{
  int delay = 2000;
  unsigned long start = millis();
  do 
  {
    while (serial->available())
    {
      char red = serial->read();
      gps.encode(red);
    }
  } while (millis() - start < delay);
}

void GPSFormatter::writeToData(volatile TelemetryData& stats)
{
  float flat, flon, fspeed, fcourse;
  unsigned long presDate, presTime;
  unsigned long fixAge, presAge;

  gps.get_datetime(&presDate, &presTime, &presAge);
  if (!(presAge == TinyGPS::GPS_INVALID_AGE))
  {
    stats.setUInt(TelemetryData::Key::GPS_DATE, presDate);
    stats.setUInt(TelemetryData::Key::GPS_TIME, presTime);
    #ifdef DEBUG_BUILD
      Serial.print("Wrote date: ");
      Serial.println(presDate);
    #endif
  }
  else
  {
    DEBUG("GPS_INVALID_AGE");
  }

  gps.f_get_position(&flat, &flon, &fixAge);
  if(!(flat == TinyGPS::GPS_INVALID_F_ANGLE || flon == TinyGPS::GPS_INVALID_F_ANGLE))
  {
    stats.setDouble(TelemetryData::Key::GPS_LAT, flat);
    stats.setDouble(TelemetryData::Key::GPS_LON, flon);
    #ifdef DEBUG_BUILD
      Serial.print("Wrote lon: ");
      Serial.println(flon);
    #endif
  }

  fspeed = gps.f_speed_mph();
  if(!(s == TinyGPS::GPS_INVALID_F_SPEED)) {
    stats.setDouble(TelemetryData::Key::GPS_SPD, fspeed);
  }

  fcourse = gps.f_course();
  if(!(fcourse == TinyGPS::GPS_INVALID_ANGLE)) {
    stats.setDouble(TelemetryData::Key::GPS_COURSE, fcourse);
  }
}

#endif
