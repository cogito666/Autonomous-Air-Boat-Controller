#ifndef MOVING_AVERAGE_FILTER
#define MOVING_AVERAGE_FILTER
#include "DataFilter.h"
#include "CircularBuffer.h"
namespace DataFilters {
	template <typename T>
	class MovingAverageFilter :public DataFilter<T> {
	public:
		enum {
			DEFAULT_SAMPLES_SIZE = 5,
			MAX_SAMPLES_SIZE = 50
		};
		typedef typename  DataFilter<T>::DataType DataType;

		MovingAverageFilter(const int& n_samples = DEFAULT_SAMPLES_SIZE) {
			this->setSamplesSize(n_samples);
		}

		double filter(const DataType& new_data) {
			_buffer.push_back(new_data);
			if(_buffer.size() <= _n_samples){
				_filtered = _filtered * (_buffer.size() - 1) + new_data;
				_filtered = _filtered / _buffer.size();
			}else{
				_filtered = _filtered - (_buffer.front() - 1.0*new_data) / _n_samples;
				_buffer.pop_front();
			}
			return _filtered;
		}

		double filtered() const {
			return _filtered;
		}

		void reset() {
			_buffer.clear();
			_filtered = DataType();
		}

	private:
		void setSamplesSize(const int& n_samples) {
			if (n_samples > MAX_SAMPLES_SIZE)
				_n_samples = MAX_SAMPLES_SIZE;
			else if (n_samples == 0)
				_n_samples = DEFAULT_SAMPLES_SIZE;
			else
				_n_samples = n_samples;
		}
	protected:
		int _n_samples;
		CircularBuffer<T, MAX_SAMPLES_SIZE+1> _buffer;
		double _filtered;
	};
}
#endif