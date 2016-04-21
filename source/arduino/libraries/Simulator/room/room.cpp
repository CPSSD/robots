#include "room.h"
#include "calc.h";
#include "Arduino.h";

Point::Point() {
	this->x = 0.0;
	this->y = 0.0;
}

Point::Point(int x, int y) {
	this->x = (float)x;
	this->y = (float)y;
}

Line::Line() {
	this->start = Point(0, 0);
	this->end = Point(0, 0);
	this->isVertical = false;
	this->m = 0.0;
	this->c = 0.0;
}

Line::Line(Point start, Point end) {
	this->start = start;
	this->end = end;
	this->isVertical = calc::checkIfVertical(this->start, this->end);
	this->m = calc::getSlopeOfLine(this->start, this->end);
	this->c = calc::getCOfLine(this->m, this->start);
}

Object::Object() {
	this->numSides = 0;
	this->sides = 0;
}

Object::Object(int numSides, Point sides[]) {
	this->numSides = numSides;
	this->sides = new Line[numSides];
	for(int i = 0; i < (this->numSides - 1); i++) {
		this->sides[i] = Line(sides[i], sides[i + 1]);
	}
	this->sides[(numSides - 1)] = Line(sides[(numSides - 1)], sides[0]);
}

Room::Room() {
	Point defaultWalls[] = { {Point(0, 0)}, {Point(300, 0)}, {Point(300, 300)}, {Point(0, 300)} };
	this->walls = Object(4, defaultWalls);
	this->robot = Point(150, 150);
	this->numObjects = 0;
	this->objects = new Object[this->numObjects];
	this->maxNumObjSides = 4;
}

Room::Room(int numWalls, Point walls[], Point robotCoOrds, int numObjects) {
	this->walls = Object(numWalls, &walls[0]);
	this->robot = robotCoOrds;
	this->numObjects = numObjects;
	this->objects = new Object[numObjects];
	this->maxNumObjSides = numWalls;
}

bool Room::addObject(int index, int numSides, Point sides[]) {
	if(index >= this->numObjects) {
		return false;
	}
	else {
		this->objects[index] = Object(numSides, sides);
		this->maxNumObjSides = max(maxNumObjSides, numSides);
		return true;
	}
}
