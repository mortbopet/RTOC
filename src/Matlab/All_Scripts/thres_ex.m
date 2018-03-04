%% version info
% Added:
% Removal of cells that don't go all the way through
% xval(1) ændret til 25, så stor celle er med
% New:
% Middel ml. nærmeste tomme frame før og efter trækkes fra isf. den nærmeste
% Blobfilter: ConvexArea/BoundingBoxArea
% frameno creation corrected
%% Prepare discarded and accepted frames file names
clear all, close all, clc

pics = fopen('ImgD1_Accepted.txt');
acc1 = textscan(pics,'%s');
acc = acc1{1,1};

pics = fopen('ImgD1_Discarded.txt');
dis1 = textscan(pics,'%s');
dis = dis1{1,1};

img = rgb2gray(im2double(imread(acc{616})));

%% Doubles of just frame numbers
frameno_acc = str2doubles(extractBetween(acc,"_","."))+1;
frameno_dis = str2doubles(extractBetween(dis,"_","."))+1;

%% Extract background from each accepted frame
count = 1;
for i = [1810 610]
    if frameno_acc(i) < min(frameno_dis) || frameno_acc(i) > max(frameno_dis) 
            [~,index] = min(abs(frameno_dis-frameno_acc(i)));
            bg = rgb2gray(im2double(imread(dis{index})));
    else
            frameno_dis_before = frameno_dis(frameno_dis<frameno_acc(i));
            dis_before = dis(frameno_dis<frameno_acc(i));
            frameno_dis_after = frameno_dis(frameno_dis>frameno_acc(i));
            dis_after = dis(frameno_dis>frameno_acc(i));
            [~,index1] = min(abs(frameno_dis_before-frameno_acc(i)));
            [~,index2] = min(abs(frameno_dis_after-frameno_acc(i)));
            bg1 = rgb2gray(im2double(imread(dis_before{index1}))); 
            bg2 = rgb2gray(im2double(imread(dis_after{index2})));
            bg = (bg1+bg2)/2;
    end  
    im = rgb2gray(im2double(imread(acc{i})));
    im1 = imabsdiff(im,bg);
    im2 = (im1-min(im1(:)))/(max(im1(:))-min(im1(:))); 
    diff(:,:,count) = im2;
    
    
    count = count + 1;
end
subplot(1,2,1)
imagesc(diff(:,:,1))
subplot(1,2,2)
imagesc(diff(:,:,2))