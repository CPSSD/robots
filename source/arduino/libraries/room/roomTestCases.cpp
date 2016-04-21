#include "roomTestCases.h"

Room roomTestCases::pickRoom(int testCaseNum){
	if (testCaseNum == 1){
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
	} else if (testCaseNum == 2){
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
	} else if (testCaseNum == 3){
		STARTING_X = 100;
		STARTING_Y = 100;
		MAP_BOUNDS = new Point[6]{Point(0, 0), Point(600, 0), Point(600, 600), Point(300, 600), Point(300, 300), Point(0, 300)};
		room = Room(6, MAP_BOUNDS, Point(STARTING_X, STARTING_Y), 2);
		delete MAP_BOUNDS;
		MAP_BOUNDS = 0;
		newObjHolder = new Point[4]{Point(125, 125), Point(175, 125), Point(175, 175), Point(125, 175)};
		room.addObject(4, newObjHolder);
		delete newObjHolder;
		newObjHolder = new Point[3]{Point(450, 350), Point(475, 400), Point(425, 400)};
		room.addObject(3, newObjHolder);
		delete newObjHolder;
		newObjHolder = 0;
		return room;
	} else if (testCaseNum == 4){
		STARTING_X = 300;
		STARTING_Y = 100;
		MAP_BOUNDS = new Point[8]{Point(0, 0), Point(600, 0), Point(600, 600), Point(400, 600), Point(400, 400), Point(200, 400), Point(200, 600), Point(0, 600)};
		room = Room(8, MAP_BOUNDS, Point(STARTING_X, STARTING_Y), 3);
		delete MAP_BOUNDS;
		MAP_BOUNDS = 0;
		newObjHolder = new Point[4]{Point(200, 200), Point(400, 200), Point(400, 300), Point(200, 300)};
		room.addObject(4, newObjHolder);
		delete newObjHolder;
		newObjHolder = new Point[5]{Point(50, 450), Point(150, 450), Point(175, 500), Point(100, 550), Point(25, 500)};
		room.addObject(5, newObjHolder);
		delete newObjHolder;
		newObjHolder = new Point[4]{Point(450, 450), Point(475, 500), Point(450, 550), Point(425, 500)};
		room.addObject(4, newObjHolder);
		delete newObjHolder;
		newObjHolder = 0;
		return room;
	} else{
		STARTING_X = 150;
		STARTING_Y = 150;
		MAP_BOUNDS = new Point[4]{Point(0, 0), Point(300, 0), Point(300, 300), Point(0, 300)};
		room = Room(4, MAP_BOUNDS, Point(STARTING_X, STARTING_Y), 0);
		delete MAP_BOUNDS;
		MAP_BOUNDS = 0;
		return room;
	}
}
