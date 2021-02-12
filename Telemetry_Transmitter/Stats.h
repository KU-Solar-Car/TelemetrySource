#ifndef STATS_H
#define STATS_H

union StatData
{
  double doubleVal;
  unsigned uIntVal;
  bool boolVal;
};

enum StatTypes
{
  BATT_VOLTAGE, BATT_CURRENT, BATT_TEMP, BMS_FAULT, GPS_TIME, GPS_LAT, GPS_LON, GPS_VEL_EAST, GPS_VEL_NOR, GPS_VEL_UP, GPS_SPD, SOLAR_VOLTAGE, SOLAR_CURRENT, MOTOR_SPD
};

struct Optional
{
  bool present;
  StatData value;
};

class Stats
{
  //Optional batteryVoltage, batteryCurrent, batteryTemperature, bmsFault, gpsTime, gpsLat, gpsLon, gpsVelocityEast, gpsVelocityNorth, gpsVelocityUp, gpsSpeed, solarVoltage, solarCurrent, motorSpeed;
  Optional m_carData[14];

  // Method for setting for each data type in the union
  // Method for getting string to send through XBee  
};

#endif
