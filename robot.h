#include "MyServos.h"
#include "MySharpSensor.h"
#include "color.h"

String visitedLocations = "OOOOOOOOOOOOOOOO";

struct robot{
  int orientation = 0;  //0(west), 1(north), 2(east), 3(south)
  byte cellLocation = 1;
  byte x;
  byte y;
};

struct cell{
  bool visited = false;   //Tells if a cell has been visited
  bool vistedBFS = false; //Tells if a cell was visited in the BFS search
  byte westWall = 2;    //2 for unknown, 0 for no wall, 1 for wall
  byte northWall = 2;        //CHANGE THIS LATER     EWHTR98ECYNR8TERYSDG8SNVRwfnfsuhewuhwsieofjsodifp
  byte eastWall = 2;
  byte southWall = 2;
  byte cellNumber;  //Tells which cell number it is
  byte prev; //Tells which cell is before it in BFS
};

struct cell cells[4][4];
struct cell bfs[17];
struct robot myRobot;

//Makes sure that everything around the robot has been checked. This only ever needs to happen in DFS and only happens in the first cell of DFS
//Since it doesn't know what is behind it
bool checkAllSide()
{
  if(cells[myRobot.x][myRobot.y].westWall != 2 && cells[myRobot.x][myRobot.y].northWall != 2 && cells[myRobot.x][myRobot.y].eastWall != 2 && cells[myRobot.x][myRobot.y].southWall != 2)
    return true;
  else
    return false; 
}

//Turns the robot 90 degrees to the right
void turnRobot()
{
  myRobot.orientation++;    //Changes the orientation value
  if(myRobot.orientation > 3)
    myRobot.orientation = 0;

  resetCounts();
  while(leftTicks < 14)     //Turns almost 90 degrees
    setSpeedsvw(0, -1);
  setSpeedsIPS(0, 0);
}

//Updates all information about the current cell
void updateCurrentCell()
{
  int left = 1, front = 1, right = 1;
  int i;

  //Marks the cell as visited
  cells[myRobot.x][myRobot.y].visited = true;

  //Done to get wall measurements to be able to figure out if there is a wall
  for(i = 0; i < 5; i++){
    takeNewMeasurement(SLEFT);
    takeNewMeasurement(SRIGHT);
    takeNewMeasurement(SLONG);
    delay(60);
  }
  
  //Checks if there is a wall
  if(getCombinedDistance(SLEFT) > 11)
    left = 0;
  if(getCombinedDistance(SLONG) > 18)
    front = 0;
  if(getCombinedDistance(SRIGHT) > 11)
    right = 0;


  //Places the walls in the correct place depending on orientation
  if(myRobot.orientation == 1){
    cells[myRobot.x][myRobot.y].westWall = left;
    cells[myRobot.x][myRobot.y].northWall = front;
    cells[myRobot.x][myRobot.y].eastWall = right;
  }
  else if(myRobot.orientation == 2){
    cells[myRobot.x][myRobot.y].northWall = left;
    cells[myRobot.x][myRobot.y].eastWall = front;
    cells[myRobot.x][myRobot.y].southWall = right;
  }
  else if(myRobot.orientation == 3){
    cells[myRobot.x][myRobot.y].eastWall = left;
    cells[myRobot.x][myRobot.y].southWall = front;
    cells[myRobot.x][myRobot.y].westWall = right;
  }
  else{
    cells[myRobot.x][myRobot.y].southWall = left;
    cells[myRobot.x][myRobot.y].westWall = front;
    cells[myRobot.x][myRobot.y].northWall = right;
  }
  
}

