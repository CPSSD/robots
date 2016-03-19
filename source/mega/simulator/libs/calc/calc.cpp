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
  float xValueDelta = distance * cos(angle);
  if(xValueDelta >= -0.012f && xValueDelta <= 0.012f) {
	  xValueDelta = 0.00;
  }
  float yValueDelta = distance * sin(angle);
  if(yValueDelta >= -0.012f && yValueDelta <= 0.012f) {
	  yValueDelta = 0.00;
  }
  /*Serial.print("Cos(");
  Serial.print(angle);
  Serial.print(") = ");
  Serial.println((cos(angle)));
  Serial.print(distance);
  Serial.print(" * Cos(");
  Serial.print(angle);
  Serial.print(") = ");
  Serial.println(xValueDelta);
  Serial.print(currentPosition.x);
  Serial.print(" + (");
  Serial.print(distance);
  Serial.print(" * Cos(");
  Serial.print(angle);
  Serial.print(")) = ");
  Serial.println(currentPosition.x + xValueDelta);
  Serial.print("Sin(");
  Serial.print(angle);
  Serial.print(") = ");
  Serial.println((sin(angle)));
  Serial.print(distance);
  Serial.print(" * Sin(");
  Serial.print(angle);
  Serial.print(") = ");
  Serial.println(yValueDelta);
  Serial.print(currentPosition.y);
  Serial.print(" + (");
  Serial.print(distance);
  Serial.print(" * Sin(");
  Serial.print(angle);
  Serial.print(")) = ");
  Serial.println(currentPosition.y + yValueDelta);*/
  temp.x2y2 = {(currentPosition.x + xValueDelta), (currentPosition.y + yValueDelta)};
  /*Serial.print(temp.x2y2.x);
  Serial.print(", ");
  Serial.println(temp.x2y2.y);*/
  return temp;
}

struct EquationOfLine calc::getEquationOfLine(MapLine line) {
  EquationOfLine equ;
  equ.xy.x = line.x1y1.x;
  equ.xy.y = line.x1y1.y;
  /*Serial.print(line.x2y2.x);
  Serial.print(" - ");
  Serial.print(equ.xy.x);
  Serial.print(" = ");
  Serial.println(line.x2y2.x - equ.xy.x);*/
  if((line.x2y2.x - equ.xy.x) >= -0.01f && (line.x2y2.x - equ.xy.x) <= 0.01f) {
    equ.isVertical = true;
  }
  else {
    equ.isVertical = false;
  }
  /*Serial.print("isVertical = ");
  Serial.println(equ.isVertical);
  Serial.print(line.x2y2.y);
  Serial.print(" - ");
  Serial.print(equ.xy.y);
  Serial.print(" = ");
  Serial.println(line.x2y2.y - equ.xy.y);
  Serial.print(line.x2y2.x);
  Serial.print(" - ");
  Serial.print(equ.xy.x);
  Serial.print(" = ");
  Serial.println(line.x2y2.x - equ.xy.x);
  Serial.print("(");
  Serial.print(line.x2y2.y - equ.xy.y);
  Serial.print(") / (");
  Serial.print(line.x2y2.x - equ.xy.x);
  Serial.print(") = ");
  Serial.println((line.x2y2.y - equ.xy.y) / (line.x2y2.x - equ.xy.x));*/
  equ.m = (line.x2y2.y - equ.xy.y) / (line.x2y2.x - equ.xy.x); //Warning: this will be NaN if line is vertical (as denominator will be zero). The boolean allows for avoiding this posing a problem
  /*Serial.print(equ.m);
  Serial.print(" * ");
  Serial.print(equ.xy.x);
  Serial.print(" = ");
  Serial.println(equ.m * equ.xy.x);
  Serial.print("(-(");
  Serial.print(equ.m * equ.xy.x);
  Serial.print(")) + ");
  Serial.print(equ.xy.y);
  Serial.print(" = ");
  Serial.println(-(equ.m * equ.xy.x) + equ.xy.y);*/
  equ.c = -(equ.m * equ.xy.x) + equ.xy.y;
  //Serial.println(equ.c);
  return equ;
}

