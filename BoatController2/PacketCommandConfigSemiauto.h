#ifndef PACKET_COMMAND_CONFIG_SEMIAUTO_H_
#define PACKET_COMMAND_CONFIG_SEMIAUTO_H_
#include "Serializable.h"
class PacketCommandConfigSemiauto : public Serializable{
public:
	enum ActionType{
		SetParam = 1
	};

	enum TargetParams{
		PGain = 1,
		DGain = 2,
		IGain = 3,
		LookAheadDistance = 4
	};

	const char* getTypeField() const{
		return "CSA";
	}

	ActionType _action;
	//SetParam
	TargetParams _SetParam_target_param;
	double _SetParam_data;
	//End SetParam
private:
	bool serializeImpl(SerializeHelper::PacketLoader& loader){
		switch(_action){
		case SetParam:
			loader.load(String(_action));
			loader.load(String(_SetParam_target_param));
			loader.load(String(_SetParam_data, 3));
			break;
		default:
			return false;
		}
		
		return true;
	}

	bool deserializeImpl(SerializeHelper::PacketUnloader& unloader) {
		String field; field.reserve(10);

		if (!unloader.unload(field)) return false;
		int action = field.toInt();
		switch(action){
		case SetParam:
			if(!unloadSetParam(unloader, field)){
				return false;
			}
			break;
		default:
			return false;
		}
	
		return true;
	}

private:
	bool unloadSetParam(SerializeHelper::PacketUnloader& unloader, String& field){
		//unload target param
		if (!unloader.unload(field)) return false;
		int target_param = field.toInt();
		switch (target_param) {
		case PGain:case DGain:case IGain:case LookAheadDistance:
			_SetParam_target_param = (TargetParams)target_param;
			break;
		default://undefined target param
			return false;
		}
		//unload data
		if (!unloader.unload(field)) return false;
		_SetParam_data = field.toFloat();
		return true;
	}

};

#endif