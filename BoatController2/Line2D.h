// Line2D.h

#ifndef _LINE2D_h
#define _LINE2D_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Point2D.h"
struct Line2D{
	Line2D():_a(0.0),_b(0.0),_c(0.0){

	}

	/*
	A line representing ax+by+c=0.
	*/
	Line2D(double a, double b, double c)
	:_a(a), _b(b), _c(c){

	}

	bool isVertical() const{
		if (fabs(_b) < 0.0001){
			return true;
		}
		if (fabs(_a / _b) > 10000.0){
			return true;
		}
		return false;
	}

	static Line2D fromGeneralForm(double a, double b, double c);
	static Line2D fromTwoPoints(Point2D p1, Point2D p2);
	static Line2D fromPointSlope(Point2D p, double m);

	double _a, _b, _c;
};

struct DirectedLine2D{
	Line2D _line;
	Point2D _dir;//a unit vector which represents the direction of this line.
};

#endif

