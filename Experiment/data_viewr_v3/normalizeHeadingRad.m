function heading2 = normalizeHeadingRad(heading)
    heading2 = heading;
    while heading2 <= -pi
        heading2 = heading2 + 2*pi;
    end
    while heading2 > pi
        heading2 = heading2 - 2*pi;
    end
end

