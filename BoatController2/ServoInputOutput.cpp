// 
// 
// 

#include "ServoInputOutput.h"
#include "PWMWidthCounterI2C.h"
#include "PWMWidthCounterSerial.h"
#include "Globals.h"

#define ThrotleMinPWM		750
#define ThrotleMaxPWM		2250

SafeServo* servo_rudder = NULL;
ServoUpdater* servo_throtle = NULL;
PWMWidthCounterIface * rf_signal_rudder = NULL;
PWMWidthCounterIface * rf_signal_throtle = NULL;
AnglePulseWidthMapper * rudder_angle_pulse_mapper = NULL;
AnglePulseWidthMapper * throtle_angle_pulse_mapper = NULL;//TODO : cannot use AnglePulseWidthMapper for throtle servo.

void initializeServos()
{
#if 0
	rf_signal_rudder = new PWMWidthCounterI2C(&Wire, PWM_READER_I2C_ADDR, 0);
	rf_signal_throtle = new PWMWidthCounterI2C(&Wire, PWM_READER_I2C_ADDR, 1);
#else
	//Serial
	PWMWidthCounterSerial * _rf_signal_rudder = new PWMWidthCounterSerial(0, true, &Serial);
	PWMWidthCounterSerial* _rf_signal_throtle = new PWMWidthCounterSerial(1, false, &Serial);
	_rf_signal_rudder->registerCounter(_rf_signal_throtle);
	rf_signal_rudder = _rf_signal_rudder;
	rf_signal_throtle = _rf_signal_throtle;
#endif

	rudder_angle_pulse_mapper = new AnglePulseWidthMapper();
	//rudder_angle_pulse_mapper->init(DEG_TO_RAD * -60, DEG_TO_RAD * 60, 1050, 1850);
	rudder_angle_pulse_mapper->init(DEG_TO_RAD * -60, DEG_TO_RAD * 60, 1100, 1900);

	servo_rudder = new SafeServo();
	servo_rudder->attach(RudderServoPin, rudder_angle_pulse_mapper->getMinPulseWidth() + RUDDER_SIGNAL_OFFSET, rudder_angle_pulse_mapper->getMaxPulseWidth() + RUDDER_SIGNAL_OFFSET);
	servo_rudder->writeMicroseconds(rudder_angle_pulse_mapper->getIdlePulseWidth() + RUDDER_SIGNAL_OFFSET);

	servo_throtle = new ServoUpdater();
	throtle_angle_pulse_mapper = new AnglePulseWidthMapper();
	throtle_angle_pulse_mapper->init(DEG_TO_RAD * -90, DEG_TO_RAD * 90, ThrotleMinPWM, ThrotleMaxPWM);
	servo_throtle->attach(ThrotleServoPin,
		throtle_angle_pulse_mapper->getMinPulseWidth(),
		throtle_angle_pulse_mapper->getMaxPulseWidth());
	servo_throtle->writeMicroseconds(throtle_angle_pulse_mapper->getIdlePulseWidth());
}
