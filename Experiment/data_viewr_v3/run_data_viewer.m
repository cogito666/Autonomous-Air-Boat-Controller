function run_data_viewer(file_handle, f_log)
%% data
global delta_t;
global traj;
global servo_time_stamps;
global way_points;
global rudder_servo_signal;
global throtle_servo_signal;
global rc_signal_time_stamps;
global rc_rudder_signal;
global rc_throtle_signal;
global boat_speed;
global boat_speed_t;
%%
global x_ax_limit;
global y_ax_limit;
global update_count;
global enable_live_view;
global ax_boat_position;
global ax_way_points;
global ax_carrot_point;
global ax_delta_t;
global ax_trj;
global ax_bird_view;
global update_cnt_period;
global boat_state;
global h_line_boat_speed;
global ax_boat_speed;
%%
clear updateBoatState % clear persistent variables
%%
if isVideoRecordEnabled()
    video_record = struct('video', VideoWriter('Output.mp4','MPEG-4'));
    video_record.video.FrameRate = 10;
    video_record.video.open();
    video_record.last_t = [];
    video_record.frame10 = {};
    update_cnt_period = 500;% the viewer should be syncronized only with state packets
end
ax_trj = [];
ax_way_points = [];
traj = [];
ax_carrot_point = [];
update_count = 0;
delta_t = [];
ax_delta_t = [];
ax_boat_position = [];
boat_speed = [];
boat_speed_t = [];
h_line_boat_speed = [];
packet = [];
try
    while 1
        packet = fgetl(file_handle);
        if ~ischar(packet)
            warning('communication finished!');
            break;
        end
        fprintf(f_log,'%s', packet);
        if isempty(packet)
            warning('empty string!');
            continue;
        end
        packet = strtrim(packet);
        if length(packet) <= 4
            continue;
        end
%         fprintf(stdout,'%s\n', packet);
        if packet(1) == '#'
            if ~checkCheckSum(packet)
                continue;
            end
            tokens = strsplit(packet(2:end), ',');
            switch tokens{1}
                case 'BS' % BoatState
                    updateBoatState(tokens);
                case 'CP' % Carrot Point
                    updateCarrotPoint(tokens);
                case 'WPS' % Current Way-Points
                    updateWayPoints(tokens);
                case 'CS'
                    updateCtrlSignals(tokens);
                case 'RCR'
                    updateRCSignals(tokens);
            end
        else
            disp(packet)
        end
        if update_count >= update_cnt_period
            if enable_live_view
                if ~isempty(x_ax_limit)
                    xlim(ax_bird_view, x_ax_limit);
                    ylim(ax_bird_view, y_ax_limit);
                    axis(ax_bird_view, 'equal');
                end
                drawnow
                if isVideoRecordEnabled()
                    if isempty(video_record.last_t)
                        video_record.last_t = boat_state.stamp;
                    end
                    if boat_state.stamp - video_record.last_t > 1000
                        NF = length(video_record.frame10);
                        if NF > 10
                            warning('More than 10 frames')
                            NF = 10;
                        end
                        for i=1:NF
                            video_record.video.writeVideo(video_record.frame10{i});
                        end
                        for i=1:10-NF
                            video_record.video.writeVideo(video_record.frame10{end});
                        end
                        video_record.last_t = boat_state.stamp;
                        video_record.frame10 = {};
                    else
                        video_record.frame10{end+1} = getframe(figure(1));
                    end
                end
            end
            update_count = 0;
        end
    end
catch ME
    warning(sprintf('Exception at Packet : %s',packet))
    warning(getReport(ME))
end
if isVideoRecordEnabled()
    video_record.video.close();
end
axis(ax_bird_view, 'equal');
end

function new_limit = updateLimit(old_limit, new_limit_data)
if isempty(old_limit)
    new_limit = new_limit_data + [-5 5];
    return;
end
new_limit = old_limit;
if abs(new_limit(1)-new_limit_data(1)) > 200 || abs(new_limit(2)-new_limit_data(2))>200
    warning('LimitChange is too much!.Seems there is broken packet');
    return;
end
if new_limit(1) > new_limit_data(1)
    new_limit(1) = new_limit_data(1) - 5;
end
if new_limit(2) < new_limit_data(2)
    new_limit(2) = new_limit_data(2) + 5;
end
%%
% new_limit = new_limit_data + [-2 2];
end

