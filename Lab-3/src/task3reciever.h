#ifndef TASK3RECIEVER_H
#define TASK3RECIEVER_H

void setupTask3Reciever();

void loopTask3Reciever();

void IRAM_ATTR dataReceived();
void IRAM_ATTR onTimer();
#endif 
