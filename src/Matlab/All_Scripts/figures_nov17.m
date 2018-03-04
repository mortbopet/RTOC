% diff: 60,110,600,690,830,910
% bin: 155,600,690,910
% morph1-4: 1080,2675

set(0,'DefaultFigureColormap',feval('gray'));
iptsetpref('ImshowInitialMagnification','fit');
% iptsetpref('ImshowInitialMagnification',100);
iptsetpref('ImshowBorder','tight');

i = 1;
im = rgb2gray(im2double(imread(acc{i})));
im1 = diff(:,:,i);          % It seems that this code requires diff to be defined on beforehand!
im2 = (im1-min(im1(:)))/(max(im1(:))-min(im1(:)));
bw1 = imbinarize(im2,0.12);
% bw1 = bw1.*channel_edge(:,:,i);
bw2 = bwareaopen(bw1,30,8); %fjerne noise
se = strel('disk',3);
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

% diff og binær:
% subplot(3,1,1)

[y,x] = size(im);

% bg subtraction
% imagesc(im)
% set(gca,'XTick',[],'YTick',[])
% set(gca,'dataAspectRatio',[1 1 1])
% set(gca,'Position',[0 0 1 1])
% set(gcf, 'Position', [100, 100, x*2, y*2])
% saveas(gcf,'im.png')
% imagesc(bg)
% set(gca,'XTick',[],'YTick',[])
% set(gca,'dataAspectRatio',[1 1 1])
% set(gca,'Position',[0 0 1 1])
% set(gcf, 'Position', [100, 100, x*2, y*2])
% saveas(gcf,'bg.png')
% imagesc(im1)
% set(gca,'XTick',[],'YTick',[])
% set(gca,'dataAspectRatio',[1 1 1])
% set(gca,'Position',[0 0 1 1])
% set(gcf, 'Position', [100, 100, x*2, y*2])
% saveas(gcf,'diff.png')

% binary
% imagesc(im)
% set(gca,'XTick',[],'YTick',[])
% set(gca,'dataAspectRatio',[1 1 1])
% set(gca,'Position',[0 0 1 1])
% set(gcf, 'Position', [100, 100, x*2, y*2])
% saveas(gcf,'bin7.png')
% imagesc(im1)
% set(gca,'XTick',[],'YTick',[])
% set(gca,'dataAspectRatio',[1 1 1])
% set(gca,'Position',[0 0 1 1])
% set(gcf, 'Position', [100, 100, x*2, y*2])
% saveas(gcf,'bin8.png')
% imagesc(bw1)
% set(gca,'XTick',[],'YTick',[])
% set(gca,'dataAspectRatio',[1 1 1])
% set(gca,'Position',[0 0 1 1])
% set(gcf, 'Position', [100, 100, x*2, y*2])
% saveas(gcf,'bin9.png')


% morph (bw1-4):

imagesc(bw1)
set(gca,'XTick',[],'YTick',[])
set(gca,'dataAspectRatio',[1 1 1])
set(gca,'Position',[0 0 1 1])
set(gcf, 'Position', [100, 100, x*2, y*2])
saveas(gcf,'m1.png')
imagesc(bw2), set(gca,'dataAspectRatio',[1 1 1],'XTick',[],'YTick',[])
set(gca,'XTick',[],'YTick',[])
set(gca,'dataAspectRatio',[1 1 1])
set(gca,'Position',[0 0 1 1])
set(gcf, 'Position', [100, 100, x*2, y*2])
saveas(gcf,'m2.png')
imagesc(bw3), set(gca,'dataAspectRatio',[1 1 1],'XTick',[],'YTick',[])
set(gca,'XTick',[],'YTick',[])
set(gca,'dataAspectRatio',[1 1 1])
set(gca,'Position',[0 0 1 1])
set(gcf, 'Position', [100, 100, x*2, y*2])
saveas(gcf,'m3.png')
imagesc(bw4), set(gca,'dataAspectRatio',[1 1 1],'XTick',[],'YTick',[])
set(gca,'XTick',[],'YTick',[])
set(gca,'dataAspectRatio',[1 1 1])
set(gca,'Position',[0 0 1 1])
set(gcf, 'Position', [100, 100, x*2, y*2])
saveas(gcf,'m4.png')



