// ElapsedTime.h

#ifndef _ELAPSEDTIME_h
#define _ELAPSEDTIME_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class ElapsedTime{
public:
	/*
	first measurement will be measured from this constructor.
	*/
	ElapsedTime();
	
	/*
	*/
	virtual ~ElapsedTime(){}

	/*
	return lastly measured time-elapse.
	*/
	uint32_t get() const;

	/*
	*/
	void reset();
	
	/*
	measure elapsed time since the last call of update().
	*/
	void update(bool begin_from_now=true);
protected:
	/*
	change the unit of time by implementing this virtual method.
	*/
	virtual uint32_t getTime() = 0;
private:
	uint32_t t_elapsed;
	uint32_t t_last_update;
};

/*
Elapsed time is measured in milliseconds
*/
class ElapsedTimeMilli : public ElapsedTime{
public:
	ElapsedTimeMilli() { reset(); }
protected:
	uint32_t getTime(){
		return millis();
	}
};

/*
Elapsed time is measured in microseconds
*/
class ElapsedTimeMicro : public ElapsedTime{
public:
	ElapsedTimeMicro() { reset(); }
protected:
	uint32_t getTime(){
		return micros();
	}
};


#endif

