#ifndef STATS_H
#define STATS_H

struct StatData
{
  enum {BATT_VOLTAGE, BATT_CURRENT, BATT_TEMP, BMS_FAULT, GPS_TIME, GPS_LAT, GPS_LON, GPS_VEL_EAST, GPS_VEL_NOR, GPS_VEL_UP, GPS_SPD, SOLAR_VOLTAGE, SOLAR_CURRENT, MOTOR_SPD, _LAST} name;
  union {
    double doubleVal;
    unsigned uIntVal;
    bool boolVal;
  };
};

struct OptionalData
{
  bool present;
  StatData value;
};

typedef OptionalData Stats[StatData::_LAST];

#endif
