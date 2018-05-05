// 
// 
// 

#include "BoatControllerSemiauto.h"
#include "ServoInputOutput.h"
#include "Globals.h"
#include "Logger.h"
#include "PacketCarrotPoint.h"
#include "PacketWayPoints.h"
#include "PacketControlSignals.h"
#define ENABLE_DEBUG_UTILS 0
#define ENABLE_DEBUG_UTILS_SERIAL_LOGGER 1
#include "DebugUtils.h"

BoatControllerSemiauto::BoatControllerSemiauto(const String& name)
	:BoatControllerInterface(name),
	_heading_controller("auto/heading", new PID("auto/heading", 2.0, 0.0)),
	_line_fitting(10), _way_points(5.0), _pid_rudder("auto/rudder", 2.0, 0),
	_controller_state(BoatSemiAutoControllerStates::BoatCtrlMode_SemiAuto_Init),
	_timeout_log_wps(1000, NULL, false)
{

}

int BoatControllerSemiauto::getControllerState() const
{
	return _controller_state;
}

void BoatControllerSemiauto::control() {
	DEBUG_UTILS_MESSAGELN("Semiauto::control - begin");
	if (!isInitialized()) {
		DEBUG_UTILS_MESSAGELN("Semiauto::control - not initailized");
		return;
	}
	if(!boat_state.isPoseUpdated()){
		DEBUG_UTILS_MESSAGELN("Semiauto::control - Pose not updated");
		return;
	}

	if (isFinished()) {
		DEBUG_UTILS_MESSAGELN("Semiauto::control-AutoController finished");
		servo_rudder->writeMicroseconds(rudder_angle_pulse_mapper->getIdlePulseWidth()+ RUDDER_SIGNAL_OFFSET);
		servo_throtle->writeMicroseconds(rf_signal_throtle->width());
		return;
	}

	DEBUG_UTILS_MESSAGELN("Semiauto::control - update waypoint");
	_elapsed_time.update();
	double dt = _elapsed_time.get()*0.001;
	_way_points.update(boat_state.getPositionXYYaw());
	if (_way_points.finished()) {
		_finished = true;
		_controller_state = BoatSemiAutoControllerStates::BoatCtrlMode_SemiAuto_Idle;
		servo_rudder->writeMicroseconds(rudder_angle_pulse_mapper->getIdlePulseWidth()+ RUDDER_SIGNAL_OFFSET);
		servo_throtle->writeMicroseconds(rf_signal_throtle->width());
		DEBUG_UTILS_MESSAGELN("Semiauto::control - WayPoint finished");
		return;
	}
	_way_points.clearPastWayPoints(2);

	switch (_controller_state) {
	case BoatSemiAutoControllerStates::BoatCtrlMode_SemiAuto_TurnLeft:
	case BoatSemiAutoControllerStates::BoatCtrlMode_SemiAuto_TurnRight:
		DEBUG_UTILS_MESSAGELN("Semiauto::control - Turnning");
		if (_way_points.currentWayPointIndex() == _way_points.size() - 1) {//target way-point reached.
			_controller_state = BoatSemiAutoControllerStates::BoatCtrlMode_SemiAuto_Running;
		}
		break;
	case BoatSemiAutoControllerStates::BoatCtrlMode_SemiAuto_Running:
		DEBUG_UTILS_MESSAGELN("Semiauto::control - Running");
		//extend the way-points such that the boat can go straight forever.
		if (_way_points.isLastWayPoint()) {
			DEBUG_UTILS_MESSAGELN("Semiauto::control - Extend Way Point");
			_way_points.createAndAddWayPoint(0, _way_points.lookAheadDistance()*2);
			logWayPoints();
		}
		break;
	case BoatSemiAutoControllerStates::BoatCtrlMode_SemiAuto_Homming:
		//do nothing
		break;
	}
	DEBUG_UTILS_MESSAGELN("Semiauto::control - compute control inpu");
	Point2D carrot = _way_points.lookAheadPoint();
	Point2D des_heading_vec = carrot - boat_state.getPositionXY();
	double angle_err = normalizeHeading(atan2(des_heading_vec._y, des_heading_vec._x) - boat_state.getHeading());
	double angle_input = _pid_rudder.update(angle_err, dt);
	assert(isfinite(angle_input));
	servo_rudder->writeMicroseconds(rudder_angle_pulse_mapper->angleToPulseWidth(angle_input)+ RUDDER_SIGNAL_OFFSET);
	servo_throtle->writeMicroseconds(rf_signal_throtle->width());

	//log way-points
	if (_timeout_log_wps.spinOnce()) {
		this->logWayPoints();
	}

	DEBUG_UTILS_MESSAGELN("Semiauto::control - logging carrot point");
	//logging look ahead point
	String message; message.reserve(100);
	PacketCarrotPoint pack_carrot_point(_way_points.lookAheadPoint());
	pack_carrot_point.serialize(message);
	loggers.logln(message.c_str(), LoggerManager::LOG_INFO);
	DEBUG_UTILS_MESSAGELN("Semiauto::control - end");
}

bool BoatControllerSemiauto::isInitialized() const {
	return _initialized;
}

