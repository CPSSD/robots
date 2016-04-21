#include "roomTestCases.h"

Room roomTestCases::pickRoom(int testCaseNum) {
	if (testCaseNum == 1) {
		STARTING_X = 200;
		STARTING_Y = 200;
		MAP_BOUNDS = new Point[4]{Point(0, 0), Point(600, 0), Point(600, 600), Point(0, 600)};
		room = Room(4, MAP_BOUNDS, Point(STARTING_X, STARTING_Y), 1);
		delete MAP_BOUNDS;
		MAP_BOUNDS = 0;
		newObjHolder = new Point[4]{Point(260, 260), Point(340, 260), Point(340, 340), Point(260, 340)};
		room.addObject(4, newObjHolder);
		delete newObjHolder;
		newObjHolder = 0;
		return room;
	} else if (testCaseNum == 2) {
		STARTING_X = 300;
		STARTING_Y = 200;
		MAP_BOUNDS = new Point[4]{Point(0, 0), Point(600, 0), Point(600, 600), Point(0, 600)};
		room = Room(4, MAP_BOUNDS, Point(STARTING_X, STARTING_Y), 2);
		delete MAP_BOUNDS;
		MAP_BOUNDS = 0;
		newObjHolder = new Point[4]{Point(215, 260), Point(295, 260), Point(295, 340), Point(215, 340)};
		room.addObject(4, newObjHolder);
		delete newObjHolder;
		newObjHolder = new Point[4]{Point(305, 260), Point(385, 260), Point(385, 340), Point(305, 340)};
		room.addObject(4, newObjHolder);
		delete newObjHolder;
		newObjHolder = 0;
		return room;
	} else {
		STARTING_X = 150;
		STARTING_Y = 150;
		MAP_BOUNDS = new Point[4]{Point(0, 0), Point(300, 0), Point(300, 300), Point(0, 300)};
		room = Room(4, MAP_BOUNDS, Point(STARTING_X, STARTING_Y), 0);
		delete MAP_BOUNDS;
		MAP_BOUNDS = 0;
		return room;
	}
}
