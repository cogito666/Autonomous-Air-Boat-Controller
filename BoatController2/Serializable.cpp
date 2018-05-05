// 
// 
// 

#include "Serializable.h"

namespace SerializeHelper{

	PacketLoader::PacketLoader(String& data)
		:_data(data){
	}

	void PacketLoader::load(const String& field){
		_data = _data + String(SIMPLE_PACKET_SEPARATOR) + field;
	}

	PacketUnloader::PacketUnloader(const String& data, const size_t& idx_from)
		:_data(data), _idx_from(idx_from), _idx(idx_from){
		_idx_end = _data.length() - 1;
	}

	PacketUnloader::PacketUnloader(const String& data, const size_t& idx_from, const size_t& idx_end)
		: _data(data), _idx_from(idx_from), _idx_end(idx_end), _idx(idx_from){
		if (_idx_end >= data.length()){
			_idx_end = data.length() - 1;
		}
	}

	bool PacketUnloader::unload(String& field){
		if (_idx > _idx_end)
			return false;
		int i = _data.indexOf(SIMPLE_PACKET_SEPARATOR, _idx);
		if (i > _idx_end)//out of searching region.
			i = -1;
		if (i < 0){
			if (_idx < _idx_end){
				//There's no more separator in the packet
				//This means it is the last field.
				field = _data.substring(_idx, _idx_end + 1);
				_idx = _idx_end + 1;
				return true;
			}
			//There's no field found.
			_idx = _idx_end + 1;
			return false;
		}
		field = _data.substring(_idx, i);
		_idx = i + 1;
		return true;
	}
}

String Serializable::ExtractTypeField(const String& packet){
	SerializeHelper::PacketUnloader unloader(packet, 1, packet.length() - 3);
	String type_field;
	if (!unloader.unload(type_field)){
		return String("");
	}
	return type_field;
}

bool Serializable::serialize(String& packet){
	packet = String(SIMPLE_PACKET_STARTER) + getTypeField();	//Starting character + type_field
	SerializeHelper::PacketLoader loader(packet);
	if (!serializeImpl(loader)){						//User defined data fields
		return setLastError(UserSerialize_Error) == No_Error;
	}
	loader.load(computeCheckSum(packet, 0, packet.length()));//Check-sum field.
	return setLastError(No_Error) == No_Error;
}

bool Serializable::isValid(const String& packet){
	if (packet.length() <= 3){
		return false;
	}
	if (packet[0] != SIMPLE_PACKET_STARTER){
		return false;
	}
	return true;
}

bool Serializable::deserialize(const String& packet){
	//test check-sum
	if (packet.substring(packet.length() - 2) != computeCheckSum(packet, 0, packet.length() - 3)) {
		clearUpdated();
		return setLastError(CheckSum_Not_Match) == No_Error;
	}

	//exclude the starting character and check-sum fields from the unloading process.
	SerializeHelper::PacketUnloader unloader(packet, 1, packet.length() - 3);
	String type_field;
	if (!unloader.unload(type_field)){
		clearUpdated();
		return setLastError(TypeField_Extract_Failed) == No_Error;
	}
	if (type_field.compareTo(getTypeField()) != 0){
		clearUpdated();
		return setLastError(TypeField_Not_Match) == No_Error;
	}
	if (!deserializeImpl(unloader)){
		clearUpdated();
		return setLastError(UserDeserialize_Error) == No_Error;
	}
	setUpdated();
	return setLastError(No_Error) == No_Error;
}

String Serializable::computeCheckSum(const String& data, const size_t& idx_from, const size_t& idx_end){
	char check = 0;
	for (size_t i = idx_from; i < idx_end; i++){
		check ^= data[i];
	}
	return toHex(check);
}

String Serializable::toHex(char val){
	static char _hex[16] = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F' };
	String ret("00");
	ret[0] = _hex[val >> 4];
	ret[1] = _hex[val & 0x0f];
	return ret;
}
