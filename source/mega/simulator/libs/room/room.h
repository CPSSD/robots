#ifndef room_h
#define room_h

#include "calc.h"

class Point {
	public:
		Point();
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
	public:
		Room(int numWalls, Point walls[], Point robotCoOrds, int numObjects);
		Point robot;
		Object walls;
		int numObjects, totalNumObjSides;
		Object* objects;
		bool addObject(int index, int numSides, Point sides[]);
};

#endif
