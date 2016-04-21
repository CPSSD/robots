#ifndef roomTestCases_h
#define roomTestCases_h

#include "room.h"

class roomTestCases {
	Point* MAP_BOUNDS;
	Point* newObjHolder;
	int STARTING_X, STARTING_Y;
	Room room;
	public:
		Room pickRoom(int testCaseNum);
};

#endif
