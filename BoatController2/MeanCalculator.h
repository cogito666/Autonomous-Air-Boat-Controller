#ifndef _MEAN_CALCULATOR_H_
#define _MEAN_CALCULATOR_H_


template <typename SampleType, size_t NSamples>
class MeanCalculator{
public:
	MeanCalculator(){
		reset();
	}

	void reset(){
		_idx = 0;
		_is_initialized = false;
		_count_samples = 0;
	}

	/*
	return true, if home position has been computed.
	For HomePosition to be initialized, 
	at least _N_POSE_SAMPLES samples are needed.
	*/
	bool isInitialized() const{
		return _is_initialized;
	}

	/*
	add a new pose sample for computing home position.
	If enough number of samples are added, mean position is computed and 
	home position starts being available.
	If the samples buffer is full, the oldest sample is replaced with the new one.
	*/
	void addSample(const SampleType& sample){
		_samples[_idx++] = sample;//add sample
		if (_idx == NSamples)//sample is added to array like circular array 
			_idx = 0;
		if (_count_samples < NSamples)
			_count_samples++;//samples count
		if (_count_samples >= NSamples) {
			computeMean();
			_is_initialized = true;
		}
	}

	const SampleType& getMeanValue() const{ 
		return _mean_val; 
	}
private:
	void computeMean(){
		_mean_val = SampleType();
		for(size_t i=0;i<_count_samples;i++){
			_mean_val += _samples[i];
		}
		_mean_val /= _count_samples;
	}
private:
	SampleType _mean_val;
	size_t _idx;
	size_t _count_samples;
	bool _is_initialized;
	SampleType _samples[NSamples];
};

#endif

