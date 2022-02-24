#ifndef STATS_H
#define STATS_H

struct TelemetryData
{
  enum Key: int
  {
    // BMS
    BATT_VOLTAGE,
    BATT_CURRENT,
    BATT_TEMP,
    BMS_FAULT,

    // GPS
    GPS_COURSE, // Degrees
    GPS_TIME, // Format hhmmsscc
    GPS_DATE, // Format ddmmyy
    GPS_LAT,
    GPS_LON,
    GPS_SPD, // MPH

    // Other
    SOLAR_VOLTAGE,
    SOLAR_CURRENT,
    MOTOR_SPD,
    // TODO: Add motor temperature

    _LAST
  };

  // TODO: Define string format and type for each of above

  struct
  {
    union {
      double doubleVal;
      unsigned uIntVal;
      bool boolVal;
    };
    bool present;
  } arr[Key::_LAST];

  void setDouble(int key, double value) volatile;
  void setUInt(int key, unsigned value) volatile;
  void setBool(int key, bool value) volatile;
  void unset(int key) volatile;


  double getDouble(int key) volatile;
  unsigned getUInt(int key) volatile;
  bool getBool(int key) volatile;
  
  bool isPresent(int key) volatile;

  void clear() volatile;
  
};


#endif
