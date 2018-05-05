// SerializeManager.h

#ifndef _DESERIALIZEMANAGER_h
#define _DESERIALIZEMANAGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Serializable.h"

#define MAX_MESSAGES_NUM			30

typedef void(*onDeserializeSuccessCallback)(Serializable*, const String&);

/*
When appropriate Serializable instance was not found, NULL will be passed.
If it's not NULL, user can check error-code by calling getLastError().
*/
typedef void(*onDeserializeFailureCallback)(const Serializable*, const String&);

class DeserializeManager{
public:
	DeserializeManager(){
		//reserve (probably) enough buffer memory to prevent'fragmented heap'
		_buffer.reserve(512); 
	}

	void onDeserializeSuccess(onDeserializeSuccessCallback callback){
		_deserialize_success = callback;
	}

	void onDeserializeFailure(onDeserializeFailureCallback callback){
		_deserialize_fail = callback;
	}

	void deserialize(const String& data);

	bool registerMessage(Serializable* message);
	
	Serializable* getMessage(const String& type_field);
protected:
	size_t _count = 0;
	Serializable* _message[MAX_MESSAGES_NUM];
	onDeserializeSuccessCallback _deserialize_success = NULL;
	onDeserializeFailureCallback _deserialize_fail = NULL;
	String _buffer;
};

#endif

