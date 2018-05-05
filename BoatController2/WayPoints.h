// WayPoints.h

#ifndef _WAYPOINTS_h
#define _WAYPOINTS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "CircularDeque.h"
#include "Point2D.h"
#include "Pose2D.h"
#include "Line2D.h"

class WayPoints{
public:
	WayPoints(double look_ahead_dist);

	/*
	return the number of way-points
	*/
	inline size_t size() const{
		return _wps.size();
	}
	
	/*
	return if a new way-point can be added.
	*/
	inline bool full() const{
		return _wps.capacity() == 0;
	}

	/*
	*/
	inline bool empty() const{
		return _wps.size() == 0;
	}

	/*
	return if way-point following has been started.
	*/
	inline bool running() const{
		return _running;
	}

	inline bool finished() const{
		return _finished;
	}

	bool isLastWayPoint() const;


	/*
	*/
	void initializeWayPoints(const DirectedLine2D& line_to_follow, const Pose2D& current_pos);


	/*
	Clear all way-points.
	*/
	void clear();
	
	/*
	Clear passed way-points excluding the latest n_remains_after way-points.
	If number of fast way-points is less or equal to n_remains_after, do not remove any way-points
	n_remains_after must be great or equal than 1.
	*/
	void clearPastWayPoints(int n_remains_after);

	/*
	Clear way-points which are not reached yet.
	This method is useful when the way-points need to be cancelled and then modified dynamically.
	To prevent the look-ahead-point from jumping, a new way-point is created and added at the current look-ahead-point automatically.
	This method is relevant only when running()==true.
	*/
	void clearFutureWayPoints();

	/*
	if it is true, reached way-points are removed.
	*/
	void setDeleteReachedWayPoints(bool delete_reached);

	/*
	set look ahead distance for the carrot point.
	*/
	void setLookAheadDistance(double dist);

	/*
	push back a waypoint.
	*/
	void addWayPoint(const Point2D& wp);
	
	/*
	create and add a waypoint.
	The waypoint is 'distance' meters away from the last waypoint and 'heading' radians 
	from direction of the last waypoint.
	! the direction of i-th waypoint : the direction of the line connecting (i-1)th and (i)th waypoints.
	*/
	void createAndAddWayPoint(const double& heading, const double& distance);

	/*
	On first update(), set _init_wp as current_pos to connect the first way-point
	and beginning position of the robot.
	*/
	void update(Pose2D current_pos);

	inline const Point2D& lookAheadPoint() const{
		return _look_ahead_point;
	}

	inline size_t currentWayPointIndex() const{
		return _idx_wp;
	}

	inline const Point2D& currentWayPoint() const{
		return _wps[_idx_wp];
	}

	inline const Point2D& firstWayPoint() const{
		return _wps.front();
	}

	inline const Point2D& lastWayPoint() const{
		return _wps.back();
	}

	inline const Point2D& operator[](const size_t& idx) const{
		return _wps[idx];
	}

	inline const double& lookAheadDistance() const{
		return _look_ahead_distance;
	}

	inline const double& phase() const{
		return _phase;
	}
protected:
	void selectWayPoint();
	void computelookAheadPoint();
	Point2D getProjectedVehiclePositionOntoWayPoint();
private:
	CircularDeque<Point2D, 30> _wps;

protected:
	bool _running;
	bool _finished;
	bool _delete_reached;
	double _look_ahead_distance;
	size_t _idx_wp;
	Pose2D _current_pos;
	double _phase;
	Point2D _look_ahead_point;
	size_t _idx_wp_look_ahead;
};

#endif

