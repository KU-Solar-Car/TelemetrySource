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
    case Key::PACK_VOLTAGE:          return sprintf(dest, "\"pack_voltage\":%6f",          getDouble(key));
    case Key::PACK_SOC:              return sprintf(dest, "\"pack_soc\":%6f",              getDouble(key));
    case Key::PACK_CURRENT:          return sprintf(dest, "\"pack_current\":%6f",          getDouble(key));
    case Key::MIN_PACK_TEMP:         return sprintf(dest, "\"min_pack_temp\":%6f",         getDouble(key));
    case Key::AVG_PACK_TEMP:         return sprintf(dest, "\"avg_pack_temp\":%6f",         getDouble(key));
    case Key::MAX_PACK_TEMP:         return sprintf(dest, "\"max_pack_temp\":%6f",         getDouble(key));
    case Key::MIN_CELL_VOLTAGE:      return sprintf(dest, "\"min_cell_voltage\":%6f",      getDouble(key));
    case Key::AVG_CELL_VOLTAGE:      return sprintf(dest, "\"avg_cell_voltage\":%6f",      getDouble(key));
    case Key::MAX_CELL_VOLTAGE:      return sprintf(dest, "\"max_cell_voltage\":%6f",      getDouble(key));
    case Key::INPUT_VOLTAGE:         return sprintf(dest, "\"input_voltage\":%6f",         getDouble(key));
    case Key::AVG_CELL_RESISTANCE:   return sprintf(dest, "\"avg_cell_resistance\":%6f",   getDouble(key));
    case Key::BMS_FAULT:             return sprintf(dest, "\"bms_fault\":%d",              getUInt(key)  );

    case Key::MOTOR_SPEED:           return sprintf(dest, "\"motor_speed\":%6f",           getDouble(key));
    case Key::MOTOR_CURRENT:         return sprintf(dest, "\"motor_current\":%6f",         getDouble(key));
    case Key::MOTOR_CONTROLLER_TEMP: return sprintf(dest, "\"motor_controller_temp\":%6f", getDouble(key));
    case Key::MOTOR_TEMP:            return sprintf(dest, "\"motor_temp\":%6f",            getDouble(key));
    case Key::MOTOR_FAULT:           return sprintf(dest, "\"motor_fault\":%d",            getUInt(key)  );

    case Key::SOLAR_VOLTAGE:         return sprintf(dest, "\"solar_voltage\":%6f",         getDouble(key));
    case Key::SOLAR_FAULT:           return sprintf(dest, "\"solar_fault\":%d",            getUInt(key)  );

    case Key::GPS_COURSE:            return sprintf(dest, "\"gps_course\":%6f",            getDouble(key));
    case Key::GPS_TIME:              return sprintf(dest, "\"gps_time\":\"%08u\"",         getUInt(key)  );
    case Key::GPS_DATE:              return sprintf(dest, "\"gps_date\":\"%06u\"",         getUInt(key)  );
    case Key::GPS_LAT:               return sprintf(dest, "\"gps_lat\":%6f",               getDouble(key));
    case Key::GPS_LON:               return sprintf(dest, "\"gps_lon\":%6f",               getDouble(key));
    case Key::GPS_SPEED:             return sprintf(dest, "\"gps_speed\":%6f",             getDouble(key));
  }
}