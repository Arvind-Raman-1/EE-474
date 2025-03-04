#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

// Pin definitions
#define LED_PIN     2     // LED connected to digital pin 2
#define PHOTO_PIN   1     // Photoresistor connected to analog pin 1
#define LCD_SDA     8     // LCD SDA on pin 8
#define LCD_SCL     9     // LCD SCL on pin 9


LiquidCrystal_I2C lcd(0x27, 16, 2);

// Binary semaphore for synchronizing light level data
SemaphoreHandle_t lightSemaphore;

// Global variables for light sensor data
const int smaWindowSize = 5;
int lightReadings[smaWindowSize] = {0};
int readingIndex = 0;
int currentLightLevel = 0;
int currentSMA = 0;
int previousSMA = -1;

// Calculate the simple moving average (SMA) from the last 5 readings
void calculateSMA() {
    long sum = 0;
    for (int i = 0; i < smaWindowSize; i++) {
        sum += lightReadings[i];
    }
    currentSMA = sum / smaWindowSize;
}

// ---------------------------
// Light Detector Task (Core 0)
// ---------------------------
// Reads the photoresistor, updates the readings array,
// calculates the SMA, and then briefly takes the semaphore to
// signal data is ready.
void TaskLightDetector(void *pvParameters) {
    for (;;) {
        // Read light level from the photoresistor
        currentLightLevel = analogRead(PHOTO_PIN);
        
        // Update the SMA window array
        lightReadings[readingIndex] = currentLightLevel;
        readingIndex = (readingIndex + 1) % smaWindowSize;
        
        // Calculate simple moving average (SMA)
        calculateSMA();
        
        // Synchronize access (using the semaphore as a mutex)
        if (xSemaphoreTake(lightSemaphore, (TickType_t)10) == pdTRUE) {
            // Data updated—release immediately
            xSemaphoreGive(lightSemaphore);
        }
        
        vTaskDelay(pdMS_TO_TICKS(100)); // Wait 100 ms before next reading
    }
}

// ---------------------------
// LCD Task (Core 0)
// ---------------------------
// Waits for the semaphore; if the SMA has changed,
// updates the LCD with the new light level and SMA.
void TaskLCD(void *pvParameters) {
    for (;;) {
        if (xSemaphoreTake(lightSemaphore, portMAX_DELAY) == pdTRUE) {
            // Update the LCD only if the SMA has changed
            if (currentSMA != previousSMA) {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("Light: ");
                lcd.print(currentLightLevel);
                lcd.setCursor(0, 1);
                lcd.print("SMA: ");
                lcd.print(currentSMA);
                previousSMA = currentSMA;
            }
            xSemaphoreGive(lightSemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// Helper function to flash the LED three times
void flashLED() {
    for (int i = 0; i < 3; i++) {
        digitalWrite(LED_PIN, HIGH);
        vTaskDelay(pdMS_TO_TICKS(200));
        digitalWrite(LED_PIN, LOW);
        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

// ---------------------------
// Anomaly Alarm Task (Core 1)
// ---------------------------
// Waits for the semaphore and checks whether the SMA is outside
// the acceptable range. If an anomaly is detected, flashes the LED.
void TaskAnomalyAlarm(void *pvParameters) {
    for (;;) {
        if (xSemaphoreTake(lightSemaphore, portMAX_DELAY) == pdTRUE) {
            // Check if the SMA indicates an anomaly
            if (currentSMA > 3800 || currentSMA < 300) {
                flashLED();
                vTaskDelay(pdMS_TO_TICKS(2000)); // 2-second pause between playbacks
            }
            xSemaphoreGive(lightSemaphore);
        }
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}

// ---------------------------
// Prime Calculation Task (Core 1)
// ---------------------------
// Loops from 2 to 5000, checking for primes and printing them.
bool isPrime(int n) {
    if (n <= 1) return false;
    for (int i = 2; i <= sqrt(n); i++) {
        if (n % i == 0)
            return false;
    }
    return true;
}

void TaskPrimeCalculation(void *pvParameters) {
    for (int i = 2; i <= 5000; i++) {
        if (isPrime(i)) {
            Serial.print("Prime: ");
            Serial.println(i);
        }
        vTaskDelay(pdMS_TO_TICKS(10)); // Yield delay to avoid blocking
    }
    // Loop indefinitely after finishing prime calculation
    for (;;) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

// ---------------------------
// setup()
// ---------------------------
// Initializes pins, serial, LCD, and the semaphore.
// Creates the four tasks and assigns them to the appropriate cores.
void setup() {
    // Initialize serial communication
    Serial.begin(115200);
    
    // Initialize pins
    pinMode(LED_PIN, OUTPUT);
    // Photoresistor on an analog pin needs no pinMode setup.
    
    // Initialize I2C for LCD using the specified SDA and SCL pins
    Wire.begin(LCD_SDA, LCD_SCL);
    lcd.init();
    lcd.backlight();
    
    // Create binary semaphore for synchronizing light level data
    lightSemaphore = xSemaphoreCreateBinary();
    // Initially give the semaphore so it can be taken by tasks
    xSemaphoreGive(lightSemaphore);
    
    // Create tasks and assign them to cores
    // Light Detector Task on Core 0
    xTaskCreatePinnedToCore(TaskLightDetector, "Light Detector Task", 2048, NULL, 1, NULL, 0);
    // LCD Task on Core 0
    xTaskCreatePinnedToCore(TaskLCD, "LCD Task", 2048, NULL, 1, NULL, 0);
    // Anomaly Alarm Task on Core 1
    xTaskCreatePinnedToCore(TaskAnomalyAlarm, "Anomaly Alarm Task", 2048, NULL, 1, NULL, 1);
    // Prime Calculation Task on Core 1
    xTaskCreatePinnedToCore(TaskPrimeCalculation, "Prime Calculation Task", 2048, NULL, 1, NULL, 1);
}

// ---------------------------
// loop()
// ---------------------------
// The main loop is empty since tasks run concurrently.
void loop() {
    vTaskDelay(pdMS_TO_TICKS(1000));
}
