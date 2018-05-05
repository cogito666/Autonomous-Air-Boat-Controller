data_tag = '_auto_turning_3';
save(strcat(file_dir,'/',log_file(1:strfind(log_file,'.txt')-1), data_tag , '.mat'),...
    'rc_rudder_signal', 'rc_signal_time_stamps', 'rc_throtle_signal', 'rudder_servo_signal', ...
   'servo_time_stamps', 'throtle_servo_signal', 'traj', 'way_points', 'delta_t');
savefig(figure(1), strcat(file_dir,'/',log_file(1:strfind(log_file,'.txt')-1), data_tag ,'_trajectories.fig'));
savefig(figure(2), strcat(file_dir,'/',log_file(1:strfind(log_file,'.txt')-1), data_tag , '_control_loop.fig'));
savefig(figure(3), strcat(file_dir,'/',log_file(1:strfind(log_file,'.txt')-1), data_tag , '_user_inputs.fig'));
savefig(figure(4), strcat(file_dir,'/',log_file(1:strfind(log_file,'.txt')-1), data_tag , '_servo_signals.fig'));