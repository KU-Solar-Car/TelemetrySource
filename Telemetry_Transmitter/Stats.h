#ifndef STATS_H
#define STATS_H

struct TelemetryData
{
  enum Key: int
  {
    // BMS
    PACK_VOLTAGE,
    PACK_SOC,
    PACK_CURRENT,
    MIN_PACK_TEMP,
    AVG_PACK_TEMP,
    MAX_PACK_TEMP,
    MIN_CELL_VOLTAGE,
    AVG_CELL_VOLTAGE,
    MAX_CELL_VOLTAGE,
    INPUT_VOLTAGE,
    AVG_CELL_RESISTANCE,
    BMS_FAULT,

    // Motor controller
    MOTOR_SPEED,
    MOTOR_CURRENT,
    MOTOR_CONTROLLER_TEMP,
    MOTOR_TEMP,
    MOTOR_FAULT,

    // Solar MPPTs
    SOLAR_VOLTAGE,
    SOLAR_FAULT,

    // GPS
    GPS_COURSE, // Degrees
    GPS_TIME, // Format hhmmsscc
    GPS_DATE, // Format ddmmyy
    GPS_LAT,
    GPS_LON,
    GPS_SPEED, // MPH

    _LAST
  };

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
  
  // writes to buf and returns length
  int toJsonString(char buf[]) volatile;
  int toKeyValuePair(char* dest, int key) volatile;
};


#endif
