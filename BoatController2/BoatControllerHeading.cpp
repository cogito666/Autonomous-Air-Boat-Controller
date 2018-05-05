#include "BoatControllerHeading.h"
#include "Logger.h"
#include "ServoInputOutput.h"
#include "Globals.h"
#include "BoatState.h"
#define ENALB_E
#define __DEBUG_UTILS_SERIAL SerialWireless
#define ENABLE_DEBUG_UTILS 0
#define ENABLE_DEBUG_UTILS_SERIAL_LOGGER 1
#include "DebugUtils.h"
#include "PacketHeadingControlState.h"

BoatControllerHeading::BoatControllerHeading(const String& name, PID * pid_heading)
	:BoatControllerInterface(name), _pid(pid_heading) {
	reset();

}

int BoatControllerHeading::getControllerState() const
{
	if (!isInitialized()) {
		return BoatHeadingControllerStates::BoatCtrlMode_Heading_Init;
	}
	return BoatHeadingControllerStates::BoatCtrlMode_Heading_Running;
}

void BoatControllerHeading::control() {
	if (rf_signal_rudder->updated()) {
		int width = rf_signal_rudder->width();
		int idle_width = rudder_angle_pulse_mapper->getIdlePulseWidth(); //TODO : should I use AnglePulseWidthMapper?
		//int idle_width = 1500;//TODO ; Hard-coded idle pulse
		//Check if the rudder handler is in idle.																												
		if (abs(idle_width - width) < 40) {//+- 20 microsec(idle) 
			//If yes, activate auto mode
			if (auto_mode == false) {
				//manual -> auto
				DEBUG_UTILS_MESSAGELN("Heading:Set Auto Mode");
				String msg = "Start auto heading control at " + String(RAD_TO_DEG*boat_state.getHeading());
				loggers.logln(msg.c_str(), LoggerManager::LOG_INFO);
				_pid->resetAll();//reset pid error terms
				_elapsed.update();
				setDesiredHeading(boat_state.getHeading());//set current heading as desired heading.
				auto_mode = true;
			}
		}
		else {
			//If no, release auto mode
			DEBUG_UTILS_MESSAGELN("Heading:Set Manual Mode");
			auto_mode = false;
		}
	}
	if (auto_mode) {
		//auto heading mode
		if (boat_state.isHeadingUpdated()) {
			DEBUG_UTILS_MESSAGELN("Heading:Controlling");
			_elapsed.update();
			double error = normalizeHeading(_heading_des - boat_state.getHeading());
			double dt = _elapsed.get()*0.001f;
			double angle_input = _pid->update(error, dt);
			assert(isfinite(angle_input));
			servo_rudder->writeMicroseconds(rudder_angle_pulse_mapper->angleToPulseWidth(angle_input)+ RUDDER_SIGNAL_OFFSET);
			String msg; msg.reserve(100);
			msg = "Heading : PID error=" + String(angle_input, 3) + " pulse="
				+ String(rudder_angle_pulse_mapper->angleToPulseWidth(angle_input));
			DEBUG_UTILS_MESSAGELN(msg.c_str());

		}
	}
	else {
		//full manual mode
		if (rf_signal_rudder->updated()) {
			servo_rudder->writeMicroseconds(rf_signal_rudder->width()+ RUDDER_SIGNAL_OFFSET);
		}
	}
	if (rf_signal_throtle->updated()) {
		servo_throtle->writeMicroseconds(rf_signal_throtle->width());
	}
	PacketHeadingControlState head_ctrl_state(_heading_des, boat_state.getHeading(), auto_mode);
	String message; message.reserve(100);
	head_ctrl_state.serialize(message);
	loggers.logln(message.c_str(), LoggerManager::LOG_INFO);
}

bool BoatControllerHeading::isInitialized() const {
	return _heading_des_set;
}

void BoatControllerHeading::initialize() {
	_elapsed.update();
	setDesiredHeading(boat_state.getHeading());
	_pid->update(0, 0);
#ifdef SERIALDEBUG_BoatControllerHeading
	String msg = String("%Initializing Heading Controller. Des Heading : ") + String(boat_state.getHeading());
	loggers.logln(msg.c_str(), LoggerManager::LOG_INFO);
#endif
}

void BoatControllerHeading::reset() {
	_heading_des_set = false;
	_pid->resetAll();
	_elapsed.update();
	auto_mode = true;
}

void BoatControllerHeading::setDesiredHeading(double heading) {
	_heading_des = heading;
	_heading_des_set = true;
}
