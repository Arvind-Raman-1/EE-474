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


void setuptask1() {
  Serial.begin(115200);

  Wire.begin(8, 9);

  lcd.init();  
  delay(2);
}

void task1() {
    
  // If there's incoming data on Serial, read it
  if (Serial.available()) {
    // Read until newline
    String inputString = Serial.readStringUntil('\n');

    // Clear screen (command 0x01)
    sendCommand(0x01);
    delay(5);   // Small delay after clear command

    // Set cursor to the beginning of the first line (command 0x80)
    sendCommand(0x80);

    // Send each character of the string to LCD in data mode
    for (size_t i = 0; i < inputString.length(); i++) {
      sendData(inputString[i]);
    }
  }
}

// --------------
// Helper functions
// --------------

// Toggle the 'Enable' bit on and then off to latch data into the LCD
static void pulseEnable(uint8_t data) {
  Wire.beginTransmission(LCD_ADDR);
  // Enable bit is bit 2
  Wire.write(data | 0b00000100); // EN = 1
  Wire.endTransmission();

  delayMicroseconds(1); // Enable pulse > 450ns

  Wire.beginTransmission(LCD_ADDR);
  Wire.write(data & 0b11111011); // EN = 0
  Wire.endTransmission();

  delayMicroseconds(50); // Let the LCD catch up
}

// Send a 4-bit nibble along with control bits (RS, RW, EN, backlight)
static void sendNibble(uint8_t nibble, bool isData) {
  uint8_t data = 0;
  
  // RS bit is bit 0: 0 = command, 1 = data
  if (isData) {
    data |= 0x01;  // Set RS = 1 for data
  }

  // Backlight bit is bit 3
  data |= BACKLIGHT;

  // The 4 bits of 'nibble' go to bits 4..7
  data |= (nibble << 4);

  // Pulse the LCD enable line
  pulseEnable(data);
}

// Send a full byte (two 4-bit transfers) to the LCD
static void sendByte(uint8_t value, bool isData) {
  // High nibble first
  sendNibble(value >> 4, isData);
  // Low nibble second
  sendNibble(value & 0x0F, isData);
}

// Send a command (RS=0)
static void sendCommand(uint8_t cmd) {
  sendByte(cmd, false);
}

// Send data (RS=1)
static void sendData(uint8_t data) {
  sendByte(data, true);
}
