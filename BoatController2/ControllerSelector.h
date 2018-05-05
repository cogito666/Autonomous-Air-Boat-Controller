// ControllerSelector.h

#ifndef _CONTROLLERSELECTOR_h
#define _CONTROLLERSELECTOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

#include "BoatControllerInterface.h"

class ControllerSelector{
	enum {MAX_CONTROLLERS_NUMBER = 10};
public:
	ControllerSelector():num(0), selected(0){

	}

	void registerController(BoatControllerInterface *controller){
		if (num >= MAX_CONTROLLERS_NUMBER)
			return;
		controllers[num++] = controller;
	}

	size_t length(){
		return num;
	}

	bool selectController(const int& idx){
		if (idx < 0 || idx >= num){
			return false;
		}
		selected = idx;
		return true;
	}

	bool selectController(const String& name){
		for (int i = 0; i < num; i++){
			if (controllers[i]->getName().compareTo(name) == 0){
				return selectController(i);
			}
		}
		return false;
	}

	BoatControllerInterface* getCurrentController(){
		return controllers[selected];
	}

protected:
	int num;
	int selected;
	BoatControllerInterface * controllers[MAX_CONTROLLERS_NUMBER];
};
#endif

