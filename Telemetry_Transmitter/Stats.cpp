#include "Stats.h"

void TelemetryData::setDouble(int key, double value) volatile
{
  arr[key].present = true;
  arr[key].doubleVal = value;
}

void TelemetryData::setUInt(int key, unsigned long value) volatile
{
  arr[key].present = true;
  arr[key].uIntVal = value;
}

void TelemetryData::setBool(int key, bool value) volatile
{
  arr[key].present = true;
  arr[key].boolVal = value;
}

void TelemetryData::unset(int key) volatile
{
  arr[key].present = false;
}


double TelemetryData::getDouble(int key) volatile
{
  return arr[key].doubleVal;
}

unsigned long TelemetryData::getUInt(int key) volatile
{
  return arr[key].uIntVal;
}

bool TelemetryData::getBool(int key) volatile
{
  return arr[key].boolVal;
}


bool TelemetryData::isPresent(int key) volatile
{
  return arr[key].present;
}


void TelemetryData::clear() volatile
{
  for (int i = 0; i < TelemetryData::Key::_LAST; i++)
  {
    unset(i);
  }
}
