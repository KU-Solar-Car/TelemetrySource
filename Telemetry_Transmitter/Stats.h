#ifndef STATS_H
#define STATS_H

struct TelemetryData
{
  enum Key: int
  {
    BATT_VOLTAGE,
    BATT_CURRENT,
    BATT_TEMP,
    BMS_FAULT,
    GPS_TIME,
    GPS_LAT,
    GPS_LON,
    GPS_VEL_EAST,
    GPS_VEL_NOR,
    GPS_VEL_UP,
    GPS_SPD,
    SOLAR_VOLTAGE,
    SOLAR_CURRENT,
    MOTOR_SPD,
    _LAST
    // TODO: Add motor temperature
  } key;
  
  union Value {
    double doubleVal;
    unsigned uIntVal;
    bool boolVal;
  } value;
  bool present;

  void setDouble(double value);
  void setUInt(unsigned value);
  void setBool(bool value);

  inline double getDouble()
  {
    return value.doubleVal;
  }
  inline unsigned getUInt()
  {
    return value.uIntVal;
  }
  inline bool getBool()
  {
    return value.boolVal;
  }
  
};


#endif
