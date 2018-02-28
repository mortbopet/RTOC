# Matlab to C++
This should act as a chronological todo-list in preparation of rewriting Lise
Aagesens Matlab-scripts for her M.Sc.-thesis, fall 2017.
As a starting-point this includes _"CellSorter.m"_ and _"FeatureVector.m"_.

#### Classes to declare
We could specialize a library for the type of data Lise is working with; I'm think a custom type for the __RBCs__.



#### Functions to declare in C++ for "CellSorter.m"

  - [ ] [textscan](https://se.mathworks.com/help/matlab/ref/textscan.html):
    Loads a text/data-file to string array
  - [ ] [str2double](https://se.mathworks.com/help/matlab/ref/str2double.html):
    Convert string-array to double-array
  - [ ] [extractBetween](https://se.mathworks.com/help/matlab/ref/extractbetween.html):
    Return string between two delimiters
  - [ ] [strel](https://se.mathworks.com/help/images/ref/strel.html):
    Create a strel-object from specified input (see ref.)
  - [ ] [rgb2gray](https://se.mathworks.com/help/matlab/ref/rgb2gray.html):
    short description
  - [ ] [im2double](https://se.mathworks.com/help/matlab/ref/im2double.html):
    short description
  - [ ] [imread](https://se.mathworks.com/help/matlab/ref/imread.html):
    short description
  - [ ] [imabsdiff](https://se.mathworks.com/help/matlab/ref/imabsdiff.html):
    short description
  - [ ] [imclose](https://se.mathworks.com/help/images/ref/imclose.html):
    short description
  - [ ] [bwareaopen](https://se.mathworks.com/help/images/ref/bwareaopen.html):
    short description
  - [ ] [imcomplement](https://se.mathworks.com/help/images/ref/imcomplement.html):
    short description
  - [ ] [imbinarize](https://se.mathworks.com/help/images/ref/imbinarize.html):
    Binarize 2-D grayscale image or 3-D volume by thresholding
  - [ ] [bwpropfilt](https://se.mathworks.com/help/images/ref/bwpropfilt.html):
    Extract objects from binary image using properties
  - [ ] [bwconncomp](https://se.mathworks.com/help/images/ref/bwconncomp.html):
    Find connected components in binary image
  - [ ] [regionprops](https://se.mathworks.com/help/images/ref/regionprops.html):
    Measure properties of image regions
  - [ ] [cat](https://se.mathworks.com/help/matlab/ref/cat.html):
    In use with regionprops (see line 95-97)
  - [ ] [imclearborder](https://se.mathworks.com/help/images/ref/imclearborder.html):
    Suppress light structures connected to image border
  - [ ] [edge](https://se.mathworks.com/help/images/ref/edge.html):
    Find edges in intensity image
  - [ ] [fliplr](https://se.mathworks.com/help/matlab/ref/fliplr.html):
    Flip array left to right (in general then; array/matrix operations - is this default in c++?)
  - [ ] [cell2mat](https://se.mathworks.com/help/matlab/ref/cell2mat.html):
    Convert cell array to ordinary array of the underlying data type
  - [ ] [vec2mat](https://se.mathworks.com/help/comm/ref/vec2mat.html):
    Convert vector into matrix
  - [ ] [rmfield](https://se.mathworks.com/help/matlab/ref/rmfield.html):
    Remove fields from structure
  - [ ] [save](https://se.mathworks.com/help/matlab/ref/save.html):
    Save variables to to file (pretty basic yea yea)

#### Functions to declare in C++ for "FeatureVector.m"
None not already mentioned.






###### Notes:
A lot of the functions show up in connection with OpenCV and c++ when googling/goofing around. It might be initially smart to check out OpenCV -> pros/cons etc.
If there's anything we can skip to program from the bottom; that might be a super nice!
Btw: [OpenCV + OpenCL](https://opencv.org/platforms/opencl.html)
