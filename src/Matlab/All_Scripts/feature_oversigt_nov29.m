donor = 'D62';
path1 = 'F:\Lise Aagesen master thesis\RBCdata\AdultDonors\AllDonorsDay7\AllDonorsDay7\291116D62';
path2 = strcat(path1,'\Img',donor);
addpath(path1,path2);

file1 = strcat('Img',donor,'_Accepted.txt');
pics = fopen(file1);
acc1 = textscan(pics,'%s');
acc = acc1{1,1};

file2 = strcat('Img',donor,'_Discarded.txt');
pics = fopen(file2);
dis1 = textscan(pics,'%s');
dis = dis1{1,1};

frameno_acc = str2doubles(extractBetween(acc,"_","."));
frameno_dis = str2doubles(extractBetween(dis,"_","."));

img = rgb2gray(im2double(imread(acc{8112})));

%%
set(0,'DefaultFigureColormap',feval('gray'));

F = 'RBCs.mat'; % name of the file
D = '.'; % absolute or relative path of base directory
S = dir(fullfile(D,'D*'));
X = [S.isdir] & ~ismember({S.name},{'.','..'});
N = {S(X).name};
donors_adult = [];
FV = [];
donor = 1;

T = fullfile(D,N{63},F);
load(T)

black = zeros(size(img));

i = 31; %celle nr 1,26,27,29,34 % 9,31,51

inlet = RBCs(i).inlet(1);
outlet = RBCs(i).outlet(1);
yref = RBCs(i).yref(1);

bb = vec2mat(cell2mat(RBCs(i).box),4);
cc = vec2mat(cell2mat(RBCs(i).centroid),2);

[~ , idx1] = min(abs(bb(:,1)+bb(:,3)-inlet));
pix1 = cell2mat(RBCs(i).pixellist(idx1));
a1 = black;
a1(pix1) = 1;
bound1 = bwboundaries(a1);
f1 = RBCs(i).frame(idx1);
img1 = rgb2gray(im2double(imread(acc{f1})));
vals1 = img1(pix1);
% b1 = bb(idx1,:);

[~ , idx2] = min(abs(cc(:,1)-(inlet+65)));
pix2 = cell2mat(RBCs(i).pixellist(idx2));
a2 = black;
a2(pix2) = 1;
bound2 = bwboundaries(a2);
f2 = RBCs(i).frame(idx2);
img2 = rgb2gray(im2double(imread(acc{f2})));
vals2 = img2(pix2);
% b2 = bb(idx2,:);

[~ , idx3] = min(abs(bb(:,1)+bb(:,3)-outlet));
pix3 = cell2mat(RBCs(i).pixellist(idx3));
a3 = black;
a3(pix3) = 1;
bound3 = bwboundaries(a3);
f3 = RBCs(i).frame(idx3);
img3 = rgb2gray(im2double(imread(acc{f3})));
vals3 = img3(pix3);
% b3 = bb(idx3,:);

[~ , idx4] = min(abs(bb(:,1)-(inlet+1)));
pix4 = cell2mat(RBCs(i).pixellist(idx4));
a4 = black;
a4(pix4) = 1;
bound4 = bwboundaries(a4);
f4 = RBCs(i).frame(idx4);
img4 = rgb2gray(im2double(imread(acc{f4})));
vals4 = img4(pix4);
% b4 = bb(idx4,:);

[~,index] = min(abs(frameno_dis-frameno_acc(f1)));
bg = rgb2gray(im2double(imread(dis{index})));

pic = bg;
pic(pix1) = vals1;
pic(pix2) = vals2;
pic(pix3) = vals3;
pic(pix4) = vals4;

[y,x] = size(pic);

% set(0,'DefaultFigureColormap',feval('gray'));
imagesc(pic), hold on
a = 1:y;
b = ones(1,length(a))*inlet;
c = ones(1,length(a))*outlet;
d = ones(1,length(a))*(inlet+65);
plot(b,a,'--','Color',[0,.6,.7],'LineWidth',2)
plot(c,a,'--','Color',[0,.6,.7],'LineWidth',2)
plot(d,a,'--','Color',[0,.6,.7],'LineWidth',2)

for k = 1:length(bound1)
    boundary = bound1{k};
    plot(boundary(:,2), boundary(:,1), 'r--', 'LineWidth', 2)
end
for k = 1:length(bound2)
    boundary = bound2{k};
    plot(boundary(:,2), boundary(:,1), 'r--', 'LineWidth', 2)
end
for k = 1:length(bound4)
    boundary = bound4{k};
    plot(boundary(:,2), boundary(:,1), 'r--', 'LineWidth', 2)
end
for k = 1:length(bound3)
    boundary = bound3{k};
    plot(boundary(:,2), boundary(:,1), 'r--', 'LineWidth', 2)
end

% rectangle('Position', [ceil(b1(1)), ceil(b1(2)), b1(3), b1(4)],...
% 	'EdgeColor','r', 'LineWidth', 0.5,'Curvature',[.2 .2])
% rectangle('Position', [ceil(b2(1)), ceil(b2(2)), b2(3), b2(4)],...
% 	'EdgeColor','r', 'LineWidth', 0.5,'Curvature',[.2 .2])
% rectangle('Position', [ceil(b3(1)), ceil(b3(2)), b3(3), b3(4)],...
% 	'EdgeColor','r', 'LineWidth', 0.5,'Curvature',[.2 .2])
% rectangle('Position', [ceil(b4(1)), ceil(b4(2)), b4(3), b4(4)],...
% 	'EdgeColor','r', 'LineWidth', 0.5,'Curvature',[.2 .2])

set(gca,'XTick',[],'YTick',[])
set(gca,'dataAspectRatio',[1 1 1])
set(gca,'Position',[0 0 1 1])
set(gcf, 'Position', [300, 300, x*3, y*3])
saveas(gcf,'features.png')
