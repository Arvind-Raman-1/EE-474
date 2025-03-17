#include <Arduino.h>

#include  "FreeRTOS.h"
#include "WiFi.h"
#include "esp_now.h"

#define trigPin 5
#define echoPin 6
#define motionSensorPin 7
#define buttonPin 8

volatile bool messageReceived = false;
uint8_t broadcastAddress[] = {0x24, 0xEC, 0x4A, 0x0E, 0xB5, 0x3C}; 
volatile bool buttonPressed = false;

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
 // Callback function called when data is sent
 // Check if the delivery was successful and print the status
 Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failed");
}

void IRAM_ATTR dataReceived(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  messageReceived = true;
}

typedef enum {
    SENSOR_ID_ULTRASONIC,  // Unique ID for the Ultrasonic sensor
    SENSOR_ID_MOTION,      // Unique ID for the Motion sensor
    SENSOR_ID_RFID,         // Unique ID for the RFID sensor
    SENSOR_ID_ALARM
} SensorID_t;

typedef struct {
    SensorID_t sensorID;
    int     sensorValue;
} sensorData_t;

int Alarm = LOW;

int state = LOW;            
int val = 0;  

QueueHandle_t sensorQueue;



typedef enum {
    TASK_PRIORITY_IDLE = 0,        // Idle tasks run at the lowest priority
    TASK_PRIORITY_RUNNING = 1,     // Running tasks at a normal priority
    TASK_PRIORITY_SCHEDULER = 2    // Scheduler tasks at the highest priority
} TaskPriority_t;

TaskHandle_t ultrasonic_handle = NULL;
TaskHandle_t send_alarm_handle = NULL;
TaskHandle_t recieve_deactivate_handle = NULL;
TaskHandle_t RFID_success_handle = NULL;
TaskHandle_t MotionRFID_task_handle = NULL;
TaskHandle_t scheduler_handle = NULL;

void ultrasonic_task(void *pvParameters){
  sensorData_t sensorData;
  long duration;
  for(;;){
    if(Alarm == LOW){
      // Clears the trigPin
      digitalWrite(trigPin, LOW);
      delayMicroseconds(2);
      // Sets the trigPin on HIGH state for 10 micro seconds
      digitalWrite(trigPin, HIGH);
      delayMicroseconds(10);
      digitalWrite(trigPin, LOW);
      // Reads the echoPin, returns the sound wave travel time in microseconds
      duration = pulseIn(echoPin, HIGH);
      // Calculating the distance
      sensorData.sensorValue = duration * 0.034 / 2;

      if(sensorData.sensorValue <10){
        Serial.println("Ultrasonic sensor detected motion");
        xQueueSend(sensorQueue, &sensorData, portMAX_DELAY);
      }
      vTaskDelay(pdMS_TO_TICKS(100));
    }
  }
}


