// UpdatableServo.h

#ifndef _UPDATABLESERVO_h
#define _UPDATABLESERVO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <Servo.h>

class ServoUpdater :public Servo {
public:
	void write(int value) {
		_updated = true;
		Servo::write(value);
	}
	void writeMicroseconds(int value) {
		_updated = true;
		Servo::writeMicroseconds(value);
	}
	void clearUpdated() {
		_updated = false;
	}
	bool isUpdated() const{
		return _updated;
	}
protected:
	bool _updated = false;
};

#endif

