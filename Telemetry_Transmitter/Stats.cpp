#include "Stats.h"
#include <stdio.h> // sprintf
#include <string.h> // strcpy strlen etc.

void TelemetryData::setDouble(int key, double value) volatile
{
  arr[key].present = true;
  arr[key].doubleVal = value;
}

void TelemetryData::setUInt(int key, unsigned value) volatile
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

unsigned TelemetryData::getUInt(int key) volatile
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

int TelemetryData::toJsonString(char buf[]) volatile
{
  strcpy(buf, "{");
  
  int bodyLength = 1; // the open bracket
  for (int k = 0; k < TelemetryData::Key::_LAST; k++)
  {
    if (isPresent(k))
    {
      bodyLength += toKeyValuePair(buf + strlen(buf), k) + 1; // append the key-value pair, plus the trailing comma
      strcat(buf, ",");
    }
  }
  // Here we are checking if we have data. If so, we need to replace the last trailing comma with a } to close the json body.
  // If not, we need to append a }, and also add 1 to the content length.
  if (buf[strlen(buf)-1] == ',')
  {
    buf[strlen(buf)-1] = '}';
  }
  else if (buf[strlen(buf)-1] == '{')
  {
    strcat(buf, "}");
    bodyLength++;
  }
  return bodyLength;
}

int TelemetryData::toKeyValuePair(char* dest, int key) volatile
{
  switch(key)
  {
    case Key::BATT_VOLTAGE: return sprintf(dest, "\"battery_voltage\":%6f", getDouble(key)); break;
    case Key::BATT_CURRENT: return sprintf(dest, "\"battery_current\":%6f", getDouble(key)); break;
    case Key::BATT_TEMP: return sprintf(dest, "\"battery_temperature\":%6f", getDouble(key)); break;
    case Key::BMS_FAULT: return sprintf(dest, "\"bms_fault\":%d", getBool(key)); break;
    case Key::GPS_COURSE: return sprintf(dest, "\"gps_course\":%6f", getDouble(key)); break;
    case Key::GPS_TIME: return sprintf(dest, "\"gps_time\":\"%08u\"", getUInt(key)); break;
    case Key::GPS_DATE: return sprintf(dest, "\"gps_date\":\"%06u\"", getUInt(key)); break;
    case Key::GPS_LAT: return sprintf(dest, "\"gps_lat\":%6f", getDouble(key)); break;
    case Key::GPS_LON: return sprintf(dest, "\"gps_lon\":%6f", getDouble(key)); break;
    case Key::GPS_SPD: return sprintf(dest, "\"gps_speed\":%6f", getDouble(key)); break;
    case Key::SOLAR_VOLTAGE: return sprintf(dest, "\"solar_voltage\":%6f", getDouble(key)); break;
    case Key::SOLAR_CURRENT: return sprintf(dest, "\"solar_current\":%6f", getDouble(key)); break;
    case Key::MOTOR_SPD: return sprintf(dest, "\"motor_speed\":%6f", getDouble(key)); break;
  }
}