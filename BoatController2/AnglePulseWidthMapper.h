// AnglePulseWidthMapper.h

#ifndef _ANGLEPULSEWIDTHMAPPER_h
#define _ANGLEPULSEWIDTHMAPPER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class AnglePulseWidthMapper {
public:
	void init(double angle1, double angle2, uint32_t width1, uint32_t width2) {
		_angle1 = angle1;
		_angle2 = angle2;
		_width1 = width1;
		_width2 = width2;
		if (angle1<angle2) {
			_min_angle = angle1;
			_max_angle = angle2;
		}
		else {
			_min_angle = angle2;
			_max_angle = angle1;
		}
		if (width1 < width2) {
			_min_width = width1;
			_max_width = width2;
		}
		else {
			_min_width = width2;
			_max_width = width1;
		}
		s1 = (angle2 - angle1) / (width2 - width1);
		s2 = 1.0 / s1;
		_idle_pulse = width1 + (0 - angle1) * s2;//zero angle pulse
	}

	double pulseWidthToAngle(uint32_t pulse_width_micro) {
		double ret = _angle1 + (pulse_width_micro - _width1) * s1;
		if (ret < _min_angle)
			ret = _min_angle;
		else if (ret > _max_angle)
			ret = _max_angle;
		return ret;
	}

	uint32_t angleToPulseWidth(double angle_deg) {
		uint32_t ret = _width1 + (angle_deg - _angle1) * s2;
		if (ret < _min_width)
			ret = _min_width;
		else if (ret > _max_width)
			ret = _max_width;
		return ret;
	}

	uint32_t getIdlePulseWidth() const {
		return _idle_pulse;
	}

	const uint32_t& getMinPulseWidth() {
		return _min_width;
	}

	const uint32_t& getMaxPulseWidth() {
		return _max_width;
	}

protected:
	double s1, s2;
	double _min_angle, _max_angle;
	uint32_t _min_width, _max_width;
	uint32_t _idle_pulse;

	double _angle1, _angle2;
	uint32_t _width1, _width2;
};
#endif

