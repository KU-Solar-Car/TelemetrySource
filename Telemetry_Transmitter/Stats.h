#ifndef STATS_H
#define STATS_H

#include "Variant.h"
#include "Optional.h"

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
  };

  Optional<Variant<bool, unsigned, double>> arr[Key::_LAST];

  template <typename T>
  void set(int key, T value)
  {
    arr[key].present = true;
    arr[key].value = (T) value;
  }
  
  void unset(int key)
  {
    arr[key].present = false;
  }
  
  template <typename T>
  T get(int key)
  {
    return arr[key].value.as<T>();
  }
  
  bool isPresent(int key)
  {
    return arr[key].present;
  }
  
  void clear()
  {
    for (int i = 0; i < TelemetryData::Key::_LAST; i++)
    {
      unset(i);
    }
  }
};
  
#endif
