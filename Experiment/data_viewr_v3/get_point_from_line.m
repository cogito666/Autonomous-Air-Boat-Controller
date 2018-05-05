function p = get_point_from_line(params, x)
    if abs(params(2)) < eps
        % vertical
        error('Cannot get point from line');
    else
        p = [x -(params(1)/params(2)*x+params(3)/params(2))];
    end
end

