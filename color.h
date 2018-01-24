#include "stack.h"
#include "Chooser.h"

#include <Adafruit_RGBLCDShield.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define sensorOut 8

int redFrequency = 0;
int blueFrequency = 0;
int greenFrequency = 0;

//Gets the color
int getColor()
{

  digitalWrite(S2, LOW);
  digitalWrite(S3, LOW);

  redFrequency = pulseIn(sensorOut, LOW);
/*
  Serial.print(redFrequency);
  Serial.print("  ");
*/
  //Code below is for green
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);

  greenFrequency = pulseIn(sensorOut, LOW);
/*
  Serial.print(greenFrequency);
  Serial.print("  ");
*/
  //Code below is for blue
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);

  blueFrequency = pulseIn(sensorOut, LOW);

//  Serial.println(blueFrequency);

  lcd.setCursor(0, 0);

  //changes LCD light to the color of the line it passes
  if(redFrequency < 46 && greenFrequency > blueFrequency && greenFrequency > 75 && redFrequency < blueFrequency)
  {
    lcd.setBacklight(RED);
    return 2; //Red
  }
  else if(redFrequency < 130 && redFrequency > greenFrequency && greenFrequency > blueFrequency && blueFrequency < 40)
  {
    lcd.setBacklight(BLUE);
    return 2; //Blue
  }  
  else
  {
    lcd.setBacklight(WHITE);
    return 0; //Nothing
  }
}
