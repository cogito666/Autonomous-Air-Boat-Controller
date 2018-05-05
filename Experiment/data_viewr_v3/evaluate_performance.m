function evaluate_performance()
close all;
folder = 'fieldtest5';
fig_name = '20170111172626_manual_turning1_trajectories.fig';
open(strcat(folder,'/', fig_name));
fig = gcf;
%%
xdata = fig.Children.Children(1).XData;
ydata = fig.Children.Children(1).YData;
X = [xdata;ydata;1:length(xdata);zeros(1,length(xdata))];
%%
set(fig, 'KeyPressFcn', @handleKey);
params.mode = 0; % 0:idle, 1:data selection, 2:fitting&drawing
params.line = createEmptySampleStruct();
params.idx_current = 1;
params.ax_canvas = gca;
params.h_line_original = fig.Children.Children(1);
text(params.ax_canvas.XLim(1)+1,params.ax_canvas.YLim(2),...
    sprintf('travel : [length]m\n(mean/var/min/max)'), 'VerticalAlignment','top','HorizontalAlignment','left','FontWeight','bold');
showHelpText()
while 1
    if length(params.line) ~= params.idx_current
        params.line(params.idx_current) = createEmptySampleStruct();
    end
    switch params.mode 
        case 0
            pause(0.1);
        case 1
            selectData();
            params.mode = 0;
        case 2
            alignSampleData();
            evaluatePerformance();
            params.idx_current = params.idx_current + 1;
            params.mode = 0;
        case -1
            set(fig, 'KeyPressFcn', []);
            break;
    end
end

%%
function handleKey(~, e)
    switch lower(e.Character)
        case 'f'
            params.mode = 2;
        case 'a'
            if params.mode ~= 1
                params.mode = 1;
            end
        case 'q'
            params.mode = -1;
        case 's'
            disp('saving the result...')
            set(fig, 'KeyPressFcn', []);
            savefig(fig, strcat('performance_eval/', fig_name));
            % https://kr.mathworks.com/help/matlab/ref/print.html#bumf4tm-1
            set(fig,'PaperPositionMode','auto');
            print(fig, strcat('performance_eval/', fig_name(1:end-3),'png'),'-dpng','-r0');
            set(fig, 'KeyPressFcn', @handleKey);
    end
    showHelpText()
end

%% 
function selectData()
    current_idx = params.idx_current;
    
    r = imrect();
    rect = r.getPosition();
    rect(3) = rect(1) + rect(3);
    rect(4) = rect(2) + rect(4);
    ind_sel = X(1,:) > rect(1) & X(1,:) < rect(3) & X(2,:) > rect(2) & X(2,:) < rect(4) & X(4,:)==0;
    ind_sel = find(ind_sel == 1);
    if ~isempty(ind_sel)
        clusters = simpleDataClustering(ind_sel, 20);
        if length(clusters) > 1
            if isempty(params.line(current_idx).sample.idx)
                ind_sel = clusters{1};
            else
                cluster_dist = realmax;
                c_sel = 1;
                for ic= 1:length(clusters)
                    dataset = clusters{ic};
                    max_dt = max(dataset);
                    max_tgt = max(params.line(current_idx).sample.idx);
                    min_dt = min(dataset);
                    min_tgt = min(params.line(current_idx).sample.idx);
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

        params.line(current_idx).sample.x = [params.line(current_idx).sample.x X(1,ind_sel)];
        params.line(current_idx).sample.y = [params.line(current_idx).sample.y X(2,ind_sel)];
        params.line(current_idx).sample.idx = [params.line(current_idx).sample.idx X(3,ind_sel)];
        params.line(current_idx).sample.idx(1)
        if isempty(params.line(current_idx).ax_sample)
            params.line(current_idx).ax_sample =...
                plot(params.ax_canvas, ...
                params.line(current_idx).sample.x, params.line(current_idx).sample.y,...
                '.', 'Color', params.line(current_idx).color);
        else
            set(params.line(current_idx).ax_sample, ...
                'XData', params.line(current_idx).sample.x, ...
                'YData', params.line(current_idx).sample.y);
        end
        X(4,ind_sel) = 1;
%         X(:,ind_sel) = [];
%         set(params.h_line_original,'XData', X(1,:), 'YData', X(2,:));
    end
    r.delete();
end

