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

bool calc::isWithinEpsilonOf(float value, float epsilon, float of) {
	if(value >= (of - epsilon) && value <= (of + epsilon)) {
		return true;
	}
	else {
		return false;
	}
}

float calc::correctFloatErr(float value, float epsilon, float of) {
	if(isWithinEpsilonOf(value, epsilon, of)) {
		return of;
	}
	else {
		return value;
	}
}

Point calc::makeLineFromPolar(float angle, float distance, Point currentPosition) {
  Point temp;
  float xValueDelta = distance * cos(angle);
  xValueDelta = correctFloatErr(xValueDelta, 0.012f, 0.00f);
  temp.x = (currentPosition.x + xValueDelta);
  float yValueDelta = distance * sin(angle);
  yValueDelta = correctFloatErr(yValueDelta, 0.012f, 0.00f);
  temp.y = (currentPosition.y + yValueDelta);
  return temp;
}

bool calc::checkIfVertical(Point start, Point end) {
	if(isWithinEpsilonOf((end.x - start.x), 0.01f, 0.00f)) {
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
  //Serial.println("getDestination!");
  Point nearestWall = robotLine.end;
  /*Serial.print("Nearest wall: ");
  Serial.print(nearestWall.x);
  Serial.print(", ");
  Serial.println(nearestWall.y);*/
  Point validInterceptPoints[(room.numObjects) + 1][room.maxNumObjSides];
  int indexVIP[(room.numObjects) + 1];
  indexVIP[0] = 0;
  for(int i = 0; i < (room.walls.numSides); i++) {
    if(hasInterception(room.walls.sides[i], robotLine)) {
		validInterceptPoints[0][indexVIP[0]] = getInterceptPoint(robotLine, room.walls.sides[i]);
		indexVIP[0]++;
    }
  }
	for(int j = 1; j < ((room.numObjects) + 1); j++) {
		indexVIP[j] = 0;
		for(int k = 0; k < room.objects[j - 1].numSides; k++) {
			if(hasInterception(room.objects[j - 1].sides[k], robotLine)) {
			validInterceptPoints[j][indexVIP[j]] = getInterceptPoint(robotLine, room.objects[j - 1].sides[k]);
			indexVIP[j]++;
			}
		}
	}
  //Determine sign of each translation in x and y, relative to robot
  float diffInXValuesDest = robotLine.end.x - robotLine.start.x;
  diffInXValuesDest = correctFloatErr(diffInXValuesDest, 0.01f, 0.00f);
  float diffInYValuesDest = robotLine.end.y - robotLine.start.y;
  diffInYValuesDest = correctFloatErr(diffInYValuesDest, 0.01f, 0.00f);
  
  float diffInXValuesWall, diffInYValuesWall, distBetweenRobotAndDest, distBetweenRobotAndWall;
  //Get distance between robot and destination
  distBetweenRobotAndDest = getDistBetweenTwoPoints(robotLine.start, nearestWall);
  
  for(int i = 0; i < ((room.numObjects) + 1); i++) {
	for(int j = 0; j < indexVIP[i]; j++) {
		//Determine sign of each translation for given interception point
		diffInXValuesWall = validInterceptPoints[i][j].x - robotLine.start.x;
		diffInXValuesWall = correctFloatErr(diffInXValuesWall, 0.01f, 0.00f);
		diffInYValuesWall = validInterceptPoints[i][j].y - robotLine.start.y;
		diffInYValuesWall = correctFloatErr(diffInYValuesWall, 0.01f, 0.00f);
		//If the signs match, we're facing the right direction
		if(( (diffInXValuesDest > 0.0 && diffInXValuesWall > 0.0) || (diffInXValuesDest == 0.0 && diffInXValuesWall == 0.0) || (diffInXValuesDest < 0.0 && diffInXValuesWall < 0.0) )
	  &&( (diffInYValuesDest > 0.0 && diffInYValuesWall > 0.0) || (diffInYValuesDest == 0.0 && diffInYValuesWall == 0.0) || (diffInYValuesDest < 0.0 && diffInYValuesWall < 0.0) )) {
		  //Get distance between robot and interception point
		  distBetweenRobotAndWall = getDistBetweenTwoPoints(robotLine.start, validInterceptPoints[i][j]);
		  //If distance between robot and interception point is shorter than between robot and destination
		  if(distBetweenRobotAndWall <= distBetweenRobotAndDest) {
			  nearestWall = validInterceptPoints[i][j];
			  distBetweenRobotAndDest = distBetweenRobotAndWall;
			}
		}
	  }
  }
  /*Serial.print("Nearest wall: ");
  Serial.print(nearestWall.x);
  Serial.print(", ");
  Serial.println(nearestWall.y);*/
  return nearestWall;
}

bool calc::hasInterception(Line border, Line robotLine) {
	/*Serial.println("hasInterception!");
	Serial.print("((");
	Serial.print(border.start.x);
	Serial.print(", ");
	Serial.print(border.start.y);
	Serial.print("), (");
	Serial.print(border.end.x);
	Serial.print(", ");
	Serial.print(border.end.y);
	Serial.println("))");*/
	if((robotLine.isVertical && border.isVertical) || (robotLine.m == border.m && (!robotLine.isVertical || !border.isVertical))) { //Lines are parallel
		//Serial.println("Lines are parallel!");
		return false;
	}
	else {
		//Line is in point-slope form (y = line.m(x - line.x) + line.y
		//Convert to slope-intercept form: y = (line.m * x) + (line.m * line.x) + line.y
		//y = line.m * x + ((line.m * line.x) + line.y)
		float xIntercept, yIntercept;
		if(robotLine.isVertical || border.isVertical) {
			if(robotLine.isVertical) {
				/*Serial.println("Robot line is vertical!");
				Serial.print("Border start x: ");
				Serial.println(border.start.x);
				Serial.print("Robot x: ");
				Serial.println(robotLine.start.x);
				Serial.print("Border end x: ");
				Serial.println(border.end.x);*/
				if((border.start.x <= robotLine.start.x && robotLine.start.x <= border.end.x) || (border.start.x >= robotLine.start.x && robotLine.start.x >= border.end.x)) {
					//Serial.println("In range!");
					return true;
				}
				else {
					//Serial.println("Out of range!");
					return false;
				}
				//yIntercept = ((robotLine.m * border.start.x) + robotLine.c);
			}
			if(border.isVertical) {
				/*Serial.println("Wall is vertical!");
				Serial.print("Robot slope: ");
				Serial.println(robotLine.m);*/
				if(border.start.x < robotLine.start.x) {
					yIntercept = (robotLine.start.y + (-(robotLine.m) * abs(robotLine.start.x - border.start.x)));
				}
				else {
					yIntercept = (robotLine.start.y + (robotLine.m * abs(robotLine.start.x - border.start.x)));
				}
				yIntercept = correctFloatErr(yIntercept, 0.01f, 0.00f);
				/*Serial.print("Intercept with vertical line: ");
				Serial.println(yIntercept);*/
				if( ( ( border.start.y < yIntercept || isWithinEpsilonOf(yIntercept, 0.01f, border.start.y) ) && ( yIntercept < border.end.y || isWithinEpsilonOf(yIntercept, 0.01f, border.end.y) ) ) || ( ( border.start.y > yIntercept || isWithinEpsilonOf(yIntercept, 0.01f, border.start.y) ) && ( yIntercept > border.end.y || isWithinEpsilonOf(yIntercept, 0.01f, border.end.y) ) ) ) {
					//Serial.println("In range!");
					return true;
				}
				else {
					//Serial.println("Out of range!");
					return false;
				}
			}
		}
		else {
			/*Serial.println("Neither line is vertical!");
			Serial.print("Numerator: ");
			Serial.println(border.c - robotLine.c);
			Serial.print("Denominator: ");
			Serial.println(robotLine.m - border.m);*/
			xIntercept = ((border.c - robotLine.c) / (robotLine.m - border.m));
			xIntercept = correctFloatErr(xIntercept, 0.01f, 0.00f);
			/*Serial.print("X Intercept: ");
			Serial.println(xIntercept);*/
			if( ( ( border.start.x < xIntercept || isWithinEpsilonOf(xIntercept, 0.01f, border.start.x) ) && ( xIntercept < border.end.x || isWithinEpsilonOf(xIntercept, 0.01f, border.end.x) ) ) || ( ( border.start.x > xIntercept || isWithinEpsilonOf(xIntercept, 0.01f, border.start.x) ) && ( xIntercept > border.end.x || isWithinEpsilonOf(xIntercept, 0.01f, border.end.x) ) ) ) {
				//Serial.println("In range!");
				return true;
			}
			else {
				//Serial.println("Out of range!");
				return false;
			}
		}
	}
}

Point calc::getInterceptPoint(Line robotLine, Line other) {
	//Serial.println("getInterceptPoint!");
	Point intercept;
	/*Serial.print("((");
	Serial.print(other.start.x);
	Serial.print(", ");
	Serial.print(other.start.y);
	Serial.print("), (");
	Serial.print(other.end.x);
	Serial.print(", ");
	Serial.print(other.end.y);
	Serial.println("))");*/
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
	}
	intercept.x = correctFloatErr(intercept.x, 0.01f, 0.00f);
	intercept.y = correctFloatErr(intercept.y, 0.01f, 0.00f);
	/*Serial.print("Intercept point: ");
	Serial.print(intercept.x);
	Serial.print(", ");
	Serial.println(intercept.y);*/
	return intercept;
}