//Since the robot can't do perfect 90 degree turns
//I created this for it to do small adjustments if it is too close to a wall
void adjustment()
{
  //This is the area where you might have to edit stuff. If it is closer to a wall, I make it turn more than normally.
  //If you figure out something better go for it. I've messed around with different
  //values to try and figure something good out but it's possible this may not work all time. I have tried it and it has worked an amount of the time.
  //There's more information on the next function to what could possibly be changed to make it work better.

  //For the further distance, I found that adjusting by one tick works best since if it is greater it usually messes up because of the front sensor.
  //When it's closer, I make it move by 2 just to make it get away from the wall as fast as possible. This does cause problems with the way the robot is facing though.
  //If it adjusts too much, it won't be facing forward anymore. If you find a better way to make the robot follow walls go for it

  if(getCombinedDistance(SLEFT) < 2.5)
  {
    resetCounts();
    while(leftTicks < 2)
      setSpeedsvw(0, -1);
  }
  else if(getCombinedDistance(SLEFT) < 5.75)
  {
    resetCounts();
    while(leftTicks < 1)
      setSpeedsvw(0, -1);
  }
  if(getCombinedDistance(SRIGHT) < 2.5){
    resetCounts();
    while(leftTicks < 2){
      setSpeedsvw(0,1);
    }
  }
  else if(getCombinedDistance(SRIGHT) < 5.75)
  {
    resetCounts();
    while(leftTicks < 1)
      setSpeedsvw(0, 1);
  }
  setSpeedsIPS(0,0);
}

//Goes to the middle of a cell
void travelToMid()
{
  //I've split traveling to the middle of a cell to multiple parts so that it will adjust while traveling to the middle. In total, the numbers on the otherside should add up to 36 since
  //It was originally leftTicks < 36. Now the numbers are 14, 10, and 12, which add up to 36. You can split it up more or change the numbers around, or try something completely
  //different. 
  resetCounts();
  while(leftTicks < 14){
    takeNewMeasurement(SLEFT);
    takeNewMeasurement(SRIGHT);
    setSpeedsIPS(4,4);
    getColor();
  }
  adjustment();
  resetCounts();
  while(leftTicks < 10)
  {
    takeNewMeasurement(SLEFT);
    takeNewMeasurement(SRIGHT);
    setSpeedsIPS(4, 4);
    getColor();
  }
  adjustment();
  resetCounts();
  while(leftTicks < 12){
    takeNewMeasurement(SLEFT);
    takeNewMeasurement(SRIGHT);
    setSpeedsIPS(4, 4);
    getColor();
  }
  adjustment();
  setSpeedsIPS(0, 0);
}

//Updates  the LCD screen. You won't have to change anything here
void updateDisplay()
{
  visitedLocations[myRobot.cellLocation - 1] = 'X';
  lcd.setCursor(0, 1);
  lcd.print("               ");
  lcd.setCursor(3, 1);
  lcd.print("O");
  
  if(myRobot.orientation == 0)
    lcd.print('W');
  else if(myRobot.orientation == 1)
    lcd.print('N');
  else if(myRobot.orientation == 2)
    lcd.print('E');
  else
    lcd.print('S');

  lcd.setCursor(0, 1);
  lcd.print("G");
  lcd.print(myRobot.cellLocation);
  lcd.setCursor(0, 0);
  lcd.print("                ");
  lcd.setCursor(0, 0);
  lcd.print(visitedLocations);
  
}

//Checks is all borders of cell around are visited or walls
bool allWallsOrVisited()
{
  if((cells[myRobot.x][myRobot.y].westWall == 1 || cells[myRobot.x][myRobot.y - 1].visited == true) && (cells[myRobot.x][myRobot.y].northWall == 1 || cells[myRobot.x - 1][myRobot.y].visited == true) &&
    (cells[myRobot.x][myRobot.y].eastWall == 1 || cells[myRobot.x][myRobot.y + 1].visited == true) && (cells[myRobot.x][myRobot.y].southWall == 1 || cells[myRobot.x + 1][myRobot.y].visited == true)){
      return true;
    }
  else
    return false;
     
}

//Clears the screen. Helpful for removing everything from the screen before printing something new out
void clearScreen()
{
  lcd.setCursor(0,0);
  lcd.print("                ");
  lcd.setCursor(0,1);
  lcd.print("                ");
}

//Sets the robot's coordinates
void setRobotCoordinates()
{
  int i, j;

  for(i = 0; i < 4; i++)
  {
    for(j = 0; i < 4; j++)
    {
      if(cells[i][j].cellNumber == myRobot.cellLocation)
      {
        myRobot.x = i;
        myRobot.y = j;
        return;
      }
    }
  }
}