%%
function evaluatePerformance()
    idx_current = params.idx_current; 
    a = polyfit(params.line(idx_current).sample.x, params.line(idx_current).sample.y, 1);
    params.line(idx_current).fitted = [a(1) -1 a(2)];   
    params.line(idx_current).ax_fitted =...
    draw_line2d(params.ax_canvas, params.line(idx_current).fitted);
    set(params.line(idx_current).ax_fitted, 'Color', params.line(idx_current).color);
    set(params.line(idx_current).ax_fitted, 'LineWidth', 2, 'LineStyle', '-.');
    
    %% evaluate go-straight performance
    params.result(idx_current).straight.err_samples = ...
        point_line_distance(params.line(idx_current).fitted, ...
        [params.line(idx_current).sample.x;params.line(idx_current).sample.y]);
    params.result(idx_current).straight.err_mean = mean(params.result(idx_current).straight.err_samples);
    params.result(idx_current).straight.err_var = var(params.result(idx_current).straight.err_samples);
    params.result(idx_current).straight.err_min = min(params.result(idx_current).straight.err_samples);
    params.result(idx_current).straight.err_max = max(params.result(idx_current).straight.err_samples);
    p1 = get_point_from_line(params.line(idx_current).fitted, params.line(idx_current).sample.x(1));
    p2 = get_point_from_line(params.line(idx_current).fitted, params.line(idx_current).sample.x(end));
    params.result(idx_current).straight.displace = norm(p1-p2);
    drawGoStraightAnnotation();
    
    
    %% evaluate turning performance
    if idx_current > 1
        p1 = get_point_from_line(params.line(idx_current).fitted, params.line(idx_current).sample.x(1));
        p2 = get_point_from_line(params.line(idx_current).fitted, params.line(idx_current).sample.x(end));
        op1 = point_line_intersection(...
            get_point_from_line(params.line(idx_current-1).fitted, 1),...
            get_point_from_line(params.line(idx_current-1).fitted, 5),...
            p1);
        op2 = point_line_intersection(...
            get_point_from_line(params.line(idx_current-1).fitted, 1),...
            get_point_from_line(params.line(idx_current-1).fitted, 5),...
            p2);
        dist1 = norm(op1-p1);
        dist2 = norm(op2-p2);
        params.result(idx_current).turn.width = [dist1 dist2];
        drawTurnAnnotation(p1, op1, dist1);
        drawTurnAnnotation(p2, op2, dist2);
    end
end

%% 
function alignSampleData()
    idx_current = params.idx_current;
    [params.line(idx_current).sample.idx, I] = sort(params.line(idx_current).sample.idx);
    params.line(idx_current).sample.x = params.line(idx_current).sample.x(I);
    params.line(idx_current).sample.y = params.line(idx_current).sample.y(I);
end

function drawTurnAnnotation(p1, p2, dist)
    doublearrow = annotation('doublearrow');
    doublearrow.Parent = params.ax_canvas;
    doublearrow.X = [p1(1) p2(1)];
    doublearrow.Y = [p1(2) p2(2)];
    
    h = text((p1(1) + p2(1)) / 2+0.2, (p1(2) + p2(2)) / 2, sprintf('%.3f', dist));
    set(h,'FontWeight','bold');
end

function drawGoStraightAnnotation()
    idx = params.idx_current;
    p = [params.line(idx).sample.x(1) params.line(idx).sample.y(1)];
    [px, I] = min(params.line(idx).ax_fitted.XData);
    py = params.line(idx).ax_fitted.YData(I);
    p = point_distant_from_line(params.line(idx).fitted, ...
        [px py],...
        -2.0);
    h = text(p(1)+1, p(2), sprintf('travel: %.1fm\n(%.3f/%.3f/%.3f/%.3f)',...
        params.result(idx).straight.displace,...
        params.result(idx).straight.err_mean, params.result(idx).straight.err_var,...
        params.result(idx).straight.err_min,params.result(idx).straight.err_max),...
        'BackgroundColor',[1 1 1 0.8]);
    set(h,'FontWeight','bold');
end
%%
function showHelpText()
    disp('Press a key to proceed')
    disp('  a : select data')
    disp('  f : finish selection and evaluate the data')
    disp('  s : save result')
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

function sample = createEmptySampleStruct()
    sample = struct('sample',struct('x', [], 'y',[], 'idx', []), 'color', randomColor(), ...
        'performance',struct(...
            'straight',struct('err_samples',[],'err_mean',0,'err_var',0,'err_min',0,'err_max',0,'displace',0),...
            'turn', []),...        
        'fitted', [0 0 0],'ax_sample', [], 'ax_fitted', []);
end

