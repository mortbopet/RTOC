 function img=imfixFF(img,img2)
% CONVERT TO GRAYSCALE DOUBLES
img = double(rgb2gray(img));
img2 = double(rgb2gray(img2));
% NORMALIZE
img = img / 255;
img2 = img2 / 255;
% SUBTRACT
img = img-img2;