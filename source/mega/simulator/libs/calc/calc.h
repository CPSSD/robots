#ifndef calc_h
#define calc_h

#include "room.h"

class Room;
class Point;
class Line;
class Object;

class calc {
	public:
		float getDistBetweenTwoPoints(Point p1, Point p2);
		float getTravelTime(unsigned long distance, float speed);
		float getDistanceTravelled(float speed, unsigned long time);
		Point makeLineFromPolar(float angle, float distance, Point currentPosition);
		static bool checkIfVertical(Point start, Point end);
		static float getSlopeOfLine(Point start, Point end);
		static float getCOfLine(float slope, Point start);
		Point getDestination(Line robotLine, Room room);
		bool hasInterception(Line border, Line robotLine);
		Point getInterceptPoint(Line robotLine, Line other);
};

#endif
