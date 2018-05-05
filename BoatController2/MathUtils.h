// MathUtils.h

#ifndef _MATHUTILS_h
#define _MATHUTILS_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
template <typename T>
int sign(const T& val) {
	if (val > 0)
		return 1;
	else if (val < 0)
		return -1;
	return 0;
}

bool compareDouble(const double& a, const double& b, const double& tol = 0.0001);

double normalizeHeading(double angle_radians);

#endif

