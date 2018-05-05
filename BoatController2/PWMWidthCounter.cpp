// 
// 
// 

#include "PWMWidthCounter.h"

#define MAX_NUM_PWM_COUNTERS	4

namespace __pwm_counter_private{
	int _input_pins[MAX_NUM_PWM_COUNTERS] = { -1, };
	volatile int _width[MAX_NUM_PWM_COUNTERS] = { 0, };
	volatile bool _updated[MAX_NUM_PWM_COUNTERS] = { false, };
	int _target[MAX_NUM_PWM_COUNTERS];
	uint32_t _t0[MAX_NUM_PWM_COUNTERS];
	uint32_t _stamp[MAX_NUM_PWM_COUNTERS] = { 0, };

#define COUNTER_INTERRUPT_FUNCTION_TEMPLATE(num)	\
	void counter_interrupt_##num(){\
		uint32_t t = micros(); \
		if (digitalRead(_input_pins[num])==_target[num]){ \
			_t0[num] = t; \
		}\
		else{\
			uint32_t t1 = t;\
			if (_t0[num] > t1){\
				_width[num] = UINT32_MAX - _t0[num] + t1 + 1;\
			}\
			else{\
				_width[num] = t1 - _t0[num];\
			}\
			_stamp[num] = t1; \
			_updated[num] = true;\
		}\
	}

	COUNTER_INTERRUPT_FUNCTION_TEMPLATE(0)
	COUNTER_INTERRUPT_FUNCTION_TEMPLATE(1)
	COUNTER_INTERRUPT_FUNCTION_TEMPLATE(2)
	COUNTER_INTERRUPT_FUNCTION_TEMPLATE(3)
	//Add or remove COUNTER_INTERRUPT_FUNCTION_TEMPLATE(id) here depending on [MAX_NUM_PWM_COUNTERS]

	void(*counterInterrupt[MAX_NUM_PWM_COUNTERS])(void) = { 
		&counter_interrupt_0, 
		&counter_interrupt_1, 
		&counter_interrupt_2, 
		&counter_interrupt_3 };
	//Add or remove function pointer here  depending on [MAX_NUM_PWM_COUNTERS]
}

PWMWidthCounterClass pwmWidthCounter1;
PWMWidthCounterClass pwmWidthCounter2;
PWMWidthCounterClass pwmWidthCounter3;
PWMWidthCounterClass pwmWidthCounter4;
//Add or remove instances here  depending on [MAX_NUM_PWM_COUNTERS]

uint32_t PWMWidthCounterClass::n_instances = 0;

PWMWidthCounterClass::PWMWidthCounterClass() :_attached(false){
	_id = n_instances++;
}

bool PWMWidthCounterClass::updated() const{
	return __pwm_counter_private::_updated[_id];
}

void PWMWidthCounterClass::clearUpdated(){
	__pwm_counter_private::_updated[_id] = false;
}

uint32_t PWMWidthCounterClass::width() const{
	return __pwm_counter_private::_width[_id];
}

void PWMWidthCounterClass::attach(int pin, Modes mode){
	if (_attached){
		detach();
	}
	__pwm_counter_private::_input_pins[_id] = pin;

	switch (mode){
	case PULSE_WIDTH:
		__pwm_counter_private::_target[_id] = HIGH;
		break;
	case PULSE_WIDTH_INVERSE:
		__pwm_counter_private::_target[_id] = LOW;
		break;
	}
	__pwm_counter_private::_t0[_id] = 0;
	pinMode(__pwm_counter_private::_input_pins[_id], INPUT_PULLUP);
	attachInterrupt(digitalPinToInterrupt(__pwm_counter_private::_input_pins[_id]), __pwm_counter_private::counterInterrupt[_id], CHANGE);
	_attached = true;
}

void PWMWidthCounterClass::detach(){
	if (_attached){
		detachInterrupt(digitalPinToInterrupt(__pwm_counter_private::_input_pins[_id]));
		_attached = false;
	}
}

uint32_t PWMWidthCounterClass::stamp() const
{
	return __pwm_counter_private::_stamp[_id];
}
