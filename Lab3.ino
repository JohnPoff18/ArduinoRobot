#include "MyEncoders.h"
#include "robot.h"

String complete = "XXXXXXXXXXXXXXXX";
bool first = true;
bool second = true;
bool third = true;
bool fourth = true;
bool broke = false;
bool load = true;

bool DFS();
void initDFS(); //Will be used to in the future to choose location and orientation
bool BFS(); // This will be used for your function
void initBFS();//For your function


unsigned int numPrograms = 2;
bool (*programList[])() = {&DFS, &BFS};
void (*setupProgramList[])() = {&initDFS,&initBFS};
const char * programNames[] {"Mapping", "Path Planning"}; //We can change the names later. Just this for now
byte startPoint = 1; //for holding the start point the person chooses
byte endingPoint = 1; //for holding the ending point the person chooses
int popValue = 0;

//The first and second stuff is just there so that when select is pressed it does the correct thing it's supposed to d o


//Fills in the number of the cells
void fillCells()
{
  int i, j;
  byte cellNumber = 1;

  for(i = 0; i < 4; i++){
    for(j = 0; j < 4; j++){
      cells[i][j].cellNumber = cellNumber;
      cellNumber++;
    }
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(sensorOut, INPUT);
  //The above connects the pins

  //Sets frequency scaling
  digitalWrite(S0, HIGH);
  digitalWrite(S1, HIGH);

  lcd.begin(16, 2);
  lcd.setCursor(0, 0);

  initEncoders();
  LServo.attach(2);
  RServo.attach(3);

  EEPROM.get(0, RPSMapLeft);
  EEPROM.get(sizeof(int) * 105, RPSMapRight);

  fillCells();

  initChooser(&lcd);
}

void loop() {
  execProgram();
}

bool DFS(){
  getColor();
  int i, j;
  uint8_t buttons = lcd.readButtons();

  if(fourth){
    setSpeedsRPS(0, 0);
    updateDisplay();
    fourth = false;
  }

  //Starts the DFS
  if(first && second)
  {
    first = false;
    push(myRobot.cellLocation);
    updateCurrentCell();
    while(true){
      //This is only done at the first grid cell. It just senses if there is a wall behind it
      if(!checkAllSide()){
        turnRobot();
        updateCurrentCell();
        updateDisplay();
      }
      //Check if location in front is blocked by a wall or if it has been visited
      //If no wall and not visited, go to that location. Else, turn robot and try again
      //If all locations have a wall or has been visited, pop from the stack and go to the
      //location on top of stack and try again

      //Everything has been visited
      if(complete == visitedLocations)
      {
        setSpeedsIPS(0,0);
        clearScreen();
        lcd.setCursor(6,0);
        lcd.print("DONE");
        break;
      }

      //I'm only commenting on this one since the rest are the same just a different orientation
      //Checks if there is a wall in front of it and if it has been visited
      resetCounts();
      if(myRobot.orientation == 1 && cells[myRobot.x][myRobot.y].northWall == 0 && cells[myRobot.x - 1][myRobot.y].visited == false){
        while(true){
          setSpeedsIPS(4, 4); //Moves the robot forward until it detects a color
          takeNewMeasurement(SLEFT);
          takeNewMeasurement(SRIGHT);
          uint8_t buttons = lcd.readButtons();
          if(buttons & BUTTON_SELECT){
            setSpeedsIPS(0,0);
            broke = true;
            break;
          }
          if(getColor() == 2){
            myRobot.x = myRobot.x - 1;  //Change position of robot
            cells[myRobot.x][myRobot.y].southWall = 0;  //Since it came from the south, there can't be a wall behind it
            myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;  //Updates the robot's cell
            push(myRobot.cellLocation);   //Pushes the current cell onto the stack
            updateDisplay();  //Updates LCD
            //adjustment();
            travelToMid();    //Travels to middle of cell
            updateCurrentCell();    //Updates the cell's walls
            break;  //Breaks out of while loop and will repeat
          } 
          delay(60);
          doOne();         
        } 
      }
      else if(myRobot.orientation == 2 && cells[myRobot.x][myRobot.y].eastWall == 0 && cells[myRobot.x][myRobot.y + 1].visited == false){
        while(true){
          setSpeedsIPS(4, 4);
          takeNewMeasurement(SLEFT);
          takeNewMeasurement(SRIGHT);
          uint8_t buttons = lcd.readButtons();
          if(buttons & BUTTON_SELECT){
            setSpeedsIPS(0,0);
            broke = true;
            break;
          }
          if(getColor() == 2){
            myRobot.y = myRobot.y + 1;
            cells[myRobot.x][myRobot.y].westWall = 0;
            myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
            push(myRobot.cellLocation);
            updateDisplay();
            //adjustment();
            travelToMid();
            updateCurrentCell(); 
            break;
          }
          delay(60);
          doOne();
        }
      }
      else if(myRobot.orientation == 3 && cells[myRobot.x][myRobot.y].southWall == 0 && cells[myRobot.x + 1][myRobot.y].visited == false){
        while(true){
          setSpeedsIPS(4, 4);
          takeNewMeasurement(SLEFT);
          takeNewMeasurement(SRIGHT);
          uint8_t buttons = lcd.readButtons();
          if(buttons & BUTTON_SELECT){
            setSpeedsIPS(0,0);
            broke = true;
            break;
          }
          if(getColor() == 2){
            myRobot.x = myRobot.x + 1;
            cells[myRobot.x][myRobot.y].northWall = 0;
            myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
            push(myRobot.cellLocation);
            updateDisplay();
            //adjustment();
            travelToMid();
            updateCurrentCell(); 
            break;
          }
          delay(60);
          doOne();
        }
      }
      else if(myRobot.orientation == 0 && cells[myRobot.x][myRobot.y].westWall == 0 && cells[myRobot.x][myRobot.y - 1].visited == false){
        while(true){
          setSpeedsIPS(4, 4);
          takeNewMeasurement(SLEFT);
          takeNewMeasurement(SRIGHT);
          uint8_t buttons = lcd.readButtons();
          if(buttons & BUTTON_SELECT){
            setSpeedsIPS(0,0);
            broke = true;
            break;
          }
          if(getColor() == 2){
            myRobot.y = myRobot.y - 1;
            cells[myRobot.x][myRobot.y].eastWall = 0;
            myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
            push(myRobot.cellLocation);
            updateDisplay();
            //adjustment();
            travelToMid();
            updateCurrentCell(); 
            break;
          }
          delay(60);
          doOne();
        }
      }
      //If everything around the robot is a wall or visited, time to backtrack
      //TTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTTT
      //The code below will probably be similar for your following the shortest path. First, you will need to probably create a new stack and push onto it the path in reverse
      //like you are doing in your stack reverse function. Instead of printing, just put it on a stack. Then you pop of the top of the stack, since that will be the current position,
      //and go to whatever is on top of the stack. Check if the cell in front is the cell on top of the stack. If it isn't, turnRobot(). If it is, move forward until you
      //detect a color. Once a color is detected, travelToMid(). Then repeat. You will have to create a new top() function since the current top() function only returns what was
      //used in my stack. Also, you'll have to change around things from my code when you change it to BFS since there are probably things in the code below you don't have to worry about.
      //You'll still need the takeNewMeasurement stuff since that is needed to have the robot do a bit of wall following. Don't copy the stuff that has to do with broke.  
      //I put a comment at one such spot below. That's in case the robot hits a wall or messes up, we can hit the select button so that we still get points for displaying the map.
      else if(allWallsOrVisited()){
        pop();  //Pops current cell
        while(true){
          if(broke)
            break;
          //Checks if the cell in front of it is what is on top of the stack. If it is, travel to it
          if(myRobot.orientation == 1 && cells[myRobot.x][myRobot.y].northWall == 0 && cells[myRobot.x - 1][myRobot.y].cellNumber == top()){
            while(true){
              setSpeedsIPS(4, 4);
              takeNewMeasurement(SLEFT);
              takeNewMeasurement(SRIGHT);
              uint8_t buttons = lcd.readButtons();    //Broke start
              if(buttons & BUTTON_SELECT){
                setSpeedsIPS(0,0);
                broke = true;
                break;
              }                                     //Broke end
              if(getColor() == 2){
                myRobot.x = myRobot.x - 1;
                myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
                updateDisplay();
                //adjustment();
                travelToMid();
                delay(100);
                break;
              }
              delay(60);
              doOne();           
            }
            break; 
          }
          else if(myRobot.orientation == 2 && cells[myRobot.x][myRobot.y].eastWall == 0 && cells[myRobot.x][myRobot.y + 1].cellNumber == top()){
            while(true){
              setSpeedsIPS(4, 4);
              takeNewMeasurement(SLEFT);
              takeNewMeasurement(SRIGHT);
              uint8_t buttons = lcd.readButtons();
              if(buttons & BUTTON_SELECT){
                setSpeedsIPS(0,0);
                broke = true;
                break;
              } 
              if(getColor() == 2){
                myRobot.y = myRobot.y + 1;
                myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
                updateDisplay();
                //adjustment();
                travelToMid(); 
                delay(100);
                break;
              }
              delay(60);
              doOne();
            }
            break;
          }
          else if(myRobot.orientation == 3 && cells[myRobot.x][myRobot.y].southWall == 0 && cells[myRobot.x + 1][myRobot.y].cellNumber == top()){
            while(true){
              setSpeedsIPS(4, 4);
              takeNewMeasurement(SLEFT);
              takeNewMeasurement(SRIGHT);
              uint8_t buttons = lcd.readButtons();
              if(buttons & BUTTON_SELECT){
                setSpeedsIPS(0,0);
                broke = true;
                break;
              } 
              if(getColor() == 2){
                myRobot.x = myRobot.x + 1;
                myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
                updateDisplay();
                //adjustment();
                travelToMid();
                delay(100);
                break;
              }
              delay(60);
              doOne();
            }
            break;
          }
          else if(myRobot.orientation == 0 && cells[myRobot.x][myRobot.y].westWall == 0 && cells[myRobot.x][myRobot.y - 1].cellNumber == top()){
            while(true){
              setSpeedsIPS(4, 4);
              takeNewMeasurement(SLEFT);
              takeNewMeasurement(SRIGHT);
              uint8_t buttons = lcd.readButtons();
              if(buttons & BUTTON_SELECT){
                setSpeedsIPS(0,0);
                broke = true;
                break;
              } 
              if(getColor() == 2){
                myRobot.y = myRobot.y - 1;
                myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
                updateDisplay();
                //adjustment();
                travelToMid();
                delay(100);
                break;
              }
              delay(60);
              doOne();
            }
            break;
          }
          else{
            turnRobot();
          }
        }
      }
      else{
        //Since there is a wall or the cell in front is visited, the robot needs to turn
        turnRobot();
        updateDisplay();
      }
      uint8_t buttons = lcd.readButtons();
      if(buttons & BUTTON_SELECT || broke)
        break;
    }
  }
  else if(buttons & BUTTON_SELECT && !first && second){
        clearScreen();
        lcd.setCursor(0,0);
        second = false;
        
        //Prints out if there is a wall or not for the first half of the cells
        for(i = 0; i < 2; i++)
        {
          for(j = 0; j < 4; j++)
          {
            if(cells[i][j].westWall == 1)
              lcd.print("W");
            else
              lcd.print("o");
            if(cells[i][j].northWall == 1)
              lcd.print("W");
            else
              lcd.print("o");
            if(cells[i][j].eastWall == 1)
              lcd.print("W");
            else
              lcd.print("o");
            if(cells[i][j].southWall == 1)
              lcd.print("W");
            else
              lcd.print("o");
          }
          lcd.setCursor(0,1);
        }
        delay(500);
  }
  else if(buttons & BUTTON_SELECT && !first && !second){
    clearScreen();
    lcd.setCursor(0,0);
    for(i = 2; i < 4; i++)
        {
          for(j = 0; j < 4; j++)
          {
            if(cells[i][j].westWall == 1)
              lcd.print("W");
            else
              lcd.print("o");
            if(cells[i][j].northWall == 1)
              lcd.print("W");
            else
              lcd.print("o");
            if(cells[i][j].eastWall == 1)
              lcd.print("W");
            else
              lcd.print("o");
            if(cells[i][j].southWall == 1)
              lcd.print("W");
            else
              lcd.print("o");
          }
          lcd.setCursor(0,1);
        }
        first = true;
  }
  else if(buttons & BUTTON_SELECT && first && !second){
    clearScreen();
    return true;
  }
  delay(300);

  return false;
}

void initDFS(){
  load = false;
  clearScreen();
  lcd.setCursor(0,0);
  lcd.print("Location");

  while(true){
    delay(200);
    uint8_t buttons = lcd.readButtons();
    lcd.setCursor(0,1);
    lcd.print(myRobot.cellLocation);
    if(buttons & BUTTON_RIGHT){
      myRobot.cellLocation++;
      if(myRobot.cellLocation > 16)
        myRobot.cellLocation = 16;
    }
    if(buttons & BUTTON_LEFT){
      myRobot.cellLocation--;
      if(myRobot.cellLocation < 1)
        myRobot.cellLocation = 1;
    }
    if(buttons & BUTTON_SELECT)
      break;
  }
  setRobotCoordinates();
  clearScreen();
  lcd.setCursor(0,0);
  lcd.print("Orientation");
  while(true){
    delay(200);
    uint8_t buttons = lcd.readButtons();
    lcd.setCursor(0,1);
    printOrientation();
    if(buttons & BUTTON_RIGHT){
      myRobot.orientation++;
      if(myRobot.orientation > 3)
        myRobot.orientation = 3;
    }
    if(buttons & BUTTON_LEFT){
      myRobot.orientation--;
      if(myRobot.orientation < 0)
        myRobot.orientation = 0;
    }
    if(buttons & BUTTON_SELECT)
      break;    
  }
  clearScreen();
  lcd.setCursor(0,0);
  lcd.print("Exit");
  while(true){
    delay(200);
    uint8_t buttons = lcd.readButtons();
    if(buttons & BUTTON_SELECT)
      break;
  }
}

bool BFS(){
  //Put your "main" code in here. This will run your code if BFS is selected. Returning false
  //makes the program loop. Returning true exits the loop andd brings you back to the options screen
  //Get current color
  getColor();
  //empty the stack from DFS
  emptyStack();
  fillBFS();
  uint8_t buttons = lcd.readButtons();

  while(true)
  {
  int current = startPoint;
  bfs[current].vistedBFS = true;
  bfs[current].prev = 0;
  queue(current);

  while(current!=0 && third)
  {
    current = front();
    dequeue();
    
    if(current==endingPoint)
    {
      emptyStack();
      while(current != 0)
      {
        queue(current);
        current = bfs[current].prev;
      }
      break;
    }

    if(bfs[current].southWall == 0 && bfs[current+4].vistedBFS==false)
    {
      queue(current+4);
      bfs[current+4].prev = current;
      bfs[current+4].vistedBFS = true;
    }

    if(bfs[current].westWall == 0 && bfs[current-1].vistedBFS==false)
    {
      queue(current-1);
      bfs[current-1].prev = current;
      bfs[current-1].vistedBFS=true;
    }

    if(bfs[current].northWall == 0 && bfs[current-4].vistedBFS==false)
    {
      queue(current-4);
      bfs[current-4].prev = current;
      bfs[current-4].vistedBFS=true;
    }

    if(bfs[current].eastWall == 0 && bfs[current+1].vistedBFS==false)
    {
      queue(current+1);
      bfs[current+1].prev = current;
      bfs[current+1].vistedBFS=true;
    }
  }
  third = false;
  
  myRobot.cellLocation = startPoint;
  setRobotCoordinates();

  setStackTop();
  current = top();
  popValue = pop();
  current = top();

  while(popValue!=-1)
  {
    if(myRobot.cellLocation==endingPoint)
    {
      setSpeedsIPS(0,0);
      return true;
    }
    if(myRobot.orientation == 1 && cells[myRobot.x - 1][myRobot.y].cellNumber == current){
            while(true){
              setSpeedsIPS(4, 4);
              takeNewMeasurement(SLEFT);
              takeNewMeasurement(SRIGHT);
              
              if(getColor() == 2){
                myRobot.x = myRobot.x - 1;
                myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
                updateDisplay();
                //adjustment();
                travelToMid();
                popValue = pop();
                current = top();
                break;
              }
              delay(60);
              doOne();           
            } 
          }
          else if(myRobot.orientation == 2 && cells[myRobot.x][myRobot.y + 1].cellNumber == current){
            while(true){
              setSpeedsIPS(4, 4);
              takeNewMeasurement(SLEFT);
              takeNewMeasurement(SRIGHT);
             
              if(getColor() == 2){
                myRobot.y = myRobot.y + 1;
                myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
                updateDisplay();
                //adjustment();
                travelToMid();
                popValue = pop();
                current = top();
                break;
              }
              delay(60);
              doOne();
            }
          }
          else if(myRobot.orientation == 3 && cells[myRobot.x + 1][myRobot.y].cellNumber == current){
            while(true){
              setSpeedsIPS(4, 4);
              takeNewMeasurement(SLEFT);
              takeNewMeasurement(SRIGHT);
             
              if(getColor() == 2){
                myRobot.x = myRobot.x + 1;
                myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
                updateDisplay();
                //adjustment();
                travelToMid();
                popValue = pop();
                current = top();
                break;
              }
              delay(60);
              doOne();
            }
          }
          else if(myRobot.orientation == 0 && cells[myRobot.x][myRobot.y - 1].cellNumber == current){
            while(true){
              setSpeedsIPS(4, 4);
              takeNewMeasurement(SLEFT);
              takeNewMeasurement(SRIGHT);
             
              if(getColor() == 2){
                myRobot.y = myRobot.y - 1;
                myRobot.cellLocation = cells[myRobot.x][myRobot.y].cellNumber;
                updateDisplay();
                //adjustment();
                travelToMid();
                popValue = pop();
                current = top();
                break;
              }
              delay(60);
              doOne();
            }
          }
          else{
            turnRobot();
          }    
    }
  }

  setSpeedsIPS(0, 0);
  return true;
}

void initBFS(){
  clearScreen();
  lcd.setCursor(0,0);
  lcd.print("Starting Location");

  while(true){
    delay(200);
    uint8_t buttons = lcd.readButtons();
    lcd.setCursor(0,1);
    lcd.print(startPoint);
    if(buttons & BUTTON_RIGHT){
      startPoint++;
      if(startPoint > 16)
        startPoint = 16;
    }
    if(buttons & BUTTON_LEFT){
      startPoint--;
      if(startPoint < 1)
        startPoint = 1;
    }
    if(buttons & BUTTON_SELECT)
      break;
  }

  clearScreen();
  lcd.setCursor(0,0);
  lcd.print("Ending Location");

  while(true){
    delay(200);
    uint8_t buttons = lcd.readButtons();
    lcd.setCursor(0,1);
    lcd.print(endingPoint);
    if(buttons & BUTTON_RIGHT){
      endingPoint++;
      if(endingPoint > 16)
        endingPoint = 16;
    }
    if(buttons & BUTTON_LEFT){
      endingPoint--;
      if(endingPoint < 1)
        endingPoint = 1;
    }
    if(buttons & BUTTON_SELECT)
      break;
  }
  
  clearScreen();
  lcd.setCursor(0,0);
  lcd.print("Orientation");
  while(true){
    delay(200);
    uint8_t buttons = lcd.readButtons();
    lcd.setCursor(0,1);
    printOrientation();
    if(buttons & BUTTON_RIGHT){
      myRobot.orientation++;
      if(myRobot.orientation > 3)
        myRobot.orientation = 3;
    }
    if(buttons & BUTTON_LEFT){
      myRobot.orientation--;
      if(myRobot.orientation < 0)
        myRobot.orientation = 0;
    }

    //It will only load the map is DFS was not executed
    if(load)
      loadMap();
    
    if(buttons & BUTTON_SELECT)
      break;    
  }
  clearScreen();
  lcd.setCursor(0,0);
  lcd.print("Exit");
  while(true){
    delay(200);
    uint8_t buttons = lcd.readButtons();
    if(buttons & BUTTON_SELECT)
      break;
  }
}

