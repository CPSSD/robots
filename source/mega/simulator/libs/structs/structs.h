#ifndef structs_h
#define structs_h

#include "Arduino.h"

struct point {
	float x, y;
};

struct MapLine {
  point x1y1, x2y2;
};

struct EquationOfLine {
  point xy;
  float m, c;
  boolean isVertical;
};

typedef struct {
	byte commandNumber;
	word uniqueID;
} Command;

typedef struct MoveCommand : Command {
	word angle;
	uint32_t magnitude;
};

struct MoveResponse {
  int id;
  float angle, magnitude, reason;
};

struct RotateCommand {
  int id, angle;
};

struct RotateResponse {
  int id, angle, reason;
};

struct ScanResponse {
  //byte commandNumber;
  unsigned int uniqueID, angle, distance;
  bool last, status;
};

#endif
