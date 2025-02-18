#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <esp_now.h>
#include <WiFi.h>
#include <Arduino.h>

LiquidCrystal_I2C lcd1(0x27, 16, 2);

volatile bool messageReceived = false;
volatile int counter = 0; 

void IRAM_ATTR dataReceived(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  counter++;
  messageReceived = true;
}

void setupTask3Reciever() {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  Wire.begin(8, 9);
  lcd1.init();
  lcd1.backlight();
  lcd1.clear();
  lcd1.setCursor(0, 0);
  lcd1.print("Counter: 0");

  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  esp_now_register_recv_cb(dataReceived);
}

void loopTask3Reciever() {
  lcd1.setCursor(0, 0);
  lcd1.print("Counter: ");
  lcd1.print(counter);
  lcd1.print("   ");
  
  if (messageReceived) {
    lcd1.setCursor(0, 1);
    lcd1.print("New Message!   ");
    messageReceived = false;
    delay(2000);
    lcd1.setCursor(0, 1);
    lcd1.print("                ");
  }

  delay(100);
}
