% Script skal køres efter CellSorter
set(0,'DefaultFigureColormap',feval('gray'));

nr = 5;

bb_width = 55;  %was 55
bb_height = 35;  %was 35
% for k = 1:size(RBCs,2)
%     bb = vec2mat(cell2mat(RBCs(k).box),4);
%     if bb(:,3) > bb_width
%         bb_width = round(max(bb(:,3)));
%     end
%    
%     if bb(:,4) > bb_height
%         bb_height = round(max(bb(:,4)));
%     end
% end

bb_im = zeros(bb_height,bb_width,size(RBCs(nr).frame,2));
masks = zeros(bb_height,bb_width,size(RBCs(nr).frame,2));
c = vec2mat(cell2mat(RBCs(nr).centroid),2);
for i = 1:size(RBCs(nr).frame,2)
    r1 = round(c(i,2)) - floor(bb_height/2);
    r2 = round(c(i,2)) + ceil(bb_height/2);
    c1 = round(c(i,1)) - floor(bb_width/2);
    c2 = round(c(i,1)) + ceil(bb_width/2);
    
    f = RBCs(nr).frame(i);
    im = rgb2gray(im2double(imread(acc{f})));
    mask = zeros(size(im));
    mask(RBCs(nr).pixellist{1,i}) = 1;

%     im = im.*mask;
    if c1 > 0 && c2 <= size(im,2)
        bb_im(:,:,i) = im(r1:r2-1,c1:c2-1);
        masks(:,:,i) = mask(r1:r2-1,c1:c2-1);
    end
end
   
% for j = 1:size(bb_im,3)
%     imagesc(bb_im(:,:,j))
%     pause(0.05)
% end

close

% Save images
f = 50;
[y,x] = size(bb_im(:,:,f));

imagesc(bb_im(:,:,f))
hold on
bound = bwboundaries(masks(:,:,f));

        for k = 1:length(bound)
            boundary = bound{k};
            plot(boundary(:,2), boundary(:,1), 'r--', 'LineWidth', 2)
        end
      
set(gca,'XTick',[],'YTick',[])
set(gca,'dataAspectRatio',[1 1 1])
set(gca,'Position',[0 0 1 1])
set(gcf, 'Position', [300, 300, x*4, y*4])
% saveas(gcf,'mask7.png')



