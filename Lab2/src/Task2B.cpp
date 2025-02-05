// FILENAME: Task2b
// Authors: Arvind Raman, Aadithya Manoj
// Date: 2/04/2025
// Description: This file controls the blinking of an LED using the GPIO and Timing registers on an ESP32.
//              It configures a timer and toggles the LED state based on a predefined interval.
// Version: 1.0

//============================================ Includes ============================================
#include "Task2B.h"  
#include <Arduino.h> 
#include "driver/gpio.h" 
#include "soc/io_mux_reg.h"
#include "soc/gpio_reg.h"  
#include "soc/gpio_periph.h" 
#include <soc/timer_group_reg.h> 

//============================================ Global Variables ============================================
static uint32_t last_toggle_time = 0; 

/* 
 * This function sets up the GPIO as an output and configures a timer.
 * It enables the timer in increment mode and updates it to start counting.
 */
void Task2B_setup() {
    // Configure the specified GPIO pin for general-purpose use
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_PIN], PIN_FUNC_GPIO);
    
    // Enable the GPIO pin as an output
    *((volatile uint32_t *)GPIO_ENABLE_REG) |= (1 << GPIO_PIN);

    // Configure the timer with a prescaler
    uint32_t timer_config = (TIMER_DIVIDER_VALUE << 13); // Set timer divider value

    // Enable the timer in increment mode
    timer_config |= TIMER_INCREMENT_MODE;
    timer_config |= TIMER_ENABLE;

    // Write configuration to the timer register
    *((volatile uint32_t *)(TIMG_T0CONFIG_REG(0))) = timer_config;
    
    // Start the timer by updating it
    *((volatile uint32_t *)(TIMG_T0UPDATE_REG(0))) = 1;
}

/* 
 * This function continuously checks the timer and toggles the LED state
 * when the predefined interval has passed.
 */
void Task2B_loop() {
    // Read the current timer value
    uint32_t current_time = *((volatile uint32_t *)(TIMG_T0LO_REG(0)));

    // Check if the required time interval has passed since last toggle
    if ((current_time - last_toggle_time) >= LED_TOGGLE_INTERVAL) {
        // Read the current GPIO output state
        uint32_t gpio_out = *((volatile uint32_t *)GPIO_OUT_REG);
        
        // Toggle the LED state by XORing the current state with the pin mask
        *((volatile uint32_t *)GPIO_OUT_REG) = gpio_out ^ (1 << GPIO_PIN);
        
        // Update the last toggle time
        last_toggle_time = current_time;
    }
    
    // Update the timer to ensure the loop gets fresh values on the next iteration
    *((volatile uint32_t *)(TIMG_T0UPDATE_REG(0))) = 1;
}