//send alarm message to the other ESP
void send_alarm(void *pvParameters){
  sensorData_t sensorData;
  for(;;){
    Serial.println("Alarm is triggered");
    //do the esp now send stuff
    int data = 0;
    esp_now_send(broadcastAddress, (uint8_t *)data, sizeof(data));
    Alarm = HIGH;  
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}


//recieve deactivation message from the other ESP
void recieve_deactivate(void *pvParameters){
  sensorData_t sensorData;
  sensorData.sensorID = SENSOR_ID_ALARM;
  for(;;){
    if(Alarm == HIGH){
      //recieve from ESP now
      if (messageReceived) {
        sensorData.sensorValue = 0;
        Serial.println("Deactivation message received");
        messageReceived = false;
        Alarm = LOW;
        xQueueSend(sensorQueue, &sensorData, portMAX_DELAY);
      }
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

//if RFID is scanned successfully send message to the other ESP
void RFID_success(void *pvParameters){
  sensorData_t sensorData;
  sensorData.sensorID = SENSOR_ID_ALARM;
  for(;;){
    if(Alarm == LOW){
       //RFID was succesfully scanned
      sensorData.sensorValue = 0;
      int data = 1;
      esp_now_send(broadcastAddress, (uint8_t *)data, sizeof(data));
      xQueueSend(sensorQueue, &sensorData, portMAX_DELAY);
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void MotionRFID_task(void *pvParameters){
  sensorData_t sensorData;
  sensorData.sensorID = SENSOR_ID_RFID;
    
  

  for(;;) {
    if (Alarm == LOW) {
      Serial.println("Activated motion and button input");

      uint32_t startTime = millis();
      bool buttonPressed = false;
      bool motionDetected = false;

      while (millis() - startTime < 10000) {
        val = digitalRead(motionSensorPin);   // read sensor value
        if (val == HIGH) {           // check if the sensor is HIGH
          if (state == LOW) {
            Serial.println("Motion detected!"); 
            motionDetected = true;
            state = HIGH;       // update variable state to HIGH
          }
        } 
        else {
          if (state == HIGH){
            Serial.println("Motion stopped!");
            motionDetected = false;
            state = LOW;       // update variable state to LOW
          }
        }
        if (!buttonPressed && digitalRead(buttonPin) == LOW) {
          buttonPressed = true;
          Serial.println("Button pressed.");
        }
        if (buttonPressed && motionDetected) break;
        vTaskDelay(pdMS_TO_TICKS(50));
      }

      sensorData.sensorValue = (buttonPressed && motionDetected) ? 1 : 0;
      xQueueSend(sensorQueue, &sensorData, portMAX_DELAY);
    }
  }
}


//dynamically adjust priority of other tasks based on the sensor data to determine corect state
void scheduler(void *pvParameters){
  sensorData_t sensorData;
  for(;;){

    if(xQueueReceive(sensorQueue, &sensorData, portMAX_DELAY)==pdPASS){
      //idle state
      //if deactivated or if rfid successfully scanned
      //if motion leaves 
      if((sensorData.sensorID == SENSOR_ID_ALARM && sensorData.sensorValue == 0)&&Alarm == LOW){
        vTaskPrioritySet(ultrasonic_handle, TASK_PRIORITY_RUNNING);
        vTaskPrioritySet(send_alarm_handle, TASK_PRIORITY_IDLE);
        vTaskPrioritySet(MotionRFID_task_handle, TASK_PRIORITY_IDLE);
        vTaskPrioritySet(recieve_deactivate_handle, TASK_PRIORITY_IDLE);
        vTaskPrioritySet(RFID_success_handle, TASK_PRIORITY_IDLE);


      }
      //checking motion state
      // if ultrasonic sensor detects motion go to motion state
      else if((sensorData.sensorID == SENSOR_ID_ULTRASONIC)&&Alarm == LOW){
        vTaskPrioritySet(ultrasonic_handle, TASK_PRIORITY_IDLE);
        vTaskPrioritySet(send_alarm_handle, TASK_PRIORITY_IDLE);
        vTaskPrioritySet(MotionRFID_task_handle, TASK_PRIORITY_RUNNING);
        vTaskPrioritySet(recieve_deactivate_handle, TASK_PRIORITY_IDLE);
        vTaskPrioritySet(RFID_success_handle, TASK_PRIORITY_IDLE);


      }
      //alarm state
      //RFID not scanned

      else if((sensorData.sensorID == SENSOR_ID_RFID&&sensorData.sensorValue == 0)&&Alarm == LOW){
        vTaskPrioritySet(ultrasonic_handle, TASK_PRIORITY_IDLE);
        vTaskPrioritySet(send_alarm_handle, TASK_PRIORITY_RUNNING);
        vTaskPrioritySet(MotionRFID_task_handle, TASK_PRIORITY_IDLE);
        vTaskPrioritySet(recieve_deactivate_handle, TASK_PRIORITY_IDLE);
        vTaskPrioritySet(RFID_success_handle, TASK_PRIORITY_IDLE);


      }
      //deactivation state
      //will get stuck in this state untill deactivation 
      else if(Alarm == HIGH){
        vTaskPrioritySet(ultrasonic_handle, TASK_PRIORITY_IDLE);
        vTaskPrioritySet(send_alarm_handle, TASK_PRIORITY_IDLE);
        vTaskPrioritySet(MotionRFID_task_handle, TASK_PRIORITY_IDLE);
        vTaskPrioritySet(recieve_deactivate_handle, TASK_PRIORITY_RUNNING);
        vTaskPrioritySet(RFID_success_handle, TASK_PRIORITY_IDLE);

      }

      //RFID succesfully scanned state
      else if((sensorData.sensorID == SENSOR_ID_RFID&&sensorData.sensorValue == 1)&&Alarm == LOW){
        vTaskPrioritySet(ultrasonic_handle, TASK_PRIORITY_IDLE);
        vTaskPrioritySet(send_alarm_handle, TASK_PRIORITY_IDLE);
        vTaskPrioritySet(MotionRFID_task_handle, TASK_PRIORITY_IDLE);
        vTaskPrioritySet(recieve_deactivate_handle, TASK_PRIORITY_IDLE);
        vTaskPrioritySet(RFID_success_handle, TASK_PRIORITY_RUNNING);

      }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
}



void setup(){
  Serial.begin(115200);
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  pinMode(motionSensorPin, INPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  WiFi.mode(WIFI_STA);

 if (esp_now_init() != ESP_OK) return; // Initialize ESP-NOW and check for success
 esp_now_register_send_cb(onDataSent); // Register the send callback function
 esp_now_register_recv_cb(dataReceived); // Register the receive callback function


 esp_now_peer_info_t peerInfo; // Data structure for handling peer information
// Copy the receiver's MAC address to peer information
 memset(&peerInfo, 0, sizeof(peerInfo));
 memcpy(peerInfo.peer_addr, broadcastAddress, 6);  
 peerInfo.channel = 0; // Set WiFi channel to 0 (default)
 peerInfo.encrypt = false; // Disable encryption
 if (esp_now_add_peer(&peerInfo) != ESP_OK) return; // Add peer and check for success

  sensorQueue = xQueueCreate(10, sizeof(sensorData_t));
  // xTaskCreate(scheduler, "Scheduler Task", 2048, NULL, TASK_PRIORITY_SCHEDULER, &scheduler_handle);
  xTaskCreate(ultrasonic_task, "Ultrasonic Task", 2048, NULL, TASK_PRIORITY_RUNNING, &ultrasonic_handle);
  xTaskCreate(send_alarm, "Alarm Task", 2048, NULL, TASK_PRIORITY_IDLE, &send_alarm_handle);
  xTaskCreate(MotionRFID_task, "MotionRFID Task", 2048, NULL, TASK_PRIORITY_IDLE, &MotionRFID_task_handle);
  xTaskCreate(recieve_deactivate, "Deactivate Task", 2048, NULL, TASK_PRIORITY_IDLE, &recieve_deactivate_handle);
  xTaskCreate(RFID_success, "RFID Task", 2048, NULL, TASK_PRIORITY_IDLE, &RFID_success_handle);
  

}
void loop(){

}


// #include "esp_mac.h"  // required - exposes esp_mac_type_t values

// String getDefaultMacAddress();
// String getInterfaceMacAddress(esp_mac_type_t interface);
// void setup() {
//  Serial.begin(115200);


//  Serial.print("Wi-Fi Station (using 'esp_efuse_mac_get_default')\t");
//  Serial.println(getDefaultMacAddress());


//  Serial.print("WiFi Station (using 'esp_read_mac')\t\t\t");
//  Serial.println(getInterfaceMacAddress(ESP_MAC_WIFI_STA));
// }


// void loop() { /* Nothing in loop */ }


// String getDefaultMacAddress() {


//  String mac = "";


//  unsigned char mac_base[6] = {0};


//  if (esp_efuse_mac_get_default(mac_base) == ESP_OK) {
//    char buffer[18];  // 6*2 characters for hex + 5 characters for colons + 1 character for null terminator
//    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
//    mac = buffer;
//  }
//  return mac;
// }


// String getInterfaceMacAddress(esp_mac_type_t interface) {


//  String mac = "";


//  unsigned char mac_base[6] = {0};


//  if (esp_read_mac(mac_base, interface) == ESP_OK) {
//    char buffer[18];  // 6*2 characters for hex + 5 characters for colons + 1 character for null terminator
//    sprintf(buffer, "%02X:%02X:%02X:%02X:%02X:%02X", mac_base[0], mac_base[1], mac_base[2], mac_base[3], mac_base[4], mac_base[5]);
//    mac = buffer;
//  }


//  return mac;
// }

