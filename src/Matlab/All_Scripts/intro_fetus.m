%% Load images containing cells
clear all; close all; clc
tic
addpath('F:\Lise Aagesen master thesis\RBCdata\FetalDonors\20170714_100x\20170714_100x\Donor_data\NS28\NS28.png');
toc

%%
pics = fopen('NS28_Accepted.txt');
acc1 = textscan(pics,'%s');
acc = acc1{1,1};

load('BGns.mat')

% define 0-area (boundary)
BG2 = BG > 0.72;
BG2 = bwareaopen(BG2,300);
se = strel('diamond',5);
BG2 = imclose(BG2,se);
% BG2 = imdilate(BG2,se);
BG2 = imfill(BG2,'holes');

BGcomp = imcomplement(BG2);

% subplot(1,2,1)
% imagesc(BG),colormap gray
% subplot(1,2,2)
% imagesc(BG2),colormap gray

%
img = rgb2gray(im2double(imread(acc{893})));

temp2 = imabsdiff(img,BG);
bw = temp2>.02;
bw2 = bw.*BGcomp;
bw3 = bwareaopen(bw2,100,8);  

se = strel('disk',4);
bw4 = imopen(bw3,se);
bw4 = imclose(bw4,se);
bw4 = imfill(bw4,'holes');

% bw4 = bwpropfilt(bw3,'ConvexArea',1,'smallest');

% subplot(2,3,1)
% imagesc(img),colormap gray
% subplot(2,3,2)
% imagesc(bw),colormap gray
% subplot(2,3,3)
% imagesc(bw2),colormap gray
% subplot(2,3,4)
% imagesc(bw3),colormap gray
% subplot(2,3,5)
% imagesc(bw4),colormap gray

%eksempelbillede
% eks = [img temp2];
% imagesc(eks),colormap gray
% set(gca,'XTick',[])
% set(gca,'YTick',[])
% set(gca,'Position',[0 0 1 1])
% saveas(gcf,'eks1.png')

subplot(1,3,1)
imagesc(img),colormap gray
set(gca,'XTick',[]),set(gca,'YTick',[])
subplot(1,3,2)
imagesc(temp2),colormap gray
set(gca,'XTick',[]),set(gca,'YTick',[])
subplot(1,3,3)
imagesc(bw),colormap gray
set(gca,'XTick',[]),set(gca,'YTick',[])
% saveas(gcf,'eks1.png')

% imagesc(img),colormap gray

% 855,880,890,900,915

%%
imadd = zeros(size(BG));

for i = [855,878,890,900,915]
    
    temp1 = rgb2gray(im2double(imread(acc{i})));
    temp2 = imabsdiff(temp1,BG);
    
    imagesc(temp2),pause(1)    
    
    bw = temp2>.01;
    bw = bw.*BGcomp;
    bw = bwareaopen(bw,200,8); 
    se = strel('disk',4);
    bw = imopen(bw,se);
    bw = imclose(bw,se);
    bw = imfill(bw,'holes');
    bw = bwpropfilt(bw,'ConvexArea',1,'smallest');
    
%     imagesc(bw),pause(1)
    
    temp3 = bw.*temp1;
    imadd = imadd + temp3;
    
end

hej = imadd>0;
hej = imcomplement(hej);

samlet = hej.*BG;

samlet = samlet + imadd;
imagesc(samlet)

set(gca,'XTick',[])
set(gca,'YTick',[])
set(gca,'Position',[0 0 1 1])
% saveas(gcf,'eks10.png')

