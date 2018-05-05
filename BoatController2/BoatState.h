// BoatState.h

#ifndef _BOATSTATE_h
#define _BOATSTATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Pose2D.h"
#include "Point2D.h"
#include "TinyGPS++.h"
enum BoatControlModes{
	BoatCtrlMode_Invalid = 0,

	BoatCtrlMode_Manual = 1,
	BoatCtrlMode_SemiAutoControl
};

class BoatState{
public:
	double getHeading() const{
		return _pos._theta;
	}

	Point2D getPositionXY() const{
		return Point2D(_pos._x, _pos._y);
	}

	Pose2D getPositionXYYaw() const{
		return _pos;
	}

	void updatePoseX(const double& x) {
		_x_updated = true;
		_pos._x = x;
	}

	void updatePoseY(const double& y) {
		_y_updated = true;
		_pos._y = y;
	}

	void updatePoseTheta(const double& theta) {
		_theta_updated = true;
		_pos._theta = theta;
	}

	bool isPoseUpdated() {
		return _x_updated && _y_updated && _theta_updated;
	}

	bool isHeadingUpdated() {
		return _theta_updated;
	}

	void clearUpdated() {
		_x_updated = _y_updated = _theta_updated = false;
	}

	uint32_t _stamp;
	BoatControlModes _ctrl_mode;	//
	int _controller_state;	//BoatManualControlStates or BoatManualControlStates
	Pose2D _pos;
	TinyGPSPlus::SentenceType _gps_sentence;
	TinyGPSPlus::FixQuality _gps_fix_quality;
	uint32_t _gps_num_satellites;
private:
	bool _x_updated;
	bool _y_updated;
	bool _theta_updated;
};

#endif

