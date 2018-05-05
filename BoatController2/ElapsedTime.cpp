// 
// 
// 

#include "ElapsedTime.h"
ElapsedTime::ElapsedTime(){
	t_elapsed = 0;
}

uint32_t ElapsedTime::get() const
{
	return t_elapsed;
}

/*
*/

void ElapsedTime::reset() {
	t_last_update = getTime();
}

void ElapsedTime::update(bool begin_from_now)
{
	uint32_t t = getTime();
	t_elapsed = t - t_last_update;
	if(begin_from_now)
		t_last_update = t;
}