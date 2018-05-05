#ifndef PACKET_RC_SIGNALS_H_
#define PACKET_RC_SIGNALS_H_
#include "Serializable.h"

class PacketRCSignals :public Serializable {
public:
	enum SignalSources{
		RC_Servo_Rudder = 1,
		RC_Servo_Throtle = 2
	};
	const char* getTypeField() const {
		return "RCSig";
	}

	SignalSources _sig_source;
	int _servo_micros = 0;
private:
	bool serializeImpl(SerializeHelper::PacketLoader& loader) {
		loader.load(String((int)_sig_source));
		loader.load(String(_servo_micros));
		return true;
	}

	bool deserializeImpl(SerializeHelper::PacketUnloader& unloader) {
		String field; field.reserve(10);
		int tmp;
		if (!unloader.unload(field)) return false;
		tmp = field.toInt();
		switch(tmp){
		case RC_Servo_Rudder:case RC_Servo_Throtle:
			_sig_source = (SignalSources)tmp;
			break;
		default:
			return false;
		}

		if (!unloader.unload(field)) return false;
		_servo_micros = field.toInt();
		return true;
	}
};

#endif