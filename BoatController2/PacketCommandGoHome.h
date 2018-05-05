#ifndef PACKET_COMMAND_GO_HOME_H_
#define PACKET_COMMAND_GO_HOME_H_
#include "Serializable.h"
class PacketCommandGoHome :public Serializable {
public:
	const char* getTypeField() const {
		return "GH";
	}
	bool serializeImpl(SerializeHelper::PacketLoader& loader) {
		return true;
	}
	bool deserializeImpl(SerializeHelper::PacketUnloader& unloader) {
		return true;
	}
};
#endif