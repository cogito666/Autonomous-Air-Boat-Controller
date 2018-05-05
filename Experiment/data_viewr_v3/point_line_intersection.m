function p = point_line_intersection(p1, p2, p0)
% compute the intersecting point between line1(connected with p1 and p2)
% and line2(perpendicular to line1 and passing through p0).
% p1 : 2 x 1 or 1 X 2 vector
% p2 : 2 x 1 or 1 X 2 vector
% p0 : 2 x 1 or 1 X 2 vector

a = p2(2) - p1(2);
b = -(p2(1) - p1(1));
c = p1(2)*p2(1) - p1(1)*p2(2);
p = [(b*(b*p0(1)-a*p0(2))-a*c)/(a*a+b*b) (a*(-b*p0(1)+a*p0(2))-b*c)/(a*a+b*b)];
end

