#ifndef STATS_H
#define STATS_H

enum StatKey: int
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
};

struct StatData
{
  union {
    double doubleVal;
    unsigned uIntVal;
    bool boolVal;
  };
  bool present;

};

typedef StatData Stats[StatKey::_LAST];

#endif
