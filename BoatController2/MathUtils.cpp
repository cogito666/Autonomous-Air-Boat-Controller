// 
// 
// 

#include "MathUtils.h"

bool compareDouble(const double& a, const double& b, const double& tol){
	if (abs(a - b) > tol)
		return false;
	return true;
}

double normalizeHeading(double radians){
	if (radians > M_PI)
		radians -= M_PI * 2;
	else if (radians < -M_PI)
		radians += M_PI * 2;
	return radians;
}
