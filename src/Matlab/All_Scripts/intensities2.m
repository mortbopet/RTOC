% Script skal køres efter CellSorter
set(0,'DefaultFigureColormap',feval('gray'));

nr = 1;
f1 = 50;
f = RBCs(nr).frame(f1);
im = rgb2gray(im2double(imread(acc{f})));

c = cell2mat(RBCs(nr).centroid(f1));
% c = round(c);

im2 = im( round(c(2)-5.5):round(c(2)+5.5) , round(c(1)-15.5):round(c(1)+15.5) );

im2a = im2(1:6,:);
im2b = flipud(im2(7:12,:));

d = imabsdiff(im2a,im2b);
s = sum(d(:))

% subplot('position',[0 .5 1 .5])
% imagesc(im2a)
% subplot('Position',[0 0 1 .5])
% imagesc(im2b)

subplot('position',[0 .5 1 .5])
imagesc(im)
hold on
% r = rectangle('Position',[round(c(1)-16) round(c(2)-5) 32 12]);
% r.FaceColor = [1 1 1 .2];
% r.EdgeColor = [1 0 0 0.5];
% r.LineWidth = .8;
% r.LineStyle = '--';
s = scatter(c(1),c(2),8,'filled');
s.MarkerFaceColor = [1 0 0];
s.MarkerEdgeColor = [.5 0 0];
set(gca,'XTick',[],'YTick',[])
subplot('Position',[0 0 1 .5])
imagesc(im2)
hold on
x = 1:size(im2,2);
y = ones(1,length(x))*5.5;
plot(x,y,'r--','LineWidth',1)  
set(gca,'XTick',[],'YTick',[])

% saveas(gcf,'symm1.png')



