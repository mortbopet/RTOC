% -------------------------------------------------------------------------
% This script is created by Lise Aagesen for her M.Sc. thesis in the fall
% of 2017. It is intended for extraction of final features for
% RBC classification (fetal vs adult) from a cell registration structure
% called "RBCs" that is generated in the script called CellSorter.
% -------------------------------------------------------------------------

%% Find RBCs files
F = 'RBCs.mat';                             % name of the file
D = '.';
S = dir(fullfile(D,'D*'));                  % name of folder to search in
X = [S.isdir] & ~ismember({S.name},{'.','..'});
N = {S(X).name};

%% Initialization
FV = [];                                    % Feature Matrix
donor = 1;                                  % 0/1 -> fetal/voksen

%% Loop through folders with an RBCs file in them
for k = 1:numel(N)
    T = fullfile(D,N{k},F);
    load(T)                                 % RBCs struct is loaded
    
    inlet = RBCs(1).inlet;
    outlet = RBCs(1).outlet;
    yref = RBCs(1).yref;
    pixelsize = 40/(outlet-inlet);          %microns/pixel
    
    % loop through RBCs struct (rows = cells)
    for i = 1:size(RBCs,2)
        
        bb = vec2mat(cell2mat(RBCs(i).box),4);
        cc = vec2mat(cell2mat(RBCs(i).centroid),2);
        
        [~ , idx1] = min(abs(bb(:,1)+bb(:,3)-inlet));
        cy = cc(idx1,2);
        vp = cy-yref;
        ecc1 = RBCs(i).eccentricity(idx1);
        circ = RBCs(i).circularity(idx1);
        dia = (RBCs(i).majoraxis(idx1))*pixelsize;
        f1 = RBCs(i).frame(idx1);
        x1 = bb(idx1,1);
        
        [~ , idx2] = min(abs(cc(:,1)-(inlet+70)));
        ecc2 = RBCs(i).eccentricity(idx2);
        symmetry = RBCs(i).symmetry(idx2);
        gradient = RBCs(i).gradient(idx2);
        
        [~ , idx3] = min(abs(bb(:,1)+bb(:,3)-outlet));
        ecc3 = RBCs(i).eccentricity(idx3);
        f3 = RBCs(i).frame(idx3);
        x3 = bb(idx3,1);
        dia3 = (RBCs(i).majoraxis(idx3))*pixelsize;
        dratio = dia3/dia;
        eccratio = ecc3/ecc2;
        
        [~ , idx4] = min(abs(bb(:,1)-inlet));
        f4 = RBCs(i).frame(idx4);
        x4 = bb(idx4,1);
        
        v1 = (f4-f1)/(x4-x1);
        v2 = (f3-f4)/(x3-x4);
        vratio = v2/v1;
        
        FV = [FV; donor vp circ dia eccratio ecc3 v1 v2 vratio ...
            dratio symmetry gradient];
    end  
end