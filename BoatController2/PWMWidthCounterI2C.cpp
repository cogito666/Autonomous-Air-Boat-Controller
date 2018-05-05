#include "PWMWidthCounterI2C.h"
#include "Globals.h"
#define ENABLE_DEBUG_UTILS 0
#define ENABLE_DEBUG_UTILS_ELAPSED_TIME_UTILS 1
#define __DEBUG_UTILS_SERIAL SerialGPS
#include "DebugUtils.h"

void PWMWidthCounterI2C::poll() {
	DEBUG_UTILS_ELAPSED_TIME_INIT();

	DEBUG_UTILS_ELAPSED_TIME_BEGIN();
	_i2c->beginTransmission(_slave_addr);
	_i2c->write(_channel_idx);
	_i2c->endTransmission();
	DEBUG_UTILS_ELAPSED_TIME_END("i2c(Send Channel)");
	//delay(3);
	DEBUG_UTILS_ELAPSED_TIME_BEGIN();
	int n = _i2c->requestFrom(_slave_addr, 4);
	DEBUG_UTILS_ELAPSED_TIME_END("i2c(Request From)");
	//delay(3);
	if (n != 4) {
		_updated = false;
		return;
	}

	DEBUG_UTILS_ELAPSED_TIME_BEGIN();
	PulseWidth width; 
	_i2c->readBytes(&width._raw[0], 4);
	DEBUG_UTILS_ELAPSED_TIME_END("i2c(Reading)");
	if (width.width == 0 || width.width < 750 || width.width > 2250) {
		_updated = false;
		return;
	}
	_width.width = width.width;
	_updated = true;
}
