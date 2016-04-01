#include "room.h"
#include "calc.h";

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

Room::Room(int numWalls, Point walls[], Point robotCoOrds, int numObjects) {
	this->walls = Object(numWalls, &walls[0]);
	this->robot = robotCoOrds;
	this->numObjects = numObjects;
	this->objects = new Object[numObjects];
}

bool Room::addObject(int index, int numSides, Point sides[]) {
	if(index >= this->numObjects) {
		return false;
	}
	else {
		this->objects[index] = Object(numSides, sides);
		return true;
	}
}
