%% Prepare discarded and accepted frames file names
% Tjek acc og dis files - fjern img 0000
tic
donor = 'NS15';
% mkdir 'NS15'
path1 = 'F:\Lise Aagesen master thesis\RBCdata\FetalDonors\20170712_100x\20170712_100x\Donor_data';
path2 = strcat(path1,'\',donor,'\',donor,'.png');
% path3 = strcat(path1,'\RBCs');
% path4 = strcat(donor,'\RBCs');
% addpath(path1,path2);

file1 = strcat(donor,'_Accepted.txt');
pics = fopen(file1);
acc1 = textscan(pics,'%s');
acc = acc1{1,1};

file2 = strcat(donor,'_Discarded.txt');
pics = fopen(file2);
dis1 = textscan(pics,'%s');
dis = dis1{1,1};

frameno_acc = str2doubles(extractBetween(acc,"_","."));
frameno_dis = str2doubles(extractBetween(dis,"_","."));

if size(frameno_acc,1) + size(frameno_dis,1) < 79997
    disp('error')
    return
end

%% Parameters and initialization
inlet = 60;                         % nederste kant
outlet = 185;                       % nederste kant
yref = 60;                          % middle of inlet
edge_thres = 0.42;                 % threshold for extracting channel edge
se_edge = strel('rectangle',[2 40]);% se for extracting channel edge
bin_thres = 0.12;                   % threshold for extracting cells
cellNum = 0;                        % cell counter
tracker = [];                       % tracker for registered cells

load('edge_NS43.mat');
bg_edge = edge_NS43;

RBCs = struct('inlet',[],'outlet',[],'yref',[],'label','', ...
    'frame',[],'centroid',{{}},'box',{{}},'pixellist',{{}}, ...
    'circularity',[],'eccentricity',[],'majoraxis',[]);

%% Cell sorting
for i = 1:3000 % going through accepted frames
    
% -----------------------------------------------
% subtract background from intensity image
% -----------------------------------------------
if frameno_acc(i) < min(frameno_dis) || frameno_acc(i) > max(frameno_dis)
    [~,index] = min(abs(frameno_dis-frameno_acc(i)));
    bg = rgb2gray(im2double(imread(dis{index})));
else
    frameno_dis_before = frameno_dis(frameno_dis<frameno_acc(i));
    dis_before = dis(frameno_dis<frameno_acc(i));
    frameno_dis_after = frameno_dis(frameno_dis>frameno_acc(i));
    dis_after = dis(frameno_dis>frameno_acc(i));
    [~,index1] = min(abs(frameno_dis_before-frameno_acc(i)));
    [~,index2] = min(abs(frameno_dis_after-frameno_acc(i)));
    bg1 = rgb2gray(im2double(imread(dis_before{index1}))); 
    bg2 = rgb2gray(im2double(imread(dis_after{index2})));
    bg = (bg1+bg2)/2;
end  
im = rgb2gray(im2double(imread(acc{i})));
diff = imabsdiff(im,bg); 

% bg_edge = bg < edge_thres;
% bg_edge = imclose(bg_edge,se_edge);
% bg_edge(:,inlet-10:inlet+10) = 0;
% bg_edge = bwareaopen(bg_edge,100);
% bg_edge = imcomplement(bg_edge);

diff = diff.*bg_edge;
norm_diff = (diff-min(diff(:)))/(max(diff(:))-min(diff(:)));
    
% -----------------------------------------------
% cell segmentation through binarization and morphological operations
% -----------------------------------------------
bw = imbinarize(norm_diff,bin_thres);
bw = bwareaopen(bw,20); %fjerne noise
se = strel('disk',4);
bw = imclose(bw,se);
bw = imfill(bw,'holes');
bw = bwpropfilt(bw,'ConvexArea',[200 1450]);
bw = bwpropfilt(bw,'MajorAxisLength',[0 65]);
bw1 = bw;
cc = bwconncomp(bw1);
stats = regionprops(cc,'Centroid','Solidity');
centroids = cat(1, stats.Centroid);
sols = cat(1, stats.Solidity);
for j = 1:size(stats,1)
    if centroids(j,1)<inlet-20 || centroids(j,1)>inlet+20
        if sols(j) < 0.87
            bw1(cc.PixelIdxList{j}) = 0;
        end
    end
end
bw = imclearborder(bw1);

% -----------------------------------------------
% Cell identification and sorting through connected component analysis
% -----------------------------------------------
cc = bwconncomp(bw, 8);
celldata = regionprops(cc,'Centroid','BoundingBox','PixelIdxList','Eccentricity','MajorAxisLength','Perimeter','Area');

% Go to next frame if no cells are present
if cc.NumObjects < 1
    continue
end

% If there are cells:

% BLOB circularity
areas = cat(1, celldata.Area);
perimeters = cat(1, celldata.Perimeter);
circ = (4*pi*areas) ./ (perimeters.^2);

for k = 1:cc.NumObjects   % going through detected BLOBs
    % If first cell for this donor:
    if cellNum == 0
        newcell = 1;
    else
        idx = find(tracker(:,3) == i-1); % find foregående frame i tracker      
        % Ingen celle(r) fundet foregående frame:
        if isempty(idx) == 1
            newcell = 1;    
        % Celle(r) fundet i foregående frame:
        else
            dist = celldata(k).Centroid(1) - tracker(idx,1);
            dist(dist < -10) = 100;
            if length(dist) > 1
                [dist, idx1] = min(dist);
                idx = idx(idx1);
            end
            if celldata(k).Centroid(1) <= inlet-5 % before inlet
                dist_thres = 5;
            else % after inlet
                dist_thres = 20;
            end
            % samme celle som tidligere
            if dist <= dist_thres
                newcell = 0;
                % ny celle
            else
                newcell = 1;
            end
        end
    end
    
    switch newcell
        case 1
            cellNum =  cellNum + 1;
            RBCs(cellNum).inlet = inlet;
            RBCs(cellNum).outlet = outlet;
            RBCs(cellNum).yref = yref;
            RBCs(cellNum).label = num2str(cellNum);
            RBCs(cellNum).frame = i;
            RBCs(cellNum).centroid = {celldata(k).Centroid};
            RBCs(cellNum).box = {celldata(k).BoundingBox};
            RBCs(cellNum).pixellist = {celldata(k).PixelIdxList};
            RBCs(cellNum).majoraxis = celldata(k).MajorAxisLength;
            RBCs(cellNum).eccentricity = celldata(k).Eccentricity;
            RBCs(cellNum).circularity = circ(k);
            tracker = [tracker; celldata(k).Centroid(1), cellNum, i];
        case 0
            same_cell = tracker(idx,2);
            RBCs(same_cell).frame = [RBCs(same_cell).frame i];
            RBCs(same_cell).centroid = [RBCs(same_cell).centroid celldata(k).Centroid];
            RBCs(same_cell).box = [RBCs(same_cell).box celldata(k).BoundingBox];
            RBCs(same_cell).pixellist = [RBCs(same_cell).pixellist celldata(k).PixelIdxList];
            RBCs(same_cell).majoraxis = [RBCs(same_cell).majoraxis celldata(k).MajorAxisLength];
            RBCs(same_cell).eccentricity = [RBCs(same_cell).eccentricity celldata(k).Eccentricity];
            RBCs(same_cell).circularity = [RBCs(same_cell).circularity circ(k)];
            tracker = [tracker; celldata(k).Centroid(1), same_cell, i];  
    end
end
end

% -----------------------------------------------
% Discard cells that don't go all the way through
% -----------------------------------------------
remove = [];
for k1 = 1:size(RBCs,2)
    bb = vec2mat(cell2mat(RBCs(k1).box),4);
    if min(bb(:,1)+bb(:,3)) > inlet-1 || max(bb(:,1)+bb(:,3)) < outlet
        remove = [remove k1];
    end
end
RBCs(remove) = [];
RBCs=rmfield(RBCs,'label');
toc
