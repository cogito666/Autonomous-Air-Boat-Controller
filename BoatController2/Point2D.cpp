// 
// 
// 

#include "Point2D.h"

Point2D operator+(const Point2D& p1, const Point2D& p2){
	return Point2D(p1._x + p2._x, p1._y + p2._y);
}

Point2D operator-(const Point2D& p1, const Point2D& p2){
	return Point2D(p1._x - p2._x, p1._y - p2._y);
}

Point2D operator*(const Point2D& p1, const double& s){
	return Point2D(s*p1._x, s*p1._y);
}

Point2D operator*(const double& s, const Point2D& p1){
	return Point2D(s*p1._x, s*p1._y);
}

Point2D operator/(const Point2D & p1, const double & div)
{
	return Point2D(p1._x/div, p1._y/div);
}