struct point calc::getDestination(struct MapLine moveLine, struct EquationOfLine robotLine, struct EquationOfLine obstacles[]) {
  //For each line, check for valid interception point, get interception point
  point nearestWall = moveLine.x2y2;
  point validInterceptPoints[4];
  int indexVIP = 0;
  for(int i = 0; i < 4; i++) {
    if(hasInterception(obstacles[i], robotLine)) {
		//Serial.println(i);
		validInterceptPoints[indexVIP] = getInterceptPoint(robotLine, obstacles[i]);
		indexVIP++;
    }
  }
  //Determine sign of each translation in x and y, relative to robot
  float diffInXValuesDest = moveLine.x2y2.x - moveLine.x1y1.x;
  if(diffInXValuesDest >= -0.01f && diffInXValuesDest <= 0.01f) {
	  diffInXValuesDest = 0.00;
  }
  /*Serial.print("Sign of translation in x for destination: ");
  Serial.println(diffInXValuesDest);
  Serial.println(moveLine.x2y2.y);
  Serial.println(moveLine.x1y1.y);
  Serial.println(moveLine.x2y2.y - moveLine.x1y1.y);*/
  float diffInYValuesDest = moveLine.x2y2.y - moveLine.x1y1.y;
  if(diffInYValuesDest >= -0.01f && diffInYValuesDest <= 0.01f) {
	  diffInYValuesDest = 0.00;
  }
  //Serial.print("Sign of translation in y for destination: ");
  //Serial.println(diffInYValuesDest);
  
  float diffInXValuesWall, diffInYValuesWall, distBetweenRobotAndDest, distBetweenRobotAndWall;
  //Get distance between robot and destination
  distBetweenRobotAndDest = getDistBetweenTwoPoints(robotLine.xy, nearestWall);
  //Serial.print("Distance to dummy destination: ");
  //Serial.println(distBetweenRobotAndDest);;
  
  for(int i = 0; i < indexVIP; i++) {
	  /*Serial.print("Wall: (");
	  Serial.print(validInterceptPoints[i].x);
	  Serial.print(", ");
	  Serial.print(validInterceptPoints[i].y);
	  Serial.println(")");*/
	//Determine sign of each translation for given interception point
	diffInXValuesWall = validInterceptPoints[i].x - robotLine.xy.x;
	if(diffInXValuesWall >= -0.01f && diffInXValuesWall <= 0.01f) {
		diffInXValuesWall = 0.00;
	}
	//Serial.print("Sign of translation in x for wall: ");
	//Serial.println(diffInXValuesWall);
	diffInYValuesWall = validInterceptPoints[i].y - robotLine.xy.y;
	if(diffInYValuesWall >= -0.01f && diffInYValuesWall <= 0.01f) {
		diffInYValuesWall = 0.00;
	}
	//Serial.print("Sign of translation in y for wall: ");
	//Serial.println(diffInYValuesWall);
	//If the signs match, we're facing the right direction
	if(( (diffInXValuesDest > 0.0 && diffInXValuesWall > 0.0) || (diffInXValuesDest == 0.0 && diffInXValuesWall == 0.0) || (diffInXValuesDest < 0.0 && diffInXValuesWall < 0.0) )
  &&( (diffInYValuesDest > 0.0 && diffInYValuesWall > 0.0) || (diffInYValuesDest == 0.0 && diffInYValuesWall == 0.0) || (diffInYValuesDest < 0.0 && diffInYValuesWall < 0.0) )) {
	  //Get distance between robot and interception point
	  distBetweenRobotAndWall = getDistBetweenTwoPoints(robotLine.xy, validInterceptPoints[i]);
	  //Serial.print("Distance to wall: ");
	  //Serial.println(distBetweenRobotAndWall);
	  //If distance between robot and interception point is shorter than between robot and destination
	  if(distBetweenRobotAndWall <= distBetweenRobotAndDest) {
		  nearestWall = validInterceptPoints[i];
		  distBetweenRobotAndDest = distBetweenRobotAndWall;
		}
	}
  }
  /*Serial.print("Nearest wall: (");
  Serial.print(nearestWall.x);
  Serial.print(", ");
  Serial.print(nearestWall.y);
  Serial.println(")");*/
  return nearestWall;
}

