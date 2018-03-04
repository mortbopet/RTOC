clear
close all
% ImageGroup1 > 3009Donor15Exp2
%load('ImgDonor17WholeBlood.mat');
load('ImageGroup1.mat');

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Data analysis

%% X and Y limits
yLimit1 = 40; % 
yLimit2 = 145; % 
xLimit1 = 137; %                   % Beginning of entrance
xOut = xLimit1;                    % X-output                    
%%
resB = zeros(1,cellNum);           % Time at input
resQ = zeros(1,cellNum);           % frames number at input
resB2 = zeros(1,cellNum);          % Time at output
resQ2 = zeros(1,cellNum);          % frames number at output

for r=1:cellNum
    %% Input 
    Lbox=length(RBCs(r).box);                     % Length of each cell's box (frames number)
    bbox=reshape(cell2mat(RBCs(r).box),[4,Lbox]); % Set all boxes of each cell in a matrix form  
    for q=1:Lbox                                  % 
        yoor1=bbox(2,q);                          % Define y-coordinate of upper edge of box
        ycoor2=bbox(2,q)+bbox(4,q);               % Define y-coordinate of lower edge of box
        xcoor=bbox(1,q) + bbox(3,q);              % Define x-coordinate of right edge of box for input    
        xcoor2 = bbox(1,q);                       % Define x-coordinate of left edge of box for output 
        if yoor1<=yLimit1 || ycoor2>=yLimit2 
            break
        end
        if xcoor >= xLimit1 && resQ(r) == 0;
                resB(r)= RBCs(r).time(q);
                resQ(r)= q;
        end
        if xcoor2 >= xOut
           resB2(r)= RBCs(r).time(q);
           resQ2(r)=q;
           break
        end        
    end    
end

EntryTime = zeros(1,cellNum);
[~, col]=find(resB);
EntryTime(col)=resB2(col)-resB(col);

%% Bar plot for entrance time
fig2 = figure(1);
for i = 1:length(col)
   entranceTime(i) = resB2(col(i))-resB(col(i));
end
%for k = 1:length(col)
    M = mean(entranceTime);
%end
hist(entranceTime)
title('Entrance Time'), ylabel('Number of Cells'), xlabel('Time [s]')
hold on 
plot(M*[1 1],[0 length(col)], '--g', 'Linewidth',4,'MarkerSize',20)
hold off
% print(fig2,'EntranceTime','-dpdf')
%% Potting of x-axis versus Time
fig1 = figure(2);
title('X-axis vs Time'), ylabel('X-axis[µm]'), xlabel('Time[s]')
hold on
for i = 1:length(col)
    r=col(i);
    Coorx=[];
    Time = [];    
    Lbox=length(RBCs(r).box);
    bbox=reshape(cell2mat(RBCs(r).box),[4,Lbox]);
    for q = 1:length(RBCs(r).time(1:end))
        Coorx(q) = (bbox(1,q) + bbox(3,q)) * pixelSize;
        Time(q) = RBCs(r).time(q)- RBCs(r).time(1);
    end
    plot(Time, Coorx)
    q_in = resQ(r);                                % the frame number of contact
    q_out = resQ2(r);                              % the frame number of exit
    plot([Time(q_in) Time(q_out)],[Coorx(q_in) Coorx(q_out)],'.','Color','red','MarkerSize',15)       
end
hold off
% print(fig1,'X-axis vs Time','-dpdf')

%% Subplots
for i =1:length(col);
    fig3=figure(100+col(i));
    set(gcf,'PaperPositionMode','auto')
    set(fig3, 'Position', [0 0 400 800])
    imgS = imread(accepted{1}{round(resB2(col(i))*frameRate)});
    img1S = double(rgb2gray(imgS));
    img1S = img1S / 255;
    img2S = abs((img1S-BG));   
    img2S = imadjust(img2S,[0 0.08],[0 1]); % CONTRAST RANGE, ADJUST
    img3S = im2bw(img2S, 0.085); 
    img4S = bwareaopen(img3S, 8);
    img5S = imclose(img4S,se);
    img6S = imfill(img5S, 'holes');
    img  = imread(accepted{1}{round(resB(col(i))*frameRate)});    
    img1 = double(rgb2gray(img));
    img1 = img1 / 255;
    img2 = abs((img1-BG));   
    img2 = imadjust(img2,[0 0.08],[0 1]);   % CONTRAST RANGE, ADJUST
    img3 = im2bw(img2, 0.085); 
    img4 = bwareaopen(img3, 8);
    img5 = imclose(img4,se);
    img6 = imfill(img5, 'holes');
subplot(4,1,1,'align');
    imagesc(img1),title(['Entrance time ',num2str(1000*EntryTime(col(i))),' ms'])
    imshow(img1+0.5*img6+0.5*img6S), title(['Entrance time ',num2str(1000*EntryTime(col(i))),' ms']) 
    imrect(gca, RBCs(col(i)).box{resQ(col(i))});
    hold on; 
    imrect(gca, RBCs(col(i)).box{resQ2(col(i))});
   %% Binary image
