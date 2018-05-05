// SensorSerialParse.h

#ifndef _SENSORSERIALPARSE_h
#define _SENSORSERIALPARSE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "AHRS.h"
#include "TinyGPS++.h"
#include "FrequencyEstimator.h"
#include "Globals.h"
extern FrequencyEstimator freq_ahrs;
extern FrequencyEstimator freq_gpgga;

void pollAHRS(SerialClassType* serial, sensor::AHRS* ahrs);
void pollGPS(SerialClassType* serial, TinyGPSPlus* gps_parser);

#endif

