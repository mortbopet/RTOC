/*This example how to read and modify GenICam features from cameras.*/
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>

#include "SgcAutoDiscoveryExample.cpp"

class SgcParameterExample : public SgcAutoDiscoveryExample{
public:
    int32_t run(){
        try{
            //initializes internal structures of the library.
            int32_t status = Fg_InitLibraries(nullptr);
            if(status != FG_OK)
                throw std::runtime_error("Cannot initialize Fg libraries.");

            /*FgAppletIteratorExample shows how to load an applet.*/
            uint32_t boardIndex = selectBoardDialog();
            std::string appletPath = selectAppletDialog(boardIndex);
            mFgHandle = FgWrapper::create(appletPath, boardIndex);

            SgcBoardHandle* board = initialiseBoard(mFgHandle->getFgHandle());
            mCamera = selectCamera(board);

            modifyWidthRegister(mCamera);

            Sgc_freeBoard(board);

            //releases internal structures of the library
            Fg_FreeLibraries();
        }catch(std::exception& e){
            //releases internal structures of the library
            Fg_FreeLibraries();
            std::cout << "Example failed: " << e.what() << std::endl;
            return -1;
        }
        return 0;
    }
private:
    /*This function reads and writes on the Width feature.*/
    void modifyWidthRegister(SgcCameraHandle* cameraHandle){
        int64_t width = 0;
        int err = Sgc_getIntegerValue(cameraHandle,"Width", &width);
        width = 1024;
        err = Sgc_setIntegerValue(cameraHandle,"Width", width);
    }

};