#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Constants & Pin Definitions
#define LED_PIN 2
#define BUTTON_PIN 4

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Peer address (Replace with actual sender ESP32 MAC address)
uint8_t senderAddress[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};

// Global state
volatile bool alarmTriggered = false;

// ESP-NOW callback
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  if(len < 1) return;

  if(incomingData[0] == 1) {
    xTaskCreate(successTask, "successTask", 2048, NULL, 1, NULL);
  } else if(incomingData[0] == 0) {
    alarmTriggered = true;
    xTaskCreate(alarmTask, "alarmTask", 2048, NULL, 1, NULL);
  }
}

// Send Deactivation Signal
void sendDeactivation(){
  uint8_t data = 1; // 1 indicates deactivation
  esp_now_send(senderAddress, &data, 1);
}

// Success Task
void successTask(void *pvParam){
  lcd.clear();
  lcd.print("Success!");
  vTaskDelay(pdMS_TO_TICKS(5000));
  lcd.clear();
  vTaskDelete(NULL);
}

// Alarm Task
void alarmTask(void *pvParam){
  lcd.clear();
  lcd.print("Intruder Alert!");

  while(alarmTriggered){
    digitalWrite(LED_PIN, HIGH);
    vTaskDelay(pdMS_TO_TICKS(500));
    digitalWrite(LED_PIN, LOW);
    vTaskDelay(pdMS_TO_TICKS(500));
  }

  digitalWrite(LED_PIN, LOW);
  lcd.clear();
  vTaskDelete(NULL);
}

// Button Task
void buttonTask(void *pvParam){
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  while(1){
    if(alarmTriggered && digitalRead(BUTTON_PIN) == LOW){
      alarmTriggered = false;
      sendDeactivation();
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void setup() {
  pinMode(LED_PIN, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  WiFi.mode(WIFI_STA);
  esp_now_init();
  esp_now_register_recv_cb(onDataRecv);

  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, senderAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;

  esp_now_add_peer(&peerInfo);

  xTaskCreate(buttonTask, "buttonTask", 2048, NULL, 1, NULL);
}

void loop() {

}
