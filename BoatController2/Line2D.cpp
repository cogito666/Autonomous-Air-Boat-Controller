// 
// 
// 

#include "Line2D.h"

Line2D Line2D::fromGeneralForm(double a, double b, double c){
	return Line2D(a, b, c);
}

Line2D Line2D::fromTwoPoints(Point2D p1, Point2D p2){
	return Line2D(p2._y - p1._y, -(p2._x - p1._x), p2._x*p1._y - p1._x*p2._y);
}

Line2D Line2D::fromPointSlope(Point2D p, double m){
	return Line2D(m, -1.0, p._y - m*p._x);
}