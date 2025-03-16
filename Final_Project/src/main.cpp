#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <IRremoteESP8266.h>
#include <IRrecv.h>
#include <IRutils.h>

#define I2C_SDA 10
#define I2C_SCL 11
#define LED_PIN 4
#define IR_PIN 9

LiquidCrystal_I2C lcd(0x27, 16, 2);
IRrecv irrecv(IR_PIN);
decode_results results;

int pressCounter = 0;

void setup() {
  Wire.begin(I2C_SDA, I2C_SCL);
  lcd.init();
  lcd.backlight();

  pinMode(LED_PIN, OUTPUT);

  irrecv.enableIRIn();

  lcd.setCursor(0, 0);
  lcd.print("Press count:");
  lcd.setCursor(0, 1);
  lcd.print("Count: 0");
}

void loop() {
  // Blink LED
  digitalWrite(LED_PIN, HIGH);
  delay(200);
  digitalWrite(LED_PIN, LOW);
  delay(200);

  // Check for IR remote input
  if (irrecv.decode(&results)) {
    pressCounter++; // Increment counter

    // Update LCD
    lcd.setCursor(0, 1);
    lcd.print("Count: ");
    lcd.print(pressCounter);
    lcd.print("   "); // Clear extra chars

    irrecv.resume();
  }
}