void BoatControllerSemiauto::initialize() {
	DEBUG_UTILS_MESSAGELN("Semiauto::initialize - begin");
	//initialized heading controller
	if (!_heading_controller.isInitialized()) {
			if (boat_state.isHeadingUpdated()) {
				DEBUG_UTILS_MESSAGELN("Semiauto::initialize - init heading controller");
				_heading_controller.initialize();
			}
	}
	else {
		DEBUG_UTILS_MESSAGELN("Semiauto::initialize - run heading controller");
		_heading_controller.control();
	}

	if (boat_state.isPoseUpdated()) {
		Point2D p_sample = boat_state.getPositionXY();
		if (_line_fitting.size() == 0) {
			DEBUG_UTILS_MESSAGELN("Semiauto::initialize - add first sample pose");
			_line_fitting.addData(p_sample);
			_last_fitting_data = p_sample;
		}
		else {
			if (norm(p_sample - _last_fitting_data) > 0.35) {
				DEBUG_UTILS_MESSAGELN("Semiauto::initialize - add sample pose");
				_line_fitting.addData(p_sample);
				_last_fitting_data = p_sample;
			}
			else {
				goto label_end_init;
			}
		}
		_controller_state = BoatSemiAutoControllerStates::BoatCtrlMode_SemiAuto_Init;
		if (_line_fitting.dataReady() && _line_fitting.numNewData() >= 5) {
			DEBUG_UTILS_MESSAGELN("Semiauto::initialize - fitting line");
			_line_fitting.fit();
			if (_line_fitting.residual() < 0.8) {
				DEBUG_UTILS_MESSAGELN("Semiauto::initialize - line accepted");
				_way_points.initializeWayPoints(_line_fitting.fittedLine(), boat_state.getPositionXYYaw());
				_initialized = true;
				_controller_state = BoatSemiAutoControllerStates::BoatCtrlMode_SemiAuto_Running;
				logWayPoints();
			}
			DEBUG_UTILS_MESSAGELN("Semiauto::initialize - log residual");
			String residual_log;
			residual_log = String("Residual = ") + String(_line_fitting.residual(), 2);
			loggers.logln(residual_log.c_str(), LoggerManager::LOG_LOGGING);
		}
		_elapsed_time.update();
	}
label_end_init:
	DEBUG_UTILS_MESSAGELN("Semiauto::initialize - end");
}

bool BoatControllerSemiauto::isFinished() const {
	return _finished;//way-points following finished.
}

void BoatControllerSemiauto::reset() {
	_heading_controller.reset();
	_initialized = false;
	_finished = false;
	_way_points.clear();
	_line_fitting.clear();
	_pid_rudder.resetAll();
	_controller_state = BoatSemiAutoControllerStates::BoatCtrlMode_SemiAuto_Init;
}

void BoatControllerSemiauto::goToHereNow(const Point2D& point) {
	if (!isInitialized() || isFinished()) {
		reset();
		_initialized = true;//force initialization.
		_way_points.addWayPoint(boat_state.getPositionXY());
		_way_points.addWayPoint(point);
	}
	else {
		_way_points.clearFutureWayPoints();
		_way_points.addWayPoint(point);
	}
	logWayPoints();
	_controller_state = BoatSemiAutoControllerStates::BoatCtrlMode_SemiAuto_Running;
}

void BoatControllerSemiauto::turnLeft(const double& D) {
	if (!isInitialized() || isFinished()) {
		return;
	}
	assert(D >= 5.0);
	double R = 0.5 * D;	
	double dist = sqrt(2 * R *R);
	
	_way_points.clearFutureWayPoints();
	_way_points.createAndAddWayPoint(DEG_TO_RAD * 45, dist);
	_way_points.createAndAddWayPoint(DEG_TO_RAD * 90, dist);
	_way_points.createAndAddWayPoint(DEG_TO_RAD * 45, _way_points.lookAheadDistance() * 4);
	_controller_state = BoatSemiAutoControllerStates::BoatCtrlMode_SemiAuto_TurnLeft;
	_target_wp_idx = _way_points.size() - 2;
	logWayPoints();
}

void BoatControllerSemiauto::turnRight(const double& D) {
	if (!isInitialized() || isFinished()) {
		return;
	}
	assert(D >= 5.0);
	double R = 0.5 * D;
	double dist = sqrt(2 * R *R);
	_way_points.clearFutureWayPoints();
	_way_points.createAndAddWayPoint(DEG_TO_RAD*-45, dist);
	_way_points.createAndAddWayPoint(DEG_TO_RAD*-90, dist);
	_way_points.createAndAddWayPoint(DEG_TO_RAD*-45, _way_points.lookAheadDistance() * 4);
	_controller_state = BoatSemiAutoControllerStates::BoatCtrlMode_SemiAuto_TurnRight;
	_target_wp_idx = _way_points.size() - 2;
	logWayPoints();
}

void BoatControllerSemiauto::goHome()
{
	if (!home_position.isInitialized()) {
		return;
	}
	goToHereNow(home_position.getMeanValue());
	_controller_state = BoatSemiAutoControllerStates::BoatCtrlMode_SemiAuto_Homming;
}

void BoatControllerSemiauto::setLookAheadDistance(const double & look_ahead_dist)
{
	_way_points.setLookAheadDistance(look_ahead_dist);
}

void BoatControllerSemiauto::logWayPoints()
{
	PacketWayPoints pack_way_points(_way_points);
	String msg; msg.reserve(250);
	pack_way_points.serialize(msg);
	loggers.logln(msg.c_str(), LoggerManager::LOG_INFO);
	_timeout_log_wps.reset();
	DEBUG_UTILS_MESSAGELN(msg.c_str());
}