function title_str = getTitleString(boat_state)
    switch boat_state.ctrl_mode
        case 1
            mode_str = 'manual';
            switch boat_state.ctrl_state
                case 1
                    state_str = 'Initializing';
                case 2
                    state_str = 'Running';
            end
        case 2
            mode_str = 'semiauto';
            switch boat_state.ctrl_state
                case 1
                    state_str = 'Initializing';
                case 2
                    state_str = 'Running';
                case 3
                    state_str = 'Idle';
                case 4
                    state_str = 'LeftTurn';
                case 5
                    state_str = 'RightTurn';
                case 6
                    state_str = 'Homming';
            end
        otherwise
            mode_str = strcat('Unknown - ', num2str(boat_state.ctrl_mode));
            state_str = strcat('Unknown -', num2str(boat_state.ctrl_state));
    end
    title_str = sprintf('%s - %s', mode_str, state_str);
end

function updateBoatState(tokens)
global traj;
global ax_trj;
global boat_state;
global delta_t;
global ax_delta_t;
global last_boat_state;
global max_n_trj;
global x_ax_limit;
global y_ax_limit;
global update_count;
global ax_carrot_point;
global ax_way_points;
global ax_boat_position;
global max_n_delta_t;
global use_absolute_pos;
global origin_pos;
global ax_bird_view;
global ax_control_loop_period;
global enable_video_record;
global h_line_boat_speed;
global ax_boat_speed;
global boat_speed;
global boat_speed_t;
persistent handle_title;
if length(tokens) ~= 9
    return;
end

%% update boat_state
boat_state_tmp = struct('stamp',str2double(tokens{2}),...
    'ctrl_mode',str2double(tokens{3}),'ctrl_state',str2double(tokens{4}),...
    'x',str2double(tokens{5}),'y',str2double(tokens{6}),'theta',str2double(tokens{7}),...
    'speed',0,...
    'n_satellites',str2double(tokens{8}));
if isempty(boat_state)
    if use_absolute_pos == 0
        setOriginPos(boat_state_tmp.x, boat_state_tmp.y);
    else
        setOriginPos(0,0);
    end
end
boat_state_tmp.x = boat_state_tmp.x - origin_pos(1);
boat_state_tmp.y = boat_state_tmp.y - origin_pos(2);
if ~isempty(boat_state)
    if abs(boat_state_tmp.x-boat_state.x) > 2 || abs(boat_state_tmp.y-boat_state.y) > 2
        % detect broken state packet(undetectable using check-sum)
        x = boat_state_tmp.x;
        y = boat_state_tmp.y;
        if use_absolute_pos == 0
            x = x + origin_pos(1);
            y = y + origin_pos(2);
        end
        warning(sprintf('Broken State Packet %f %f', x, y))
        return;
    end    
end
boat_state_tmp.speed = speedLowPassFilter(0.001*boat_state_tmp.stamp, [boat_state_tmp.x, boat_state_tmp.y]);
boat_state = boat_state_tmp;
%%

%% show state text
state_txt = sprintf('stamp:%u\npos:%.3f %.3f %.1f', boat_state.stamp,...
    boat_state.x, boat_state.y, rad2deg(boat_state.theta));

%% do not plot carrot point and way-points in manual mode.
if ~isempty(last_boat_state) && boat_state.ctrl_mode == 1 && last_boat_state.ctrl_mode == 2
    if ~isempty(ax_carrot_point)
        set(ax_carrot_point, 'XData',[],'YData',[]);
    end
    if ~isempty(ax_way_points)
        set(ax_way_points, 'XData',[], 'YData',[]);
    end
end

%%
boat_speed_t(end+1) = 0.001*boat_state.stamp;
boat_speed(end+1) = boat_state.speed;

if isempty(h_line_boat_speed)
    axes(ax_boat_speed);
    h_line_boat_speed = plot(boat_speed_t, boat_speed,'k'); grid on;
    xlabel('t(sec)','FontSize',15);
    ylabel('speed(m/s)','FontSize',15);
    title('Speed','FontSize',15);
    ylim([0 5]);
else
    set(h_line_boat_speed, 'XData',boat_speed_t, 'YData', boat_speed);
end
%%
boat_vertices = getTriangularBoatVertices(1, boat_state.x, boat_state.y, boat_state.theta);
if isempty(ax_boat_position)
    axes(ax_bird_view);
    ax_boat_position = plot(boat_vertices(1,:), boat_vertices(2,:),'r','LineWidth',2);hold on; grid on;
    xlabel('X(m)','FontSize',15);ylabel('Y(m)','FontSize',15);
else
    set(ax_boat_position, 'XData', boat_vertices(1,:), 'YData', boat_vertices(2,:));
end
%% update trajectory
traj(end+1,:) = [boat_state.x boat_state.y boat_state.theta];

