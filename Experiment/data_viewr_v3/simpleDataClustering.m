function clusters = simpleDataClustering(data, threshold, dim)
% data : N(number of sample data) X Dimensions
    if nargin < 3
        dim = 1;
    end
    if length(data) ~= size(data,1)
        data = data';
    end
    %% sort data
    [X, I] = sort(data(:,dim),'ascend');
    data = data(I, :);
    %% create first cluster
    clusters = {[data(1,:)]};
    cl_cnt = 1;
    idx = 2;
    N = size(data,1);
    p = data(1, dim);
    %% start clustering
    while idx <= N
        d = data(idx, dim);
        if d - p > threshold
            %% new cluster
            cl_cnt = cl_cnt + 1;
            clusters{cl_cnt} = [data(idx, :)];
        else
            %% insert data to current cluster
            clusters{cl_cnt}(end+1, :) = data(idx, :);
        end
        p = d;
        idx = idx +1;
    end
end