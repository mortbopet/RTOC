%% coordinates (inlet+outlet+yref)
img = rgb2gray(im2double(imread(dis{500})));
imagesc(img)
% [x,y] = getpts; % manuel indtegning af kant

%% edge - automatisk
bw = img<0.42;
se = strel('rectangle',[1 30]);
bw = imclose(bw,se);
bw(:,inlet-10:inlet+10) = 0;
bw = bwareaopen(bw,100);
bw = imcomplement(bw);

subplot(1,2,1)
imagesc(img)
subplot(1,2,2)
imagesc(bw)

%% edge - manuel
hej = zeros(size(img));
for i = 1:size(x,1)
    hej(round(y(i)),round(x(i))) = 1;
end

se = strel('rectangle',[4 50]);
hej = imclose(hej,se);
hej(:,inlet-10:inlet+10) = 0;
hej = bwareaopen(hej,100);

edge_NS61EDTA = imcomplement(hej);

imagesc(edge_NS61EDTA)

%% binary
nr = 100;

img = rgb2gray(im2double(imread(acc{nr})));

frameno_dis_before = frameno_dis(frameno_dis<frameno_acc(nr));
dis_before = dis(frameno_dis<frameno_acc(nr));
frameno_dis_after = frameno_dis(frameno_dis>frameno_acc(nr));
dis_after = dis(frameno_dis>frameno_acc(nr));
[~,index1] = min(abs(frameno_dis_before-frameno_acc(nr)));
[~,index2] = min(abs(frameno_dis_after-frameno_acc(nr)));
bg1 = rgb2gray(im2double(imread(dis_before{index1}))); 
bg2 = rgb2gray(im2double(imread(dis_after{index2})));
bg = (bg1+bg2)/2;

diff = imabsdiff(img,bg);
norm_diff = (diff-min(diff(:)))/(max(diff(:))-min(diff(:)));    
bw1 = imbinarize(norm_diff,0.12);

subplot(1,2,1)
imagesc(img)
subplot(1,2,2)
imagesc(bw1)


