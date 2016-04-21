#ifndef room_h
#define room_h

#include "calc.h"

class Point {
	public:
		Point();
		Point(float x, float y);
		Point(int x, int y);
		float x, y;
};

class Line {
	public:
		Line();
		Line(Point start, Point end);
		Point start, end;
		float m, c;
		bool isVertical;
};

class Object {
	public:
		Object();
		Object(int numSides, Point sides[]);
		int numSides;
		Line* sides;
};

class Room {
	int index;
	public:
		Room();
		Room(int numWalls, Point walls[], Point robotCoOrds, int numObjects);
		Point robot, farthest;
		Object walls;
		int numObjects, maxNumObjSides;
		Object* objects;
		bool addObject(int numSides, Point sides[]);
};

#endif
