// DutyCounter.h

#ifndef PWMWIDTH_COUNTER_POLLING_H_
#define PWMWIDTH_COUNTER_POLLING_H_

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "PWMWidthCounterIface.h"
class PWMWidthCounterPolling : public PWMWidthCounterIface{
public:
	PWMWidthCounterPolling() : _first(true),_width(0){

	}

	void poll(){
		int val = digitalRead(_digital_pin);
		if (_first){
			_last_val = val;
			_first = false;
			return;
		}

		int change = val - _last_val;
		switch (_logic){
		case PULSE_WIDTH:
			if (change > 0){
				//rising edge
				_t0 = micros();
			}
			else if (change < 0){
				//falling edge
				_width = micros() - _t0;
				_stamp = millis();
				_updated = true;
			}
			break;
		case PULSE_WIDTH_INVERSE:
			if (change > 0){
				//rising edge
				_width = micros() - _t0;
				_stamp = millis();
				_updated = true;
			}
			else if (change < 0){
				//falling edge
				_t0 = micros();
			}
			break;
		}
		_last_val = val;
	}

	bool updated() const{
		return _updated;
	}

	uint32_t stamp() const{
		return _stamp;
	}

	void clearUpdated(){
		_updated = false;
	}

	uint32_t width() const{
		return _width;
	}

	void attach(int pin, Modes mode){
		this->_digital_pin = pin;
		this->_logic = mode;
	}
protected:
	int _digital_pin;
	Modes _logic;
	uint32_t _width;
	uint32_t _t0;
	int _last_val;
	bool _updated;
	bool _first;
	uint32_t _stamp;
};
#endif

