#ifndef _PWMWIDTHCOUNTER_IFACE_h
#define _PWMWIDTHCOUNTER_IFACE_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

class PWMWidthCounterIface{
public:
	enum Modes {
		PULSE_WIDTH = 1,
		PULSE_WIDTH_INVERSE
	};

	virtual ~PWMWidthCounterIface(){}
	virtual void poll() = 0;
	virtual bool updated() const = 0;
	virtual void clearUpdated() = 0;
	virtual uint32_t width() const = 0;
	virtual void attach(int pin, Modes mode) = 0;
	virtual uint32_t stamp() const { return 0; }
};

#endif