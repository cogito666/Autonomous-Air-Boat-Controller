// 
// 
// 

#include "BoatControllers.h"
#include "Logger.h"
#include "Globals.h"
BoatControllerManual* boat_controller_manual = NULL;
BoatControllerSemiauto* boat_controller_semiauto = NULL;

ControllerSelector* boat_controller_selector = NULL;

void initializeBoatControllers()
{
	boat_controller_selector = new ControllerSelector();
	loggers.logln("Creating Manual Controller", LoggerManager::LOG_INFO);
	boat_controller_manual = new BoatControllerManual("manual");
	loggers.logln("Creating Semi-Auto Controller", LoggerManager::LOG_INFO);
	boat_controller_semiauto = new BoatControllerSemiauto("semiauto");
	
	loggers.logln("Registering Controllers", LoggerManager::LOG_INFO);
	boat_controller_selector->registerController(boat_controller_manual);
	boat_controller_selector->registerController(boat_controller_semiauto);
	
	loggers.logln("Selecting Manual Controller", LoggerManager::LOG_INFO);
	boat_controller_selector->selectController("manual");
	boat_state._ctrl_mode = BoatControlModes::BoatCtrlMode_Manual;
}
