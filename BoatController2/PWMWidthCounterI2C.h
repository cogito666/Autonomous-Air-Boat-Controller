// PWMWidthCounterI2C.h

#ifndef _PWMWIDTHCOUNTERI2C_h
#define _PWMWIDTHCOUNTERI2C_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "PWMWidthCounterIface.h"
#include <Wire.h>

class PWMWidthCounterI2C : public PWMWidthCounterIface{
protected:
	union PulseWidth{
		uint32_t width;
		uint8_t _raw[4];
	};
public:
	PWMWidthCounterI2C(TwoWire* i2c, int slave_addr, uint8_t channel_idx)
		:_i2c(i2c), _slave_addr(slave_addr), _channel_idx(channel_idx), _updated(false){

	}

	void poll();

	bool updated() const {
		return _updated;
	}

	void clearUpdated() {
		_updated = false;
	}

	uint32_t width() const{
		return _width.width;
	}

	/*
	This method is irrelevant in i2c
	*/
	void attach(int, Modes){}
protected:
	TwoWire* _i2c;
	int _slave_addr;
	uint8_t _channel_idx;
	PulseWidth _width;
	bool _updated;
};

#endif

