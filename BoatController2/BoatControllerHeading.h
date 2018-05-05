// BoatControllerHeading.h

#ifndef _BOATCONTROLLERHEADING_h
#define _BOATCONTROLLERHEADING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "BoatControllerInterface.h"
#include "PWMWidthCounterIface.h"
#include "PID.h"
#include "ElapsedTime.h"

class BoatControllerHeading :public BoatControllerInterface{
public:
	enum BoatHeadingControllerStates {
		BoatCtrlMode_Heading_Init = 1,
		BoatCtrlMode_Heading_Running = 2
	};

	BoatControllerHeading(const String& name, PID * pid_heading);
	int getControllerState() const;
	void control();
	bool isInitialized() const;
	void initialize();
	bool isFinished() const{ return false; }//It never ends.
	void reset();
	void setDesiredHeading(double heading);
	inline PID* getPID() { return _pid; }
	const double& getDesiredHeading()const {
		return _heading_des;
	}
protected:
	double _heading_des;
	bool _heading_des_set;
	PID* _pid;
	ElapsedTimeMilli _elapsed;
	bool auto_mode;
};

#endif

