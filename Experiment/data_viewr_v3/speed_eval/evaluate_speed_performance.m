function evaluate_speed_performance()
close all;

params.plot.fig_trj = open('20170111171435_trajectory_2p5s.fig');
params.plot.ax_trj = gca;
params.plot.fig_speed = open('20170111171435_speed_2p5s.fig');
params.plot.ax_speed = gca;
 
figure();
set(gcf,'Color',[1 1 1]);
params.plot.result.fig = gcf;
params.plot.result.ax_trj = subplot(1,2,1);
params.plot.result.ax_speed = subplot(1,2,2);
%%
xdata = params.plot.fig_trj.Children(1).Children(1).XData;
ydata = params.plot.fig_trj.Children(1).Children(1).YData;
speed = params.plot.fig_speed.Children.Children(1).YData;
speed_t = params.plot.fig_speed.Children.Children(1).XData;
X = [1:length(xdata);xdata;ydata];
V = [1:length(xdata);speed_t;speed];

params.plot.h_line_trj_selected = [];
params.plot.h_line_speed_selected = [];
params.plot.result.h_line_trj = [];
params.plot.result.h_line_speed = [];
params.selected.selected_flag = zeros(1, length(X));
params.selected.idx = [];
params.selected.trj = struct('x',[],'y',[]);
params.selected.speed = struct('t',[],'speed',[]);
%%
set(params.plot.fig_trj, 'KeyPressFcn', @handleKey);
running = 1;
figure(params.plot.fig_trj);
while running
    pause(0.1);
end

%%
function handleKey(~, e)
    switch lower(e.Character)
        case 'f'
            evaluatePerformance();
        case 'a'
            selectData();
            updatePlots();
        case 'q'
            running = 0;
        case 's'
            disp('saving the result...')
            name = datestr(now(),'yyyymmddHHMMSS');
            savefig(params.plot.result.fig, strcat('speed_eval/result_2p5s/', name,'.fig'));
            % https://kr.mathworks.com/help/matlab/ref/print.html#bumf4tm-1
            set(params.plot.result.fig,'PaperPositionMode','auto');
            print(params.plot.result.fig, strcat('speed_eval/result_2p5s/', name,'.png'),'-dpng','-r0');
            print(params.plot.fig_trj, strcat('speed_eval/result_2p5s/',name,'_trj.png'), '-dpng','-r0');
        case 'c'
            clearSelections();
            updatePlots();
    end
    figure(params.plot.fig_trj);
end

%% 
function selectData()
    axes(params.plot.ax_trj);
    r = imrect();
    rect = r.getPosition();
    rect(3) = rect(1) + rect(3);
    rect(4) = rect(2) + rect(4);
    ind_sel = X(2,:) > rect(1) & X(2,:) < rect(3) & X(3,:) > rect(2) & X(3,:) < rect(4) & params.selected.selected_flag ==0;
    ind_sel = find(ind_sel == 1);
    if ~isempty(ind_sel)
        clusters = simpleDataClustering(ind_sel, 20);
        if length(clusters) > 1
            if isempty(params.selected.idx)
                ind_sel = clusters{1};
            else
                cluster_dist = realmax;
                c_sel = 1;
                for ic= 1:length(clusters)
                    dataset = clusters{ic};
                    max_dt = max(dataset);
                    max_tgt = max(params.selected.idx);
                    min_dt = min(dataset);
                    min_tgt = min(params.selected.idx);
                    if max_dt<max_tgt&&min_dt>min_tgt
                        c_sel = ic;
                        break;
                    end
                    c_dist = min([abs(max_dt-max_tgt), abs(max_dt-min_tgt), abs(min_dt-max_tgt), abs(min_dt-min_tgt)]);
                    if c_dist < cluster_dist                
                        cluster_dist = c_dist;
                        c_sel = ic;
                    end
                end
                ind_sel = clusters{c_sel};
            end
        end
        params.selected.selected_flag(ind_sel) = 1;
        
        params.selected.idx = sort([params.selected.idx X(1,ind_sel)]);
        params.selected.trj.x = X(2,params.selected.idx);
        params.selected.trj.y = X(3,params.selected.idx);
        params.selected.speed.t = V(2,params.selected.idx);
        params.selected.speed.speed = V(3,params.selected.idx);
    end
    r.delete();
end

function clearSelections()
    params.selected.selected_flag = zeros(1, length(X));
    params.selected.idx = [];
    params.selected.trj = struct('x',[],'y',[]);
    params.selected.speed = struct('t',[],'speed',[]);
end

