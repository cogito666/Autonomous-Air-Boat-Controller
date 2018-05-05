#ifndef PACKET_COMMAND_TURN_H_
#define PACKET_COMMAND_TURN_H_
#include "Serializable.h"
class PacketCommandTurn :public Serializable {
public:
	enum Directions{
		INVALID = 0,
		Left = 1,
		Right = 2,
	};

	const char* getTypeField() const {
		return "TRN";
	}

	bool serializeImpl(SerializeHelper::PacketLoader& loader) {
		loader.load(String((int)_direction));
		loader.load(String(_diameters, 3));
		return true;
	}

	bool deserializeImpl(SerializeHelper::PacketUnloader& unloader) {
		String field;
		if (!unloader.unload(field)) {
			return false; 
		}
		int dir = field.toInt();
		switch(dir){
		case Left:case Right:
			_direction = (Directions)dir;
			break;
		default:
			_direction = INVALID;
		}
		if (!unloader.unload(field)) { 
			return false; 
		}
		_diameters = field.toFloat();
		return true;
	}

	Directions _direction;
	double _diameters;
};
#endif