#include <Arduino.h>
#include "task1.h" 
#include "task3reciever.h"
#include <LiquidCrystal_I2C.h>


void setup() {
  //setuptask1();
  setupTask3Reciever();
  
}


void loop() {
  //task1();
  loopTask3Reciever();
}
