% Script skal køres efter CellSorter
set(0,'DefaultFigureColormap',feval('gray'));

nr = 1;
f1 = 2;
f2 = 12; 
f3 = 24;
f4 = 32;
f5 = 40;

fa = RBCs(nr).frame(f1);
fb = RBCs(nr).frame(f2);
fc = RBCs(nr).frame(f3);
fd = RBCs(nr).frame(f4);
fe = RBCs(nr).frame(f5);

im1 = rgb2gray(im2double(imread(acc{fa})));
im2 = rgb2gray(im2double(imread(acc{fb})));
im3 = rgb2gray(im2double(imread(acc{fc})));
im4 = rgb2gray(im2double(imread(acc{fd})));
im5 = rgb2gray(im2double(imread(acc{fe})));

pix1 = cell2mat(RBCs(nr).pixellist(f1));
pix2 = cell2mat(RBCs(nr).pixellist(f2));
pix3 = cell2mat(RBCs(nr).pixellist(f3));
pix4 = cell2mat(RBCs(nr).pixellist(f4));
pix5 = cell2mat(RBCs(nr).pixellist(f5));

vals1 = im1(pix1);
vals2 = im2(pix2);
vals3 = im3(pix3);
vals4 = im4(pix4);
vals5 = im5(pix5);

[~,index] = min(abs(frameno_dis-frameno_acc(fa)));
bg = rgb2gray(im2double(imread(dis{index})));

pic = bg;
pic(pix1) = vals1;
pic(pix2) = vals2;
pic(pix3) = vals3;
pic(pix4) = vals4;
pic(pix5) = vals5;

[y,x] = size(pic);

imagesc(pic), hold on
set(gca,'XTick',[],'YTick',[])
set(gca,'dataAspectRatio',[1 1 1])
set(gca,'Position',[0 0 1 1])
set(gcf, 'Position', [300, 300, x*2, y*2])
 saveas(gcf,'eks2.png')