function updatePlots()
    if isempty(params.plot.h_line_trj_selected)
        axes(params.plot.ax_trj); hold on;
        params.plot.h_line_trj_selected =...
            plot(params.plot.ax_trj, ...
            params.selected.trj.x, params.selected.trj.y,'r.');
        axes(params.plot.ax_speed); hold on;
        params.plot.h_line_speed_selected = ...
            plot(params.plot.ax_speed,...
            params.selected.speed.t, params.selected.speed.speed,'r');
    else
        set( params.plot.h_line_trj_selected, ...
            'XData', params.selected.trj.x, ...
            'YData', params.selected.trj.y);
        set(params.plot.h_line_speed_selected, ...
            'XData', params.selected.speed.t, ...
            'YData', params.selected.speed.speed);
    end
end

%%
function evaluatePerformance()
    params.result.speed.mean = mean(params.selected.speed.speed);
    params.result.speed.stdev = std(params.selected.speed.speed);
    [params.result.speed.min_v, min_idx] = min(params.selected.speed.speed);
    [params.result.speed.max_v, max_idx] = max(params.selected.speed.speed);
    params.result.trj.displace = accumDisplacement(params.selected.trj.x, params.selected.trj.y);
    params.result.trj.travel_time = params.selected.speed.t(end) - params.selected.speed.t(1);
    
    cla(params.plot.result.ax_trj);
    params.plot.result.h_line_trj = plot(params.plot.result.ax_trj, ...
        params.selected.trj.x, params.selected.trj.y,'k.','MarkerSize',5);
    xlim(params.plot.result.ax_trj, [min(params.selected.trj.x)-5 max(params.selected.trj.x)+5]);
    axis(params.plot.result.ax_trj, 'equal');
    set(params.plot.result.ax_trj,'XGrid','on','YGrid','on');
    min_p = [params.selected.speed.t(min_idx), params.result.speed.min_v];
    max_p = [params.selected.speed.t(max_idx), params.result.speed.max_v];
    
    cla(params.plot.result.ax_speed);
    params.plot.result.h_line_speed = plot(params.plot.result.ax_speed, ...
        params.selected.speed.t, params.selected.speed.speed,'k');
    
    ylim(params.plot.result.ax_speed, [0 5]);
    set(params.plot.result.ax_speed,'XGrid','on','YGrid','on');
    
    drawnow;
    
    axes(params.plot.result.ax_speed); 
    hold on;
    plot(params.plot.result.ax_speed.XLim, params.result.speed.mean*ones(1,2),'r:','LineWidth',2);
    
    h_arr = annotation('textarrow');
    h_arr.Parent = params.plot.result.ax_speed;
    h_arr.X = [min_p(1)-5 min_p(1)];
    h_arr.Y = [min_p(2) min_p(2)];
    h_arr.String = sprintf('%.3f m/s',params.result.speed.min_v);

    h_arr = annotation('textarrow');
    h_arr.Parent = params.plot.result.ax_speed;
    h_arr.X = [max_p(1)-5 max_p(1)];
    h_arr.Y = [max_p(2) max_p(2)];
    h_arr.String = sprintf('%.3f m/s',params.result.speed.max_v);
    
    text(params.plot.result.ax_speed.XLim(1)+2, params.plot.result.ax_speed.YLim(2)-0.1,...
    sprintf('average : %.3f\nstdev : %.3f\nmin : %.3f\nmax : %.3f',...
                params.result.speed.mean,params.result.speed.stdev,...
                params.result.speed.min_v, params.result.speed.max_v),...
                'VerticalAlignment', 'top');
    title('Speed','FontSize',15);
    xlabel('t(sec)','FontSize',15);
    ylabel('speed(m/s)','Fontsize',15);
    
            
    axes(params.plot.result.ax_trj);
    text(params.plot.result.ax_trj.XLim(1)+2, params.plot.result.ax_trj.YLim(2)-0.1, ...
        sprintf('travel : %.1f m\ntravel time : %.3f sec', ...
        params.result.trj.displace, params.result.trj.travel_time),...
        'VerticalAlignment','top');
    title('Trajectory','FontSize',15);
    xlabel('X(m)','FontSize',15);
    ylabel('Y(m)','Fontsize',15);
end

%%
function showHelpText()
    disp('Press a key to proceed')
    disp('  a : select data')
    disp('  f : finish selection and evaluate the data')
    disp('  s : save result')
    disp('  c : clear selection')
    disp('  q : quit')
    
end
end

%%
function c = randomColor()
    persistent cmap;
    persistent cnt;
    if isempty(cmap)
        cmap = hsv(20);
        cmap = cmap(randperm(20)', :);
        cnt = 1;
    end
    c = cmap(cnt,:);
    cnt = cnt + 1;
end

function displacement = accumDisplacement(x,y)
    displacement = 0;
    for i=2:length(x)
        displacement = displacement + norm([x(i) y(i)] - [x(i-1) y(i-1)]);
    end
end
