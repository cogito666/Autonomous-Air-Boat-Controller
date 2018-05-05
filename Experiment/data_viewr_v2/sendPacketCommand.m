%% Open serial port
clear all;
s = serial('COM13', 'BaudRate', 115200);
s.DataTerminalReady = 'off';
set(s,'Timeout',5.0);
fopen(s);
%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% send Semiauto Configure Command
% action : 
%   SetParam = 1
%     SetParam_target_param
%     	PGain = 1
%     	DGain = 2
%     	IGain = 3
%     	LookAheadDistance = 4
%     SetParam_data
cmd_config_semiauto = struct('action', 1, ...
                            'SetParam_target_param', 4,...
                            'SetParam_data', 5.0);
packet_str = createPacketCommand(@loadFuncConfigSemiauto,'ConfigSemiAuto',cmd_config_semiauto);
disp(packet_str)

%% send Reset Board Command
cmd_config_reset = struct();
packet_str = createPacketCommand(@loadFuncReset,'Reset',cmd_config_reset);
disp(packet_str)

%% %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% actual sending
disp(sprintf('send %s',packet_str))
fwrite(s, packet_str);
disp('waiting for the response')
tic;
while 1
    t_wait = toc;
    if t_wait >= 5.0 % max waiting time
        warning('No response from the controller');
        break;
    end
    packet_rcv = fgetl(s);
    packet_rcv = strtrim(packet_rcv);
%     disp(packet_rcv)
    if ~ischar(packet_rcv) % serial timeout occured.
        warning('serial timeout!');
        break;
    end
    if isempty(packet_rcv) % filter white space only packets
%         warning('empty string!');
        continue;
    end
    if packet_rcv(1) == '@' %special character for response
        disp(sprintf('Resp : %s',packet_rcv))
        continue;
    end
end
%%
fclose(s);