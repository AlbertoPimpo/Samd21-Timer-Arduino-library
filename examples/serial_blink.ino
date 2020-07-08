#include "Samd21Timer.h"


volatile int state = HIGH;
volatile int blinkCounter = 0;


void blink(){
  digitalWrite(LED_BUILTIN, state);
  state = state == HIGH ? LOW : HIGH;
  blinkCounter ++;
}

void writeOnSerial(){
  SerialUSB.println("test");
}



void setup() {
  SerialUSB.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  Timer.enable(TIMER_4, 0.5, blink);
  Timer.enable(TIMER_3, 1, writeOnSerial);
}

void loop() {
  if (blinkCounter >= 4){
    Timer.disable(TIMER_4);
  }
  
}
