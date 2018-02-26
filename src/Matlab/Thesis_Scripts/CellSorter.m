% ------------------------------------------------------------------------- % This script is created by Lise Aagesen for her M.Sc. thesis in the fall
% of 2017. It is intended for registration and feature extraction of RBC
% recordings for a subsequent RBC classification (fetal vs adult).
% -------------------------------------------------------------------------
%% Prepare l i s t s of discarded and accepted frames
donor = 'D1'; mkdir 'D1'
% image location :
path1a = 'F:\Lise Aagesen master thesis\RBCdata\AdultDonors'; path1b = '\AllDonorsDay1\AllDonorsDay1\081116D1';
path1 = strcat (path1a , path1b );
path2 = strcat(path1,'\Img',donor);
path3 = strcat(path1, '\RBCs' ); % for storing results
path4 = strcat(donor, '\RBCs' ); % for storing results
addpath(path1,path2);
file1 = strcat('Img',donor,'_Accepted.txt');
pics = fopen(file1);
acc1 = textscan(pics,'%s');
acc = acc1{1,1};
file2 = strcat('Img',donor,'_Discarded.txt');
pics = fopen(file2);
dis1 = textscan(pics,'%s');
dis = dis1 {1 ,1};

% list of pure frame numbers
frameno_acc = str2doubles(extractBetween(acc,"_","."));
frameno_dis = str2doubles(extractBetween(dis,"_","."));

%% Manual inputs
inlet = 80;
outlet = 210;
yref = 63;
edge_thres = 0.272 ;
se_edge = strel('rectangle',[1 30]);%SE for extracting channel edge
cellNum = 0; % used for cell registration

tracker = []; % used for cell registration
% struct for storing features of RBCs
RBCs = struct('inlet',[],'outlet',[],'yref',[],'label','', ...
            'frame',[],'centroid',{{}},'box',{{}},'pixellist',{{}}, ...
            'circularity',[],'symmetry',[],'gradient',[],'eccentricity', ...
            [],'majoraxis',[]);