%% update period
if ~isempty(last_boat_state)
    delta_t(end+1) = boat_state.stamp - last_boat_state.stamp;
end

if isempty(handle_title)
    axes(ax_bird_view);
    handle_title = title(getTitleString(boat_state),'FontSize',20); 
else
    handle_title.String = getTitleString(boat_state);
end
%%
if max_n_delta_t>0 && length(delta_t) > max_n_delta_t
    delta_t(1) = [];
end
if isempty(ax_delta_t) && ~isempty(delta_t)
    axes(ax_control_loop_period); grid on; hold on;
    ax_delta_t = plot(1, delta_t,'LineWidth',1);
    title('Control Loop Period(ms)','FontSize',15);
else
    set(ax_delta_t, 'XData', 1:length(delta_t), 'YData', delta_t);
end

if isempty(ax_trj)
    axes(ax_bird_view);  grid on;
    ax_trj = plot(traj(:,1), traj(:,2),'k.','MarkerSize',8);
else
if max_n_trj>0 && length(traj) > max_n_trj
    set(ax_trj, 'XData', traj(end-max_n_trj:end,1), 'YData',traj(end-max_n_trj:end,2));
else
    set(ax_trj, 'XData', traj(:,1), 'YData',traj(:,2));
end
end   
x_ax_limit = updateLimit(x_ax_limit, minmax(traj(:,1)'));
y_ax_limit = updateLimit(y_ax_limit, minmax(traj(:,2)'));

last_boat_state = boat_state;
if isVideoRecordEnabled()
    update_count = update_count + 1000;
else
    update_count = update_count + 1;
end
end

function updateCarrotPoint(tokens)
global x_ax_limit;
global y_ax_limit;
global ax_carrot_point;
global carrot_point;
global origin_pos;
global use_absolute_pos;
global ax_bird_view;
if length(tokens) ~= 4
    return;
end
carrot_point = [str2double(tokens{2}), str2double(tokens{3})];
if ~use_absolute_pos
    if isempty(origin_pos)
        setOriginPos(carrot_point(1), carrot_point(2));
    end
    carrot_point = carrot_point - origin_pos;
end
if isempty(ax_carrot_point)
    axes(ax_bird_view); grid on;
    ax_carrot_point = plot(carrot_point(1), carrot_point(2), 'rx','MarkerSize',15,'LineWidth',2);
else
    ax_carrot_point.XData = carrot_point(1);
    ax_carrot_point.YData = carrot_point(2);
end
x_ax_limit = updateLimit(x_ax_limit, minmax(carrot_point(:,1)'));
y_ax_limit = updateLimit(y_ax_limit, minmax(carrot_point(:,2)'));

end
%%
function updateWayPoints(tokens)
global x_ax_limit;
global y_ax_limit;
global ax_way_points;
global way_points;
global update_count;
global origin_pos;
global use_absolute_pos;
global ax_bird_view;
global aggregate_way_points;
n_wps = str2double(tokens{2});
if 2*n_wps+3 == length(tokens)
    way_points = zeros(n_wps,2);
    for i=1:n_wps
        way_points(i,:) = [str2double(tokens{2+2*(i-1)+1}),...
            str2double(tokens{2+2*(i-1)+2})];
    end
    if ~use_absolute_pos
        if isempty(origin_pos)
            setOriginPos(way_points(1,1), way_points(1,2));
        end
        way_points = way_points - repmat(origin_pos, size(way_points,1), 1);
    end
    if isempty(ax_way_points)
        axes(ax_bird_view); hold on; grid on;
        ax_way_points = plot(way_points(:,1), way_points(:, 2), 'bo-'...
            ,'MarkerSize',7,'MarkerEdgeColor','k','MarkerFaceColor','m',...
            'LineWidth',1);
    else
        if aggregate_way_points
            plot(way_points(:,1), way_points(:, 2), 'bo-'...
            ,'MarkerSize',7,'MarkerEdgeColor','k','MarkerFaceColor','m',...
            'LineWidth',1);
        else
            ax_way_points.XData = way_points(:,1);
            ax_way_points.YData = way_points(:,2);
        end
    end
    x_ax_limit = updateLimit(x_ax_limit, minmax(way_points(:,1)'));
    y_ax_limit = updateLimit(y_ax_limit, minmax(way_points(:,2)'));
else
    warning(sprintf('way-points packet seems broken(n_tokens=%d)', length(tokens)))
end
    update_count = update_count + 10;
end
%%
function updateCtrlSignals(tokens)
global servo_time_stamps;
global ax_rudder_servo_signal;
global rudder_servo_signal;
global ax_throtle_servo_signal;
global throtle_servo_signal;
global boat_state;
global max_n_servo_signal;
global ax_servo_control_signal;
persistent servo_time_stamp0;

if length(tokens) ~= 4
    return;
end
if isempty(boat_state)
    return;
end
if max_n_servo_signal>0 && length(throtle_servo_signal) == max_n_servo_signal
    throtle_servo_signal(1) = [];
    rudder_servo_signal(1) = [];
    servo_time_stamps(1) = [];
end
if isempty(servo_time_stamp0)
    servo_time_stamp0 = 0.001*boat_state.stamp;
end
servo_time_stamps(end+1) = 0.001*boat_state.stamp - servo_time_stamp0;
rudder_servo_signal(end+1) = str2double(tokens{2});
throtle_servo_signal(end+1) = str2double(tokens{3});
if isempty(ax_rudder_servo_signal)
    axes(ax_servo_control_signal); hold on;
    ax_rudder_servo_signal = plot(servo_time_stamps, rudder_servo_signal, 'k','LineWidth',1,'DisplayName','Rudder');
    ylim([750 2250]);
    grid on;
    title('Control Signals(RC Signal - \musec)','FontSize',15);
    legend('off'); legend('show');
else
    set(ax_rudder_servo_signal,'XData', servo_time_stamps, 'YData', rudder_servo_signal);
end
if isempty(ax_throtle_servo_signal)
    axes(ax_servo_control_signal); hold on;
    ax_throtle_servo_signal = plot(servo_time_stamps, throtle_servo_signal, 'r','LineWidth',1,'DisplayName','Throtle');
    ylim([750 2250]);
    grid on;
    legend('off'); legend('show');
else
    set(ax_throtle_servo_signal,'XData', servo_time_stamps, 'YData', throtle_servo_signal);
end
end
%%
function updateRCSignals(tokens)
global rc_signal_time_stamps;
global ax_rc_rudder_signal;
global rc_rudder_signal;
global ax_rc_throtle_signal;
global rc_throtle_signal;
global max_n_rc_signal;
global boat_state;
global ax_rc_remocon_signal;
persistent rc_signal_time0;

if length(tokens) ~= 4
    return;
end
if isempty(boat_state)
    return;
end
if max_n_rc_signal>0 && length(rc_throtle_signal) == max_n_rc_signal
    rc_throtle_signal(1) = [];
    rc_rudder_signal(1) = [];
    rc_signal_time_stamps(1) = [];
end
if isempty(rc_signal_time0)
    rc_signal_time0 =0.001*boat_state.stamp;
end
rc_signal_time_stamps(end+1) = 0.001*boat_state.stamp - rc_signal_time0;

rc_rudder_signal(end+1) = str2double(tokens{2});
rc_throtle_signal(end+1) = str2double(tokens{3});
if isempty(ax_rc_rudder_signal)
    axes(ax_rc_remocon_signal); hold on;
    ax_rc_rudder_signal = plot(rc_signal_time_stamps, rc_rudder_signal, 'k','LineWidth',1,'DisplayName','Rudder');
    ylim([750 2250]);
    grid on;
    title('User Input(RC Signal - \musec)','FontSize',15);
    legend('off'); legend('show');
else
    set(ax_rc_rudder_signal,'XData', rc_signal_time_stamps, 'YData', rc_rudder_signal);
end
if isempty(ax_rc_throtle_signal)
    axes(ax_rc_remocon_signal); hold on;
    ax_rc_throtle_signal = plot(rc_signal_time_stamps, rc_throtle_signal, 'r','LineWidth',1,'DisplayName','Throtle');
    ylim([750 2250]);
    grid on;
    legend('off'); legend('show');
else
    set(ax_rc_throtle_signal,'XData', rc_signal_time_stamps, 'YData', rc_throtle_signal);
end
end

function vertices = getTriangularBoatVertices(scale, x,y,theta)
    T = [cos(theta) -sin(theta) x; sin(theta) cos(theta) y; 0 0 1];
    vertices = T * scale * [1.5    0   0   1.5; ...
                            0   -0.5  0.5  0; ...
                            1     1    1   1];
    vertices = vertices(1:2,:);
end

function setOriginPos(x,y)
global origin_pos;
origin_pos = [x,y];
end

function enabled = isVideoRecordEnabled()
    global enable_video_record;
    if ~isempty(enable_video_record) && enable_video_record ~= 0
        enabled = 1;
    else
        enabled = 0;
    end
end
