discard = fopen('ImgD1_Discarded.txt');
bg_img = textscan(discard,'%s');
bg_img = bg_img{1,1};

background = zeros(size(img));

tic
for i = 1:length(bg_img)
    bg = rgb2gray(im2double(imread(bg_img{i})));
    background = background + bg;
end
toc

background = background/length(bg_img);

BG = medfilt2(background);
imagesc(BG), colormap gray