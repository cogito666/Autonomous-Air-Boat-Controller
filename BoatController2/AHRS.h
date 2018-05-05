// AHRS.h

#ifndef _AHRS_h
#define _AHRS_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "arduino.h"
#else
	#include "WProgram.h"
#endif

namespace sensor{
	class AHRS{
	public:
		enum PacketState{
			PACKET_PARSING_DOING = 1,
			PACKET_PARSING_ERROR ,
			PACKET_PARSING_COMPLETE
		};

		/*
		*/
		AHRS(){
			m_err_msg[0] = NULL;
		}

		virtual ~AHRS(){
		}

		/*
		1바이트씩 데이터를 읽으면서 packet parsing 진행.
		만약 parsing이 완료되면 m_is_updated를 true로 set하고 true를 반환.
		parsing이 완료되지 않으면 false를 반환.
		
		주의 : 
			packet parsing이 완료된 후 데이터를 읽지 않고 계속 encode를 수행할 경우
			기존의 데이터를 잃게 됨.
		Return 
			PACKET_PARSING_DOING : Currently parsing, no error.
			PACKET_PARSING_ERROR : Error is detected during parsing. The detail about the error can be seen by calling getErrorMessage(). Sub-class of this interface should fill m_err_msg buffer with the relevant error message.
			PACKET_PARSING_COMPLETE : Parsing has been completed. Reading orientation is available.
		*/
		virtual PacketState encode(const char& c) = 0;

		virtual bool updated() const = 0;
		virtual void clearUpdated() = 0;
		/*
		return lastly acquired orientation in RPY representation
		*/
		virtual void getRPY(double& R, double& P, double& Y) = 0;

		/*
		Optional implementations
		*/
		virtual bool getAcceleration(double& ax, double& ay, double& az){
			return false;
		}

		/*
		Optional implementations
		*/
		virtual bool getMagnetometer(double& mx, double& my, double& mz){
			return false;
		}

		/*
		Optional implementations
		*/
		virtual bool getGyro(double& gx, double& gy, double& gz){
			return false;
		}

		/*
		Optional implementations
		*/
		virtual bool getTemperature(double& deg){
			return false;
		}

		/*
		packet parsing에 사용된 모든 내부 데이터들을 reset.
		*/
		virtual void reset() = 0;

		/*
		*/
		const char* getErrorMessage(){
			return m_err_msg;
		}
	protected:
		char m_err_msg[50];
	};
}

#endif

