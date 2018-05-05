// LineFit.h

#ifndef _LINEFIT_h
#define _LINEFIT_h
#include "CircularDeque.h"
#include "MathUtils.h"
#include "PointLineMath.h"

template <int MaxNPoints>
class LineFit{
public:
	/*
	*/
	LineFit(int min_n_points);

	void clear(){
		_points.clear();
		_new_points_cnt = 0;
	}

	size_t size()const { return _points.size(); }

	/*
	return true, if sufficient number of data has been added.
	*/
	bool dataReady() const;

	/*
	add a new data into the data queue.
	if the number of data gathered so far is more than the capacity(MaxNPoints), 
	the oldest data is replaced with the new data.
	*/
	void addData(const Point2D& p);

	/*
	return true, if fit() call is needed to reflect the gathered data.
	*/
	bool isFittingNeeded() const;

	/*
	*/
	size_t numNewData() const{
		return _new_points_cnt;
	}

	/*
	perform line fitting.
	But the process has slight difference from normal line fitting.
	Basically a linear polynomial does not have a direction, but this function 
	computes basic linear polynomial and the direction of the line.
	The direction is determined by the flows of sample points.
	*/
	void fit();

	/*
	return the fitted line by fit() call.
	There must be a precedent call to fit() before querying the fitted line.
	*/
	const DirectedLine2D& fittedLine() const;

	/*
	return the residual of fitted line.
	There must be a precedent call to fit().
	*/
	const double& residual() const;
protected:
	void computeResidual();
protected:
	CircularDeque<Point2D, MaxNPoints> _points;
	DirectedLine2D _line_fitted;
	double _residual;
	const int _min_n_points;
	int _new_points_cnt;
};
////////////////////////////////////////////////////
//Template-Class implementation
template <int MaxNPoints>
LineFit<MaxNPoints>::LineFit(int min_n_points)
:_min_n_points(min_n_points), _new_points_cnt(0), _residual(0.0){
	if (_min_n_points > MaxNPoints){
		//TODO : ERROR! _min_n_points cannot be greater than the capacity of the container.
	}
}

template <int MaxNPoints>
bool LineFit<MaxNPoints>::dataReady() const{
	return _points.size() >= _min_n_points;
}

template <int MaxNPoints>
void LineFit<MaxNPoints>::addData(const Point2D& p){
	if (_points.full())
		_points.pop_front();
	_points.push_back(p);
	_new_points_cnt++;
}

template <int MaxNPoints>
bool LineFit<MaxNPoints>::isFittingNeeded() const{
	return _new_points_cnt > 0;
}

template <int MaxNPoints>
void LineFit<MaxNPoints>::fit(){
	double x_mean = 0.0, y_mean = 0.0;
	double numerator = 0.0, denominator = 0.0;
	for (size_t i = 0; i < _points.size(); i++){
		x_mean += _points[i]._x;
		y_mean += _points[i]._y;
	}
	x_mean /= _points.size();
	y_mean /= _points.size();

	double tmp1, tmp2;

	for (size_t i = 0; i < _points.size(); i++){
		tmp1 = _points[i]._x - x_mean;
		tmp2 = _points[i]._y - y_mean;
		numerator += tmp1 * tmp2;//x-y covariance
		denominator += tmp1 * tmp1;//x-variance
	}

	if (denominator < 0.001){//almost vertical
		//compute line parameters
		_line_fitted._line = Line2D::fromGeneralForm(1, 0, x_mean);
		
		//compute direction of the line
		int y_dir = 0;
		for (size_t i = 1; i < _points.size(); i++){
			y_dir += sign(_points[i]._y - _points[i - 1]._y);
		}

		_line_fitted._dir._x = 0;
		_line_fitted._dir._y = sign(y_dir);
		if (_line_fitted._dir._y == 0)//If extracting the direction failed,
			_line_fitted._dir._y = 1;//assume positive direction
	}
	else{
		//compute line parameters
		double slope = numerator / denominator;
		_line_fitted._line = Line2D::fromGeneralForm(slope, -1, y_mean - slope*x_mean);
		
		//compute direction of the line
		int x_dir = 0;
		for (size_t i = 1; i < _points.size(); i++){
			x_dir += sign(_points[i]._x - _points[i - 1]._x);
		}
		_line_fitted._dir._x = sign(x_dir) * sqrt(1 / (1 + slope*slope));
		_line_fitted._dir._y = slope * _line_fitted._dir._x;
	}
	computeResidual();
	_new_points_cnt = 0;
}

template <int MaxNPoints>
void LineFit<MaxNPoints>::computeResidual(){
	_residual = 0.0;
	for (size_t i = 0; i < _points.size(); i++){
		_residual += distancePointLine(_points[i], _line_fitted._line);
	}
}

template <int MaxNPoints>
const DirectedLine2D& LineFit<MaxNPoints>::fittedLine() const{
	return _line_fitted;
}

template <int MaxNPoints>
const double& LineFit<MaxNPoints>::residual() const{
	return _residual;
}

#endif

