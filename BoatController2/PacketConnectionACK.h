#ifndef PACKET_CONNECTION_ACK_H_
#define PACKET_CONNECTION_ACK_H_
#include "Serializable.h"

class PacketConnectionACK :public Serializable {
public:
	PacketConnectionACK(uint32_t pack_count) :_pack_count(pack_count) {

	}
	// Cmd type maker
	const char* getTypeField() const {
		return "CNA";
	}

	// make Cmd using loader
	bool serializeImpl(SerializeHelper::PacketLoader& loader) {
		loader.load(String(_pack_count));
		return true;
	}

	// parse Msg from boat
	bool deserializeImpl(SerializeHelper::PacketUnloader& unloader) {
		String field;
		if (!unloader.unload(field)) return false;
		_pack_count = field.toInt();
		return true;
	}

	uint32_t _pack_count;
};
#endif