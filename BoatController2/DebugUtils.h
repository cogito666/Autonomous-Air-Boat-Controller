#ifndef _DEBUG_UTILS_H_
#define _DEBUG_UTILS_H_

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif
#include <assert.h>
#define TIME_PRECISION_MILLIS	1
#define TIME_PRECISION_MICROS	2

#define TIME_PRECESION	TIME_PRECISION_MILLIS

#ifndef __DEBUG_UTILS_SERIAL
#define __DEBUG_UTILS_SERIAL					Serial
#endif

#if ENABLE_DEBUG_UTILS

	#if ENABLE_DEBUG_UTILS_SERIAL_LOGGER
		#define DEBUG_UTILS_MESSAGE(msg)					__DEBUG_UTILS_SERIAL.print(msg)
		#define DEBUG_UTILS_MESSAGELN(msg)					do{__DEBUG_UTILS_SERIAL.println(msg);__DEBUG_UTILS_SERIAL.flush();}while(0)
		#define DEBUG_UTILS_FUNCTION_STAMP()				__DEBUG_UTILS_SERIAL.print(__PRETTY_FUNCTION__)
		#define DEBUG_UTILS_FUNCTION_STAMPLN()				__DEBUG_UTILS_SERIAL.println(__PRETTY_FUNCTION__)
		#define DEBUG_UTILS_LINE_NUMBER_STAMP()				__DEBUG_UTILS_SERIAL.print(__LINE__)
		#define DEBUG_UTILS_LINE_NUMBER_STAMPLN()			__DEBUG_UTILS_SERIAL.println(__LINE__)
		#define DEBUG_UTILS_STAMP_FUNCTION_LINE_NUMBER()	do{__DEBUG_UTILS_SERIAL.print(__PRETTY_FUNCTION__" at ");\
															__DEBUG_UTILS_SERIAL.println(__LINE__);}while(0)
	#else
		#define DEBUG_UTILS_MESSAGE(msg)
		#define DEBUG_UTILS_MESSAGELN(msg)			
		#define DEBUG_UTILS_FUNCTION_STAMP()
		#define DEBUG_UTILS_FUNCTION_STAMPLN()		
		#define DEBUG_UTILS_LINE_NUMBER_STAMP()		
		#define DEBUG_UTILS_LINE_NUMBER_STAMPLN()	
		#define DEBUG_UTILS_STAMP_FUNCTION_LINE_NUMBER()
	#endif

	#if ENABLE_DEBUG_UTILS_ELAPSED_TIME_UTILS
		namespace __private_DEBUG_UTILS {class ElapsedTime;}
		#if TIME_PRECESION==TIME_PRECISION_MICROS
			#define __TIME_FUNC()	micros()
			#define __TIME_UNIT		"us"
		#else
			#define __TIME_FUNC()	millis()
			#define __TIME_UNIT		"ms"
		#endif
		
		/*elapsed time utils*/
		#define DEBUG_UTILS_ELAPSED_TIME_INIT()		__private_DEBUG_UTILS::ElapsedTime __elapsed_time
		#define DEBUG_UTILS_ELAPSED_TIME_BEGIN()	__elapsed_time.reset()
		#define DEBUG_UTILS_ELAPSED_TIME_END(msg)		do{__elapsed_time.update(false);\
													__DEBUG_UTILS_SERIAL.print(msg " : ");\
													__DEBUG_UTILS_SERIAL.print(__elapsed_time.get());\
													__DEBUG_UTILS_SERIAL.println(__TIME_UNIT);}while(0)
		/*
		named elapsed time utils
		*/
		#define DEBUG_UTILS_NAMED_ELAPSED_TIME_INIT(name)	__private_DEBUG_UTILS::ElapsedTime __named_elapsed_time_##name
		#define DEBUG_UTILS_NAMED_ELAPSED_TIME_RESET(name)	__named_elapsed_time_##name .reset()
		#define DEBUG_UTILS_NAMED_ELAPSED_TIME_PRINT(name, msg)	do{__named_elapsed_time_##name.update(false);\
			__DEBUG_UTILS_SERIAL.print(#name "[" msg "] : ");\
			__DEBUG_UTILS_SERIAL.print(__named_elapsed_time_##name.get()); \
			__DEBUG_UTILS_SERIAL.println(__TIME_UNIT);\
			}while (0)
		#define DEBUG_UTILS_NAMED_ELAPSED_TIME_PRINT_AND_RESET(name, msg)	do{__named_elapsed_time_##name.update(true);\
			__DEBUG_UTILS_SERIAL.print(#name "[" msg "] : "); \
			__DEBUG_UTILS_SERIAL.print(__named_elapsed_time_##name.get()); \
			__DEBUG_UTILS_SERIAL.println(__TIME_UNIT);\
			}while (0)
		#define DEBUG_UTILS_NAMED_ELAPSED_TIME_PRINT_IF(name, _cond, msg)	do{__named_elapsed_time_##name.update(false);\
			if(__named_elapsed_time_##name.get() _cond){\
				__DEBUG_UTILS_SERIAL.print(#name "[" msg "] : "); \
				__DEBUG_UTILS_SERIAL.print(__named_elapsed_time_##name.get()); \
				__DEBUG_UTILS_SERIAL.println(__TIME_UNIT);\
			}}while(0)
		#define DEBUG_UTILS_NAMED_ELAPSED_TIME_PRINT_IF_AND_RESET(name, _cond, msg)	do{__named_elapsed_time_##name.update(true);\
			if(__named_elapsed_time_##name.get() _cond){\
				__DEBUG_UTILS_SERIAL.print(#name "[" msg "] : "); \
				__DEBUG_UTILS_SERIAL.print(__named_elapsed_time_##name.get()); \
				__DEBUG_UTILS_SERIAL.println(__TIME_UNIT);\
			}}while(0)
	#else
		#define DEBUG_UTILS_ELAPSED_TIME_INIT()		
		#define DEBUG_UTILS_ELAPSED_TIME_BEGIN()	
		#define DEBUG_UTILS_ELAPSED_TIME_END(msg)	

		#define DEBUG_UTILS_NAMED_ELAPSED_TIME_INIT(name)
		#define DEBUG_UTILS_NAMED_ELAPSED_TIME_RESET(name)
		#define DEBUG_UTILS_NAMED_ELAPSED_TIME_PRINT_FROM_LAST_RESET(name, msg)
		#define DEBUG_UTILS_NAMED_ELAPSED_TIME_PRINT_AND_RESET(name, msg)
		#define DEBUG_UTILS_NAMED_ELAPSED_TIME_PRINT_IF(name, _exp, msg)	
		#define DEBUG_UTILS_NAMED_ELAPSED_TIME_PRINT_IF_AND_RESET(name, _cond, msg)
	#endif

#else
	#define DEBUG_UTILS_MESSAGE(msg)
	#define DEBUG_UTILS_MESSAGELN(msg)			
	#define DEBUG_UTILS_FUNCTION_STAMP()
	#define DEBUG_UTILS_FUNCTION_STAMPLN()		
	#define DEBUG_UTILS_LINE_NUMBER_STAMP()		
	#define DEBUG_UTILS_LINE_NUMBER_STAMPLN()	
	#define DEBUG_UTILS_STAMP_FUNCTION_LINE_NUMBER()

	#define DEBUG_UTILS_ELAPSED_TIME_INIT()		
	#define DEBUG_UTILS_ELAPSED_TIME_BEGIN()	
	#define DEBUG_UTILS_ELAPSED_TIME_END(msg)	

	#define DEBUG_UTILS_NAMED_ELAPSED_TIME_INIT(name)
	#define DEBUG_UTILS_NAMED_ELAPSED_TIME_RESET(name)
	#define DEBUG_UTILS_NAMED_ELAPSED_TIME_PRINT_FROM_LAST_RESET(name, msg)
	#define DEBUG_UTILS_NAMED_ELAPSED_TIME_PRINT_AND_RESET(name, msg)	
	#define DEBUG_UTILS_NAMED_ELAPSED_TIME_PRINT_IF(name, _exp, msg)	
	#define DEBUG_UTILS_NAMED_ELAPSED_TIME_PRINT_IF_AND_RESET(name, _cond, msg)

#endif

#if ENABLE_DEBUG_UTILS_ELAPSED_TIME_UTILS&&ENABLE_DEBUG_UTILS
namespace __private_DEBUG_UTILS {
	class ElapsedTime {
	public:
		ElapsedTime() {
			reset();
		}

		uint32_t get() const {
			return t_elapsed;
		}

		void update(bool begin_from_now){
			uint32_t t = __TIME_FUNC();
			t_elapsed = t - t_last_update;
			if (begin_from_now)
				reset();
		}

		void reset(){
			t_last_update = __TIME_FUNC();
		}
	private:
		uint32_t t_elapsed;
		uint32_t t_last_update;
	};
}
#endif

#endif