#include "Arduino.h"
#include "calc.h"
#include "math.h"
#include "room.h"

float calc::getDistBetweenTwoPoints(Point p1, Point p2) {
  return sqrt((pow(p2.x - p1.x, 2)) + (pow(p2.y - p1.y, 2)));
}

float calc::getTravelTime(unsigned long distance, float speed) {
	return distance / speed;
}

float calc::getDistanceTravelled(float speed, unsigned long time) {
	return speed * time;
}

Point calc::makeLineFromPolar(float angle, float distance, Point currentPosition) {
  Point temp;
  float xValueDelta = distance * cos(angle);
  if(xValueDelta >= -0.012f && xValueDelta <= 0.012f) {
	  xValueDelta = 0.00;
  }
  temp.x = (currentPosition.x + xValueDelta);
  float yValueDelta = distance * sin(angle);
  if(yValueDelta >= -0.012f && yValueDelta <= 0.012f) {
	  yValueDelta = 0.00;
  }
  temp.y = (currentPosition.y + yValueDelta);
  return temp;
}

bool calc::checkIfVertical(Point start, Point end) {
	if((end.x - start.x) >= -0.01f && (end.x - start.x) <= 0.01f) {
		return true;
	}
	else {
		return false;
	}
}

float calc::getSlopeOfLine(Point start, Point end) {
	return((end.y - start.y) / (end.x - start.x));
}

float calc::getCOfLine(float slope, Point start) {
	return(-(slope * start.x) + start.y);
}

Point calc::getDestination(Line robotLine, Room room) {
  //For each line, check for valid interception point, get interception point
  Point nearestWall = robotLine.end;
  Point validInterceptPoints[room.walls.numSides];
  int indexVIP = 0;
  for(int i = 0; i < (room.walls.numSides); i++) {
    if(hasInterception(room.walls.sides[i], robotLine)) {
		//Serial.println(i);
		validInterceptPoints[indexVIP] = getInterceptPoint(robotLine, room.walls.sides[i]);
		indexVIP++;
    }
  }
  //Determine sign of each translation in x and y, relative to robot
  float diffInXValuesDest = robotLine.end.x - robotLine.start.x;
  if(diffInXValuesDest >= -0.01f && diffInXValuesDest <= 0.01f) {
	  diffInXValuesDest = 0.00;
  }
  float diffInYValuesDest = robotLine.end.y - robotLine.start.y;
  if(diffInYValuesDest >= -0.01f && diffInYValuesDest <= 0.01f) {
	  diffInYValuesDest = 0.00;
  }
  
  float diffInXValuesWall, diffInYValuesWall, distBetweenRobotAndDest, distBetweenRobotAndWall;
  //Get distance between robot and destination
  distBetweenRobotAndDest = getDistBetweenTwoPoints(robotLine.start, nearestWall);
  
  for(int i = 0; i < indexVIP; i++) {
	//Determine sign of each translation for given interception point
	diffInXValuesWall = validInterceptPoints[i].x - robotLine.start.x;
	if(diffInXValuesWall >= -0.01f && diffInXValuesWall <= 0.01f) {
		diffInXValuesWall = 0.00;
	}
	diffInYValuesWall = validInterceptPoints[i].y - robotLine.start.y;
	if(diffInYValuesWall >= -0.01f && diffInYValuesWall <= 0.01f) {
		diffInYValuesWall = 0.00;
	}
	//If the signs match, we're facing the right direction
	if(( (diffInXValuesDest > 0.0 && diffInXValuesWall > 0.0) || (diffInXValuesDest == 0.0 && diffInXValuesWall == 0.0) || (diffInXValuesDest < 0.0 && diffInXValuesWall < 0.0) )
  &&( (diffInYValuesDest > 0.0 && diffInYValuesWall > 0.0) || (diffInYValuesDest == 0.0 && diffInYValuesWall == 0.0) || (diffInYValuesDest < 0.0 && diffInYValuesWall < 0.0) )) {
	  //Get distance between robot and interception point
	  distBetweenRobotAndWall = getDistBetweenTwoPoints(robotLine.start, validInterceptPoints[i]);
	  //If distance between robot and interception point is shorter than between robot and destination
	  if(distBetweenRobotAndWall <= distBetweenRobotAndDest) {
		  nearestWall = validInterceptPoints[i];
		  distBetweenRobotAndDest = distBetweenRobotAndWall;
		}
	}
  }
  return nearestWall;
}

bool calc::hasInterception(Line border, Line robotLine) {
	if((robotLine.isVertical && border.isVertical) || (robotLine.m == border.m && (!robotLine.isVertical || !border.isVertical))) { //Lines are parallel
		return false;
	}
	else {
		//Line is in point-slope form (y = line.m(x - line.x) + line.y
		//Convert to slope-intercept form: y = (line.m * x) + (line.m * line.x) + line.y
		//y = line.m * x + ((line.m * line.x) + line.y)
		float intercept = ((robotLine.m * border.start.x) + ((-(robotLine.m * robotLine.start.x)) + robotLine.start.y));
		if(robotLine.isVertical || border.isVertical) {
			if(robotLine.isVertical) {
				return true;
			}
			if(border.isVertical) {
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
}

Point calc::getInterceptPoint(Line robotLine, Line other) {
  Point intercept;
  //Special case: one of the lines is vertical.
  //In this event, the x-value of the intercept point will be constant for all values of y.
  //Therefore, we can substitute that value into the equation of the non-vertical line and solve for y.
  if(other.isVertical) {
    intercept.x = other.start.x;
    intercept.y = (robotLine.m * intercept.x) + robotLine.c;
  }
  else if(robotLine.isVertical) {
    intercept.x = robotLine.start.x;
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
	if(intercept.y >= -0.01f && intercept.y <= 0.01f) {
		intercept.y = 0.00;
	}
  }
  return intercept;
}
