#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>

// Initialize the LCD 
LiquidCrystal_I2C lcd1(0x27, 16, 2);

volatile bool messageReceived = false;
volatile int counter = 0;  \

hw_timer_t *timer = NULL;

// flips the message recieved boolean 
void IRAM_ATTR dataReceived(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  messageReceived = true;
}

// This timer ISR increments the counter every second.
void IRAM_ATTR onTimer() {
  counter++;
}

void setupTask3Reciever() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);

  // Initialize I2C
  Wire.begin(8, 9);

  // Initialize the LCD
  lcd1.init();
  lcd1.backlight();
  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("Counter: 0");

  // Timer set up
  timer = timerBegin(0, 80, true); 
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true); 
  timerAlarmEnable(timer);

  // Initialize ESP-NOW.
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  esp_now_register_recv_cb(dataReceived);
}

void loopTask3Reciever() {
  // Check if a new message was received.
  if (messageReceived) {
    lcd1.clear();
    lcd1.setCursor(0, 0);
    lcd1.print("New Message!");
    messageReceived = false;
    delay(2000);  // Hold the "New Message!" display for 2 seconds.
  } else {
    // Display the current counter value.
    lcd1.clear();
    lcd1.setCursor(0, 0);
    lcd1.print("Counter: ");
    lcd1.print(counter);
    delay(100);  //delay to stabilize the display refresh.
  }
}
