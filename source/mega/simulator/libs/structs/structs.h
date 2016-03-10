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

#endif
