//FILENAME part2.h
//Authors: Arvind Raman, Aadithya Manoj
// 2/18/2025
//this file contains the function declarations for part2.cpp
//------------------------------include statements------------------------------------

#include<stdio.h>
#include<Arduino.h>

//------------------------------global variables------------------------------------
#define MAX_NUM_TASK 10
#define true 1
#define false 0
#define LED_PIN 6
//-----------------------------function declarations-----------------------------------
void p2setup();
void p2loop();

void blink();
void count();
void music();
void printer();

//-----------------------------struct definition-----------------------------------
struct TCB {
    void (*taskFunction)();  // Pointer to the task function
    bool isRunning;          // State of the task
    bool isDone;             // Whether the task is completed or not
    int pid;                 // Unique process ID
    int priority;	      // The priority level of the task
   };
   
