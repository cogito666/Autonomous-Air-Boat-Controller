// DataFilter.h

#ifndef _DATAFILTER_h
#define _DATAFILTER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
namespace DataFilters {
	template <typename T>
	class DataFilter {
	public:
		typedef T DataType;

		/**
		* @brief apply filter to \p new_data and return filtered value.
		* @details ...
		* @param new data to be filtered
		* @return filtered data
		*/
		virtual double filter(const DataType& new_data) = 0;

		/**
		* @brief return lastly filtered value
		* @return lastly filtered value
		*/
		virtual double filtered() const = 0;

		/**
		* @brief clear all internal data and restart the filter
		*/
		virtual void reset() = 0;
	};
}
#endif

