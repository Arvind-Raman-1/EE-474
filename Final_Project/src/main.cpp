#include <WiFi.h>
#include <esp_now.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// Constants & Pin Definitions
#define LED_PIN 2
#define BUTTON_PIN 15

// LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Peer address (Replace with actual sender ESP32 MAC address)
uint8_t senderAddress[] = {0x24, 0xEC, 0x4A, 0x0E, 0xBD, 0x1C};

// Global state
volatile bool alarmTriggered = false;

// Function prototypes
void successTask(void *pvParam);
void alarmTask(void *pvParam);
void buttonTask(void *pvParam);

// ESP-NOW callback (Data Received)
void onDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  Serial.printf("Data received. Length: %d\n", len);

  if(len < 1) return;

  if(incomingData[0] == 1) {
    Serial.println("Success signal received.");
    xTaskCreate(successTask, "successTask", 2048, NULL, 1, NULL);
  } else if(incomingData[0] == 0) {
    Serial.println("Alarm signal received.");
    alarmTriggered = true;
    xTaskCreate(alarmTask, "alarmTask", 2048, NULL, 1, NULL);
  }
}

// ESP-NOW callback (Data Sent)
void onDataSend(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("Data sent status: ");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Fail");
}

// Send Deactivation Signal
void sendDeactivation(){
  uint8_t data = 1; // 1 indicates deactivation
  esp_err_t result = esp_now_send(senderAddress, &data, 1);
  Serial.printf("Deactivation signal sent, result: %s\n", (result == ESP_OK) ? "Success" : "Failed");
}

// Success Task
void successTask(void *pvParam){
  Serial.println("Starting successTask");
  lcd.clear();
  lcd.print("Success!");
  vTaskDelay(pdMS_TO_TICKS(5000));
  lcd.clear();
  Serial.println("successTask finished");
  vTaskDelete(NULL);
}

// Alarm Task
void alarmTask(void *pvParam){
  Serial.println("Starting alarmTask");
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
  Serial.println("alarmTask finished");
  vTaskDelete(NULL);
}

// Button Task
void buttonTask(void *pvParam){
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  Serial.println("Starting buttonTask");

  while(1){
    if(alarmTriggered && digitalRead(BUTTON_PIN) == LOW){
      Serial.println("Button pressed: deactivating alarm.");
      alarmTriggered = false;
      sendDeactivation();
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  Wire.begin(10,11);
  lcd.init();
  lcd.backlight();
  lcd.clear();

  WiFi.mode(WIFI_STA);
  if (esp_now_init() == ESP_OK) {
    Serial.println("ESP-NOW initialized successfully.");
  } else {
    Serial.println("ESP-NOW initialization failed.");
  }

  esp_now_register_recv_cb(onDataRecv);
  esp_now_register_send_cb(onDataSend);

  esp_now_peer_info_t peerInfo;
  memset(&peerInfo, 0, sizeof(peerInfo));
  memcpy(peerInfo.peer_addr, senderAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  peerInfo.ifidx = WIFI_IF_STA; 

  if (esp_now_add_peer(&peerInfo) == ESP_OK) {
    Serial.println("Peer added successfully.");
  } else {
    Serial.println("Failed to add peer.");
  }
    xTaskCreate(buttonTask, "buttonTask", 2048, NULL, 1, NULL);
  }

void loop() {

}