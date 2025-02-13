#include <Wire.h>
#include <Arduino.h>
#include <LiquidCrystal_I2C.h>

#include "task1.h" 

#define LCD_BACKLIGHT 0x08

LiquidCrystal_I2C lcd(0x27, 16, 2); 


static const uint8_t LCD_ADDR       = 0x27;  
static const uint8_t LCD_BACKLIGHT  = 0x08;  
static const uint8_t ENABLE_BIT     = 0x04;  
static const uint8_t RW_BIT         = 0x00;  
static const uint8_t RS_BIT         = 0x01;  

static void i2cWrite(uint8_t data)
{
  Wire.beginTransmission(LCD_ADDR);
  Wire.write(data);
  Wire.endTransmission();
}



static void pulseEnable(uint8_t data)
{

  i2cWrite(data | ENABLE_BIT);
  delayMicroseconds(1);


  i2cWrite(data & ~ENABLE_BIT);
  delayMicroseconds(50);
}

 
static void write4bits(uint8_t nibble)
{
  i2cWrite(nibble);
  pulseEnable(nibble);
}



static void sendByte(uint8_t value, bool isData)
{
  uint8_t mode = LCD_BACKLIGHT | RW_BIT;
  if (isData) {
    mode |= RS_BIT; 
  }

  write4bits((value & 0xF0) | mode);
  write4bits(((value << 4) & 0xF0) | mode);
}

static void sendCommand(uint8_t cmd) { sendByte(cmd, false); }
static void sendData(uint8_t data)   { sendByte(data, true);  }


static void lcdCommandInitSequence()
{
  delay(50);

 
  sendCommand(0x03);
  delayMicroseconds(4500);
  sendCommand(0x03);
  delayMicroseconds(4500);
  sendCommand(0x03);
  delayMicroseconds(150);

  // Now set 4-bit mode
  sendCommand(0x02);

  // 4-bit, 2 lines, 5x8 font
  sendCommand(0x28);

  // Display on, cursor off, blink off
  sendCommand(0x0C);

  // Clear display
  sendCommand(0x01);
  delayMicroseconds(2000);
}

void setuptask1() {
  Serial.begin(115200);
  Wire.begin();   
  lcd.init();     
  delay(2);
}

void task1() 
{

  if (Serial.available() > 0) 
  {

    String line = Serial.readStringUntil('\n');


    sendCommand(0x01);
    delayMicroseconds(2000);

    for (size_t i = 0; i < line.length(); i++) {
      sendData(line[i]);
    }
  }
}
