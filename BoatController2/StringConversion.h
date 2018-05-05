// StringConversion.h

#ifndef _STRINGCONVERSION_h
#define _STRINGCONVERSION_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Point2D.h"
#include "Line2D.h"

String to_string(const Point2D& p);

String to_string(const Line2D& line);

#endif

