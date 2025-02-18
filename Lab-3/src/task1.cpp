#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "task1.h"


#define LCD_ADDR 0x27


LiquidCrystal_I2C lcd(LCD_ADDR, 16, 2);

#define BACKLIGHT 0x08

static void pulseEnable(uint8_t data);
static void sendNibble(uint8_t nibble, bool isData);
static void sendByte(uint8_t value, bool isData);
static void sendCommand(uint8_t cmd);
static void sendData(uint8_t data);


// store incoming characters
static String inputBuffer = "";

void setuptask1() {
  Serial.begin(115200);

  //   Wire.begin(8, 9);
  Wire.begin();   

  lcd.init();  
  delay(2);
}

void task1() {
  // Read all available characters from Serial
  while (Serial.available() > 0) {
    char c = Serial.read();

    // Enter logic
    if (c == '\n') {
      // Clear the LCD
      sendCommand(0x01);
      delay(5);   

      sendCommand(0x80);

      // Print the buffered string on the LCD
      for (size_t i = 0; i < inputBuffer.length(); i++) {
        sendData(inputBuffer[i]);
      }

      inputBuffer = "";
    }
    else if (c != '\r') {

      inputBuffer += c;
    }
  }
}


// Toggle the 'Enable' bit 
static void pulseEnable(uint8_t data) {
  Wire.beginTransmission(LCD_ADDR);
  // Enable bit is bit 2
  Wire.write(data | 0b00000100); // EN = 1
  Wire.endTransmission();

  delayMicroseconds(1); // Enable pulse > 450ns

  Wire.beginTransmission(LCD_ADDR);
  Wire.write(data & 0b11111011); // EN = 0
  Wire.endTransmission();

  delayMicroseconds(50); 
}

// Send a 4-bit nibble 
static void sendNibble(uint8_t nibble, bool isData) {
  uint8_t data = 0;
  
  // RS bit 
  if (isData) {
    data |= 0x01;  // Set RS = 1 for data
  }

  // Backlight bit 
  data |= BACKLIGHT;

  
  data |= (nibble << 4);

  // Pulse the LCD enable line
  pulseEnable(data);
}

// Send a full byte to the LCD
static void sendByte(uint8_t value, bool isData) {

  sendNibble(value >> 4, isData);

  sendNibble(value & 0x0F, isData);
}

static void sendCommand(uint8_t cmd) {
  sendByte(cmd, false);
}

static void sendData(uint8_t data) {
  sendByte(data, true);
}
