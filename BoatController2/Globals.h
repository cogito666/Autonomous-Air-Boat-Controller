// Globals.h

#ifndef _GLOBALS_h
#define _GLOBALS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "BoatState.h"
#include "Point2D.h"
#include "MeanCalculator.h"
/////////////////////////////////////////////////////
//#define SerialClassType	HardwareSerial
#define SerialClassType UARTClass
/////////////////////////////////////////////////////
#define SerialAHRS			Serial3
#define SerialGPS			Serial2
#define SerialWireless		Serial1

#define RUNNING_INDICATOR_LED	14

#define XBEE_RESET_PIN					36
#define PWM_READER_RESET_PIN			35
#define SerialDebug_OUTPUT_ENALBE_PIN	37

#define RUDDER_SIGNAL_OFFSET	-90

#define PWM_READER_I2C_ADDR		8

#define RudderServoPin		8
#define ThrotleServoPin		9

#define WATCHDOG_TIMEOUT_MS	3500
extern BoatState boat_state;
extern MeanCalculator<Point2D, 30> home_position;
extern bool board_reset_required;
void initializeSerialCommunicatoins();

#endif

