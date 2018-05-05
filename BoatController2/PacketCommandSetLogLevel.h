#ifndef PACKET_COMMAND_SET_LOG_LEVEL_H_
#define PACKET_COMMAND_SET_LOG_LEVEL_H_
#include "Serializable.h"
#include "Logger.h"
class PacketCommandSetLogLevel :public Serializable {
public:
	enum LogLevels {//make sure enums are same on receiver
		LOG_VERBOSE = 0,
		LOG_DEBUG,
		LOG_LOGGING,
		LOG_INFO,
		LOG_ERROR
	};

	PacketCommandSetLogLevel(){
	}

	PacketCommandSetLogLevel(const LogLevels& target_log_level)
	:_target_log_level(target_log_level){
	}

	const char* getTypeField() const {
		return "SLL";
	}

	bool serializeImpl(SerializeHelper::PacketLoader& loader) {
		loader.load(String(_target_log_level));
		return true;
	}

	bool deserializeImpl(SerializeHelper::PacketUnloader& unloader) {
		String field;
		unloader.unload(field);
		_target_log_level = (LogLevels)field.toInt();
		return true;
	}

	LogLevels _target_log_level;
};
#endif