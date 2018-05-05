// TimeoutTimer.h

#ifndef _TIMEOUTTIMER_h
#define _TIMEOUTTIMER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class TimerTimeout
{
public:
	typedef void(*TimeoutCallback)(const uint32_t&);

	TimerTimeout(uint32_t duration, TimeoutCallback callback, bool auto_reset)
	:_callback(callback), _auto_reset(auto_reset){
		setDuration(duration);
	}

	virtual ~TimerTimeout(){}

	void setDuration(uint32_t duration){
		_duration = duration;
	}

	/*
	elapsed time will be counted from now.
	*/
	void reset(){
		_t_start = getTime();
		_callback_called = false;
	}

	bool spinOnce(){
		uint32_t t_now = getTime();
		if(t_now - _t_start >= _duration){
			if (!_callback_called) {
				if (_callback)
					_callback(t_now);
				_callback_called = true;
			}
			if (_auto_reset)
				reset();
			return true;
		}
		return false;
	}
protected:
	/*
	The resolution of timeout will depend on the resolution of getTime() implementation.
	*/
	virtual uint32_t getTime() = 0;
	uint32_t _t_start;
	uint32_t _duration;
	TimeoutCallback _callback;
	bool _callback_called;
	bool _auto_reset;
};


class TimerTimeoutMilli:public TimerTimeout{
public:
	TimerTimeoutMilli(uint32_t duration_milli, TimeoutCallback callback = NULL, bool auto_reset = false) 
		:TimerTimeout(duration_milli,callback, auto_reset){
		reset();
	}
protected:
	uint32_t getTime(){
		return millis();
	}
};

class TimerTimeoutMicro :public TimerTimeout{
public:
	TimerTimeoutMicro(uint32_t duration_micro, TimeoutCallback callback = NULL, bool auto_reset = false) 
		:TimerTimeout(duration_micro, callback, auto_reset) {
		reset();
	}
protected:
	uint32_t getTime(){
		return micros();
	}
};

#endif

