//FILENAME PART2
//Authors: Arvind Raman, Aadithya Manoj
// 2/04/2025
//this file controls the blinking of an LED using the GPIO and Timing registers

//============================================includes============================================
#include <Arduino.h>
#include "driver/gpio.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_periph.h"
#include <soc/timer_group_reg.h>



//============================================macros============================================
#define GPIO_PIN 5 // Pin D2
#define TIMER_DIVIDER_VALUE 80 //make it 1Mhz
#define TIMER_INCREMENT_MODE (1<<30) //set the timer to increment mode
#define TIMER_ENABLE (1<<31) //Enable timer bit
#define LED_TOGGLE_INTERVAL 1000000 // LED toggle interval: 1 second for a 1Mhz clock
static uint32_t last_toggle_time = 0;


void setup(){
    PIN_FUNC_SELECT(GPIO_PIN_MUX_REG[GPIO_PIN], PIN_FUNC_GPIO);
    *((volatile uint32_t *)GPIO_ENABLE_REG) |= (1<<GPIO_PIN);

    uint32_t timer_config = (TIMER_DIVIDER_VALUE<<13);

    timer_config |= TIMER_INCREMENT_MODE;
    timer_config |= TIMER_ENABLE;

    *((volatile uint32_t *)(TIMG_T0CONFIG_REG(0))) = timer_config;
    *((volatile uint32_t *)(TIMG_T0UPDATE_REG(0))) = 1;
}

void loop(){
    uint32_t current_time = *((volatile uint32_t *)(TIMG_T0LO_REG(0)));

    if((current_time-last_toggle_time)>=LED_TOGGLE_INTERVAL){
        uint32_t gpio_out = *((volatile uint32_t *)GPIO_OUT_REG);
        *((volatile uint32_t *)GPIO_OUT_REG) = gpio_out ^ (1<<GPIO_PIN);
        last_toggle_time = current_time;

    }

    *((volatile uint32_t *)(TIMG_T0UPDATE_REG(0))) = 1;
}
