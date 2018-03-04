% FrameFinder.m loads images of RBCs passing through microfluidic channels
% and performs a neighbor-subtraction to identify if any movement is
% present in the image sequence in order to discard images without any RBC
% movement.
%
% Authors:  Henrik Thirstrup
% Date:     September 2015.
%
% INPUT     = .PNG IMAGES
% OUTPUT    = .TXT WITH ACCEPTED/DISCARDED IMAGES NAME
    
clear; close all; clc
    
% IMAGES FOLDER
%folder = 'D:\RBC-blodbanken\PolyNanoDemonstrator\20161004\PolyNanoChip1_ch4a_10mbar';
%folder = '..\PolyNanoChips\20161004\PolyNanoChip1_ch4a_10mbar\PolyNanoChip1_ch4a_10mbar';
%folder = '..\PolyNanoChips\20161004\PolyNanoChip1_ch4a_10mbar\test2';
folder = 'F:\RBCdata\AdditionalData\20170926\Spike-P8-D107';

textdir = folder

% ADD FRAMES DIRECTORY
addpath(genpath(folder))

% LIST IMAGES
files = dir(folder);
files(1:3) = []; % (CM:me- files that doesn't contain image) (%Tony: (1:3))
%%
% SORT FILES CORRECTLY
name = {files.name};
str  = sprintf('%s#', name{:});
num  = sscanf(str,'Spike-P8-D107_%d.png#');       % for 10mbar 
% adjust as needed, e.g.     num  = sscanf(str, 'MyImages_%d.png#');
[dummy, index] = sort(num);
name = name(index)'; 

disp([num2str(length(name)), ' images found']);
%%
% VARIABLE INITIALIZATION
n = 3;                                  % neighbor distance 
crit = 0;                               % intensity criteria
critbank =zeros(1,length(files));       % criteria storage
accept=zeros(1,length(files)); 
imgDiff = 0;                            % image subtraction
lim1 = 0.0354;    % 0.06 for files with D107, D108; 0.0354 for NS62_EDTA;
                % 0.04 for NS62_hep_S1; 0.0354 for NS62_hep_S2;
                % 0.05 for P8_S1, P8_S2, P8_S3; 

% intensity threshold

% OPEN TEXT FOR WRITING
facc = fopen([textdir,'_Accepted.txt'],'w+');
fdis = fopen([textdir,'_Discarded.txt'],'w+');

close all
 tic
%for loop over all the pictures
sl=1;
for i = 1:length(name)
    
    % READ IMAGE AND SET FOV
    img = imread(name{i});
        
    % NEIGHBOR SUBTRACTION
    if i-n > 0 && rem(i,n) == 0
        imgN = imread(name{i-n});
        imgDiff = imfixFF(img,imgN);
       
        % PEAK DETECTOR
        crit = max(max(imgDiff));
        critbank(i) = crit;
    end
    
    % DUMP CONSECUTIVE IMAGES BELOW THRESHOLD
    if i-n > 0 && crit <= lim1 && critbank(i-n) <= lim1 && i>10
        fprintf(fdis,'%s\n',name{i});
        accept(i)=0;
    else
        fprintf(facc,'%s\n',name{i});
        accept(i)=1;
    end
%             % SHOW IMAGES FOR VERIFICATION
%            fig1 = figure(1);
%            set(gcf,'PaperPositionMode','auto')
%            set(fig1, 'Position', [0 0 1200 1000])
%            subplot(1,3,1)
%            imagesc(img); colormap gray; axis image
%            subplot(1,3,2)
%            imagesc(imgDiff); colormap gray; axis image;
%            text(0,0,files(i).name,'BackgroundColor',[0 1 0])
%            subplot(1,3,3)
%            plot(critbank(1:i).*accept(1:i),'b');
%            hold on
%            plot(critbank(1:i).*(-accept(1:i)+1),'r');
%            drawnow % pause(0.5)
end
toc
i=length(name);
figure(2)
plot(critbank(1:i).*accept(1:i),'b');
hold on
plot(critbank(1:i).*(-accept(1:i)+1),'r');
drawnow % pause(0.5)

% CLOSE TEXTS
fclose(facc);
fclose(fdis);

% OPEN TEXTS
facc = fopen([textdir,'_Accepted.txt']);
fdis = fopen([textdir,'_Discarded.txt']);

% GRAB NAMES
accepted = textscan(facc,'%s');
discarded = textscan(fdis,'%s');

% RESULT
disp([num2str(length(accepted{1})), ' frames accepted after ', num2str(i), ' iterations']);
