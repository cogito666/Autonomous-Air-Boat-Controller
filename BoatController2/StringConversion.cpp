// 
// 
// 

#include "StringConversion.h"


String to_string(const Point2D& p){
	char buffer[100];
	sprintf(buffer, "%.3lf %.3lf", p._x, p._y);
	return String(buffer);
}


String to_string(const Line2D& line){
	char buffer[100];
	sprintf(buffer, "%.3lf %.3lf %.3lf", line._a, line._b, line._c);
	return String(buffer);
}