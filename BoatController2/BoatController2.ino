#include "PacketHeadingControlState.h"
#include "SafeServo.h"
#include <Wire.h>
#include "Globals.h"
#include "ServoInputOutput.h"
#include "PacketCommands.h"
#include "BoatControllers.h"
#include "SensorSerialParse.h"
#include "EBIMU.h"
#include "SerialLogger.h"
#include "Logger.h"
#include "TimeoutTimer.h"
#include "LatLonToUTM.h"
#include "StringConversion.h"
#include "FrequencyEstimator.h"
#include "PacketControlSignals.h"
#include "PacketRCReadings.h"

#define BOARD_DUE
#if !defined(BOARD_DUE)
#include <avr/wdt.h>
#else
#include <watchdog.h>
#endif
#define ENABLE_DEBUG_UTILS 0
#define ENABLE_DEBUG_UTILS_ELAPSED_TIME_UTILS 0
#include "DebugUtils.h"

void watchdogSetup();

sensor::AHRS * ahrs = NULL;
TinyGPSPlus * gps = NULL;
TimerTimeoutMilli timeout(3000,&timeoutCallback, true);
FrequencyEstimator rudder_rf_freq, throtle_rf_freq;
FrequencyEstimator loop_freq;
void setup() {
	initIndicatingLED();
	resetXbee();
	resetPWMReadingBoard();
	initializeSerialCommunicatoins();

	pinMode(SerialDebug_OUTPUT_ENALBE_PIN, INPUT_PULLUP);

	delay(200);
	Wire.begin();
	Wire.setClock(10000);

	loggers.setLogLevel(LoggerManager::LOG_LOGGING);
	loggers.registerLogger(new SerialLogger(&SerialWireless));

	Serial.begin(115200);
	if (digitalRead(SerialDebug_OUTPUT_ENALBE_PIN) == LOW) {
		//Register Serial(micro-USB) port for logging
		loggers.registerLogger(new SerialLogger(&Serial));
	}

	loggers.logln("Initializing the control board...", LoggerManager::LOG_INFO);
	////////////////////Initialize AHRS/////////////////////////////////////////
	ahrs = new sensor::AHRS_EBIMU(sensor::AHRS_EBIMU::BINARY_PACKET,
		sensor::AHRS_EBIMU::ANGLE_OUTPUT_EULER);
	gps = new TinyGPSPlus();
	
	loggers.logln("Initializing Servos", LoggerManager::LOG_INFO);
	initializeServos();
	loggers.logln("Initializing Packet Commands", LoggerManager::LOG_INFO);
	initializePacketCommands();
	loggers.logln("Initializing Controllers", LoggerManager::LOG_INFO);
	initializeBoatControllers();

	timeout.reset();
	loggers.logln("Initializing Finished", LoggerManager::LOG_INFO);

	rudder_rf_freq.reset();
	throtle_rf_freq.reset();
#if defined(BOARD_DUE)
	watchdogEnable(2000);
#else
	wdt_enable(WDTO_2S);
#endif
}

