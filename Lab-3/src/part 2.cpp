//FILENAME part2.cpp
//Authors: Arvind Raman, Aadithya Manoj
// 2/18/2025
//this file uses a TCB to run tasks with different priorities
//the tasks are blink, count, music, and printer


//------------------------------include statements------------------------------------
#include<stdio.h>
#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "part 2.h"


//------------------------------global variables------------------------------------
TCB tasks[MAX_NUM_TASK];
LiquidCrystal_I2C lcd(0x27, 16, 2);



//------------------------------setup functions-----------------------------------
void p2setup(){
    Serial.begin(9600);
    //Wire.begin();
    lcd.init();
    pinMode(LED_PIN, OUTPUT);   

    tasks[0].taskFunction = blink;
    tasks[0].isRunning = false;
    tasks[0].isDone = false;
    tasks[0].pid = 0;
    tasks[0].priority = 1;

    tasks[1].taskFunction = count;
    tasks[1].isRunning = false;
    tasks[1].isDone = false;
    tasks[1].pid = 1;
    tasks[1].priority = 3;

    tasks[2].taskFunction = music;
    tasks[2].isRunning = false;
    tasks[2].isDone = false;
    tasks[2].pid = 2;
    tasks[2].priority = 2;


    tasks[3].taskFunction = printer;
    tasks[3].isRunning = false;
    tasks[3].isDone = false;
    tasks[3].pid = 3;
    tasks[3].priority = 4;

}


//-----------------------------loop function-----------------------------------
void p2loop(){
    //chech highest priority
    if(!(tasks[0].isDone&tasks[1].isDone&tasks[2].isDone&tasks[3].isDone)){
        //run tasks
        //find highest priority
        int HIGH_P_PID = -1;
        for(int i = 0; i<MAX_NUM_TASK; i++){
            if(!tasks[i].isRunning&&!tasks[i].isDone){
                if(HIGH_P_PID == -1 || tasks[i].priority>tasks[HIGH_P_PID].priority){
                    HIGH_P_PID = i;
                }
            }
        }

        //run highest priority task and update struct params
        if(HIGH_P_PID!=-1){
            tasks[HIGH_P_PID].isRunning = true;
            tasks[HIGH_P_PID].taskFunction();
            tasks[HIGH_P_PID].isRunning = false;
            tasks[HIGH_P_PID].isDone = true;
            Serial.println(tasks[HIGH_P_PID].priority); 
        }
    }

    //if everything is done reset done flag
    else{
        tasks[0].priority = tasks[0].priority%4 + 1;
        tasks[1].priority = tasks[1].priority%4 + 1;
        tasks[2].priority = tasks[2].priority%4 + 1;
        tasks[3].priority = tasks[3].priority%4 + 1;

        tasks[0].isDone = false;
        tasks[1].isDone = false;
        tasks[2].isDone = false;
        tasks[3].isDone = false;
    }

}


//-----------------------------task function definitions-----------------------------------
//task functions 
void blink(){
    for(int i = 0; i<8; i++){
        digitalWrite(LED_PIN, HIGH);
        delay(1000);
        digitalWrite(LED_PIN, LOW);
        delay(1000);
        
    }
    Serial.print("LED Blinker: ");
}


void count(){
    lcd.clear();
    lcd.backlight();
    for(int i = 1; i<=10; i++){
        lcd.clear();
        lcd.print("count: ");
        lcd.print(i);
        delay(1000);
        
    }
    Serial.print("Counter: ");
}

void music(){
    int ledChannel = 0;
    int resolution = 8;         // 8-bit resolution: duty cycle from 0-255
    int baseFreq = 5000;        // Base frequency 5 kHz

    // Setup LEDC for PWM control
    ledcSetup(ledChannel, baseFreq, resolution);
    ledcAttachPin(5, ledChannel);

    ledcWrite(ledChannel, 50); 
    delay(1000);
    ledcWrite(ledChannel, 250); 
    delay(1000);
    ledcWrite(ledChannel, 150); 
    delay(1000);
    ledcWrite(ledChannel, 10); 
    delay(1000);
    ledcWrite(ledChannel, 15);
    delay(1000); 
    ledcWrite(ledChannel, 45);
    delay(1000); 
    ledcWrite(ledChannel, 175);
    delay(1000); 
    ledcWrite(ledChannel, 30);
    delay(1000); 
    ledcWrite(ledChannel, 180);
    delay(1000); 
    ledcWrite(ledChannel, 40); 
    delay(1000);
    ledcWrite(ledChannel, 0); 

    Serial.print("music: ");
}

void printer(){
    for (int i = 0; i < 26; i++) {
        Serial.printf("%c, ", 'A' + i);
        delay(100);
    }
    Serial.println();
    Serial.print("Alphabet Printer: ");
}


