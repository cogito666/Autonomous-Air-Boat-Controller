function d = point_line_distance(params, p)
% params : [a b c] which constitute a line of ax+by+c=0
% p : point(s) from which the distance will be calculated.(2 X N)
if size(p,2) == 2
    p = p';
end
d = abs(params(1)*p(1,:)+params(2)*p(2,:)+params(3))/sqrt(params(1)*params(1)+params(2)*params(2));
end

