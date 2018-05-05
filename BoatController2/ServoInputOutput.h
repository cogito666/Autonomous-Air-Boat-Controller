// ServoInputOutput.h

#ifndef _SERVOINPUTOUTPUT_h
#define _SERVOINPUTOUTPUT_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include <Servo.h>
#include "SafeServo.h"
#include "ServoUpdater.h"
#include "PWMWidthCounterIface.h"
#include "AnglePulseWidthMapper.h"

extern SafeServo* servo_rudder;
extern ServoUpdater* servo_throtle;
extern PWMWidthCounterIface * rf_signal_rudder;
extern PWMWidthCounterIface * rf_signal_throtle;
extern AnglePulseWidthMapper * rudder_angle_pulse_mapper;
extern AnglePulseWidthMapper * throtle_angle_pulse_mapper;

void initializeServos();

#endif

