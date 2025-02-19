//FILENAME part2.h
//Authors: Arvind Raman, Aadithya Manoj
// 2/18/2025
//this file contains the function declarations for part2.cpp
//------------------------------global variables------------------------------------

#ifndef TASK3RECIEVER_H
#define TASK3RECIEVER_H


//-----------------------------function declarations-----------------------------------
void setupTask3Reciever();

void loopTask3Reciever();

void IRAM_ATTR dataReceived();
void IRAM_ATTR onTimer();
#endif 
