% Script skal køres efter CellSorter
set(0,'DefaultFigureColormap',feval('gray'));

nr = 6;
f1 = 38;
f = RBCs(nr).frame(f1);
im = rgb2gray(im2double(imread(acc{f})));
dia = RBCs(nr).majoraxis(f1);

c = cell2mat(RBCs(nr).centroid(f1));
% c = round(c);

im2 = im( round(c(2)-4.5):round(c(2)+4.5) , round(c(1)-22.5):round(c(1)+22.5) );
im2 = (im2-min(im2(:)))/(max(im2(:))-min(im2(:)));

d = imabsdiff(im2,fliplr(im2));
s = sum(d(:))/dia

figure(1)
imagesc(im)
hold on
r = rectangle('Position',[round(c(1)-22) round(c(2)-5) 46 10]);
r.FaceColor = [1 1 1 .2];
r.EdgeColor = [1 0 0 0.5];
r.LineWidth = .8;
r.LineStyle = '--';
s = scatter(c(1),c(2),8,'filled');
s.MarkerFaceColor = [1 0 0];
s.MarkerEdgeColor = [.5 0 0];
set(gca,'XTick',[],'YTick',[])
set(gca,'dataAspectRatio',[1 1 1])
set(gca,'Position',[0 0 1 1])
set(gcf, 'Position', [300, 300, size(im,2)*2, size(im,1)*2])
% saveas(gcf,'v5.png')
% 
figure(2)
imagesc(im2)
hold on
y = 1:size(im2,1);
x = ones(1,length(y))*23.5;
plot(x,y,'r--','LineWidth',1)  
set(gca,'XTick',[],'YTick',[])
set(gca,'dataAspectRatio',[1 1 1])
set(gca,'Position',[0 0 1 1])
set(gcf, 'Position', [600, 100, size(im2,2)*7, size(im2,1)*7])
% saveas(gcf,'v6.png')

% figure(3)
% im3 = im( round(c(2)-11.5):round(c(2)+11.5) , round(c(1)-26.5):round(c(1)+26.5) );
% imagesc(im3)
% set(gca,'XTick',[],'YTick',[])
% set(gca,'dataAspectRatio',[1 1 1])
% set(gca,'Position',[0 0 1 1])
% set(gcf, 'Position', [500, 100, size(im3,2)*7, size(im3,1)*7])
% saveas(gcf,'vertikal20.png')


