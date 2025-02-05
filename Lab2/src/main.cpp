#include <Arduino.h>
#include "Task1.h"
#include "Task2.h"
#include "Task3.h"


void setup() {
  
  setupTask1();
  //setupTask2();
  //setupTask3();
}

void loop() {
  // Call the task functions you want to run.
  task1();

  //task2_digitalWrite();
  // task2_directRegister();

  //task3(); 
}
