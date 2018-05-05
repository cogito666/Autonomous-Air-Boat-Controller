// 
// 
// 

#include "PacketCommands.h"
#include "Logger.h"
#include "ControllerSelector.h"
#include "Globals.h"
#include "BoatControllerSemiauto.h"
#include "PacketCommandResetBoard.h"

DeserializeManager *packet_cmd_parsing_manager = NULL;

PacketCommandSetLogLevel *packet_cmd_set_log_level = NULL;
PacketCommandChangeControlMode *packet_cmd_change_ctrl_mode = NULL;
PacketCommandPumpControl *packet_cmd_pump_ctrl = NULL;
PacketCommandGoHome *packet_cmd_go_home = NULL;
PacketCommandTurn *packet_cmd_turn = NULL;
PacketCommandConfigSemiauto * packet_cmd_config_semiauto = NULL;
PacketCommandResetBoard * packet_cmd_reset_board = NULL;
PacketConnectionACK * packet_connection_ack = NULL;
extern ControllerSelector* boat_controller_selector;
extern BoatControllerSemiauto* boat_controller_semiauto;

void conductSetLogLevelCommand();
void conductChangeCtrlModeCommand();

void deserializeFailedCallback(const Serializable* serializable, const String& packet){
	loggers.log("!Broken : ", LoggerManager::LOG_INFO);
	loggers.logln(packet.c_str(), LoggerManager::LOG_INFO);
}

void initializePacketCommands()
{
	packet_cmd_set_log_level = new PacketCommandSetLogLevel();
	packet_cmd_change_ctrl_mode = new PacketCommandChangeControlMode();
	packet_cmd_pump_ctrl = new PacketCommandPumpControl();
	packet_cmd_go_home = new PacketCommandGoHome();
	packet_cmd_turn = new PacketCommandTurn();
	packet_cmd_config_semiauto = new PacketCommandConfigSemiauto();
	packet_cmd_reset_board = new PacketCommandResetBoard();
	packet_connection_ack = new PacketConnectionACK(0);

	packet_cmd_parsing_manager = new DeserializeManager();
	packet_cmd_parsing_manager->registerMessage(packet_cmd_set_log_level);
	packet_cmd_parsing_manager->registerMessage(packet_cmd_change_ctrl_mode);
	packet_cmd_parsing_manager->registerMessage(packet_cmd_pump_ctrl);
	packet_cmd_parsing_manager->registerMessage(packet_cmd_go_home);
	packet_cmd_parsing_manager->registerMessage(packet_cmd_turn);
	packet_cmd_parsing_manager->registerMessage(packet_cmd_config_semiauto);
	packet_cmd_parsing_manager->registerMessage(packet_cmd_reset_board);
	packet_cmd_parsing_manager->registerMessage(packet_connection_ack);

	packet_cmd_parsing_manager->onDeserializeFailure(deserializeFailedCallback);
	//packet_cmd_parsing_manager->onDeserializeSuccess(deserializeCallback);
}

