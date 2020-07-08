#include <Samd21Timer.h>



void callback(){
  SerialUSB.println("Test");
}

void setup() {
}

void loop() {
  Timer.enable(TIMER_5, 1.0, callback);
  Timer.disable(TIMER_5);
}