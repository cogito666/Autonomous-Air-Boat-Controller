function loaded_packet_str = loadFuncConfigSemiauto(packet_str, data_struct)
    loaded_packet_str = packet_str;
    loaded_packet_str = loadField(loaded_packet_str, num2str(data_struct.action));
    switch  data_struct.action
        case 1 % SetParam        
            loaded_packet_str = loadField(loaded_packet_str, num2str(data_struct.SetParam_target_param));
            loaded_packet_str = loadField(loaded_packet_str, num2str(data_struct.SetParam_data));
        otherwise
            loaded_packet_str = '';%invalid action type. return empty string
            return;
    end
end

