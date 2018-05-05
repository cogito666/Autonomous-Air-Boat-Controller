#ifndef PACKET_COMMAND_PUMP_CONTROL_H_
#define PACKET_COMMAND_PUMP_CONTROL_H_
#include "Serializable.h"
class PacketCommandPumpControl :public Serializable {
public:
	const char* getTypeField() const {
		return "PC";
	}
	bool serializeImpl(SerializeHelper::PacketLoader& loader) {
		loader.load(String(_on_off));
		return true;
	}
	bool deserializeImpl(SerializeHelper::PacketUnloader& unloader) {
		String field;
		if (!unloader.unload(field)) return false;
		if (field.toInt())
			_on_off = true;
		else
			_on_off = false;
		return true;
	}

	bool _on_off;
};
#endif