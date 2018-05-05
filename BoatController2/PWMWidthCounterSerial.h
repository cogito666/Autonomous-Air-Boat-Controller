// PWMWidthCounterSerial.h

#ifndef _PWMWIDTHCOUNTERSERIAL_h
#define _PWMWIDTHCOUNTERSERIAL_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "PWMWidthCounterIface.h"
#include "Utils.h"
#include "Globals.h"

class PWMWidthCounterSerial:public PWMWidthCounterIface{
	enum{ PARSE_BUFFER_SIZE = 100, 
		MAX_REGISTERED_COUNTERS = 10};
public:
	PWMWidthCounterSerial(int channel, bool is_this_parser, SerialClassType* serial);
	virtual ~PWMWidthCounterSerial(){}
	void registerCounter(PWMWidthCounterSerial* counter);

	void poll();

	bool updated() const{
		return _updated;
	}

	void clearUpdated() {
		_updated = false;
	}

	uint32_t width() const{
		return _width;
	}
	uint32_t stamp() const {
		return _stamp;
	}
	void attach(int pin, Modes mode){}
protected:
	void update(uint32_t width, uint32_t stamp) {
		_width = width;
		_stamp = stamp;
		_updated = true;
	}

	void encode(const char& c);

	void commit();

	int _idx;
	char parse_buf[PARSE_BUFFER_SIZE];
protected:
	bool _updated;
	uint32_t _width;
	SerialClassType * _serial;
	bool _is_this_parser;
	int _channel_idx;
	//valid for an instance which marked as parser
	PWMWidthCounterSerial* _channels[MAX_REGISTERED_COUNTERS];
	uint32_t _stamp;
};

#endif