%% Cell registration
for i = 1:size(acc,1) % loop through accepted frames
    % ---------------------------------------------------------------------
    % subtract background from intensity image
    % ---------------------------------------------------------------------
    % select background
    % if first frame, only one background
    if frameno_acc(i) < min(~frameno_dis) || frameno_acc(i) > ...
            max( frameno_dis )
        [~,index] = min(abs(frameno_dis-frameno_acc(i)));
        bg = rgb2gray(im2double(imread(dis{index})));
    % else an average of the two closest
    else
        frameno_dis_before = frameno_dis(frameno_dis<frameno_acc(i));
        dis_before = dis (frameno_dis<frameno_acc( i ));
        frameno_dis_after = frameno_dis(frameno_dis>frameno_acc(i));
        dis_after = dis (frameno_dis>frameno_acc( i ));
        [~,index1 ] = min(abs(frameno_dis_before -frameno_acc( i )));
        [~,index2] = min(abs(frameno_dis_after-frameno_acc(i)));
        bg1 = rgb2gray(im2double(imread(dis_before{index1})));
        bg2 = rgb2gray(im2double(imread(dis_after{index2})));
        bg = (bg1+bg2)/2;
    end

    % subtract background and edge
    im = rgb2gray(im2double(imread(acc{i })));
    diff = imabsdiff(im,bg);
    bg_edge = bg < edge_thres;
    bg_edge = imclose(bg_edge,se_edge);
    bg_edge(:, inlet-10: inlet+10) = 0;
    bg_edge = bwareaopen(bg_edge,100);
    bg_edge = imcomplement(bg_edge);
    diff = diff .* bg_edge;
    % normalize
    norm_diff = (diff - min(diff(:))) / (max(diff(:)) - min(diff(:)));
    % ---------------------------------------------------------------------
    % cell segmentation through binarization and morphological operations
    % ---------------------------------------------------------------------
    bw = imbinarize(norm_diff,0.12);

    bw = bwareaopen(bw,20);
    se = strel('disk',4);
    bw = imclose(bw,se);
    bw = imfill(bw,'holes');
    bw = bwpropfilt(bw, 'ConvexArea' ,[200 1450]);
    bw = bwpropfilt(bw, 'MajorAxisLength ' ,[0 65]);
    bw1 = bw;
    cc = bwconncomp(bw1);
    stats = regionprops(cc,'Centroid','Solidity');
    centroids = cat(1, stats.Centroid );
    sols = cat(1, stats.Solidity );
    for j = 1:size(stats ,1)
        if centroids(j,1)<inlet -20 || centroids(j,1)>inlet+20
            if sols(j) < 0.87
                bw1(cc.PixelIdxList{j}) = 0;
            end
        end
    end
    bw = imclearborder(bw1);
    % ---------------------------------------------------------------------
    % Cell identification and sorting through connected component analysis
    % ---------------------------------------------------------------------
    cc = bwconncomp(bw, 8);
    celldata = regionprops(cc,'Centroid','BoundingBox','PixelIdxList',...
                           'Eccentricity','MajorAxisLength','Perimeter','Area');
    % Go to next frame if no cells are present
    if cc.NumObjects < 1
        continue
    end
    
    % If there are cells :
    % BLOB circularity
    areas = cat(1, celldata.Area);
    perimeters = cat(1, celldata.Perimeter);
    circ = (4*pi * areas) ./ (perimeters.^2);

    for k = 1:cc.NumObjects % loop over detected BLOBs
        if round(celldata(k).Centroid(2) - 5.5) > 0 && ...
              round(celldata(k).Centroid(2) + 5.5) < size(im,1) && ...
              round(celldata(k).Centroid(1) - 22.5 ) > 0 && ...
              round(celldata(k).Centroid(1) + 22.5) < size(im,2)

            % gradient score
            im2 = im(round(celldata(k).Centroid(2)-5.5): ...
                    round(celldata(k).Centroid(2)+5.5), ...
                    round(celldata(k).Centroid(1)-22.5): ...
                    round(celldata(k).Centroid(1)+22.5 ));
            ime = edge(im2,'Canny');
            ime = ime(6:7 ,:);
            s1 = sum(ime(:));
            % symmetry about vertical axis
            im3 = im(round(celldata(k).Centroid(2)-4.5): ...
                    round(celldata(k).Centroid(2)+4.5), ...
                    round(celldata(k).Centroid(1)-22.5): ...
                    round(celldata(k).Centroid(1)+22.5 ));
            im3 = (im3-min(im3(:))) / (max(im3(:)) -min(im3(:)));
            d1 = imabsdiff(im3, fliplr (im3));
            s2 = sum(d1(:)) / celldata(k).MajorAxisLength;
        else
            s1 = 0;
            s2 = 0;
        end

        if cellNum == 0
            newcell = 1;
        % if not first cell:
        else
            % find frame number of previous detected cell
            idx = find(tracker(:,3) == i - 1);
            % if no cell is found in the previous frame this is a new cell
            if isempty(idx) == 1
                newcell = 1;

                % if not a new cell:
            else
                % dist from this cell to one(s) found in previous frame
                dist = celldata(k).Centroid(1) - tracker(idx,1);
                dist ( dist < -10) = 100;

                % if more than one cell in previous frame, find nearest
                if length(dist) > 1
                    [dist, idx1]=min(dist);
                    idx = idx(idx1 );
                end

                % define distance threshold :
                if celldata(k).Centroid(1) <= inlet -5
                    % before inlet
                    dist_thres = 5;
                else
                    % after inlet
                    dist_thres = 20;
                end

                % determine if same cell or new cell based on dist thres
                if dist <= dist_thres
                    % same cell
                    newcell = 0;
                else
                    newcell = 1; % new cell end
                end
            end
        end

        % feature registration (cases : same or new cell )
        switch newcell
            case 1
                cellNum = cellNum + 1;
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
                RBCs(cellNum).symmetry = s1;
                RBCs(cellNum).gradient = s2;
                tracker = [tracker; celldata(k).Centroid(1), cellNum, i ];
            case 0
                same_cell = tracker(idx ,2);
                RBCs(same_cell).frame = [RBCs(same_cell).frame i];
                RBCs(same_cell).centroid = [RBCs(same_cell).centroid ...
                    celldata(k).Centroid];
                RBCs(same_cell).box = [RBCs(same_cell).box ...
                    celldata(k).BoundingBox];
                RBCs(same_cell).pixellist = [RBCs(same_cell).pixellist ...
                    celldata(k).PixelIdxList];
                RBCs(same_cell).majoraxis = [RBCs(same_cell).majoraxis ...
                    celldata(k).MajorAxisLength];
                RBCs(same_cell).eccentricity = [RBCs(same_cell).eccentricity ...
                    celldata(k).Eccentricity];
                RBCs(same_cell).circularity = [RBCs(same_cell).circularity circ(k)];
                RBCs(same_cell).symmetry = [RBCs(same_cell).symmetry s1];
                RBCs(same_cell).gradient = [RBCs(same_cell).gradient s2];
                tracker = [tracker; celldata(k).Centroid(1), same_cell, i] ;
        end
    end
end

% -------------------------------------------------------------------------
% Discard cells that don't go all the way through
% -------------------------------------------------------------------------

remove = [];
for k1 = 1:size(RBCs,2)
    bb = vec2mat(cell2mat(RBCs(k1).box),4);
    if min(bb(:,1)+bb(: ,3)) > inlet - 1 || max(bb(:,1) + bb(: ,3)) < outlet
        remove = [ remove k1 ] ;
    end
end
RBCs(remove) = [];
RBCs = rmfield(RBCs,'label');
% store the RBCs struct
save(path3,'RBCs')
save(path4,'RBCs')
