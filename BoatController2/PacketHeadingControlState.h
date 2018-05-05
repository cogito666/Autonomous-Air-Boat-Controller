// PacketHeadingControlState.h

#ifndef _PACKETHEADINGCONTROLSTATE_h
#define _PACKETHEADINGCONTROLSTATE_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Serializable.h"
class PacketHeadingControlState :public Serializable {
public:
	PacketHeadingControlState() {

	}

	PacketHeadingControlState(const double& des_heading, const double& actual_heading, const bool& auto_mode) 
	:_des_heading(des_heading), _actual_heading(actual_heading),_auto_mode(auto_mode){

	}

	double _des_heading, _actual_heading;
	bool _auto_mode;
private:
	const char* getTypeField() const {
		return "HCS";
	}

	bool serializeImpl(SerializeHelper::PacketLoader& loader) {
		loader.load(_auto_mode ? "auto" : "manual");
		loader.load(String(_des_heading, 3));
		loader.load(String(_actual_heading, 3));
		return true;
	}

	bool deserializeImpl(SerializeHelper::PacketUnloader& unloader) {
		String field; field.reserve(20);
		if (!unloader.unload(field))return false;
		if (field.compareTo("auto") == 0) {
			_auto_mode = true;
		}
		else if (field.compareTo("manual") == 0) {
			_auto_mode = false;
		}
		else {
			return false;
		}
		if (!unloader.unload(field))return false;
		_des_heading = field.toFloat();
		if (!unloader.unload(field))return false;
		_actual_heading = field.toFloat();
		return true;
	}
};
#endif

