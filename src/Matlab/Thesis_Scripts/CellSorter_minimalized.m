% -------------------------------------------------------------------------
% This script is created by Lise Aagesen for her M.Sc. thesis in the fall
% of 2017. It is intended for registration and feature extraction of RBC
% recordings for a subsequent RBC classification (fetal vs adult).
% -------------------------------------------------------------------------

%% Prepare lists of discarded and accepted frames
% image location :
path1 = '/Users/jl/Dropbox/DTU/04_Semester4/fagprojekt/data';
path2 = strcat(path1,'/ImgD1');
addpath(path1,path2);
file1 = strcat(path1,'/st_accepted.txt');
pics = fopen(file1);
acc1 = textscan(pics,'%s');
acc = acc1{1,1};
file2 = strcat(path1,'/st_discarded.txt');
pics = fopen(file2);
dis1 = textscan(pics,'%s');
dis = dis1 {1 ,1};

% list of pure frame numbers
frameno_acc = str2double(extractBetween(acc,"1_","."));
frameno_dis = str2double(extractBetween(dis,"1_","."));

%% Manual inputs
inlet = 80;
outlet = 210;
yref = 63;
edge_thres = 0.272 ;
se_edge = strel('rectangle',[1 30]);%SE for extracting channel edge
cellNum = 0; % used for cell registration

%% Cell registration
for i = 1:150 % size(acc,1) % loop through accepted frames
    bg = rgb2gray(im2double(imread(dis{10})));

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
    bw = bwpropfilt(bw, 'MajorAxisLength' ,[0 65]);
    % imshow(bw);
    bw1 = bw;
    cc = bwconncomp(bw1);
    stats = regionprops(cc,'Centroid','Solidity');
    % disp(cat(1, stats.Centroid));

      % Extract centroid and Solidity for all objects in cc
    centroids = cat(1, stats.Centroid );
    sols = cat(1, stats.Solidity );
    for j = 1:size(stats ,1)
        if centroids(j,1)<inlet -20 || centroids(j,1)>outlet+20
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
    celldata.Eccentricity
    figure(1);
    imshow(bw);
                       
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
           round(celldata(k).Centroid(1) - 22.5) > 0 && ...
           round(celldata(k).Centroid(1) + 22.5) < size(im,2)

            % gradient score
            im2 = im(round(celldata(k).Centroid(2) - 5.5): ...
                     round(celldata(k).Centroid(2) + 5.5), ...
                     round(celldata(k).Centroid(1) - 22.5): ...
                     round(celldata(k).Centroid(1) + 22.5));
            [ime, thres] = edge(im2,'Canny');
            ime = ime(6:7 ,:);
            s1 = sum(ime(:));
            im2 = bw(round(celldata(k).Centroid(2) - 5.5): ...
                     round(celldata(k).Centroid(2) + 5.5), ...
                     round(celldata(k).Centroid(1) - 22.5): ...
                     round(celldata(k).Centroid(1) + 22.5));
            [ime, thres] = edge(im2,'Canny');
            ime = ime(6:7 ,:);
            s2 = sum(ime(:));
            % sprintf("%d %d",s1,s2);
        end
    end

end