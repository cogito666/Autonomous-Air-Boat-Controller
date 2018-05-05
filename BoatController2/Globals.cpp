// 
// 
// 

#include "Globals.h"

BoatState boat_state;
MeanCalculator<Point2D, 30> home_position;
bool board_reset_required = false;
TimerTimeoutMilli connection_ack_timeout(10000, NULL, false);

void initializeSerialCommunicatoins(){
	SerialWireless.begin(57600);
	SerialWireless.setTimeout(2);
	SerialAHRS.begin(115200);
	SerialAHRS.setTimeout(5);
	SerialGPS.begin(38400);
	SerialGPS.setTimeout(5);
}