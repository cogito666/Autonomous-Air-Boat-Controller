// BoatControllers.h

#ifndef _BOATCONTROLLERS_h
#define _BOATCONTROLLERS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "BoatControllerManual.h"
#include "BoatControllerSemiauto.h"
#include "ControllerSelector.h"

extern ControllerSelector* boat_controller_selector;
extern BoatControllerManual* boat_controller_manual;
extern BoatControllerSemiauto* boat_controller_semiauto;

void initializeBoatControllers();
#endif

