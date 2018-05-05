#include "DeserializeManager.h"
#define ENABLE_DEBUG_UTILS 0
#define ENABLE_DEBUG_UTILS_SERIAL_LOGGER 1
#include "DebugUtils.h"
void DeserializeManager::deserialize(const String & data) {
	int start_idx = 0;
	const size_t max_n_packet = 5;
	size_t packet_count = 0;
	while (true) {
		if (++packet_count >= max_n_packet) {
			DEBUG_UTILS_MESSAGELN("Max num packet reached.");
			break;
		}
		int i_newline = data.indexOf("\n", start_idx);
		if (i_newline < 0) {
			_buffer += data.substring(start_idx);
			return;
		}
		_buffer += data.substring(start_idx, i_newline);
		_buffer.trim();
		DEBUG_UTILS_MESSAGE("deserializing...");
		DEBUG_UTILS_MESSAGELN(_buffer);
		String type_field = Serializable::ExtractTypeField(_buffer);
		Serializable * msg = getMessage(type_field);
		if (msg == NULL || !msg->isValid(_buffer) || !msg->deserialize(_buffer)) {
			if (_deserialize_fail) {
				DEBUG_UTILS_MESSAGELN("Deserialize Failed");
				_deserialize_fail(msg, _buffer);
			}
			_buffer = "";
			start_idx = i_newline + 1;
			continue;
		}

		if (_deserialize_success) {
			DEBUG_UTILS_MESSAGELN("Deserialize Succeeded");
			_deserialize_success(msg, _buffer);
		}
		_buffer = "";
		start_idx = i_newline + 1;
	}
}

bool DeserializeManager::registerMessage(Serializable * message) {
	if (_count >= MAX_MESSAGES_NUM)
		return false;
	for (size_t i = 0; i<_count; i++) {
		if (strcmp(_message[i]->getTypeField(), message->getTypeField()) == 0) {
			return false;
		}
	}
	_message[_count++] = message;
	return true;
}

Serializable * DeserializeManager::getMessage(const String & type_field) {
	for (size_t i = 0; i<_count; i++) {
		if (type_field.compareTo(_message[i]->getTypeField()) == 0) {
			return _message[i];
		}
	}
	return NULL;
}
