function speed = speedLowPassFilter(stamp, pos)
    persistent data;
    if isempty(data)
        data = struct('dt',[],'dist',[],'last_pos',pos,'last_stamp',stamp, 'speed', 0);
        speed = 0;
        return;
    end
    if sum(isnan(pos))
        speed = data.speed;
        return
    end
    dt = stamp - data.last_stamp;
    dist = norm(pos - data.last_pos);
    
    data.dt(end+1) = dt;
    data.dist(end+1) = dist;
    
    while sum(data.dt) > 2.5
        data.dt(1) = [];
        data.dist(1) = [];
    end
    
    speed = sum(data.dist) / sum(data.dt);
    if isnan(speed)
        sprintf('%f / %f - dt:%f dist:%f pos:[%f %f]',sum(data.dist), sum(data.dt),dt,dist,pos(1), pos(2))
    end
    
    data.last_pos = pos;
    data.last_stamp = stamp;
    data.speed = speed;
end

