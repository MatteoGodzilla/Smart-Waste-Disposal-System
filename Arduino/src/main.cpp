#include <Arduino.h>
#include <Wire.h>

#include "pins.h"
#include "Scheduler.h"

Scheduler scheduler;

void setup() {
  // put your setup code here, to run once:
  scheduler.init();
}

void loop() {
  // put your main code here, to run repeatedly:
  scheduler.schedule();
}
