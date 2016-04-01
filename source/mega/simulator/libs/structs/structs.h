#ifndef STRUCTS_H
#define STRUCTS_H

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

typedef enum {
	moveNum  = 1,
	stopNum  = 2,
	rotateNum  = 3,
	scanNum  = 4
} comNums;

#endif
