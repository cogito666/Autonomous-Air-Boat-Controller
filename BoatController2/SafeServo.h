// SafeServo.h

#ifndef _SAFESERVO_h
#define _SAFESERVO_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <Servo.h>
#include "ServoUpdater.h"
class SafeServo :public ServoUpdater {  // overRiding
private:
	int _pin;
	int _stepPerLoop = 23;
	int _stepSize = 3;

	int _leftMaximumMicrosecond = 1850;
	int _rightMaximumMicrosecond = 1050;

	int _lastMicrosecondVar;
	int _desiredPositionMicrosecond;

	bool _first = true;
	bool _isSettle = true;
public:
	void writeMicroseconds(int var) {
		if (var > this->_leftMaximumMicrosecond) {
			this->_desiredPositionMicrosecond = _leftMaximumMicrosecond;
			this->_isSettle = false;
		}
		else if (var < this->_rightMaximumMicrosecond) {
			this->_desiredPositionMicrosecond = _rightMaximumMicrosecond;
			this->_isSettle = false;
		}
		else {
			this->_desiredPositionMicrosecond = var;
			this->_isSettle = false;
		}
	}

	void move() {
		if (_first) {
			ServoUpdater::writeMicroseconds(this->_desiredPositionMicrosecond);
			this->_lastMicrosecondVar = _desiredPositionMicrosecond;
			_first = false;
		}
		else {
			if (_isSettle == false) {
				for (int i = 0; i < _stepPerLoop; i++) {
					if (abs(_lastMicrosecondVar - _desiredPositionMicrosecond) < _stepSize) {
						this->_lastMicrosecondVar = _desiredPositionMicrosecond;
						this->_isSettle = true;
						break;
					}

					if (_lastMicrosecondVar > _desiredPositionMicrosecond) {
						this->_lastMicrosecondVar -= _stepSize;
						ServoUpdater::writeMicroseconds(_lastMicrosecondVar);
					}
					else {
						this->_lastMicrosecondVar += _stepSize;
						ServoUpdater::writeMicroseconds(_lastMicrosecondVar);
					}
				}
			}
		}
	}
};

#endif

