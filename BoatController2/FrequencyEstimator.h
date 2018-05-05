// FrequencyEstimator.h

#ifndef _FREQUENCYESTIMATOR_h
#define _FREQUENCYESTIMATOR_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

class FrequencyEstimator{
public:
	FrequencyEstimator(const uint32_t& period_milli=1000){
		period = period_milli;
		reset();
	}

	void reset(){
		freq = 0.0;
		t_begin = t_last = millis();
		count = 0;
	}

	void checkout(const int& cnt = 1){
		t_last = millis();
		count += cnt;
		if (t_last - t_begin >= period){
			freq = count / (0.001*period);
			t_begin = t_last;
			count = 0;
		}
	}

	const double& getFrequency() const{
		return freq;
	}

protected:
	uint32_t count;
	uint32_t t_begin, t_last;
	uint32_t period;
	double freq;
};

#endif

