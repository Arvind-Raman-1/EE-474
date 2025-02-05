//FILENAME main.cpp
//Authors: Arvind Raman, Aadithya Manoj
// 2/04/2025
//this file controls which task is being run on the ESP32



#include <Arduino.h>
#include "Task1.h"
#include "Task2B.h"
#include "Task2.h"
#include "Task3.h"


void setup() {
  
  setupTask1();
  //Task2B_setup();
  //setupTask2();
  //setupTask3();
}

void loop() {
  // Call the task functions you want to run.
  task1();

  //Task2B_loop();

  //task2_digitalWrite();
  // task2_directRegister();

  //task3(); 
}
