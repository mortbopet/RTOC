%% Prepare discarded and accepted frames file names
% clc

% tic
% path1 = 'F:\Lise Aagesen master thesis\RBCdata\AdultDonors\AllDonorsDay1\AllDonorsDay1\081116D1';
% path2 = 'F:\Lise Aagesen master thesis\RBCdata\AdultDonors\AllDonorsDay1\AllDonorsDay1\081116D1\ImgD1';
% addpath(path1,path2);
% toc

pics = fopen('ImgD1_Accepted.txt');
acc1 = textscan(pics,'%s');
acc = acc1{1,1};

pics = fopen('ImgD1_Discarded.txt');
dis1 = textscan(pics,'%s');
dis = dis1{1,1};

frameno_acc = str2doubles(extractBetween(acc,"_","."));
frameno_dis = str2doubles(extractBetween(dis,"_","."));

if size(frameno_acc,1) + size(frameno_dis,1) < 79997
    disp('error')
    return
end

img = rgb2gray(im2double(imread(dis{616})));
% imagesc(img)

inlet = 80;
outlet = 210;
yref = 49;
edge_thres = 0.272;
se_edge = strel('rectangle',[1 30]);
bin_thres = 0.11;

antal = 1000;

%% RBC STORAGE
RBCs = struct('label','','frame',[],'centroid',{{}},'box',{{}});
 
%% PARAMETERS AND INITIALIZATION
cellNum = 0;              % cell counter
tracker = [];             % track cell center 

%% Subtract background from each accepted frame
% diff = zeros(size(img,1),size(img,2),antal);
% channel_edge = zeros(size(img,1),size(img,2),antal);
% count = 1;
% tic
for i = 1:antal  
    
    frame = frameno_acc(i);
    
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
    bg_edge = bg < edge_thres;
    % se from top
    bg_edge = imclose(bg_edge,se_edge);
    bg_edge(:,inlet-10:inlet+10) = 0;
    bg_edge = bwareaopen(bg_edge,100);
    bg_edge = imcomplement(bg_edge);
%     channel_edge(:,:,count) = bg_edge;
%     count = count + 1;

% Binary with just cells
% bw = zeros(size(img,1),size(img,2),antal);
% tic

    im1 = diff;
    im2 = (im1-min(im1(:)))/(max(im1(:))-min(im1(:)));    
    bw1 = imbinarize(im2,bin_thres);
    bw1 = bw1.*bg_edge;
    bw2 = bwareaopen(bw1,20); %fjerne noise
    se = strel('disk',4);
    bw3 = imclose(bw2,se);
    bw4 = imfill(bw3,'holes');
    bw5 = bwpropfilt(bw4,'ConvexArea',[200 1450]);
    bw6 = bwpropfilt(bw5,'MajorAxisLength',[0 65]);
    bw7 = bw6;
    ccc = bwconncomp(bw6);
    stats = regionprops(ccc,'Centroid','Solidity');
    centroids = cat(1, stats.Centroid);
    sols = cat(1, stats.Solidity);
    for j = 1:size(stats,1)
        if centroids(j,1)<inlet-20 || centroids(j,1)>inlet+20
            if sols(j) < 0.87
                bw7(ccc.PixelIdxList{j}) = 0;
            end
        end
    end
    bw = imclearborder(bw7);


% Cell identification/separation
% tic
% Going through all accepted frames

    % Find connected components (cells)
    cc = bwconncomp(bw, 8);
    celldata = regionprops(cc,'Centroid','BoundingBox','PixelIdxList');

    % Go to next frame if no cells are present
    if cc.NumObjects < 1
        continue
    end
    
    % FIRST CELL(s) ----------------------------------------------
    if cellNum == 0
        for i1 = 1:cc.NumObjects % each detected BLOB in this frame
                cellNum =  cellNum + 1;
                RBCs(cellNum).label = num2str(cellNum);
                RBCs(cellNum).frame = i;
                RBCs(cellNum).centroid = {celldata(i1).Centroid};
                RBCs(cellNum).box = {celldata(i1).BoundingBox};
                RBCs(cellNum).pixellist = {celldata(i1).PixelIdxList};
                tracker = [tracker; celldata(i1).Centroid(1), cellNum, frame];
        end
        continue % next frame
    end
    
    for k1 = 1:cc.NumObjects        
        idx = find(tracker(:,3) == frame-1); % foregående frame i tracker
        
        % Ingen i foregående frame:
        if isempty(idx) == 1
            newcell = 1;% 

            
        % Celle(r) fundet i foregående frame:    
        else
            dist = celldata(k1).Centroid(1) - tracker(idx,1);
            dist(dist < -10) = 100;
            if length(dist) > 1
                [dist, idx1] = min(dist);
                idx = idx(idx1);
            end
            
            if celldata(k1).Centroid(1) <= inlet - 5 % before inlet
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
        switch newcell
            case 1
                    cellNum =  cellNum + 1;
                    RBCs(cellNum).label = num2str(cellNum);
                    RBCs(cellNum).frame = i;
                    RBCs(cellNum).centroid = {celldata(k1).Centroid};
                    RBCs(cellNum).box = {celldata(k1).BoundingBox};
                    RBCs(cellNum).pixellist = {celldata(k1).PixelIdxList};
                    tracker = [tracker; celldata(k1).Centroid(1), cellNum, frame];
            case 0
                    same_cell = tracker(idx,2);
                    RBCs(same_cell).frame = [RBCs(same_cell).frame i];
                    RBCs(same_cell).centroid = [RBCs(same_cell).centroid celldata(k1).Centroid];
                    RBCs(same_cell).box = [RBCs(same_cell).box celldata(k1).BoundingBox];
                    RBCs(same_cell).pixellist = [RBCs(same_cell).pixellist celldata(k1).PixelIdxList];
                    tracker = [tracker; celldata(k1).Centroid(1), same_cell, frame];
        end
    end 
end

% toc

%% fjerne celler der ikke kommer hele vejen igennem (works)
remove = [];
for k2 = 1:size(RBCs,2)
    bb = vec2mat(cell2mat(RBCs(k2).box),4);
    if min(bb(:,1)+bb(:,3)) > inlet-1 || max(bb(:,1)+bb(:,3)) < outlet
        remove = [remove k2];
    end
end

RBCs(remove) = [];

RBCs=rmfield(RBCs,'label');

