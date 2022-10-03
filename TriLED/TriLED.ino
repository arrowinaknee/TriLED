#include "lamp.h"
#include "wifi.h"

void setup() {
  setup_wifi();
  setup_lamp();
}

void loop() {
  loop_wifi();
  loop_lamp();
}