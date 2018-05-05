// Point2D.h

#ifndef _POINT2D_h
#define _POINT2D_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

struct Point2D{
	Point2D():_x(0),_y(0){

	}

	Point2D(double x, double y):
	_x(x), _y(y){

	}

	Point2D& operator+=(const Point2D& other){
		_x += other._x;
		_y += other._y;
		return *this;
	}

	Point2D& operator-=(const Point2D& other){
		_x -= other._x;
		_y -= other._y;
		return *this;
	}

	Point2D& operator*=(const double& s){
		_x *= s;
		_y *= s;
		return *this;
	}

	Point2D& operator/=(const double& div){
		_x /= div;
		_y /= div;
		return *this;
	}

	double _x, _y;
};

Point2D operator+(const Point2D& p1, const Point2D& p2);
Point2D operator-(const Point2D& p1, const Point2D& p2);
Point2D operator*(const Point2D& p1, const double& s);
Point2D operator*(const double& s, const Point2D& p1);
Point2D operator/(const Point2D& p1, const double& div);
#endif

