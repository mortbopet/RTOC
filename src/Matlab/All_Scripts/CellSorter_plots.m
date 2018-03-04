% Script skal køres efter CellSorter
set(0,'DefaultFigureColormap',feval('gray'));
%% se alle
for i = 1:1000
    img = rgb2gray(im2double(imread(acc{i})));
    imagesc(img)
    pause(0.001)
end
close

%%
nr = 3370; %3915

im1 = diff(:,:,nr);
im2 = (im1-min(im1(:)))/(max(im1(:))-min(im1(:)));
bw1 = imbinarize(im2,0.2);
bw1 = bw1.*channel_edge(:,:,nr);
bw2 = bwareaopen(bw1,20,8); %fjerne noise
se = strel('disk',3);
bw3 = imclose(bw2,se);
bw4 = imfill(bw3,'holes');
bw4 = imclearborder(bw4);

% subplot(2,1,1)
% imagesc(rgb2gray(im2double(imread(acc{nr}))))
% subplot(2,1,2)
% imagesc(bw4)

[y,x] = size(bw4);
figure
imagesc(rgb2gray(im2double(imread(acc{nr}))));
set(gca,'XTick',[],'YTick',[])
set(gca,'dataAspectRatio',[1 1 1])
set(gca,'Position',[0 0 1 1])
set(gcf, 'Position', [300, 300, x*2, y*2])
saveas(gcf,'features_Kirstine.png')
saveas(gcf,'circ3.png')
figure
imagesc(bw4)
set(gca,'XTick',[],'YTick',[])
set(gca,'dataAspectRatio',[1 1 1])
set(gca,'Position',[0 0 1 1])
set(gcf, 'Position', [300, 300, x*2, y*2])
saveas(gcf,'circ4.png')

cc = bwconncomp(bw4);
stats = regionprops(cc,'Area','Perimeter');
circularity = (4*pi*stats.Area)/stats.Perimeter^2

%% Binary with just cells (plot)
% bw = zeros(size(img,1),size(img,2),5000);

i = 4918;
im = rgb2gray(im2double(imread(acc{i+10000})));
im1 = diff(:,:,i);
im2 = (im1-min(im1(:)))/(max(im1(:))-min(im1(:)));
bw1 = imbinarize(im2,0.11);
bw1 = bw1.*channel_edge(:,:,i);
bw2 = bwareaopen(bw1,20,8); %fjerne noise
se = strel('disk',4);
bw3 = imclose(bw2,se);
bw4 = imfill(bw3,'holes');
bw5 = bwpropfilt(bw4,'ConvexArea',[200 1450]);
bw6 = bwpropfilt(bw5,'MajorAxisLength',[0 65]);
bw7 = bw6;
cc = bwconncomp(bw6);
stats = regionprops(cc,'Centroid','Solidity');
centroids = cat(1, stats.Centroid);
sols = cat(1, stats.Solidity);
for j = 1:size(stats,1)
    if centroids(j,1)<inlet-20 || centroids(j,1)>inlet+20
        if sols(j) < 0.87
            bw7(cc.PixelIdxList{j}) = 0;
        end
    end
end
bw8 = imclearborder(bw7);
%     bw(:,:,i) = bw8;


bn = [im im2 bw1 bw2 ;...
    bw3 bw4 bw5 bw6 ;...
    bw7 bw8 bw8 bw8] ;

% bn = [im im2 ; ...
%   bw1 bw2 ;...
%   bw3  bw8] ;

imshow(bn),set(gca,'XTick',[],'YTick',[],'Position',[0 0 1 1]);
hold on
line([size(diff,2)*2 size(diff,2)*2],[0 size(diff,1)*3],'LineWidth',1)
line([size(diff,2)*3 size(diff,2)*3],[0 size(diff,1)*3],'LineWidth',1)
line([size(diff,2) size(diff,2)],[0 size(diff,1)*3],'LineWidth',1)
line([0 size(diff,2)*4],[size(diff,1) size(diff,1)],'LineWidth',1)
line([0 size(diff,2)*4],[size(diff,1)*2 size(diff,1)*2],'LineWidth',1)

% saveas(gcf,'boat2.png')

%% look at frames

for i = 1:size(RBCs,2)
    str = sprintf('Cell no %d',i)
    frames = RBCs(i).frame;
    centroids = RBCs(i).centroid;
    for j = 1:length(frames)
        c = centroids{j}; x = c(1); y = c(2);
        subplot('position',[0 .5 1 .5])
        img = rgb2gray(im2double(imread(acc{frames(j)})));
        imagesc(img), hold on
        scatter(x,y,20,'filled','r')
        bound = bwboundaries(bw(:,:,frames(j)));
        for k = 1:length(bound)
            boundary = bound{k};
            plot(boundary(:,2), boundary(:,1), 'w--', 'LineWidth', 1)
        end
        subplot('Position',[0 0 1 .5])
        imagesc(bw(:,:,frames(j))), hold on
        scatter(x,y,20,'filled','r'), hold off
        pause(0.005)
    end
    close
    %     pause(0.2)
end
close

%% one cell
set(0,'DefaultFigureColormap',feval('gray'));

nr = 1;
frames = RBCs(nr).frame;
centroids = RBCs(nr).centroid;

for j = 1:length(frames)
    c = centroids{j}; x = c(1); y = c(2);
    subplot('position',[0 .5 1 .5])
    img = rgb2gray(im2double(imread(acc{frames(j)})));
    imagesc(img)
    set(gca,'XTick',[],'YTick',[])
    hold on
    scatter(x,y,20,'filled','r')
    bound = bwboundaries(bw(:,:,frames(j)));
    for k = 1:length(bound)
        boundary = bound{k};
        plot(boundary(:,2), boundary(:,1), 'w--', 'LineWidth', 1)
    end
    subplot('Position',[0 0 1 .5])
    imagesc(bw(:,:,frames(j)))
    set(gca,'XTick',[],'YTick',[])
    hold on
    scatter(x,y,20,'filled','r')
    
    hold off
    pause
end
close







