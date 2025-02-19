#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>

// Initialize LCD at I2C address 0x27 with 16 columns and 2 rows
LiquidCrystal_I2C lcd1(0x27, 16, 2);

volatile bool messageReceived = false; // Flag set by ISR when a new message is received
volatile int counter = 0;              // Incremented by timer ISR every second

hw_timer_t *timer = NULL;

// ESP-NOW receive callback: set flag when a message is received
void IRAM_ATTR dataReceived(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  messageReceived = true;
}

// Timer ISR: increments counter every second
void IRAM_ATTR onTimer() {
  counter++;
}

void setupTask3Reciever() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Initialize I2C on default pins (SDA, SCL)
  Wire.begin(8, 9);

  // Set up the LCD display
  lcd1.init();
  lcd1.backlight();
  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("Counter: 0");

  // Configure hardware timer: Timer 0 with prescaler 80 (1 µs per tick)
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true); // Set timer to trigger every 1 second
  timerAlarmEnable(timer);

  // Initialize ESP-NOW and register callback
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(dataReceived);
}

void loopTask3Reciever() {
  // If a new message is received, display it on the LCD for 2 seconds
  if (messageReceived) {
    lcd1.clear();
    lcd1.setCursor(0, 0);
    lcd1.print("New Message!");
    messageReceived = false;
    delay(2000);
  } else {
    // Otherwise, update the LCD with the current counter value
    lcd1.clear();
    lcd1.setCursor(0, 0);
    lcd1.print("Counter: ");
    lcd1.print(counter);
    delay(100); // Brief delay to stabilize the display refresh
  }
}