void checkAndConductPacketCommands() {
	if(packet_cmd_reset_board->isUpdated()){
		loggers.logln("@Reset Board", LoggerManager::LOG_INFO);
		board_reset_required = true;
		packet_cmd_reset_board->clearUpdated();
	}

	if (packet_cmd_set_log_level->isUpdated()) {
		loggers.logln("@Set Log LV", LoggerManager::LOG_INFO);
		conductSetLogLevelCommand();
		packet_cmd_set_log_level->clearUpdated();
	}

	if (packet_cmd_change_ctrl_mode->isUpdated()) {
		loggers.logln("@Change ctrl mode", LoggerManager::LOG_INFO);
		conductChangeCtrlModeCommand();
		packet_cmd_change_ctrl_mode->clearUpdated();
		loggers.flush();
		delay(1);
	}

	if (packet_cmd_pump_ctrl->isUpdated()) {
		loggers.logln("@No pump yet", LoggerManager::LOG_ERROR);
		packet_cmd_pump_ctrl->clearUpdated();
	}

	if (packet_cmd_go_home->isUpdated()) {
		if(boat_state._ctrl_mode == BoatControlModes::BoatCtrlMode_SemiAutoControl){
			loggers.logln("@Go Home", LoggerManager::LOG_INFO);
			boat_controller_semiauto->goHome();
		}else{
			loggers.logln("@GoHome only available at semiauto", LoggerManager::LOG_ERROR);
		}
		packet_cmd_go_home->clearUpdated();
	}

	if (packet_cmd_turn->isUpdated()) {
		if (boat_state._ctrl_mode == BoatControlModes::BoatCtrlMode_SemiAutoControl) {
			if (boat_controller_semiauto->getControllerState() ==
				BoatControllerSemiauto::BoatCtrlMode_SemiAuto_Running) {
				switch (packet_cmd_turn->_direction) {
				case PacketCommandTurn::Left:
					boat_controller_semiauto->turnLeft(packet_cmd_turn->_diameters);
					loggers.logln("@LeftTurn", LoggerManager::LOG_INFO);
					break;
				case PacketCommandTurn::Right:
					boat_controller_semiauto->turnRight(packet_cmd_turn->_diameters);
					loggers.logln("@RightTurn", LoggerManager::LOG_INFO);
					break;
				default:
					loggers.logln("@Undefed Turn", LoggerManager::LOG_ERROR);
					break;
				}
			}else{
				loggers.logln("@Turn only available at runninlg", LoggerManager::LOG_ERROR);
			}
		}else{
			loggers.logln("@Turn only available at semiauto", LoggerManager::LOG_ERROR);
		}
		packet_cmd_turn->clearUpdated();
	}

	if(packet_cmd_config_semiauto->isUpdated()){
		if (boat_controller_semiauto->isInitialized() && !boat_controller_semiauto->isFinished()) {
			loggers.logln("@SemiAuto cannot be configed while running", LoggerManager::LOG_ERROR);
		}
		else {
			String message; message.reserve(60);
			switch (packet_cmd_config_semiauto->_action) {
			case PacketCommandConfigSemiauto::ActionType::SetParam:
			{
				switch (packet_cmd_config_semiauto->_SetParam_target_param) {
				case PacketCommandConfigSemiauto::PGain:
					boat_controller_semiauto->getPIDRudder()->setP(packet_cmd_config_semiauto->_SetParam_data);
					message = "@Set SemiAuto P gain : " + String(packet_cmd_config_semiauto->_SetParam_data, 3);
					break;
				case PacketCommandConfigSemiauto::DGain:
					boat_controller_semiauto->getPIDRudder()->setD(packet_cmd_config_semiauto->_SetParam_data);
					message = "@Set SemiAuto D gain : " + String(packet_cmd_config_semiauto->_SetParam_data, 3);
					break;
				case PacketCommandConfigSemiauto::IGain:
					boat_controller_semiauto->getPIDRudder()->setI(packet_cmd_config_semiauto->_SetParam_data);
					message = "@Set SemiAuto I gain : " + String(packet_cmd_config_semiauto->_SetParam_data, 3);
					break;
				case PacketCommandConfigSemiauto::LookAheadDistance:
					boat_controller_semiauto->setLookAheadDistance(packet_cmd_config_semiauto->_SetParam_data);
					message = "@Set SemiAuto Look-Ahead-Dist: " + String(packet_cmd_config_semiauto->_SetParam_data, 3);
					break;
				}
			}
			}

			loggers.logln(message.c_str(), LoggerManager::LOG_INFO);
		}
	}
	if (packet_connection_ack->isUpdated()) {

		packet_connection_ack->clearUpdated();
	}
}
//////////////////////////////////////////////////////////////////////////////////////
void conductSetLogLevelCommand() {
	switch (packet_cmd_set_log_level->_target_log_level) {
	case PacketCommandSetLogLevel::LOG_VERBOSE:
		loggers.setLogLevel(LoggerManager::LOG_VERBOSE);
		break;
	case PacketCommandSetLogLevel::LOG_DEBUG:
		loggers.setLogLevel(LoggerManager::LOG_DEBUG);
		break;
	case PacketCommandSetLogLevel::LOG_LOGGING:
		loggers.setLogLevel(LoggerManager::LOG_LOGGING);
		break;
	case PacketCommandSetLogLevel::LOG_INFO:
		loggers.setLogLevel(LoggerManager::LOG_INFO);
		break;
	case PacketCommandSetLogLevel::LOG_ERROR:
		loggers.setLogLevel(LoggerManager::LOG_ERROR);
		break;
	default:
		loggers.logln("@Invalid Log LV", LoggerManager::LOG_ERROR);
		break;
	}
}

void conductChangeCtrlModeCommand() {
	switch (packet_cmd_change_ctrl_mode->_target_ctrl_mode) {
	case BoatControlModes::BoatCtrlMode_Manual:
	{
		if(boat_state._ctrl_mode == BoatControlModes::BoatCtrlMode_Manual){
			loggers.logln("@manual already", LoggerManager::LOG_ERROR);
			return;
		}
		if (boat_controller_selector->selectController("manual")) {
			boat_state._ctrl_mode = BoatControlModes::BoatCtrlMode_Manual;
			boat_controller_selector->getCurrentController()->reset();
			loggers.logln("@start manual mode", LoggerManager::LOG_INFO);
		}
		else {
			loggers.logln("@cannot switch to manual", LoggerManager::LOG_ERROR);
		}
	}
	break;
	case BoatControlModes::BoatCtrlMode_SemiAutoControl:
	{
		if (boat_state._ctrl_mode == BoatControlModes::BoatCtrlMode_SemiAutoControl){
			loggers.logln("@already semiauto", LoggerManager::LOG_ERROR);
			break;
		}
		if(!home_position.isInitialized()){
			loggers.logln("@home isn't initted", LoggerManager::LOG_ERROR);
			break;
		}
		if(boat_controller_selector->selectController("semiauto")) {
			boat_state._ctrl_mode = BoatControlModes::BoatCtrlMode_SemiAutoControl;
			boat_controller_selector->getCurrentController()->reset();
			loggers.logln("@start semiauto mode", LoggerManager::LOG_INFO);
		}
		else {
			loggers.logln("@cannot switch to semiauto", LoggerManager::LOG_ERROR);
		}
	}
	break;
	default:
		loggers.logln("@invalid ctrl mode", LoggerManager::LOG_ERROR);
		break;
	}
}