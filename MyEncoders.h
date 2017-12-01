#include <PinChangeInt.h>

#define ENCODER_PORT_RIGHT 11
#define ENCODER_PORT_LEFT 10


#ifndef __MY_ENCODERS__
#define __MY_ENCODERS__

unsigned long rightTicks;
unsigned long leftTicks;
unsigned long rightTime;
unsigned long leftTime;
unsigned long oldRightTime;
unsigned long oldLeftTime;

//The two functions below increment the counts
void risingRight(){
  rightTicks++;
  oldRightTime = rightTime;
  rightTime = millis();
}

void risingLeft(){
  leftTicks++;
  oldLeftTime = leftTime;
  leftTime = millis();
}

//this function sets the tick counts to 0
void resetCounts(){
  noInterrupts();
  rightTicks = 0;
  leftTicks = 0;
  interrupts();
}

//this function should return the left and right tickcounts 
// since either the start of the program or the last
//call to the function resetCounts()
void getCounts(unsigned long counts[]){
  counts[0] = leftTicks;
  counts[1] = rightTicks;
}


//this function should return the instantaneous speeds of the wheels
//meassured in revolutions per second.
void getSpeeds(float speeds[]){
  unsigned long current = millis();

  if(current - leftTime >= 1500)
    speeds[0] = 0;
  else
    speeds[0] = 1 / (((leftTime - oldLeftTime) * 0.001) * 32);
  if(current - rightTime >= 1500)
    speeds[1] = 0;
  else
    speeds[1] = 1 / (((rightTime - oldRightTime) * 0.001) * 32);
}

//this function should include whatever code necessary to initialize this module
void initEncoders(){
  rightTicks = 0;
  leftTicks = 0;
  leftTime = millis();
  rightTime = millis();
  //Code below initializes encoder stuff
  pinMode(ENCODER_PORT_RIGHT, INPUT_PULLUP);
  pinMode(ENCODER_PORT_LEFT, INPUT_PULLUP);
  PCintPort::attachInterrupt(ENCODER_PORT_RIGHT, &risingRight, RISING);
  PCintPort::attachInterrupt(ENCODER_PORT_LEFT, &risingLeft, RISING);
}


#endif
