%% failed
close all;
open('fieldtest5\20170111170603_auto_turning_with_homing1_only_turning_trajectory.fig')
fig = gcf;

xdata = fig.Children.Children(1).XData;
ydata = fig.Children.Children(1).YData;

n_line_expected = 9;
%%
ransac_options.epsilon = 1e-6;
ransac_options.P_inlier = 0.2;
% ransac_options.T_noise_squared = 0.15;
ransac_options.sigma = 0.01;
ransac_options.est_fun = @estimate_line;
ransac_options.man_fun = @error_line;
ransac_options.mode = 'MSAC';
ransac_options.Ps = [];
ransac_options.notify_iters = [];
ransac_options.min_iters = 100;
ransac_options.fix_seed = false;
ransac_options.reestimate = true;
ransac_options.stabilize = false;
%%
X = [xdata;ydata];
lines = zeros(3, n_line_expected);
for i=1:n_line_expected
    [results, ransac_options] = RANSAC(X, ransac_options);
    ind = results.CS;
    lines(:,i) = results.Theta;
    draw_line2d(gca, lines(:,i));
    line_ax = gca;
    plot(X(1, ind), X(2, ind),'.','Color',line_ax.Children(end).Color);
    X(:,ind) = [];
end