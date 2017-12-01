#include <Adafruit_RGBLCDShield.h>
//Adafruit_RGBLCDShield lcd2 = Adafruit_RGBLCDShield();


struct myStack{
  byte stackArr[16];
  int top = -1;
  byte front = 0;
};

struct myStack stack;
int i;

void push(int number)
{
  if(stack.top == 15)
  {
    Serial.println("Stack is full");
  }
  else
  {
    stack.top++;
    stack.stackArr[stack.top] = number;
  }
}

int pop()
{
  int number;
  
  if(stack.top == -1)
  {
    Serial.println("Stack is empty");
    return - 1;
  }
  else
  {
    number = stack.stackArr[stack.top];
    stack.top--;
    return number;
  }
}

bool empty()
{
  if(stack.top == -1)
    return true;
  else
    return false;
}

int top()
{
  return stack.stackArr[stack.top];
}

int front()
{
  return stack.stackArr[0];
}

void queue(int number)
{
  for(i = 0; i < 16; i++)
  {
    if(stack.stackArr[i]==0)
    {
      stack.stackArr[i] = number;
      break;
    }
  }
}

void dequeue()
{
  for(i = 1; i<16; i++)
  {
    stack.stackArr[i-1] = stack.stackArr[i];
  }
}

void emptyStack()
{
  for(i = 0; i<16; i++)
  {
    stack.stackArr[i]=0;
  }
}  

void printStackReverse()
{
  for(i = 0; i<16; i++)
  {
    Serial.print("");
    if(stack.stackArr[i]==0)
      continue;
    else
      Serial.println(stack.stackArr[i] + ' ');
  }
  return;
}

void setStackTop()
{
  for(i=0; i<16; i++)
  {
   Serial.println("");
   if(stack.stackArr[i]==0)
   {   
      stack.top = i-1;
      break;
   }
  }
}




