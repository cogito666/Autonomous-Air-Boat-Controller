// EBIMU.h

#ifndef _EBIMU_h
#define _EBIMU_h

#if defined(ARDUINO) && ARDUINO >= 100
#include "arduino.h"
#else
#include "WProgram.h"
#endif

#include "AHRS.h"
#include "CheckSum.h"
namespace sensor{
#define __EBIMU_BUFFER_SIZE	10
	class Data16;

	class AHRS_EBIMU :public AHRS{
	public:
		enum PacketMode{
			ASCII_PACKET = 1,
			BINARY_PACKET
		};

		enum AngleOutputMode{
			ANGLE_OUTPUT_EULER = 1,
			ANGLE_OUTPUT_QUATERNION
		};

		enum GyroScopeOutputMode{
			GYRO_OUTPUT_NONE = 0,
			GYRO_OUTPUT_RAW
		};

		enum AccerlationOutputMode{
			ACC_OUTPUT_NONE = 0,
			ACC_OUTPUT_RAW,
			ACC_OUTPUT_LINEAR_LOCAL,
			ACC_OUTPUT_LINEAR_GLOBAL,
			ACC_VELOCITY_LOCAL,
			ACC_VELOCITY_GLOBAL
		};

		enum MagnetometerOutputMode{
			MAG_OUTPUT_NONE = 0,
			MAG_OUTPUT_RAW
		};

		enum DistanceOutputMode{
			DIST_OUTPUT_NONE = 0,
			DIST_OUTPUT_LOCAL,
			DIST_OUTPUT_GLOBAL

		};

		enum TemperatureOutputMode{
			TEMP_OUTPUT_NONE = 0,
			TEMP_OUTPUT_DEGREES
		};

		AHRS_EBIMU(PacketMode pack_mode = BINARY_PACKET,
			AngleOutputMode angle_mode = ANGLE_OUTPUT_EULER,
			GyroScopeOutputMode gyro_mode = GYRO_OUTPUT_NONE,
			AccerlationOutputMode acc_mode = ACC_OUTPUT_NONE,
			MagnetometerOutputMode mag_mode = MAG_OUTPUT_NONE,
			TemperatureOutputMode temp_mode = TEMP_OUTPUT_NONE,
			DistanceOutputMode dist_mode = DIST_OUTPUT_NONE);

		void getRPY(double& R, double& P, double& Y){
			R = rpy[0];
			P = rpy[1];
			Y = rpy[2];
		}

		bool updated() const{
			return m_updated;
		}
		void clearUpdated() {
			m_updated = false;
		}

		//TODO : 현재 RPY(binary)모드에서만 동작함.
		PacketState encode(const char& c);
		void reset();
	private:
		PacketState binary_encode(const char& c);
		PacketState ascii_encode(const char& c);//Not implemented.
		
		void commit();

		class Data16{
		private:
			typedef union {
				char b[2];
				int16_t data;
			} Data16Type;
			int pos;
			Data16Type data;

		public:
			Data16(){
				reset();
			}

			void reset(){
				pos = 1;
			}

			bool encode(const char& c){
				data.b[pos--] = c;
				if (pos < 0){
					reset();
					return true;
				}
				return false;
			}

			const int16_t& read() const{
				return data.data;
			}
		};

	private:
		char m_buffer[__EBIMU_BUFFER_SIZE];
		int m_buffer_pos;
		int m_ele_count;
		int m_bytes_count;

		CheckSumAdding<uint16_t> m_bin_check_sum;
		Data16 m_bin_data;

		PacketMode packet_mode;

		double rpy[3];

		double rpy_tmp[3];

		bool m_updated;
	};
}
#endif

