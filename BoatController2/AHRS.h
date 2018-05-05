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
		1����Ʈ�� �����͸� �����鼭 packet parsing ����.
		���� parsing�� �Ϸ�Ǹ� m_is_updated�� true�� set�ϰ� true�� ��ȯ.
		parsing�� �Ϸ���� ������ false�� ��ȯ.
		
		���� : 
			packet parsing�� �Ϸ�� �� �����͸� ���� �ʰ� ��� encode�� ������ ���
			������ �����͸� �Ұ� ��.
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
		packet parsing�� ���� ��� ���� �����͵��� reset.
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

