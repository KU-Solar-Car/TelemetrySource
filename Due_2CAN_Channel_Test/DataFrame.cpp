#include "DataFrame.h"

DataFrame::DataFrame(double voltage, double current, double soc) {
  this->voltage = voltage;
  this->current = current;
  this->soc = soc;
}

String DataFrame::toJson() {
  String json = "{\"current\":" + String(current) + ",\"voltage\":" + String(voltage) + ",\"soc\":" + String(soc) + "}";
  return json;
}
