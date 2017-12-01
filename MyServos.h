#include <Servo.h>
#include <EEPROM.h>
#include "MyEncoders.h"

Servo LServo;
Servo RServo;
int RPSMapLeft[105];
int RPSMapRight[105];

#ifndef __MY_SERVOS__
#define __MY_SERVOS__

//NOTE: you are allowed to modify this file as long as the functions
//implement the requested functionality


//this function should set the left and right speeds of the wheel
//when both inputs are positive the robot should move forward
//when both inputs are negative the robot should move backward
//microsLeft should encode the left wheel velocity and viceversa
void setSpeeds(int microsLeft, int microsRight){
  int speedLeft = 1500 + microsLeft;
  int speedRight = 1500 - microsRight;
  
  LServo.writeMicroseconds(speedLeft);
  RServo.writeMicroseconds(speedRight);
}




//same as the function setSpeeds, but the inputs now indicate 
//the revolutions per secont (rps) at which each wheel should spin
void setSpeedsRPS(float rpsLeft, float rpsRight){
  int rpsL, rpsR;

  if(rpsLeft < 0 && rpsRight < 0){
    rpsL = -rpsLeft;
    rpsR = -rpsRight;
    setSpeeds(RPSMapLeft[52 - rpsL], RPSMapRight[52 - rpsR]);
  }
  else if(rpsLeft < 0){
    rpsL = -rpsLeft;
    rpsR = rpsRight;
    setSpeeds(RPSMapLeft[52 - rpsL], RPSMapRight[52 + rpsR]);
  }
  else if(rpsRight < 0){
    rpsL = rpsLeft;
    rpsR = -rpsRight;
    setSpeeds(RPSMapLeft[52 + rpsL], RPSMapRight[52 - rpsR]);
  }
  else{
    rpsL = rpsLeft;
    rpsR = rpsRight;
    setSpeeds(RPSMapLeft[52 + rpsL], RPSMapRight[52 + rpsR]);
  }
}

//same as the function setSpeeds, but the inputs now indicate 
//the inches per second (ips) at which each wheel should spin
void setSpeedsIPS(float ipsLeft, float ipsRight){
  float circumference = 2.61 * 3.14;
  int rpsLeft, rpsRight;

  rpsLeft = (ipsLeft / circumference) * 100;
  rpsRight = (ipsRight / circumference) * 100;
  

  setSpeedsRPS(rpsLeft, rpsRight);
}


//this function is meant for doing whatever necessary for the 
//functions 'setSpeedsIPS' and 'setSpeedsRPS' to work properly
void calibrate(){
  int i, j;
  int locationLeft, locationRight;
  unsigned long past, current;
  float speeds[2];
  float sumLeft, sumRight, averageLeft, averageRight, count;
  int bottomLeft, bottomRight, topLeft, topRight;

  /*Sets all values in the array to -1*/
  for(i = 1;i < 105; i++){
    RPSMapLeft[i] = -200;
    RPSMapRight[i] = -200;
  }

  /*Goes from 150 to 2 in incerments of 2. This number gets sent to setSpeeds*/
  for(j = 60; j >= -59; j -= 3){
    count = 0;
    sumLeft = 0;
    sumRight = 0;
    setSpeeds(j, j);
    delay(500);
    past = millis();
    /*Gets the average speed over a period of time*/
    while(millis() - past < 1500){
      getSpeeds(speeds);
      sumLeft += speeds[0];
      sumRight += speeds[1];
      count++;
      if(count == 28)
        break;
      delay(30);
    }
      averageLeft = sumLeft / count;
      averageRight = sumRight / count;
      locationLeft = averageLeft * 100;
      locationRight = averageRight * 100;
      /*Places the microseconds into the arrays*/
      if(j > 0){
        RPSMapLeft[52 + locationLeft] = j;
        RPSMapRight[52 + locationRight] = j;
      }
      else{
        RPSMapLeft[52 - locationLeft] = j;
        RPSMapRight[52 - locationRight] = j;
      }

  }
  setSpeeds(0, 0);

  
  RPSMapLeft[0] = -60;
  RPSMapRight[0] = -60;
  RPSMapLeft[52] = 0;
  RPSMapRight[52] = 0;
  
/*
 for(i = 0; i < 81; i++){
    Serial.print(i);
    Serial.print("  ");
    Serial.print(RPSMapLeft[i]);
    Serial.print("  ");
    Serial.println(RPSMapRight[i]);
  }
*/

  /*Fills in any missing values with the average of the next non-missing value
    and the value directly under it*/
  for(i = 1; i < 105; i++){
    if(RPSMapLeft[i] == -200){
      for(j = i; j < 105; j++){
        if(RPSMapLeft[j] != -200){
          RPSMapLeft[i] = (RPSMapLeft[i - 1] + RPSMapLeft[j]) / 2;
          break;
        }
        if(j == 104)
          RPSMapLeft[i] = RPSMapLeft[i - 1];
      }
    }
    if(RPSMapRight[i] == -200){
      for(j = i; j < 105; j++){
        if(RPSMapRight[j] != -200){
          RPSMapRight[i] = (RPSMapRight[i - 1] + RPSMapRight[j]) / 2;
          break;
        }
        if(j == 104)
          RPSMapRight[i] = RPSMapRight[i - 1];
      }
    }
  }

int eeAddress = 0;
EEPROM.put(eeAddress, RPSMapLeft);
eeAddress += sizeof(int) *105;
EEPROM.put(eeAddress, RPSMapRight);
/*
  for(i = 0; i < 81; i++){
    Serial.print(i);
    Serial.print("  ");
    Serial.print(RPSMapLeft[i]);
    Serial.print("  ");
    Serial.println(RPSMapRight[i]);
  }*/
}


//this function should make the robot move with a linear speed of v 
//measured in inches per second, and an angular speed of w
//in rads per second. Positive ws should indicate a counterclockwise spin.
void setSpeedsvw(float v, float w){
  float radius;
  float velocityLeft, velocityRight;
  float wheel_distance = 2;
  
  if(w < 0){
    w = -w;
    radius = v / w;
    velocityLeft = w * (radius + wheel_distance);
    velocityRight = w * (radius - wheel_distance);
  }
  else{
    radius = v / w;
    velocityLeft = w * (radius - wheel_distance);
    velocityRight = w * (radius + wheel_distance);
  }

  if(velocityLeft > 6)
    velocityLeft = 6;
  else if(velocityLeft < -6)
    velocityLeft = -6;
  if(velocityRight > 6)
    velocityRight = 6;
  else if(velocityRight < -6)
    velocityRight = -6;
  

  setSpeedsIPS(velocityLeft, velocityRight);
}



#endif 
