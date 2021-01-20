#ifndef STATS_H
#define STATS_H

template <typename T>
struct Optional
{
  bool present;
  T value;
};

struct Stats
{
  unsigned secondsSinceMidnight;
  Optional<double> batteryVoltage;
  Optional<double> batteryCurrent;
  Optional<double> batteryTemperature;
  Optional<bool> bmsFault;
  Optional<unsigned> gpsTime;
  Optional<double> gpsLat;
  Optional<double> gpsLon;
  Optional<double> gpsVelocityEast;
  Optional<double> gpsVelocityNorth;
  Optional<double> gpsVelocityUp;
  Optional<double> gpsSpeed;
  Optional<double> solarVoltage;
  Optional<double> solarCurrent;
  Optional<double> motorSpeed;
};

#endif