//This is used to do adjustments while the robot is traveling to another cell. If you find a better way you can remove this. You can also
//add any other functions you want that can make the robot move better
void doOne(){
  if(leftTicks > 12){
    resetCounts();
    adjustment();
  }
}

void printOrientation(){
  if(myRobot.orientation == 0)
    lcd.print('W');
  else if(myRobot.orientation == 1)
    lcd.print('N');
  else if(myRobot.orientation == 2)
    lcd.print('E');
  else
    lcd.print('S');
}

void fillBFS()
{ 
  bfs[1] = cells[0][0];
  bfs[2] = cells[0][1];
  bfs[3] = cells[0][2];
  bfs[4] = cells[0][3];
  bfs[5] = cells[1][0];
  bfs[6] = cells[1][1];
  bfs[7] = cells[1][2];
  bfs[8] = cells[1][3];
  bfs[9] = cells[2][0];
  bfs[10] = cells[2][1];
  bfs[11] = cells[2][2];
  bfs[12] = cells[2][3];
  bfs[13] = cells[3][0];
  bfs[14] = cells[3][1];
  bfs[15] = cells[3][2];
  bfs[16] = cells[3][3];
}

//This will be used to load a map if we need to during the lab and so that
//you can test BFS
void loadMap()
{
  //1
  cells[0][0].westWall = 1;
  cells[0][0].northWall = 1;
  cells[0][0].eastWall = 0;
  cells[0][0].southWall = 0;
  //2
  cells[0][1].westWall = 0;
  cells[0][1].northWall = 1;
  cells[0][1].eastWall = 0;
  cells[0][1].southWall = 1;
  //3
  cells[0][2].westWall = 0;
  cells[0][2].northWall = 1;
  cells[0][2].eastWall = 0;
  cells[0][2].southWall = 0;
  //4
  cells[0][3].westWall = 0;
  cells[0][3].northWall = 1;
  cells[0][3].eastWall = 1;
  cells[0][3].southWall = 0;
  //5
  cells[1][0].westWall = 1;
  cells[1][0].northWall = 0;
  cells[1][0].eastWall = 0;
  cells[1][0].southWall = 0;
  //6
  cells[1][1].westWall = 0;
  cells[1][1].northWall = 1;
  cells[1][1].eastWall = 1;
  cells[1][1].southWall = 1;
  //7
  cells[1][2].westWall = 1;
  cells[1][2].northWall = 0;
  cells[1][2].eastWall = 1;
  cells[1][2].southWall = 0;
  //8
  cells[1][3].westWall = 1;
  cells[1][3].northWall = 0;
  cells[1][3].eastWall = 1;
  cells[1][3].southWall = 0;
  //9
  cells[2][0].westWall = 1;
  cells[2][0].northWall = 0;
  cells[2][0].eastWall = 1;
  cells[2][0].southWall = 0;
  //10
  cells[2][1].westWall = 1;
  cells[2][1].northWall = 1;
  cells[2][1].eastWall = 0;
  cells[2][1].southWall = 0;
  //11
  cells[2][2].westWall = 0;
  cells[2][2].northWall = 0;
  cells[2][2].eastWall = 1;
  cells[2][2].southWall = 1;
  //12
  cells[2][3].westWall = 1;
  cells[2][3].northWall = 0;
  cells[2][3].eastWall = 1;
  cells[2][3].southWall = 0;
  //13
  cells[3][0].westWall = 1;
  cells[3][0].northWall = 0;
  cells[3][0].eastWall = 1;
  cells[3][0].southWall = 1;
  //14
  cells[3][1].westWall = 1;
  cells[3][1].northWall = 0;
  cells[3][1].eastWall = 0;
  cells[3][1].southWall = 1;
  //15
  cells[3][2].westWall = 0;
  cells[3][2].northWall = 1;
  cells[3][2].eastWall = 0;
  cells[3][2].southWall = 1;
  //16
  cells[3][3].westWall = 0;
  cells[3][3].northWall = 0;
  cells[3][3].eastWall = 1;
  cells[3][3].southWall = 1;
  
}

