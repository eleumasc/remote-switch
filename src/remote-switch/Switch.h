#ifndef SWITCH_H_
#define SWITCH_H_

#include <Arduino.h>

class Switch {

  static int pin;

  static bool status;

  Switch() {}

public:

  static void init(int pin);

  static bool getStatus();

  static void toggle(bool on);

};

#endif