subplot(4,1,2,'align')
    imagesc(img1), title(['Entrance time ',num2str(1000*EntryTime(col(i))),' ms'])
    imshow(img6+img6S), title(['Entrance time ',num2str(1000*EntryTime(col(i))),' ms'])
    imrect(gca, RBCs(col(i)).box{resQ(col(i))});
    hold on; 
    imrect(gca, RBCs(col(i)).box{resQ2(col(i))}); 
subplot(4,1,3,'align')
    title('X-axis vs Time'), ylabel('X-axis[µm]'), xlabel('Time[s]')
    r=col(i);
    Coorx=[];
    Time = [];    
    Lbox=length(RBCs(r).box);
    bbox=reshape(cell2mat(RBCs(r).box),[4,Lbox]);
    for q = 1:length(RBCs(r).time(1:end))
        Coorx(q) = (bbox(1,q) + bbox(3,q)) * pixelSize;
        Time(q) = RBCs(r).time(q) - RBCs(r).time(1);
    end
    plot(Time, Coorx)
    hold on
    q_in = resQ(r);                                % the frame number of contact
    q_out = resQ2(r);                              % the frame number of exit
    plot([Time(q_in) Time(q_out)],[Coorx(q_in) Coorx(q_out)],'.','Color','red','MarkerSize',15)   
    title('X-axis vs Time'), ylabel('X-axis[µm]'), xlabel('Time[s]')
    hold on;
subplot(4,1,4,'align')
    title('V_x vs Time'), ylabel('V_x[µm/s]'), xlabel('Time[s]')
    r=col(i);
    Coorx=[];
    Time = [];    
    Lbox=length(RBCs(r).box);
    bbox=reshape(cell2mat(RBCs(r).box),[4,Lbox]);
    for q = 1:length(RBCs(r).time(1:end))
        Coorx(q)=(bbox(1,q)+bbox(3,q))*pixelSize;
        Time(q)=RBCs(r).time(q)-RBCs(r).time(1);
    end
    V_x(2:length(Coorx))=diff(Coorx);     % the difference
    V_x(1)=0;
    plot(Time,V_x)
    q_in = resQ(r);                        % the frame number of contact
    q_out = resQ2(r);                      % the frame number of exit
    hold on
    plot([Time(q_in) Time(q_out)],[V_x(q_in) V_x(q_out)],'.','Color','red','MarkerSize',20)
    title('V_x vs Time'), ylabel('V_x[µm/s]'), xlabel('Time[s]')
    clear Coorx Time V_x
    hold on
% Print images 
%     filename = ['EntranceTime_' num2str(i) '.pdf']
%     print(fig3, ['PrintFile/' filename],'-dpdf')
            
end
%% Print images
%% Velocity vs Time
% fig5 = figure(3);
% title('V_x vs Time'), xlabel('V_x[µm/s]'), ylabel('Time[s]')
% hold on
% clear V_x
% for i = 1:length(col)
%     r=col(i);
%     Coorx=[];
%     Time = [];    
%     Lbox=length(RBCs(r).box);
%     bbox=reshape(cell2mat(RBCs(r).box),[4,Lbox]);
%     
%     for q = 1:length(RBCs(r).time(1:end))
%         Coorx(q)=(bbox(1,q)+bbox(3,q))*pixelSize;
%         Time(q)=RBCs(r).time(q)-RBCs(r).time(1);
%     end
%     V_x(2:length(Coorx))=diff(Coorx);     %the difference
%     V_x(1)=0;
%     plot(Time, V_x)
%     q_in = resQ(r);                        %the frame number of contact
%     q_out = resQ2(r);                      %the frame number of exit
% %     plot([V_x(q_in) V_x(q_out)],[Time(q_in) Time(q_out)],'.','Color','red','MarkerSize',20)
%     clear Coorx Time V_x
% end
% hold off
% %print(fig5,'V_x vs Time','-dpdf')


%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% Individual cells plot test
%     figure (11)
%     for r = 10
%     coorx=[];
%     time = [];  
%     Lbox =length(RBCs(r).box);
%     bbox =reshape(cell2mat(RBCs(r).box),[4,Lbox]);
%     for q = 1:length(RBCs(r).time); %(1:end))
%         coorx(q) = (bbox(1,q) + bbox(3,q)) * pixelSize;
%         time(q) = RBCs(r).time(q) - RBCs(r).time(1);  
%     end
%     plot(time, coorx),ylabel('X-axis[µm]'),xlabel('Time[s]')
%     hold on
%     q_in = resQ(r);                                % the frame number of contact
%     q_out = resQ2(r);                              % the frame number of exit
%     plot([time(q_in) time(q_out)],[coorx(q_in) coorx(q_out)],'.','Color','red','MarkerSize',15)  
% end
