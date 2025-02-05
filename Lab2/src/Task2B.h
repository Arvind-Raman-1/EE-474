
//============================================macros============================================
#define GPIO_PIN 5 // Pin D2
#define TIMER_DIVIDER_VALUE 80 //make it 1Mhz
#define TIMER_INCREMENT_MODE (1<<30) //set the timer to increment mode
#define TIMER_ENABLE (1<<31) //Enable timer bit
#define LED_TOGGLE_INTERVAL 1000000 // LED toggle interval: 1 second for a 1Mhz clock
static uint32_t last_toggle_time = 0;

//============================================Functoin Definitions============================================

void Task2B_setup();
void Task2B_loop();