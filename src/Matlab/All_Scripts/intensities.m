% Script skal køres efter CellSorter
set(0,'DefaultFigureColormap',feval('gray'));

nr = 16;
f1 = 40;
f = RBCs(nr).frame(f1);
im = rgb2gray(im2double(imread(acc{f})));

c = cell2mat(RBCs(nr).centroid(f1));
c = round(c);

[y,x] = size(im);

a1 = (c(2)-5:(c(2)-2));
a2 = ones(length(a1))*c(1);
a3 = (c(2)+2:(c(2)+5));

b1 = (c(1)-14:(c(1)-2));
b2 = ones(length(b1))*c(2);
b3 = (c(1)+2:(c(1)+14));

v1 = im((c(2)-5):(c(2)-1),c(1));
v2 = flipud(im((c(2)+1):(c(2)+5),c(1)));
v3 = im(c(2),(c(1)-14):(c(1)-1));
v4 = fliplr(im(c(2),(c(1)+1):(c(1)+14)));

s1 = abs( v1-v2 );
% s2 = abs( v3-v4 );

s = sum(s1)

imagesc(im)

hold on
plot(a2,a1,'r.','LineWidth',.5)
plot(a2,a3,'r.','LineWidth',.5)
% plot(b1,b2,'r.','LineWidth',.5)
% plot(b3,b2,'r.','LineWidth',.5)
scatter(c(1),c(2),17,'MarkerEdgeColor',[.7 0 0],...
    'MarkerFaceColor',[1 0 0],'LineWidth',1.5)
      
set(gca,'XTick',[],'YTick',[])
set(gca,'dataAspectRatio',[1 1 1])
set(gca,'Position',[0 0 1 1])
set(gcf, 'Position', [100, 100, x*4, y*4])
% saveas(gcf,'symH16.png')



