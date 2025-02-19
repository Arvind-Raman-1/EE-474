//FILENAME Part3send.cpp
//Authors: Arvind Raman, Aadithya Manoj
// 2/18/2025
//this file contains the functions to send data using ESP-NOW when an interrupt is triggered by a button press

//------------------------------include statements------------------------------------
#include <Arduino.h>
#include "part3send.h"
#include <esp_now.h>
#include <WiFi.h>


// Replace with receiver's MAC address
uint8_t broadcastAddress[] = {0x24, 0xEC, 0x4A, 0x0E, 0xB5, 0x3C}; 
volatile bool buttonPressed = false;


// =========> TODO: Create an ISR function to handle button press 

//-----------------------------function definitions-----------------------------------
void IRAM_ATTR send(){
    buttonPressed = true;
    //Serial.println("Button pressed");
}




void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
 // Callback function called when data is sent
 // Check if the delivery was successful and print the status
 Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Success" : "Failed");


}

//-----------------------------setup and loop functions-----------------------------------
void setup3() {

 WiFi.mode(WIFI_STA);


 // =========> TODO: Set button pin as input and attach an interrupt

 //sets the pin mode for the button
pinMode(BUTTON_PIN, INPUT_PULLUP);



//attaches the interrupt to the button pin, specifying that the pin is digital
attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), &send, FALLING);

 if (esp_now_init() != ESP_OK) return; // Initialize ESP-NOW and check for success
 esp_now_register_send_cb(onDataSent); // Register the send callback function


 esp_now_peer_info_t peerInfo; // Data structure for handling peer information
// Copy the receiver's MAC address to peer information
 memset(&peerInfo, 0, sizeof(peerInfo));
 memcpy(peerInfo.peer_addr, broadcastAddress, 6);  
 peerInfo.channel = 0; // Set WiFi channel to 0 (default)
 peerInfo.encrypt = false; // Disable encryption
 if (esp_now_add_peer(&peerInfo) != ESP_OK) return; // Add peer and check for success
}


void loop3() {
// =========> TODO: Check if button has been pressed, if so send data using
//			esp_now_send and check the result. 

//checks if the button has been pressed and sends a message if it has
//sets the buttonPressed flag to false after sending the message
if(buttonPressed){
    buttonPressed = false;
    const char *message = "Hello, this is ESP32";
    esp_now_send(broadcastAddress, (uint8_t *)message, strlen(message));
}
}

