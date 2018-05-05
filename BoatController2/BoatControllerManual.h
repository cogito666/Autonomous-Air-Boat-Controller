// BoatControllerManual.h

#ifndef _BOATCONTROLLERMANUAL_h
#define _BOATCONTROLLERMANUAL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "BoatControllerInterface.h"
#include "PWMWidthCounterIface.h"
#include <Servo.h>

class BoatControllerManual :public BoatControllerInterface{
public:
	enum BoatManualControllerStates {
		BoatCtrlMode_Manual_Init = 1,
		BoatCtrlMode_Manual_Running = 2
	};
	BoatControllerManual(const String& name);
	int getControllerState() const;
	void control();
	bool isInitialized() const;
	void initialize();
	bool isFinished() const { return false; }//it never ends
	void reset();
protected:
	PWMWidthCounterIface * rudder_signal;
	Servo * rudder_servo;
	bool _initialized;
};
#endif

