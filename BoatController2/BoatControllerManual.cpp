// 
// 
// 

#include "BoatControllerManual.h"
#include "Globals.h"
#include "Logger.h"
#include "ServoInputOutput.h"
#include "PacketControlSignals.h"
#define ENABLE_DEBUG_UTILS 0
#include "DebugUtils.h"
BoatControllerManual::BoatControllerManual(const String& name)
:BoatControllerInterface(name){
	reset();
}

int BoatControllerManual::getControllerState() const
{
	if(!isInitialized()){
		return BoatManualControllerStates::BoatCtrlMode_Manual_Init;
	}
	return BoatManualControllerStates::BoatCtrlMode_Manual_Running;
}

void BoatControllerManual::control(){
	if (rf_signal_rudder->updated()){
		servo_rudder->writeMicroseconds(rf_signal_rudder->width()+ RUDDER_SIGNAL_OFFSET);
	}
	if(rf_signal_throtle->updated()){
		servo_throtle->writeMicroseconds(rf_signal_throtle->width());
	}
}

bool BoatControllerManual::isInitialized() const{
	return _initialized;
}

void BoatControllerManual::initialize(){
	loggers.logln("Initializing Manual Controller", LoggerManager::LOG_INFO);
	_initialized = true;
}

void BoatControllerManual::reset(){
	_initialized = false;
}