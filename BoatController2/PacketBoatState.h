#ifndef PACKET_BOAT_STATE_H_
#define PACKET_BOAT_STATE_H_
#include "Serializable.h"
#include "BoatState.h"
class PacketBoatState:public Serializable{
public:
	PacketBoatState(BoatState* state):_state(state){

	}

	void setBoatState(BoatState * state){
		_state = state;
	}

	const char* getTypeField() const {
		return "BS";
	}

	bool serializeImpl(SerializeHelper::PacketLoader& loader){
		if (!_state)	return false;
		loader.load(String(_state->_stamp));
		loader.load(String((int)_state->_ctrl_mode));
		loader.load(String((int)_state->_controller_state));
		loader.load(String(_state->_pos._x, 3));//x
		loader.load(String(_state->_pos._y, 3));//y
		loader.load(String(_state->_pos._theta, 4));//yaw
		loader.load(String(_state->_gps_num_satellites));//number of satellites
		return true;
	}

	bool deserializeImpl(SerializeHelper::PacketUnloader& unloader){
		if (!_state)	return false;
		String field;
		if (!unloader.unload(field)) { return false; }
		_state->_stamp = field.toInt();
		if (!unloader.unload(field)) { return false; }
		_state->_ctrl_mode = (BoatControlModes)field.toInt();
		if (!unloader.unload(field)) { return false; }
		_state->_controller_state = field.toInt();
		if (!unloader.unload(field)) { return false; }
		_state->_pos._x = field.toFloat();
		if (!unloader.unload(field)) { return false; }
		_state->_pos._y = field.toFloat();
		if (!unloader.unload(field)) { return false; }
		_state->_pos._theta = field.toFloat();
		if (!unloader.unload(field)) { return false; }
		_state->_gps_num_satellites = field.toInt();
		return true;
	}
private:
	BoatState * _state = NULL;
};
#endif