// 
// 
// 

#include "WayPoints.h"
#include "PointLineMath.h"
#define ENABLE_DEBUG_UTILS 0
#include "DebugUtils.h"

WayPoints::WayPoints(double look_ahead_dist)
:_look_ahead_distance(look_ahead_dist), _phase(0.0), _delete_reached(false), _running(false), _finished(false){

}

bool WayPoints::isLastWayPoint() const
{
	if(size() > 0)	
		return _idx_wp == size()-1;
	return true;//no way-points
}

void WayPoints::initializeWayPoints(const DirectedLine2D& line_to_follow, const Pose2D& current_pos){
	clear();
	Point2D wp1 = intersectingPointPointLine(Point2D(current_pos._x, current_pos._y), line_to_follow._line);
	Point2D wp2 = wp1 + line_to_follow._dir * 2 * _look_ahead_distance;
	this->addWayPoint(wp1);
	this->addWayPoint(wp2);
}

void WayPoints::clear(){
	_wps.clear();
	_running = false;
	_finished = false;
	_idx_wp = 0;
	_phase = 0.0;
	_idx_wp_look_ahead = 0;
}

void WayPoints::clearPastWayPoints(int n_remains_after)
{
	if (n_remains_after < 1)
		n_remains_after = 1;
	int n_should_be_removed = _idx_wp - n_remains_after;
	if(n_should_be_removed == 0){
		// do nothing.
		return;
	}
	for (int i = 0; i < n_should_be_removed; i++) {
		_wps.pop_front();
		_idx_wp--;
		_idx_wp_look_ahead--;
	}
}

void WayPoints::clearFutureWayPoints(){
	if (running() == false) return;//it's not running
	if (finished() == true) return;//every way-points are reached.

	Point2D current_wp = currentWayPoint();
	//delete all future way points
	while (_wps.size() > _idx_wp_look_ahead){
		_wps.pop_back();
	}
	//create and add a new way-point at lastly-computed look-ahead-point.
	_wps.push_back(_look_ahead_point);
	if (_idx_wp_look_ahead == _idx_wp){
		//current way-point has been replaced.
		//recompute internal variables.
		selectWayPoint();
	}
}

void WayPoints::setDeleteReachedWayPoints(bool delete_reached){
	if (running()){
		return;
	}
	_delete_reached = delete_reached;
}

void WayPoints::setLookAheadDistance(double dist){
	_look_ahead_distance = dist;
}

void WayPoints::addWayPoint(const Point2D& wp){
	if (finished()) return;//call clear() to reset _finished flag.
	_wps.push_back(wp);
}

void WayPoints::createAndAddWayPoint(const double& heading, 
	const double& distance){
	if (this->size() < 2)//at least 2 waypoints are required.
		return;
	assert(distance > 0.5);
	const Point2D& wp_last = _wps.at(_wps.size() - 1);
	const Point2D& wp_last_last = _wps.at(_wps.size() - 2);

	Point2D vec = wp_last - wp_last_last;
	vec = rotate2D(vec, heading);

	//TODO : possible error => last two waypoints are too close
	double scale = distance / norm(vec);
	vec *= scale;
	
	this->addWayPoint(wp_last + vec);
}

void WayPoints::update(Pose2D current_pos){
	if (empty())//Not way-points is defined.
		return;
	if (finished())//call clear() first to clear finished flag.
		return;
	if (!_running){ //first update call
		//set the first way-point as current vehicle position.
		if (norm(Point2D(current_pos._x, current_pos._y) - _wps.front()) > 5.0){
			//if the first way-point is too far, add current position as the first way-point.
			_wps.push_front(Point2D(current_pos._x, current_pos._y));
		}
		for (size_t i = 0; i < _wps.size(); i++){
			//DEBUG_Serial.println(String(i) + String("\t:") + to_string(_wps[i]));
		}
		_running = true;
		_phase = 0.0;
		_idx_wp = 1;
	}
	_current_pos = current_pos;
	selectWayPoint();
	computelookAheadPoint();
}

void WayPoints::selectWayPoint(){
	Point2D p_proj = getProjectedVehiclePositionOntoWayPoint();
	Point2D wp_seg = _wps[_idx_wp] - _wps[_idx_wp - 1];
	Point2D phase_seg = p_proj - _wps[_idx_wp - 1];
	double s = norm(phase_seg) / norm(wp_seg);
	if (dotProduct(wp_seg, phase_seg) < 0)
		s = s * -1;
	//DEBUG_Serial.println(String("Phase ") + String(_phase, 3));
	if (s >= _phase) {
		_phase = s;
	}
	if (_phase >= 1.0){
		if (_delete_reached){
			//delete first way-point from the list
			//way-point index does not change!
			_wps.pop_front();
		}
		else{
			//just increment index of way-point.
			_idx_wp++;
			//DEBUG_Serial.println(String("wp_idx ") + String(_idx_wp));
		}
		_phase = 0.0;

		if (_wps.size() == 1 || _idx_wp == _wps.size()) {
			//the last way-point is reached.
			_finished = true;
		}
	}
}

void WayPoints::computelookAheadPoint(){
	Point2D p_proj = interpolated2D(_wps[_idx_wp - 1], _wps[_idx_wp], _phase);
	//DEBUG_Serial.println(String("proj ") + to_string(p_proj));
	size_t idx = _idx_wp;
	Point2D p1 = p_proj;
	double seg_length = norm(_wps[idx] - p1);
	double dist_accum = seg_length;
	while (dist_accum < _look_ahead_distance){
		if (idx == (_wps.size()-1)){//last way-point reached
			_look_ahead_point = _wps.back();//use last way-point as lookaheadpoint.
			_idx_wp_look_ahead = idx;
			return;
		}
		p1 = _wps[idx++];
		seg_length = norm(_wps[idx] - p1);
		dist_accum += seg_length;
	}
	//DEBUG_Serial.println(String("dist_accum ") + String(dist_accum));
	_idx_wp_look_ahead = idx;
	_look_ahead_point = interpolated2D(p1, _wps[idx], 1.0 - (dist_accum - _look_ahead_distance) / seg_length);
}

Point2D WayPoints::getProjectedVehiclePositionOntoWayPoint(){
	return intersectingPointPointLine(
		Point2D(_current_pos._x, _current_pos._y),
		Line2D::fromTwoPoints(_wps[_idx_wp - 1], _wps[_idx_wp]));
}
