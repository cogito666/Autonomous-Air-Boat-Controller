function p = point_distant_from_line(params, p0, dist)
% compute a point distant from p0(a point on the line parameterized by
% params).

v = [params(1) params(2)];
v = v / norm(v);
p = p0 + dist * v;
end

