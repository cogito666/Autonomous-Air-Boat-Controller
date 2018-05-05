#ifndef PACKET_RC_READIGNS_H_
#define PACKET_RC_READIGNS_H_
#include "Serializable.h"

class PacketRCReadings :public Serializable {
public:
	const char* getTypeField() const {
		return "RCR";
	}

	int _rudder_servo_reading = 0;
	int _throtle_servo_reading = 0;
private:
	bool serializeImpl(SerializeHelper::PacketLoader& loader) {
		loader.load(String(_rudder_servo_reading));
		loader.load(String(_throtle_servo_reading));
		return true;
	}

	bool deserializeImpl(SerializeHelper::PacketUnloader& unloader) {
		String field; field.reserve(10);

		if (!unloader.unload(field)) return false;
		_rudder_servo_reading = field.toInt();

		if (!unloader.unload(field)) return false;
		_throtle_servo_reading = field.toInt();
		return true;
	}
};

#endif