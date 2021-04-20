#include "Stats.h"

void TelemetryData::setDouble(double value)
{
  present = true;
  this->value.doubleVal = value;
}

void TelemetryData::setUInt(unsigned value)
{
  present = true;
  this->value.uIntVal = value;
}

void TelemetryData::setBool(bool value)
{
  present = true;
  this->value.boolVal = value;
}
