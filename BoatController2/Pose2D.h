// Pose2D.h

#ifndef _POSE2D_h
#define _POSE2D_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
struct Pose2D{
	Pose2D():_x(0),_y(0),_theta(0){

	}

	Pose2D(const double& x, const double& y, const double& theta):_x(x), _y(y), _theta(theta){

	}

	double _x, _y;
	double _theta;
};

#endif

