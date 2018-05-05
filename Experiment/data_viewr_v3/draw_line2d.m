function h_line = draw_line2d(h_ax, params)
    if abs(params(2)) > 0.00001
        f = @(x)(-(params(1)/params(2)*x+params(3)/params(2)));
        fplot(h_ax, f, h_ax.XLim);
        lines = get(h_ax,'Children');
        h_line = lines(1);
    else
        error('vertical line is not implemented')
    end
end

