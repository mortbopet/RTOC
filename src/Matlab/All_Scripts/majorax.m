% kør efter hej2
set(0,'DefaultFigureColormap',feval('gray'));

hej = zeros(size(img));
idx = RBCs(3).pixellist{20};
hej(idx) = 1;

cc = bwconncomp(hej, 8);
stats = regionprops(cc,'PixelList');
list = stats.PixelList;
distances = zeros(length(list),1);


for h = 1:length(list)
    xp = list(h,1);
    yp = list(h,2);
    
    distances(h) = max(sqrt((list(:,1)-xp).^2 + (list(:,2)-yp).^2));
end

d = max(distances)
%%
cell = 15;
f = 25;
set(0,'DefaultFigureColormap',feval('gray'));

hej = zeros(size(img));
idx = RBCs(cell).pixellist{f};
hej(idx) = 1;
idx1 = RBCs(cell).frame(f);
im = rgb2gray(im2double(imread(acc{idx1})));

cc = bwconncomp(hej, 8);

stats = regionprops(cc,'Centroid','Orientation','MajorAxisLength')

deltax = stats.MajorAxisLength * cosd(stats.Orientation);
deltay = stats.MajorAxisLength * sind(-stats.Orientation);

xvals = [stats.Centroid(1)-deltax/2 stats.Centroid(1)+deltax/2];
yvals = [stats.Centroid(2)-deltay/2 stats.Centroid(2)+deltay/2];

imagesc(im), hold on
set(gca,'XTick',[],'YTick',[])
set(gca,'dataAspectRatio',[1 1 1])
set(gca,'Position',[0 0 1 1])
set(gcf, 'Position', [300, 300, size(hej,2)*2, size(hej,1)*2])
line(xvals,yvals,'Color',[0 .7 .8 0.85],'LineStyle','-','LineWidth', 1)
bound = bwboundaries(hej);
for k = 1:length(bound)
    boundary = bound{k};
    plot(boundary(:,2), boundary(:,1), 'r--', 'LineWidth', 1)
end
saveas(gcf,'dia10.png')
