# RTOC
Real Time Object Classification



### Brief:

This project is derived from the master thesis by Lisa O. Aagensen, DTU Physics. The initial goal was to not only speed up the process in acquiring images and processing these images for data extraction, but also to work towards a real-time approach where images are acquired and analyzed while found objects are classified by a loaded ML model.

[Video on how to use the program here.](https://www.youtube.com/watch?v=9l0NIpTe4tE)


# Build instructions
**Note:** for easy configuration, debuggin etc., it can be highly recommended use QtCreator (bundled with most Qt installations) when working with the project.
### Dependencies:
 - Boost (1.66.0 or higher) https://www.boost.org/
 - OpenCV (3.4.1 or higher) https://opencv.org/
 - Qt (5.8 or higher) https://www.qt.io/download
 - CMake (3.8 or higher) https://cmake.org/

## Linux & OSX

## Windows
On Windows, a compiler needs to be available - *make sure that your compiler version matches the one which your boost version has been built with*. [Microsoft Visual Studio Community](https://visualstudio.microsoft.com/vs/community/) can be downloaded for free, to access the MSVC compiler.

Load the CMakeLists.txt file in the CMake gui and run configuration. If all dependencies are available and all required paths are set,  the CMake configuration should report success. In the project directory, run
 ```
 cmake.exe --build . --target all
 ``` 
 to build the app. 

A new(er) version of boost is currently required for building RTOC. On Windows,we explicitely define the boost *.lib* files which are linked to. Set environment variable **BOOST_ROOT** to the path of your compiled boost libraries and make sure that the name of your libraries is equal to the *libboost_..._1_##.lib* files which are specified in the [CMakeLists.txt](CMakeLists.txt) file. Currently, we build RTOC with Boost 1.66 using the Visual Studio 2017 compiler (vc141). **If you are using another boost version, or your boost libraries named otherwise, you must manually configure the CMakeLists.txt file to suit your needs.**

### Building with SiliconSoftware support
RTOC contains an interface to a SiliconSoftware microEnable 5 VQ8-CXP6D ironman framegrabber card. To build support for this interface, a SiliconSoftware runtime installation must be present on the system. RTOC has been built using [SiliconSoftware Runtime v.5.1.1](https://silicon.software/blog-runtime-551/).

To enable support for the framegrabber, one must set the "**BUILD_INROMAN"** flag in the CMake configuration to "**ON**". The configuration of this can be a bit tricky, and sometimes fails - running the CMake configuration a couple of times in succession usually fixes the issues (These are issues in the CMake files provided by the SiliconSoftware libraries.)