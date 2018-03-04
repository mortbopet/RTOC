%% Load images containing cells
clear all; close all; clc

% load('BG.mat')
pics = fopen('ImgD1_Accepted.txt');
acc1 = textscan(pics,'%s');
acc = acc1{1,1};

img = rgb2gray(im2double(imread(acc{8112})));

%     temp2 = imabsdiff(img,BG);
%     
%     bw = temp2>.01;
%  
%     bw = bwareaopen(bw,200);
%      
%     se = strel('disk',4);
%     bw = imclose(bw,se);
%     bw = imfill(bw,'holes');
%     
imagesc(img),colormap gray

% 4050,4080,4095,4105,4115,1375,1382 to på en gang
% 4200,4240,4255,4270,4285,4290
% 2170,2210,2220,2230,2240 kanalkant sidder fast på sidste
% 5690,5720,5735,5745
% 29950,29975,29988,29999
% 9960,9990,10000,10007,10015
% 8030,8080,8095,8105,8112

%%
imadd = zeros(size(BG));

for i = [8030,8080,8095,8105,8112]
    
    temp1 = rgb2gray(im2double(imread(acc{i})));
    temp2 = imabsdiff(temp1,BG);
    
    bw = temp2>.01;
    bw = bwareaopen(bw,200);
    se = strel('disk',4);
    bw = imclose(bw,se);
    bw = imfill(bw,'holes');
    
%     imagesc(bw),pause(2)
    
    temp3 = bw.*temp1;
    imadd = imadd + temp3;
    
end

hej = imadd>0;
hej = imcomplement(hej);

samlet = hej.*BG;

samlet = samlet + imadd;
imagesc(samlet),colormap gray

set(gca,'XTick',[],'YTick',[])
set(gca,'Position',[0 0 1 1])
% saveas(gcf,'eks10.png')

