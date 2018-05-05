// 
// 
// 

#include "PointLineMath.h"
#include "MathUtils.h"
#include <math.h>
double norm(const Point2D& p){
	return sqrt(p._x*p._x + p._y*p._y);
}

double dotProduct(const Point2D& p1, const Point2D& p2){
	return p1._x*p2._x + p1._y*p2._y;
}

Point2D rotate2D(const Point2D& p, const double& angle){
	double c = cos(angle), s = sin(angle);
	return Point2D(c*p._x - s*p._y, s*p._x + c*p._y);
}

bool isParallel(const Line2D& L1, const Line2D& L2){
	return compareDouble(L1._a*L2._b, L2._a*L1._b, 0.0001);
}

bool isVertical(const Line2D& line){
	if (fabs(line._b) < 0.0001){
		return true;
	}
	if (fabs(line._a / line._b) > 10000.0){
		return true;
	}
	return false;
}

Point2D intersectingPointLineLine(const Line2D& L1, const Line2D& L2){
	double tmp = (L2._a * L1._c - L1._a * L2._c) / (L1._a * L2._b - L2._a * L1._b);
	return Point2D((-L1._b*tmp - L1._c)/L1._a, tmp);
}

Point2D intersectingPointPointLine(const Point2D& p, const Line2D& line){
	double tmp = line._a*line._a + line._b*line._b;
	double tmp2 = line._b*p._x - line._a*p._y;
	return Point2D((line._b*tmp2 - line._a*line._c) / tmp, (-line._a*tmp2 - line._b*line._c) / tmp);
}

double distancePointLine(const Point2D& p, const Line2D& L){
	return abs(L._a*p._x + L._b*p._y + L._c) / sqrt(L._a*L._a + L._b*L._b);
}

Point2D interpolated2D(const Point2D& begin, const Point2D& end, double s){
	return begin + s*(end - begin);
}

Point2D getPointOnLine(const Point2D& p0, const Line2D& line, const double& dist){
	if (isVertical(line)){
		return Point2D(p0._x, p0._y + dist);
	}
	else{
		double m = -line._a / line._b;
		double delta_x =  sign(dist) * sqrt(dist * dist / (1 + m*m));
		double delta_y = delta_x * m;

		return Point2D(p0._x + delta_x, p0._y + delta_y);
	}
}