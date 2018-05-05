function packet_str = createPacketCommand(loadFunction, packet_type_str ,data_struct)
    packet_str = strcat('#',packet_type_str);
    packet_str = loadFunction(packet_str, data_struct);
    if isempty(packet_str)
        error('Error while loading data fields')
    end
    chk = computeCheckSum(packet_str);
    packet_str = loadField(packet_str, chk);
end

