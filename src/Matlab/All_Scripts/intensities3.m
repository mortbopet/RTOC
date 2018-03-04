% Script skal køres efter CellSorter
set(0,'DefaultFigureColormap',feval('gray'));

nr = 11;
f1 = 25;
f = RBCs(nr).frame(f1);
im = rgb2gray(im2double(imread(acc{f})));

c = cell2mat(RBCs(nr).centroid(f1));
% c = round(c);

im2 = im( round(c(2)-5.5):round(c(2)+5.5) , round(c(1)-22.5):round(c(1)+22.5) );
ime = edge(im2,'Canny');
ime2 = ime(6:7,:);

s = sum(ime2(:))

figure
imagesc(im)
% hold on
% r = rectangle('Position',[round(c(1)-22.5) round(c(2)-5.5) 46 12]);
% r.FaceColor = [1 1 1 .2];
% r.EdgeColor = [1 0 0 0.5];
% r.LineWidth = .8;
% r.LineStyle = '--';
% s = scatter(c(1),c(2),8,'filled');
% s.MarkerFaceColor = [1 0 0];
% s.MarkerEdgeColor = [.5 0 0];
% set(gca,'XTick',[],'YTick',[])
% set(gca,'dataAspectRatio',[1 1 1])
% set(gca,'Position',[0 0 1 1])
% set(gcf, 'Position', [300, 300, size(im,2)*2, size(im,1)*2])
% % saveas(gcf,'edge4.png')
% 
% figure
% imagesc(im2)
% % hold on
% % r = rectangle('Position',[1 6 45 2]);
% % r.FaceColor = [1 1 1 .3];
% % r.EdgeColor = [.2 .7 1 0.5];
% % r.LineWidth = .8;
% % r.LineStyle = '-';
% set(gca,'XTick',[],'YTick',[])
% set(gca,'dataAspectRatio',[1 1 1])
% set(gca,'Position',[0 0 1 1])
% set(gcf, 'Position', [600, 100, size(im2,2)*7, size(im2,1)*7])
% % saveas(gcf,'edeg5.png')
% 
% figure
% imagesc(ime)
% hold on
% r = rectangle('Position',[1 5.5 45 2]);
% r.FaceColor = [1 1 1 .3];
% r.EdgeColor = [.2 .7 1 0.5];
% r.LineWidth = .8;
% r.LineStyle = '-';
% set(gca,'XTick',[],'YTick',[])
% set(gca,'dataAspectRatio',[1 1 1])
% set(gca,'Position',[0 0 1 1])
% set(gcf, 'Position', [300, 100, size(ime,2)*7, size(ime,1)*7])
% % saveas(gcf,'edge6.png')

figure
im3 = im( round(c(2)-11.5):round(c(2)+11.5) , round(c(1)-26.5):round(c(1)+26.5) );
imagesc(im3)
set(gca,'XTick',[],'YTick',[])
set(gca,'dataAspectRatio',[1 1 1])
set(gca,'Position',[0 0 1 1])
set(gcf, 'Position', [300, 100, size(im3,2)*7, size(im3,1)*7])
% saveas(gcf,'canny20.png')

