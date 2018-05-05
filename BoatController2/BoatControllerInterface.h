// BoatControllerInterface.h

#ifndef _BOATCONTROLLERINTERFACE_h
#define _BOATCONTROLLERINTERFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
class BoatControllerInterface{
public:
	BoatControllerInterface(const String& name):_name(name){}
	virtual ~BoatControllerInterface(){}
	virtual void control() = 0;
	virtual bool isInitialized() const = 0;
	virtual void initialize() = 0;
	virtual bool isFinished() const = 0;
	virtual void reset() = 0;
	virtual int getControllerState() const = 0 ;
	const String& getName() const { return _name; }
protected:
	String _name;
};

#endif

