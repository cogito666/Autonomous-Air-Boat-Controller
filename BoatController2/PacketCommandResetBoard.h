#ifndef PACKET_COMMAND_RESET_BOARD_H_
#define PACKET_COMMAND_RESET_BOARD_H_
#include "Serializable.h"
class PacketCommandResetBoard : public Serializable{
public:
	const char* getTypeField() const {
		return "RB";
	}

	bool serializeImpl(SerializeHelper::PacketLoader& loader) {
		return true;
	}

	bool deserializeImpl(SerializeHelper::PacketUnloader& unloader) {
		return true;
	}
};
#endif