void loop()
{
	DEBUG_UTILS_ELAPSED_TIME_INIT();
	/******************************* Sensors and Control Inputs(RF Signal) updates *******************************/
	updateSensorData();
	DEBUG_UTILS_ELAPSED_TIME_BEGIN();
	if(!home_position.isInitialized()){
		initializeHomePosition();
	}
	DEBUG_UTILS_ELAPSED_TIME_END("Init Home");
	/******************************* Packet Commands Parsing *******************************/
	DEBUG_UTILS_ELAPSED_TIME_BEGIN();
	if (SerialWireless.available()) {
		packet_cmd_parsing_manager->deserialize(SerialWireless.readString());
	}
	checkAndConductPacketCommands();
	DEBUG_UTILS_ELAPSED_TIME_END("Command Parsing");

	/******************************* Run (current)Controller *******************************/
	DEBUG_UTILS_ELAPSED_TIME_BEGIN();
	BoatControllerInterface * controller = boat_controller_selector->getCurrentController();
	if (controller != NULL) {
		if (!controller->isInitialized()) {
			controller->initialize();
		}
		else {
			controller->control();
		}
	}
	else {
		loggers.logln("Null controller", LoggerManager::LOG_INFO);
		//Error log
	}
	DEBUG_UTILS_ELAPSED_TIME_END("Control Loop");

	/******************************* Send Current State Packet *******************************/
	DEBUG_UTILS_ELAPSED_TIME_BEGIN();
	updateBoatState();
	sendStatePacket();
	DEBUG_UTILS_ELAPSED_TIME_END("Send StatePacket");

	/******************************* Logging Additional Information *******************************/
	logAdditionalData();

	/******************************* Timeout Event For Debugging *******************************/
	DEBUG_UTILS_ELAPSED_TIME_BEGIN();
	timeout.spinOnce();
	DEBUG_UTILS_ELAPSED_TIME_END("Timeout Spin");

	/******************************* Clear Updated *******************************/
	DEBUG_UTILS_ELAPSED_TIME_BEGIN();
	//TinyGPS clears the updated flag by itself.
	rf_signal_rudder->clearUpdated();
	rf_signal_throtle->clearUpdated();
	ahrs->clearUpdated();
	boat_state.clearUpdated();
	servo_rudder->clearUpdated();
	servo_throtle->clearUpdated();

	servo_rudder->move();

	DEBUG_UTILS_ELAPSED_TIME_END("Clear Updated");

	/******************************* Watch-dot reset *******************************/
	DEBUG_UTILS_ELAPSED_TIME_BEGIN();
	if (!board_reset_required) {//if board-reset is required, do not reset the watchdog.
#if defined(BOARD_DUE)
		watchdogReset();
#else
		wdt_reset();
#endif

	}
	DEBUG_UTILS_ELAPSED_TIME_END("Watch Dog");

	loop_freq.checkout(1);
}
void watchdogSetup() {

}

void timeoutCallback(const uint32_t& t) {
	static int output = 1;
	String diagnostic = "LP:"+String(loop_freq.getFrequency(),1) + " IMU:" + String(freq_ahrs.getFrequency(), 1) +
		" GGA:" + String(freq_gpgga.getFrequency(), 1) + " RC1:" + 
		String(rudder_rf_freq.getFrequency(), 1) + " RC2:" + String(throtle_rf_freq.getFrequency(), 1);
	loggers.logln(diagnostic.c_str(), LoggerManager::LOG_INFO);
	digitalWrite(RUNNING_INDICATOR_LED, output);
	output = !output;
}
	
void sendStatePacket() {
	if (boat_state.isPoseUpdated()) {
		String state_msg; state_msg.reserve(100);
		PacketBoatState state_packet(&boat_state);
		if (!state_packet.serialize(state_msg)) {
			loggers.logln("Serialize failed", LoggerManager::LOG_ERROR);
			return;
		}
		loggers.logln(state_msg.c_str(), LoggerManager::LOG_INFO);
	}
}

void updateSensorData() {
	DEBUG_UTILS_NAMED_ELAPSED_TIME_INIT(sensor_update);
	pollAHRS(&SerialAHRS, ahrs);//ahrs update
	DEBUG_UTILS_NAMED_ELAPSED_TIME_PRINT_AND_RESET(sensor_update, "AHRS");
	pollGPS(&SerialGPS, gps);	//gps update
	DEBUG_UTILS_NAMED_ELAPSED_TIME_PRINT_AND_RESET(sensor_update, "GPS");
	rf_signal_rudder->poll();	//RF Signals update(Rudder)
	DEBUG_UTILS_NAMED_ELAPSED_TIME_PRINT_AND_RESET(sensor_update, "RFRudder");
	rf_signal_throtle->poll();	//RF Signals update(Throtle)
	DEBUG_UTILS_NAMED_ELAPSED_TIME_PRINT_AND_RESET(sensor_update, "RFThrtole");

	if (ahrs->updated()) {
		double RPY[3];
		ahrs->getRPY(RPY[0], RPY[1], RPY[2]);
		RPY[2] = -1 * RPY[2] - 90 + 8.15;
		boat_state.updatePoseTheta(normalizeHeading(DEG_TO_RAD * RPY[2]));
		boat_state._stamp = millis();
	}
	if (gps->location.isUpdated() && gps->sentenceType() == TinyGPSPlus::GPS_SENTENCE_GPGGA) {
		//Control Mode is updated when switching controllers.
		double x, y, k;
		LL2UTM::UTMZone zone;
		LL2UTM::geoConverterWGS84.convertLL2UTM(gps->location.lat(), gps->location.lng(), zone, x, y, k);
		boat_state.updatePoseX(x);
		boat_state.updatePoseY(y);
		boat_state._stamp = millis();
		boat_state._gps_sentence = gps->sentenceType();
		boat_state._gps_fix_quality = gps->fixQuality();
		boat_state._gps_num_satellites = gps->satellites.value();
	}

	if (rf_signal_rudder->updated()) {
		rudder_rf_freq.checkout(1);
	}
	else {
		rudder_rf_freq.checkout(0);
	}
	if (rf_signal_throtle->updated()) {
		throtle_rf_freq.checkout(1);
	}
	else {
		throtle_rf_freq.checkout(0);
	}
}

