function [trj] = run_data_viewer(file_handle, f_log)
global traj;
global boat_state;
global last_boat_state;
global delta_t;
global max_n_trj;
global max_n_delta_t;
global max_n_servo_signal;
global x_ax_limit;
global y_ax_limit;
global carrot_point;
global way_points;
global update_count;
global enable_live_view;
global ax_boat_position;
global ax_way_points;
global ax_carrot_point;
global ax_delta_t;
global ax_trj;
global ax_state_text;
global use_absolute_pos;
update_cnt_period = 8;
%%
clear updateBoatState % clear persistent variables
%%
figure(1); clf; hold on;
set(figure(1),'Color',[1 1 1]);

%%
ax_state_text = [];
ax_trj = [];
ax_way_points = [];
trj = [];
ax_carrot_point = [];
update_count = 0;
delta_t = [];
ax_delta_t = [];
ax_boat_position = [];
try
    while 1
        packet = fgetl(file_handle);
        if ~ischar(packet)
            warning('communication finished!');
            break;
        end
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
                case 'BoatState' % BoatState
                    updateBoatState(tokens);
                case 'Carrot' % Carrot Point
                    updateCarrotPoint(tokens);
                case 'WPS' % Current Way-Points
                    updateWayPoints(tokens);
                case 'CtrlSignals'
                    updateCtrlSignals(tokens);
            end
        else
            disp(packet)
        end
        if update_count >= update_cnt_period
            if enable_live_view
                if ~isempty(x_ax_limit)
                    xlim(x_ax_limit);
                    ylim(y_ax_limit);
                end
                axis equal
                drawnow
            end
            update_count = 0;
        end
    end
catch ME
    sprintf('Packet : %s',packet)
    warning(getReport(ME))
end
end

function new_limit = updateLimit(old_limit, new_limit_data)
if isempty(old_limit)
    new_limit = new_limit_data + [-5 5];
    return;
end
new_limit = old_limit;
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
global ax_state_text;
global max_n_delta_t;
global use_absolute_pos;
global origin_pos;
if length(tokens) ~= 8
    return;
end
    
%% update boat_state
if isempty(boat_state)
    boat_state = struct('stamp',0,'x',0,'y',0,'theta',0,'ctrl_mode',0,'ctrl_state',0);
end
boat_state.stamp= str2double(tokens{2});
boat_state.ctrl_mode = str2double(tokens{3});
boat_state.ctrl_state = str2double(tokens{4});
boat_state.x = str2double(tokens{5});
boat_state.y = str2double(tokens{6});
boat_state.theta = str2double(tokens{7});
% boat_state.theta = -1*str2double(tokens{7})-0.5*pi+8.15*pi/180;
% boat_state.theta = normalizeHeadingRad(boat_state.theta);
%%
if use_absolute_pos == 0
    if isempty(origin_pos)
        setOriginPos(boat_state.x, boat_state.y);
    end
    boat_state.x = boat_state.x - origin_pos(1);
    boat_state.y = boat_state.y - origin_pos(2);
end
%% show state text
subplot(2,2,[1,2]);
state_txt = sprintf('stamp:%u\npos:%.3f %.3f %.1f', boat_state.stamp,...
    boat_state.x, boat_state.y, rad2deg(boat_state.theta));
if isempty(ax_state_text)
    xlim_ = get(gca,'XLim');
    ylim_ = get(gca,'YLim');
    ax_state_text = text(xlim_(1)+0.05*diff(xlim_), ...
        ylim_(2)-0.05*diff(ylim_),state_txt); hold on;
else
    xlim_ = get(gca,'XLim');
    ylim_ = get(gca,'YLim');
    ax_state_text.Position(1) =xlim_(1)+0.05*diff(xlim_);
    ax_state_text.Position(2) =ylim_(2)-0.05*diff(ylim_);
    ax_state_text.String = state_txt;
end
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
boat_vertices = getTriangularBoatVertices(1, boat_state.x, boat_state.y, boat_state.theta);
if isempty(ax_boat_position)
    subplot(2,2,[1,2]);
    ax_boat_position = plot(boat_vertices(1,:), boat_vertices(2,:),'r','LineWidth',2);hold on; grid on;
else
    set(ax_boat_position, 'XData', boat_vertices(1,:), 'YData', boat_vertices(2,:));
