clear all;
global max_n_servo_signal
global max_n_delta_t
global max_n_trj
global max_n_rc_signal;
global enable_live_view;
global use_absolute_pos;
global ax_bird_view;
global ax_control_loop_period;
global ax_rc_remocon_signal;
global ax_servo_control_signal;
global aggregate_way_points;
global update_cnt_period;
update_cnt_period = 8;
N_DATA_VISIBLE = 400;
max_n_servo_signal = N_DATA_VISIBLE;
max_n_delta_t = N_DATA_VISIBLE;
max_n_trj = N_DATA_VISIBLE; % 0 : every trajectory, > 0 : plot only max_n_trj trajectories
max_n_rc_signal = N_DATA_VISIBLE;
enable_live_view = 0;
use_absolute_pos = 0;
aggregate_way_points = 0;
%%
figure(1); clf;
ax_bird_view = subplot(2,2,[1,2]);
set(gcf,'Color',[1 1 1]);
ax_control_loop_period = subplot(2,2,3);
set(gcf,'Color',[1 1 1]);
ax_servo_control_signal = subplot(2,2,4);
set(gcf,'Color',[1 1 1]);
figure(2); clf;
ax_rc_remocon_signal = gca;
set(gcf,'Color',[1 1 1]);
autoArrangeFigures(2,2,1);
%%
s = serial('COM3','BaudRate',115200);
s.DataTerminalReady = 'off';
set(s,'Timeout',5.0);
f_log = fopen(strcat('fieldtest5/',datestr(now(),'yyyymmddHHMMSS'),'.txt'),'w');
fopen(s);
finishup1 = onCleanup(@() fclose(s));
finishup2 = onCleanup(@() fclose(f_log));
%%
try
    run_data_viewer(s, f_log);
catch
    warning('data_viewer aborted')
end
%%
fclose(s);
fclose(f_dummy);