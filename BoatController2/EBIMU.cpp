// 
// 
// 

#include "EBIMU.h"
#include <stdlib.h>
#include <stdio.h>


namespace sensor{
	AHRS_EBIMU::AHRS_EBIMU(PacketMode pack_mode,
		AngleOutputMode angle_mode,
		GyroScopeOutputMode gyro_mode,
		AccerlationOutputMode acc_mode,
		MagnetometerOutputMode mag_mode,
		TemperatureOutputMode temp_mode,
		DistanceOutputMode dist_mode) :packet_mode(pack_mode){
		m_updated = false;
		reset();
	}

	void AHRS_EBIMU::reset(){
		m_buffer_pos = 0;
		m_ele_count = 0;
		m_bytes_count = 0;
		m_bin_check_sum.reset();
		m_bin_data.reset();
	}
	
	AHRS_EBIMU::PacketState AHRS_EBIMU::encode(const char& c){
		m_bytes_count++;
		switch (packet_mode){
		case ASCII_PACKET:
			sprintf(m_err_msg, "ASCII packet mode isn't implemented");
			return PACKET_PARSING_ERROR;
			//return ascii_encode(c);
		case BINARY_PACKET:
			return binary_encode(c);
		default:
			sprintf(m_err_msg, "Undefined packet mode(%d)", (int)packet_mode);
			return PACKET_PARSING_ERROR;
		}
	}

	void AHRS_EBIMU::commit(){
		rpy[0] = rpy_tmp[0];
		rpy[1] = rpy_tmp[1];
		rpy[2] = rpy_tmp[2];
		m_updated = true;
	}

#define DATA_BYTES	(2+3*2)//including SOP
	AHRS_EBIMU::PacketState AHRS_EBIMU::binary_encode(const char& c){
		if (m_bytes_count <= 2 && c!= 0x55){
			//SOP is missing
			reset();
			return PACKET_PARSING_DOING;
		}

		if (m_bytes_count <= DATA_BYTES){
			m_bin_check_sum.push(c);
			if (m_bytes_count <= 2){
				//SOP(2 Bytes)
				return PACKET_PARSING_DOING;
			}
		}
		
		if (m_bin_data.encode(c)){
			if (m_bytes_count <= DATA_BYTES){
				rpy_tmp[m_ele_count++] = 0.01 * m_bin_data.read();
			}
			else{
				//check sum
				uint16_t chk = *((uint16_t*)&m_bin_data.read());
				if (m_bin_check_sum.compare(chk)){
					reset();
					commit();
					return PACKET_PARSING_COMPLETE;
				}
				else{
					sprintf(m_err_msg, "Binary check-sum error(%x - %x)",
						chk, m_bin_check_sum.getResult());
					reset();
					return PACKET_PARSING_ERROR;
				}
			}
		}
		return PACKET_PARSING_DOING;
	}

	//AHRS_EBIMU::PacketState AHRS_EBIMU::ascii_encode(const char& c){
	//	switch (c){
	//	case '\n':	case ','://separator 문자, line ending 문자
	//		m_buffer[m_buffer_pos] = NULL;
	//		this->m_rpy.rpy[m_ele_count++] = atof(m_buffer); //버퍼의 문자를 숫자로 변환
	//		if (c == '\n'){
	//			m_is_updated = true;
	//			return PACKET_PARSING_COMPLETE;
	//		}
	//		break;
	//	case '-':
	//		if (m_buffer_pos != 0){
	//			//에러 : minus기호는 가장 처음 나와야 함.
	//			reset();
	//			sprintf(m_err_msg, "\'-\' must come at first.");
	//			return PACKET_PARSING_ERROR;
	//		}
	//		m_buffer[m_buffer_pos++] = c;
	//		break;
	//	case '.'://소수점
	//		m_buffer[m_buffer_pos++] = c;
	//		break;
	//	default://숫자
	//		if (c > '0' && c < '9'){
	//			m_buffer[m_buffer_pos++] = c;
	//		}
	//		else{
	//			//에러 : 허용되지 않은 문자 수신
	//			reset();
	//			sprintf(m_err_msg, "Invalid character(\'%c\') received", c);
	//			return PACKET_PARSING_ERROR;
	//		}
	//		break;
	//	}
	//	return PACKET_PARSING_DOING;
	//}
}