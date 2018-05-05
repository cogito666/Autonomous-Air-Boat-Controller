function realtime_plot_boat_data
clear all;
global max_n_servo_signal
global max_n_delta_t
global max_n_trj
global enable_live_view;
global use_absolute_pos;
max_n_servo_signal = 200;
max_n_delta_t = 200;
max_n_trj = 200; % 0 : every trajectory, > 0 : plot only max_n_trj trajectories
enable_live_view = 1;
use_absolute_pos = 0;
f_log = fopen(strcat('fieldtest4/',datestr(now(),'yyyymmddHHMMSS'),'.txt'),'w');
s = serial('COM11','BaudRate',115200);
s.DataTerminalReady = 'off';
set(s,'Timeout',5.0);
fopen(s);
finishup1 = onCleanup(@() fclose(s));
finishup2 = onCleanup(@() fclose(f_log));
%%
try
    trj = run_data_viewer(s, f_log);
catch
    warning('data_viewer aborted')
end
end