clear all;
clear run_data_viewer;
%%
global delta_t;
global traj;
global servo_time_stamps;
global way_points;
global rudder_servo_signal;
global throtle_servo_signal;
global rc_signal_time_stamps;
global rc_rudder_signal;
global rc_throtle_signal;
%%
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
global enable_video_record;
global ax_boat_speed;
update_cnt_period = 20;
N_DATA_VISIBLE = 0;
max_n_servo_signal = N_DATA_VISIBLE;
max_n_delta_t = N_DATA_VISIBLE;
max_n_trj = N_DATA_VISIBLE; % 0 : every trajectory, > 0 : plot only max_n_trj trajectories
max_n_rc_signal = N_DATA_VISIBLE;
enable_live_view = 1;
use_absolute_pos = 0;
aggregate_way_points = 0;
enable_video_record = 0;
%%
figure(1); clf;
set(gcf,'Color',[1 1 1]);
ax_bird_view = subplot(2,3,[1,2,4,5]);  
ax_rc_remocon_signal = subplot(2,3,3);
ax_servo_control_signal = subplot(2,3,6);
figure(2); clf;
set(gcf,'Color',[1 1 1]);
ax_control_loop_period = gca;
figure(3); clf;
set(gcf,'Color',[1 1 1]);
ax_boat_speed = gca;
%%
% figure(1); clf;
% ax_bird_view = gca; 
% set(gcf,'Color',[1 1 1]);
% xlabel('X(m)','FontSize',15);
% ylabel('Y(m)','FontSize',15);
% figure(2); clf;
% ax_control_loop_period = gca;
% set(gcf,'Color',[1 1 1]);
% figure(3); clf;
% ax_rc_remocon_signal = gca;
% set(gcf,'Color',[1 1 1]);
% xlabel('t(msec)','FontSize',15);
% ylabel('RC signal(\musec)','FontSize',15)
% figure(4); clf;
% ax_servo_control_signal = gca;
% set(gcf,'Color',[1 1 1]);
% xlabel('t(msec)','FontSize',15);
% ylabel('RC signal(\musec)','FontSize',15)
% autoArrangeFigures(2,4,2);
%%
file_dir = 'fieldtest5';
log_file = '20170111171435.txt';
s = fopen(strcat(file_dir, '/', log_file),'r');
f_dummy = fopen('dummy_log.txt','w');
% finishup1 = onCleanup(@() fclose(s));
% finishup2 = onCleanup(@() fclose(f_dummy));
try
    run_data_viewer(s, f_dummy);
catch
    warning('data_viewer aborted')
end
%%
fclose(s);
fclose(f_dummy);