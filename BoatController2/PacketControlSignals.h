#ifndef PACKET_CONTROL_SIGNALS_H_
#define PACKET_CONTROL_SIGNALS_H_
#include "Serializable.h"

class PacketControlSignals:public Serializable {
public:
	const char* getTypeField() const{
		return "CS";
	}

	int _rudder_servo_micros = 0;
	int _throtle_servo_micros = 0;
private: 
	bool serializeImpl(SerializeHelper::PacketLoader& loader) {
		loader.load(String(_rudder_servo_micros));
		loader.load(String(_throtle_servo_micros));
		return true;
	}

	bool deserializeImpl(SerializeHelper::PacketUnloader& unloader){
		String field; field.reserve(10);

		if (!unloader.unload(field)) return false;
		_rudder_servo_micros = field.toInt();

		if (!unloader.unload(field)) return false;
		_throtle_servo_micros = field.toInt();
		return true;
	}	
};

#endif