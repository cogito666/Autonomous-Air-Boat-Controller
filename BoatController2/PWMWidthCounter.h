// PWMWidthCounter.h

#ifndef _PWMWIDTHCOUNTER_h
#define _PWMWIDTHCOUNTER_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "PWMWidthCounterIface.h"

/*
Do not create your own instance of PWMWidthCounterClass.
Using pwmWidthCounterX[1...N] is allowed.
*/
/*
virtual void poll() = 0;
virtual bool updated() const = 0;
virtual void clearUpdated() = 0;
virtual uint32_t width() const = 0;
*/
class PWMWidthCounterClass : public PWMWidthCounterIface
{
public:
	PWMWidthCounterClass();
	void poll(){/*No poll*/}
	bool updated() const;
	void clearUpdated();
	uint32_t width() const;
	void attach(int pin, Modes mode);
	void detach();
	uint32_t stamp() const;
protected:
	uint32_t _id;
	bool _attached;
	static uint32_t n_instances;
};

extern PWMWidthCounterClass pwmWidthCounter1;
extern PWMWidthCounterClass pwmWidthCounter2;
extern PWMWidthCounterClass pwmWidthCounter3;
extern PWMWidthCounterClass pwmWidthCounter4;
#endif