end
%% update trajectory
traj(end+1,:) = [boat_state.x boat_state.y boat_state.theta];

%% update period
if ~isempty(last_boat_state)
    delta_t(end+1) = boat_state.stamp - last_boat_state.stamp;
end

subplot(2,2,[1,2]);
title(getTitleString(boat_state),'FontSize',20); 
if max_n_delta_t>0 && length(delta_t) > max_n_delta_t
    delta_t(1) = [];
end
if isempty(ax_delta_t) && ~isempty(delta_t)
    subplot(2,2,3); grid on; hold on;
    ax_delta_t = plot(1, delta_t,'LineWidth',1);
    title('Control Loop Period(ms)','FontSize',15);
else
    set(ax_delta_t, 'XData', 1:length(delta_t), 'YData', delta_t);
end

if isempty(ax_trj)
    subplot(2,2,[1,2]);  grid on;
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
update_count = update_count + 1;
end

function updateCarrotPoint(tokens)
global x_ax_limit;
global y_ax_limit;
global ax_carrot_point;
global carrot_point;
global origin_pos;
global use_absolute_pos;
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
    subplot(2,2,[1,2]); grid on;
    ax_carrot_point = plot(carrot_point(1), carrot_point(2), 'r.','MarkerSize',15);
else
    ax_carrot_point.XData = carrot_point(1);
    ax_carrot_point.YData = carrot_point(2);
end
x_ax_limit = updateLimit(x_ax_limit, minmax(carrot_point(:,1)'));
y_ax_limit = updateLimit(y_ax_limit, minmax(carrot_point(:,2)'));

end

function updateWayPoints(tokens)
global x_ax_limit;
global y_ax_limit;
global ax_way_points;
global way_points;
global update_count;
global origin_pos;
global use_absolute_pos;
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
        subplot(2,2,[1,2]); hold on; grid on;
        ax_way_points = plot(way_points(:,1), way_points(:, 2), 'bx:','MarkerSize',15, 'LineWidth',3);
    else
        ax_way_points.XData = way_points(:,1);
        ax_way_points.YData = way_points(:,2);
    end
    x_ax_limit = updateLimit(x_ax_limit, minmax(way_points(:,1)'));
    y_ax_limit = updateLimit(y_ax_limit, minmax(way_points(:,2)'));
else
    warning(sprintf('way-points packet seems broken(n_tokens=%d)', length(tokens)))
end
    update_count = update_count + 10;
end

function updateCtrlSignals(tokens)
global servo_time_stamps;
global ax_rudder_servo_signal;
global rudder_servo_signal;
global ax_throtle_servo_signal;
global throtle_servo_signal;
global boat_state;
global max_n_servo_signal;
if length(tokens) ~= 6
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
servo_time_stamps(end+1) = boat_state.stamp;
rudder_servo_signal(end+1) = str2double(tokens{2});
throtle_servo_signal(end+1) = str2double(tokens{3});
if isempty(ax_rudder_servo_signal)
    subplot(2,2,4); hold on;
    ax_rudder_servo_signal = plot(servo_time_stamps, rudder_servo_signal, 'k','LineWidth',2,'DisplayName','Rudder');
    grid on;
    title('Servo Signals(\musec)','FontSize',15);
    legend('off'); legend('show');
else
    set(ax_rudder_servo_signal,'XData', servo_time_stamps, 'YData', rudder_servo_signal);
end
if isempty(ax_throtle_servo_signal)
    subplot(2,2,4); hold on;
    ax_throtle_servo_signal = plot(servo_time_stamps, throtle_servo_signal, 'r','LineWidth',2,'DisplayName','Throtle');
    grid on;
    legend('off'); legend('show');
else
    set(ax_throtle_servo_signal,'XData', servo_time_stamps, 'YData', throtle_servo_signal);
end
end

function vertices = getTriangularBoatVertices(scale, x,y,theta)
    T = [cos(theta) -sin(theta) x; sin(theta) cos(theta) y; 0 0 1];
    vertices = T * scale * [1 0 0 1;0 -0.3 0.3 0;1 1 1 1];
    vertices = vertices(1:2,:);
end

function setOriginPos(x,y)
global origin_pos;
origin_pos = [x,y];
end