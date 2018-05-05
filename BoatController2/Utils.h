// Utils.h

#ifndef _UTILS_h
#define _UTILS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

bool isNumeric(const String& val);
bool isInteger(const String& val);

template <typename T>
int find_from_first(T* val, const T& target, int len){
	for (int i = 0; i < len; i++){
		if (*(val + i) == target){
			return i;
		}
	}
	return -1;
}

template <typename T>
int find_from_last(T* val, const T& target, int len){
	for (int i = len-1; i >= 0; i--){
		if (*(val + i) == target){
			return i;
		}
	}
	return -1;
}

#endif