boolean calc::hasInterception(EquationOfLine border, EquationOfLine robotMoveLine) {
	/*Serial.print("Robot is vertical = ");
	Serial.println(robotMoveLine.isVertical);
	Serial.print("Wall is vertical = ");
	Serial.println(border.isVertical);
	Serial.print("Robot slope: ");
	Serial.println(robotMoveLine.m);
	Serial.print("Wall slope: ");
	Serial.println(border.m);
	Serial.print("Slopes equal?: ");
	Serial.println(robotMoveLine.m == border.m);*/
	if((robotMoveLine.isVertical && border.isVertical) || (robotMoveLine.m == border.m && (!robotMoveLine.isVertical || !border.isVertical))) { //Lines are parallel
		//Serial.println("Lines are parallel.");
		return false;
	}
	else {
		//Line is in point-slope form (y = line.m(x - line.x) + line.y
		//Convert to slope-intercept form: y = (line.m * x) + (line.m * line.x) + line.y
		//y = line.m * x + ((line.m * line.x) + line.y)
		/*Serial.print("Wall's x-coordinate: ");
		Serial.println(border.xy.x);
		Serial.print("Robot's y-coordinate: ");
		Serial.println(robotMoveLine.xy.y);
		Serial.print(robotMoveLine.m);
		Serial.print(" * ");
		Serial.print(border.xy.x);
		Serial.print(" = ");
		Serial.println(robotMoveLine.m * border.xy.x);
		Serial.print(robotMoveLine.m * border.xy.x);
		Serial.print(" + ");
		Serial.print(robotMoveLine.xy.y);
		Serial.print(" = ");
		Serial.println((robotMoveLine.m * border.xy.x) + robotMoveLine.xy.y);
		Serial.print("(");
		Serial.print(robotMoveLine.m * border.xy.x);
		Serial.print(") + ((");
		Serial.print(robotMoveLine.m * border.xy.x);
		Serial.print(") + ");
		Serial.print(robotMoveLine.xy.y);
		Serial.print(") = ");
		Serial.println((robotMoveLine.m * border.xy.x) + ((-(robotMoveLine.m * robotMoveLine.xy.x)) + robotMoveLine.xy.y));*/
		float intercept = ((robotMoveLine.m * border.xy.x) + ((-(robotMoveLine.m * robotMoveLine.xy.x)) + robotMoveLine.xy.y));
		//Serial.print("Intercept: ");
		//Serial.println(intercept);
		if(robotMoveLine.isVertical || border.isVertical) {
			if(robotMoveLine.isVertical) {
				//Serial.println("Since wall is not vertical, the robot's line must intersect the wall somewhere (if not necessarily at a sane location).");
				return true;
			}
			if(border.isVertical) {
				//Serial.print("Wall is vertical, and the robot's line is not, ");
				if(intercept >= 0.0 && intercept <= 300.0) {
					//Serial.println("and the intercept point is in a sane space.");
					return true;
				}
				else {
					//Serial.println("but the intercept point is out of bounds.");
					return false;
				}
			}
		}
		else {
			//Serial.println("Neither line is vertical.");
			return true;
		}
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
	/*Serial.print("Robot slope: ");
	Serial.println(robotLine.m);
	Serial.print("Wall slope: ");
	Serial.println(other.m);
	Serial.print(robotLine.m);
	Serial.print(" - ");
	Serial.print(other.m);
	Serial.print(" = ");
	Serial.println(robotLine.m - other.m);*/
    float denominator = robotLine.m - other.m;
	/*Serial.print("Wall's c: ");
	Serial.println(other.c);
	Serial.print("Robot's c: ");
	Serial.println(robotLine.c);
	Serial.print(other.c);
	Serial.print(" - ");
	Serial.print(robotLine.c);
	Serial.print(" = ");
	Serial.println(other.c - robotLine.c);*/
    float numerator = other.c - robotLine.c;
	/*Serial.print(numerator);
	Serial.print(" / ");
	Serial.print(denominator);
	Serial.print(" = ");
	Serial.println(numerator / denominator);*/
    intercept.x = numerator / denominator;
    //Substitute the x-value into either equation of the lines and solve for y
	/*Serial.print(robotLine.m);
	Serial.print(" * ");
	Serial.print(intercept.x);
	Serial.print(" = ");
	Serial.println(robotLine.m * intercept.x);
	Serial.print("(");
	Serial.print(robotLine.m * intercept.x);
	Serial.print(") + ");
	Serial.print(robotLine.c);
	Serial.print(" = ");
	Serial.println((robotLine.m * intercept.x) + robotLine.c);*/
    intercept.y = (robotLine.m * intercept.x) + robotLine.c;
	if(intercept.y >= -0.01f && intercept.y <= 0.01f) {
		intercept.y = 0.00;
	}
  }
  /*Serial.print("Intercept point: (");
  Serial.print(intercept.x);
  Serial.print(", ");
  Serial.print(intercept.y);
  Serial.println(")");*/
  return intercept;
}
