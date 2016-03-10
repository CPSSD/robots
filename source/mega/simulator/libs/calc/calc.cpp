#include "Arduino.h"
#include "calc.h"
#include "math.h"
#include "structs.h"

float calc::getDistBetweenTwoPoints(point p1, point p2) {
  return sqrt((pow(p2.x - p1.x, 2)) + (pow(p2.y - p1.y, 2)));
}

float calc::getTravelTime(unsigned long distance, float speed) {
	return distance / speed;
}

float calc::getDistanceTravelled(float speed, unsigned long time) {
	return speed * time;
}

struct MapLine calc::makeLineFromPolar(float angle, float distance, point currentPosition) {
  MapLine temp;
  temp.x1y1 = {currentPosition.x, currentPosition.y};
  temp.x2y2 = {(currentPosition.x + (distance * cos(angle))), (currentPosition.y + (distance * sin(angle)))};
  return temp;
}

struct EquationOfLine calc::getEquationOfLine(MapLine line) {
  EquationOfLine equ;
  equ.xy.x = line.x1y1.x;
  equ.xy.y = line.x1y1.y;
  if((line.x2y2.x - equ.xy.x) == 0.00) {
    equ.isVertical = true;
  }
  else {
    equ.isVertical = false;
  }
  equ.m = (line.x2y2.y - equ.xy.y) / (line.x2y2.x - equ.xy.x); //Warning: this will be NaN if line is vertical (as denominator will be zero). The boolean allows for avoiding this posing a problem
  equ.c = -(equ.m * equ.xy.x) + equ.xy.y;
  return equ;
}

struct point calc::getDestination(struct MapLine moveLine, struct EquationOfLine robotLine, struct EquationOfLine obstacles[]) {
  //For each line, check for valid interception point, get interception point
  point nearestWall = moveLine.x2y2;
  point validInterceptPoints[4];
  int indexVIP = 0;
  for(int i = 0; i < 4; i++) {
    if(hasInterception(obstacles[i], robotLine)) {
      //Serial.println("Has intercept point");
      validInterceptPoints[indexVIP] = getInterceptPoint(robotLine, obstacles[i]);
      indexVIP++;
    }
  }
  //Determine sign of each translation in x and y, relative to robot
  float diffInXValuesDest = moveLine.x2y2.x - moveLine.x1y1.x;
  float diffInYValuesDest = moveLine.x2y2.y - moveLine.x1y1.y;
  
  float diffInXValuesWall, diffInYValuesWall, distBetweenRobotAndDest, distBetweenRobotAndWall;
  //Get distance between robot and destination
  distBetweenRobotAndDest = getDistBetweenTwoPoints(robotLine.xy, nearestWall);
  
  for(int i = 0; i <= indexVIP; i++) {
	//Determine sign of each translation for given interception point
	diffInXValuesWall = validInterceptPoints[i].x - robotLine.xy.x;
	diffInYValuesWall = validInterceptPoints[i].y - robotLine.xy.y;
	//If the signs match, we're facing the right direction
	if(( (diffInXValuesDest > 0.0 && diffInXValuesWall > 0.0) || (diffInXValuesDest == 0.0 && diffInXValuesWall == 0.0) || (diffInXValuesDest < 0.0 && diffInXValuesWall < 0.0) )
  &&( (diffInYValuesDest > 0.0 && diffInYValuesWall > 0.0) || (diffInYValuesDest == 0.0 && diffInYValuesWall == 0.0) || (diffInYValuesDest < 0.0 && diffInYValuesWall < 0.0) )) {
	  //Get distance between robot and interception point
	  distBetweenRobotAndWall = getDistBetweenTwoPoints(robotLine.xy, validInterceptPoints[i]);
	  //If distance between robot and interception point is shorter than between robot and destination
	  if(distBetweenRobotAndWall < distBetweenRobotAndDest) {
		  nearestWall = validInterceptPoints[i];
		  distBetweenRobotAndDest = distBetweenRobotAndWall;
		}
	}
  }
    
  /*//Do the same for the first point
  float diffInXValuesWallOne = validInterceptPoints[0].x - robotLine.xy.x;
  float diffInYValuesWallOne = validInterceptPoints[0].y - robotLine.xy.y;
  //If the signs match, that's the point in consideration, else it's the other one
  if(( (diffInXValuesDest > 0.0 && diffInXValuesWallOne > 0.0) || (diffInXValuesDest == 0.0 && diffInXValuesWallOne == 0.0) || (diffInXValuesDest < 0.0 && diffInXValuesWallOne < 0.0) )
  &&( (diffInYValuesDest > 0.0 && diffInYValuesWallOne > 0.0) || (diffInYValuesDest == 0.0 && diffInYValuesWallOne == 0.0) || (diffInYValuesDest < 0.0 && diffInYValuesWallOne < 0.0) )) {
    nearestWall = validInterceptPoints[0];
  }
  else {
    nearestWall = validInterceptPoints[1];
  }
  //Get distance between robot and destination, and robot and wall
  float distBetweenRobotAndDest = getDistBetweenTwoPoints(moveLine.x1y1, moveLine.x2y2);
  float distBetweenRobotAndNearWall = getDistBetweenTwoPoints(robotLine.xy, nearestWall);
  //If distance between robot and destination is shorter than between robot and wall, return destination, else return interception point
  if(distBetweenRobotAndDest < distBetweenRobotAndNearWall) {
    return moveLine.x2y2;
  }*/
  return nearestWall;
}

boolean calc::hasInterception(EquationOfLine border, EquationOfLine robotMoveLine) {
  if((robotMoveLine.isVertical && border.isVertical) || (robotMoveLine.m == border.m && (!robotMoveLine.isVertical || !border.isVertical))) { //Lines are parallel
    return false;
  }
  else if(robotMoveLine.isVertical || border.isVertical) {
    if(robotMoveLine.isVertical) {
      return true;
    }
    if(border.isVertical) {
      //Line is in point-slope form (y = line.m(x - line.x) + line.y
      //Convert to slope-intercept form: y = (line.m * x) + (line.m * line.x) + line.y
      //y = line.m * x + ((line.m * line.x) + line.y)
      float intercept = ((robotMoveLine.m * border.xy.x) + ((robotMoveLine.m * robotMoveLine.xy.x) + robotMoveLine.xy.y));
      if(intercept >= 0.0 && intercept <= 300.0) {
        return true;
      }
      else {
        return false;
      }
    }
  }
  else {
    return true;
  }
}

struct point calc::getInterceptPoint(EquationOfLine robotLine, EquationOfLine other) {
  point intercept;
  //Special case: one of the lines is vertical.
  //In this event, the x-value of the intercept point will be constant for all values of y.
  //Therefore, we can substitute that value into the equation of the non-vertical line and solve for y.
  if(other.isVertical) {
    intercept.x = other.xy.x;
    intercept.y = (robotLine.m * intercept.x) + robotLine.c;
  }
  else if(robotLine.isVertical) {
    intercept.x = robotLine.xy.x;
    intercept.y = (other.m * intercept.x) + other.c;
  }
  else {
    //We set the two equations equal to each other and manipulate the expression to solve for x.
    // m1x + c1 = m2x + c2 -> m1x - m2x = c2 - c1 -> x = (c2 - c1) / (m1 - m2)
    float denominator = robotLine.m - other.m;
    float numerator = other.c - robotLine.c;
    intercept.x = numerator / denominator;
    //Substitute the x-value into either equation of the lines and solve for y
    intercept.y = (robotLine.m * intercept.x) + robotLine.c;
  }
  return intercept;
}