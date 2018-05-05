// 
// 
// 

#include "PWMWidthCounterSerial.h"
#include "Globals.h"
#define __DEBUG_UTILS_SERIAL	SerialWireless
#define ENABLE_DEBUG_UTILS 0
#define ENABLE_DEBUG_UTILS_SERIAL_LOGGER 1
#include "DebugUtils.h"

PWMWidthCounterSerial::PWMWidthCounterSerial(int channel, bool is_this_parser, SerialClassType* serial) :
	_serial(serial), _idx(0), _is_this_parser(is_this_parser), _channel_idx(channel),_updated(false), _width(0),_stamp(0) {
	for (size_t i = 0; i < MAX_REGISTERED_COUNTERS; i++) {
		_channels[i] = NULL;
	}
	if (_is_this_parser) {
		registerCounter(this);
	}
}

void PWMWidthCounterSerial::registerCounter(PWMWidthCounterSerial * counter) {
	assert(_is_this_parser == true);
	assert(_channels[counter->_channel_idx] == NULL);
	_channels[counter->_channel_idx] = counter;
}

void PWMWidthCounterSerial::poll() {
	if (_is_this_parser) {
		char buffer[PARSE_BUFFER_SIZE+1];
		int n = _serial->available();
		if (n) {
			if (n > PARSE_BUFFER_SIZE)
				n = PARSE_BUFFER_SIZE;
			buffer[n] = 0;
			_serial->readBytes(buffer, n);
			for (int i = 0; i < n; i++) {
				encode(buffer[i]);
			}
			DEBUG_UTILS_MESSAGELN(buffer);
		}
	}
}

void PWMWidthCounterSerial::encode(const char & c) {
	switch (c) {
	case '\n':
		parse_buf[_idx] = NULL;
		commit();
		_idx = 0;
		break;
	default:
		parse_buf[_idx++] = c;
		break;
	}
}

void PWMWidthCounterSerial::commit() {
	DEBUG_UTILS_MESSAGELN("commit1"); 
	uint32_t stamp = millis();
	int idx_sep = find_from_first(parse_buf,(char)0x20, _idx);
	if (idx_sep < 0) {
		DEBUG_UTILS_MESSAGELN("commit1.5"); 
		return;
	}
	DEBUG_UTILS_MESSAGELN("commit2"); 

	parse_buf[idx_sep] = NULL;
	if (strlen(&parse_buf[0]) < 1 || strlen(&parse_buf[idx_sep + 1]) < 3) {
		return;
	}
	DEBUG_UTILS_MESSAGELN("commit3"); 

	int channel = atoi(&parse_buf[0]);
	if (channel < 0 || channel >= MAX_REGISTERED_COUNTERS) {
		DEBUG_UTILS_MESSAGELN("commit3.5"); 
		return;//out of range
	}
	DEBUG_UTILS_MESSAGELN("commit4"); 

	if (_channels[channel] == NULL) {
		DEBUG_UTILS_MESSAGELN("commit4.5"); 
		return;//unregistered channel
	}
	DEBUG_UTILS_MESSAGELN("commit5"); 

	int data = atoi(&parse_buf[idx_sep + 1]);
	if (data < 750 || data > 2250) {
		DEBUG_UTILS_MESSAGELN("commit5.5"); 
		return;//wrong data
	}
	DEBUG_UTILS_MESSAGELN("commit6"); 

	_channels[channel]->update(data, stamp);
	DEBUG_UTILS_MESSAGELN("commit7"); 
}
