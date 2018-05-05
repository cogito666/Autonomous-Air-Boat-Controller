// PointLineMath.h

#ifndef _POINTLINEMATH_h
#define _POINTLINEMATH_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Point2D.h"
#include "Line2D.h"

double norm(const Point2D& p);

double dotProduct(const Point2D& p1, const Point2D& p2);

/*
rotate 'p' by the 'angle' rad.
*/
Point2D rotate2D(const Point2D& p, const double& angle);

/*
return true if two lines,L1 and L2, are (exactly or almost) parallel.
return false otherwise.
*/
bool isParallel(const Line2D& L1, const Line2D& L2);

/*
return true, if 'line' is (almost) vertical.
*/
bool isVertical(const Line2D& line);

/*
return the intersecting point of two lines.
Make sure two lines are not parallel!
*/
Point2D intersectingPointLineLine(const Line2D& L1, const Line2D& L2);

/*
return the intersecting point between 'line' and its perpendicular line which passes 'p'. 
*/
Point2D intersectingPointPointLine(const Point2D& p, const Line2D& line);

/*
return the distance between a point 'p' and a line 'L'.
*/
double distancePointLine(const Point2D& p, const Line2D& L);

/*
s represents position on the line defined by two points(begin, end).
	s=0.0      : begin
	s=1.0      : end
	0.0<s<1.0  : interpolate
	s>1.0      : extrapolate
	s<0.0      : negative extrapolate
*/
Point2D interpolated2D(const Point2D& begin, const Point2D& end, double s);

/*
return a Point2D on a line 'line' which are 'dist' far away from 'p0'.
if 'dist' > 0
	return a point on the direction of incresing 'x'.
else if 'dist' < 0
	return a point on the direction of decreasing 'x'.
else
	return a point equal to 'p0'.
*/
Point2D getPointOnLine(const Point2D& p0, const Line2D& line, const double& dist);
#endif

