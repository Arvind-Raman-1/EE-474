#include "Task3.h"
#include <Arduino.h>

// setupTask3() configures the PWM for the LED using the LEDC library.
void setupTask3() {
  // Configure the LEDC PWM channel:
  //   Channel: 0
  //   Frequency: 5000 Hz
  //   Resolution: 8 bits (duty cycle ranges from 0 to 255)
  ledcSetup(0, 5000, 8);

  // Attach LED_PIN to the PWM channel 0.
  ledcAttachPin(LED_PIN, 0);

  // No special setup is needed for analogRead() on the ESP32.
}

// task3() reads the ambient light using analogRead() from the photoresistor circuit
// and adjusts the LED brightness via the LEDC PWM.
void task3() {
   Serial.begin(115200);
  // Read the analog value from the photoresistor.
  int sensorValue = analogRead(PHOTO_PIN);
  
  // Map the sensor reading (expected range: 0-4095) to the PWM duty cycle range (0-255).
  int dutyCycle = map(sensorValue, 0, 4095, 0, 255);
  
  // Set the LED brightness using the LEDC PWM function.
  ledcWrite(0, dutyCycle);
  
  // Optionally, print the values to the serial monitor for debugging.
  Serial.print("Sensor Value: ");
  Serial.print(sensorValue);
  Serial.print("  Duty Cycle: ");
  Serial.println(dutyCycle);

  // A short delay can help stabilize sensor readings.
  delay(50);
}
