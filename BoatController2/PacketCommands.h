#ifndef PACKET_COMMANDS_H_
#define PACKET_COMMANDS_H_
#include "PacketCommandChangeControlMode.h"
#include "PacketCommandGoHome.h"
#include "PacketCommandPumpControl.h"
#include "PacketCommandTurn.h"
#include "PacketBoatState.h"
#include "PacketCommandSetLogLevel.h"
#include "PacketCommandConfigSemiauto.h"
#include "PacketConnectionACK.h"

#include "DeserializeManager.h"

extern DeserializeManager *packet_cmd_parsing_manager;

extern PacketCommandSetLogLevel *packet_cmd_set_log_level;
extern PacketCommandChangeControlMode *packet_cmd_change_ctrl_mode;
extern PacketCommandPumpControl *packet_cmd_pump_ctrl;
extern PacketCommandGoHome *packet_cmd_go_home;
extern PacketCommandTurn *packet_cmd_turn;
extern PacketCommandConfigSemiauto * packet_cmd_config_semiauto;
extern PacketConnectionACK * packet_connection_ack;
void initializePacketCommands();
void checkAndConductPacketCommands();
#endif