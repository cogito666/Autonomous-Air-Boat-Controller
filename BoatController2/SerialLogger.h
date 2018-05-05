// SerialLogger.h

#ifndef _SERIALLOGGER_h
#define _SERIALLOGGER_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif
#include "Logger.h"
#include "Globals.h"
class SerialLogger :public IfaceLogger{
public:
	SerialLogger(SerialClassType* serial) :m_serial(serial){

	}

	virtual ~SerialLogger(){

	}

	void write(const char* msg){
		m_serial->print(msg);
	}

	void flush(){
		m_serial->flush();
	}
protected:
	SerialClassType* m_serial;
};
#endif

