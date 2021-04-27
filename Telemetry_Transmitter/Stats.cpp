#include "Stats.h"

void TelemetryData::setDouble(int key, double value)
{
  arr[key].present = true;
  arr[key].doubleVal = value;
}

void TelemetryData::setUInt(int key, unsigned value)
{
  arr[key].present = true;
  arr[key].uIntVal = value;
}

void TelemetryData::setBool(int key, bool value)
{
  arr[key].present = true;
  arr[key].boolVal = value;
}

void TelemetryData::unset(int key)
{
  arr[key].present = false;
}


double TelemetryData::getDouble(int key)
{
  return arr[key].doubleVal;
}

unsigned TelemetryData::getUInt(int key)
{
  return arr[key].uIntVal;
}

bool TelemetryData::getBool(int key)
{
  return arr[key].boolVal;
}


bool TelemetryData::isPresent(int key)
{
  return arr[key].present;
}


void TelemetryData::clear()
{
  for (int i = 0; i < TelemetryData::Key::_LAST; i++)
  {
    unset(i);
  }
}
