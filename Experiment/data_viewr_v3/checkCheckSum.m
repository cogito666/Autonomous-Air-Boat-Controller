function is_valid = checkCheckSum(packet)
      sep_idx = strfind(packet, ',');
    if isempty(sep_idx)
        is_valid = 0;
        return;
    end
    chk_idx = sep_idx(end);
    if length(packet) < chk_idx+2
        %invalid packet length
        is_valid = 0;
        return;
    end
    chk_rcv = packet(chk_idx+1:chk_idx+2);
    %% check check-sum is a valid hex
    for i=1:2
        if ~(chk_rcv(i)>='0'&&chk_rcv(i)<='9')...
                &&~(chk_rcv(i)>='A'&&chk_rcv(i)<='F')...
                &&~(chk_rcv(i)>='a'&&chk_rcv(i)<='f')
            % invalid hex
            is_valid = 0;
            return;
        end
    end
    %% check check-sum
    chk = 0;
    for i=1:chk_idx-1
        chk = bitxor(chk, uint8(packet(i)));
    end
    is_valid = chk == hex2dec(chk_rcv);
end