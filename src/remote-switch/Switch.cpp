#include "Switch.h"

void Switch::init(int pin) {
  Switch::pin = pin;
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

bool Switch::getStatus() {
  return status;
}

void Switch::toggle(bool on) {
  digitalWrite(pin, on ? LOW : HIGH);
  status = on;
}

int Switch::pin = 0;

bool Switch::status = 0;
