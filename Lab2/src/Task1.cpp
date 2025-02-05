//FILENAME Task2.cpp
//Authors: Arvind Raman, Aadithya Manoj
// 2/04/2025
//this file turns an LED on and off using direct register access



#include "Task1.h"
#include <Arduino.h>
#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_periph.h"

#define GPIO_PIN 5  // Pin D2

// This function configures GPIO_PIN as a GPIO output
void setupTask1() {
  // Set the pin's multiplexer to GPIO function.
  PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_PIN], PIN_FUNC_GPIO);

  // Configure the GPIO pin as an output using direct register access.
  volatile uint32_t* gpio_enable = (volatile uint32_t*) GPIO_ENABLE_REG;
  *gpio_enable |= (1 << GPIO_PIN);
}

// Task 1: Blink the LED using direct register access.
void task1() {
  volatile uint32_t* gpio_out = (volatile uint32_t*) GPIO_OUT_REG;
  
  // Turn LED ON (set bit)
  *gpio_out |= (1 << GPIO_PIN);
  delay(1000);  // 1 second delay
  
  // Turn LED OFF (clear bit)
  *gpio_out &= ~(1 << GPIO_PIN);
  delay(1000);  // 1 second delay
}
