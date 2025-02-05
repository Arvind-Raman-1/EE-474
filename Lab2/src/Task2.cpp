//FILENAME Task2.cpp
//Authors: Arvind Raman, Aadithya Manoj
// 2/04/2025
//this file compares the use of digital write to the use of direct register access for blinking an LED



//============================================ Includes ============================================
#include "Task2.h"
#include <Arduino.h>
#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_periph.h"

#define GPIO_PIN 5  // Pin D2

// For Task 2, you might need additional configuration.
void setupTask2() {
  Serial.begin(115200);
  // When using digitalWrite(), you need to configure the pin with pinMode().
  pinMode(GPIO_PIN, OUTPUT);
}

// Task 2a: Performance test using digitalWrite()
void task2_digitalWrite() {
  unsigned long startTime = micros();
  for (int i = 0; i < 1000; i++) {
    digitalWrite(GPIO_PIN, HIGH);
    digitalWrite(GPIO_PIN, LOW);
  }
  unsigned long elapsed = micros() - startTime;
  Serial.print("DigitalWrite elapsed time (us): ");
  Serial.println(elapsed);
  delay(1000);  // Delay between tests
}

// Task 2b: Performance test using direct register access.
void task2_directRegister() {
  volatile uint32_t* gpio_out = (volatile uint32_t*) GPIO_OUT_REG;
  unsigned long startTime = micros();
  for (int i = 0; i < 1000; i++) {
    *gpio_out |= (1 << GPIO_PIN);   // Set pin HIGH
    *gpio_out &= ~(1 << GPIO_PIN);    // Set pin LOW
  }
  unsigned long elapsed = micros() - startTime;
  Serial.print("Direct register access elapsed time (us): ");
  Serial.println(elapsed);
  delay(1000);  // Delay between tests
}