void updateBoatState() {
	boat_state._controller_state = boat_controller_selector->getCurrentController()->getControllerState();
}

void initializeHomePosition() {
	static TimerTimeoutMilli bad_fix_message_timeout(500, NULL, true);
	if (boat_state.isPoseUpdated()) {
		if (boat_state._gps_fix_quality < TinyGPSPlus::FIX_QUALITY_GPS_FIX) {
			if (bad_fix_message_timeout.spinOnce()) {
				loggers.logln("Bad GPS fix", LoggerManager::LOG_INFO);
			}
			home_position.reset();
			return;
		}
		LL2UTM::UTMZone zone;
		double easting, northing, k;
		LL2UTM::geoConverterWGS84.convertLL2UTM(gps->location.lat(), gps->location.lng(), zone, easting, northing, k);
		home_position.addSample(Point2D(easting, northing));
		if(home_position.isInitialized()){
			String message; message.reserve(50);
			message = "Home position acquired : " + to_string(home_position.getMeanValue());
			loggers.logln(message.c_str(), LoggerManager::LOG_INFO);
		}else{
			loggers.logln("Acquiring GPS", LoggerManager::LOG_INFO);
		}
	}
}

void resetXbee() {
	pinMode(XBEE_RESET_PIN, OUTPUT);//XBee reset
	digitalWrite(XBEE_RESET_PIN, LOW);
	delay(10);
	digitalWrite(XBEE_RESET_PIN, HIGH);
	//pinMode(XBEE_RESET_PIN, INPUT_PULLUP);
}

void resetPWMReadingBoard(){
	pinMode(PWM_READER_RESET_PIN, OUTPUT);//Arduino Mega 2560 reset
	digitalWrite(PWM_READER_RESET_PIN, LOW);
	delay(10);
	digitalWrite(PWM_READER_RESET_PIN, HIGH);
	pinMode(PWM_READER_RESET_PIN, INPUT);
}

void initIndicatingLED() {
	pinMode(RUNNING_INDICATOR_LED, OUTPUT);
	for (int i = 0; i < 3; i++) {
		digitalWrite(RUNNING_INDICATOR_LED, HIGH);
		delay(200);
		digitalWrite(RUNNING_INDICATOR_LED, LOW);
		delay(200);
	}
	digitalWrite(RUNNING_INDICATOR_LED, HIGH);
	delay(800);
	digitalWrite(RUNNING_INDICATOR_LED, LOW);
}

void logAdditionalData() {
	//logging control singals
	static TimerTimeoutMilli log_timeout(30, NULL, true);
	if(log_timeout.spinOnce()){
		String message; message.reserve(100);
		PacketControlSignals packet_boat_control_signals;
		packet_boat_control_signals._rudder_servo_micros = servo_rudder->readMicroseconds();
		packet_boat_control_signals._throtle_servo_micros = servo_throtle->readMicroseconds();
		packet_boat_control_signals.serialize(message);
		loggers.logln(message.c_str(), LoggerManager::LOG_INFO);

		PacketRCReadings packet_rc_readings;
		packet_rc_readings._rudder_servo_reading = rf_signal_rudder->width();
		packet_rc_readings._throtle_servo_reading = rf_signal_throtle->width();
		packet_rc_readings.serialize(message);
		loggers.logln(message.c_str(), LoggerManager::LOG_INFO);
	}
}