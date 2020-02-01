#ifndef DATA_FRAME_H
#define DATA_FRAME_H

#include <Arduino.h>

class DataFrame {
  public:
    DataFrame(double voltage, double current, double soc);
    double voltage;
    double current;
    double soc; // state of charge
    String toJson();
};

#endif
