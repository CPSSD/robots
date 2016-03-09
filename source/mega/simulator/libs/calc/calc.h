#ifndef calc_h
#define calc_h

#include "Arduino.h"
#include "structs.h"

class calc {
  public:
    float getDistBetweenTwoPoints(point p1, point p2);
	float getTravelTime(unsigned long distance, float speed);
	float getDistanceTravelled(float speed, unsigned long time);
	struct MapLine makeLineFromPolar(float angle, float distance, point currentPosition);
	struct EquationOfLine getEquationOfLine(MapLine line);
	struct point getDestination(struct MapLine moveLine, struct EquationOfLine robotLine, struct EquationOfLine obstacles[]);
	boolean hasInterception(EquationOfLine border, EquationOfLine robotMoveLine);
	struct point getInterceptPoint(EquationOfLine robotLine, EquationOfLine other);
};

#endif