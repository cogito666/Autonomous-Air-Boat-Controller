function chk_hex_str = computeCheckSum(packet_string)
    %% check check-sum
    chk = 0;
    for i=1:length(packet_string)
        chk = bitxor(chk, uint8(packet_string(i)));
    end
    chk_hex_str = dec2hex(chk,2);
end

