fig = openfig('fieldtest5/20170111172626_manual_turning1_trajectories.fig');
%%
fig.Children.Children(1).XData(end-20:end) = [];
fig.Children.Children(1).YData(end-20:end) = [];
%%
fig.Children.Children(1).XData(1:10) = [];
fig.Children.Children(1).YData(1:10) = [];
