/*This example show how to discover GenICam cameras manually.*/
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif


#include <functional>

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>
#include "SgcExample.h"

static inline int Sgc_setCameraPropertyWithType(SgcCameraHandle * cam, const char * prop, unsigned int val)
{
    return Sgc_setCameraPropertyWithType(cam, prop, &val, SGC_PROPERTY_TYPE_UINT, NULL);
}

static inline int Sgc_setCameraPropertyWithType(SgcCameraHandle * cam, const char * prop, unsigned int val, unsigned int link)
{
    return Sgc_setCameraPropertyWithType(cam, prop, &val, SGC_PROPERTY_TYPE_UINT, &link);
}

class SgcManualDiscoveryExample : public SgcExample{

public:

    SgcManualDiscoveryExample():
      mCamera(nullptr){
      }

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
              SgcCameraHandle* camera = discoverCamera(board);
              
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

protected:	
    SgcCameraHandle* mCamera;

    SgcBoardHandle* initialiseBoard(Fg_Struct* fgHandle){
        SgcBoardHandle* boardHandle = nullptr;
        if(Sgc_initBoard(fgHandle, 0, &boardHandle) != SGC_OK)
            throw std::runtime_error("Cannot initialize the board.");

        Sleep(1000);
        return boardHandle;
    }

    SgcCameraHandle* discoverCamera(SgcBoardHandle* boardHandle){
        SgcCameraHandle* cameraHandle = nullptr;
        Sgc_getCamera(boardHandle, 0, &cameraHandle);

        // Initialise camera
        if(Sgc_setCameraPropertyWithType(cameraHandle, CAM_PROP_NROFLINKS_S, 4))
            throw std::runtime_error("Setting number of links");
        if(Sgc_setCameraPropertyWithType(cameraHandle, CAM_PROP_MASTERID_S, 1))
            throw std::runtime_error("Setting camera master id");
        if(Sgc_setCameraPropertyWithType(cameraHandle, CAM_PROP_LINKSPEED_S, LINK_SPEED_3125))
            throw std::runtime_error("Setting link speed");
        if(Sgc_setCameraPropertyWithType(cameraHandle, CAM_PROP_STREAMPACKET_SIZE_S, 8192))
            throw std::runtime_error("Setting maximum stream packet size");
        if(Sgc_setCameraPropertyWithType(cameraHandle, CAM_PROP_FGPORT_S, 0, 0))
            throw std::runtime_error("Setting up link 1 frame grabber port");
        if(Sgc_setCameraPropertyWithType(cameraHandle, CAM_PROP_FGPORT_S, 1, 1))
            throw std::runtime_error("Setting up link 2 frame grabber port");
        if(Sgc_setCameraPropertyWithType(cameraHandle, CAM_PROP_FGPORT_S, 2, 2))
            throw std::runtime_error("Setting up link 3 frame grabber port");
        if(Sgc_setCameraPropertyWithType(cameraHandle, CAM_PROP_FGPORT_S, 3, 3))
            throw std::runtime_error("Setting up link 4 frame grabber port");

        // Connect camera
        if(Sgc_LinkConnect(boardHandle, cameraHandle))
            throw std::runtime_error("Connecting the links");
        if(Sgc_connectCamera(cameraHandle))
            throw std::runtime_error("Downloading camera XML");

        return cameraHandle;
    }


};