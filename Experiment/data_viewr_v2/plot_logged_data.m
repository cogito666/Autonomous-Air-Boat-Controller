function plot_logged_data
clear all;
global max_n_servo_signal
global max_n_delta_t
global max_n_trj
global enable_live_view;
global use_absolute_pos;
max_n_servo_signal = 200;
max_n_delta_t = 200;
max_n_trj = 0; % 0 : every trajectory, > 0 : plot only max_n_trj trajectories
enable_live_view = 1;
use_absolute_pos = 0;
%%
file_dir = 'fieldtest2';
log_file = '20170104175154_LeftRight_7m_Stuck2Times_and_Homing.txt';
s = fopen(strcat(file_dir, '/', log_file),'r');
f_dummy = fopen('dummy_log.txt','w');
finishup1 = onCleanup(@() fclose(s));
finishup2 = onCleanup(@() fclose(f_dummy));
try
    trj = run_data_viewer(s, f_dummy);
catch
    warning('data_viewer aborted')
end
end
