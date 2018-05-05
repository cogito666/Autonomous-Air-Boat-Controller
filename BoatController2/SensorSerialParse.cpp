// 
// 
// 

#include "SensorSerialParse.h"
#include "DebugUtils.h"
#include "Logger.h"
FrequencyEstimator freq_ahrs, freq_gpgga;

void pollAHRS(SerialClassType* serial, sensor::AHRS* ahrs){
	#define pollAHRS_bufsize  512
	char buffer[pollAHRS_bufsize];
	int n = serial->available();
	if (n > pollAHRS_bufsize){
		n = pollAHRS_bufsize;
	}
	serial->readBytes(buffer, n);
	for (int i = 0; i < n; i++){
		sensor::AHRS::PacketState parse_state = ahrs->encode(buffer[i]);
		if(parse_state == sensor::AHRS::PACKET_PARSING_COMPLETE){
			freq_ahrs.checkout(1);
		}
		else {
			if(parse_state == sensor::AHRS::PACKET_PARSING_ERROR){
				loggers.logln(ahrs->getErrorMessage(), LoggerManager::LOG_ERROR);
			}
			freq_ahrs.checkout(0);
		}
	}
}

void pollGPS(SerialClassType* serial, TinyGPSPlus* gps_parser){
#define pollGPS_bufsize  512
	char buffer[pollGPS_bufsize];
	int n = serial->available();
	if (n > pollGPS_bufsize) {
		n = pollGPS_bufsize;
	}
	serial->readBytes(buffer, n);
	for (int i = 0; i < n; i++) {
		if (gps_parser->encode(buffer[i])) {
			if(gps_parser->sentenceType() == TinyGPSPlus::GPS_SENTENCE_GPGGA){
				freq_gpgga.checkout(1);
			}
			else {
				freq_gpgga.checkout(0);
			}
			return;
		}
	}
}