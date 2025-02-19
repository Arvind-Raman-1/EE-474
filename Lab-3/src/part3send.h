//FILENAME Part3send.h
//Authors: Arvind Raman, Aadithya Manoj
// 2/18/2025
//this file contains the function declarations for part3send.cpp

//------------------------------include statements------------------------------------
#include <Arduino.h>
#include <esp_now.h>
#define BUTTON_PIN 6




//-----------------------------function declarations-----------------------------------
void setup3();
void loop3();
void IRAM_ATTR send();

void onDataSent(const uint8_t *mac_addr, esp_now_send_status_t status);
