#ifndef PACKET_COMMAND_CHANGE_CONTROL_MODE_H_
#define PACKET_COMMAND_CHANGE_CONTROL_MODE_H_
#include "Serializable.h"
#include "BoatState.h"

class PacketCommandChangeControlMode :public Serializable {
public:
	const char* getTypeField() const {
		return "CCM";
	}

	bool serializeImpl(SerializeHelper::PacketLoader& loader) {
		loader.load(String((int)_target_ctrl_mode));
		return true;
	}

	bool deserializeImpl(SerializeHelper::PacketUnloader& unloader) {
		String field;
		if (!unloader.unload(field)) { return false; }
		int mode = field.toInt();
		switch (mode) {
		case 	BoatCtrlMode_Manual: case BoatCtrlMode_SemiAutoControl:
			_target_ctrl_mode = (BoatControlModes)mode;
			break;
		default:
			return false;
		}
		return true;
	}

	BoatControlModes _target_ctrl_mode;
};
#endif