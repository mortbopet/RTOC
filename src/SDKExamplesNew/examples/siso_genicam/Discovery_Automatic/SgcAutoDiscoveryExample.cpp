/*This example show how to discover GenICam cameras automatically.*/
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


class SgcAutoDiscoveryExample : public SgcExample{
public:

    SgcAutoDiscoveryExample():
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

              /*FgDmaChannelExample shows dma initialization.*/
              int32_t dmaPort = selectDmaPortDialog(mFgHandle);
              mDmaHandle = DmaMemWrapper::create(mFgHandle, dmaPort);

              mDisplay= DisplayWrapper::create(
                  std::weak_ptr<DmaMemWrapper>(mDmaHandle), 
                  dmaPort,
                  std::weak_ptr<FgWrapper>(mFgHandle));

              SgcBoardHandle* board = initialiseBoard(mFgHandle->getFgHandle());
              std::vector<std::pair<std::string, SgcCameraHandle*>> cameras = discoverCameras(board);
              PRINT_INFO(mLog, "Discovered cameras");
              for(size_t i = 0; i < cameras.size(); i++)
                  std::cout << "   " << cameras.at(i).first << std::endl;

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

    /*initializes board handle*/
    SgcBoardHandle* initialiseBoard(Fg_Struct* fgHandle){
        SgcBoardHandle* boardHandle = nullptr;
        if(Sgc_initBoard(fgHandle, 0, &boardHandle) != SGC_OK)
            throw std::runtime_error("Cannot initialize the board.");

        Sleep(1000);
        return boardHandle;
    }

    /*creates a camera handle with the given index */
    static std::pair<std::string, SgcCameraHandle *> getCameraByIndex(SgcBoardHandle* board, int32_t index){
        SgcCameraHandle *camHandle = nullptr;

        if (Sgc_getCameraByIndex(board, index, &camHandle) != SGC_OK)
            throw std::runtime_error("No camera for given index.");

        SgcCameraInfo* cameraInfo = Sgc_getCameraInfo(camHandle);
        if (cameraInfo == nullptr || 
            cameraInfo->deviceVendorName == nullptr ||
            cameraInfo->deviceModelName == nullptr)
            throw std::runtime_error("Sgc_getCameraInfo failed on camera.");


        if (Sgc_connectCamera(camHandle) != SGC_OK)
            throw std::runtime_error("Sgc_connectCamera failed on camera");

        return std::pair<std::string, SgcCameraHandle *>(cameraInfo->deviceModelName, camHandle);
    }

    /*discovers Cameras and returns a vector of tuples consisting of the camera handle and the name*/
    static std::vector<std::pair<std::string, SgcCameraHandle*>> discoverCameras(SgcBoardHandle* boardHandle){
        int32_t portMask = 0xFF;
        int32_t timeoutInMilliSec = 5000;
        if(Sgc_scanPorts(boardHandle, portMask, timeoutInMilliSec, 0) != SGC_OK)
            throw std::runtime_error("Cannot scan board ports.");

        int32_t nrOfDevices = Sgc_getCameraCount(boardHandle);

        if (nrOfDevices <= 0)
            throw std::runtime_error("No camera detected");

        std::vector<std::pair<std::string, SgcCameraHandle*>> candidates;
        for(int32_t i = 0; i < nrOfDevices; i++){
            try{
                std::pair<std::string, SgcCameraHandle *> cam = getCameraByIndex(boardHandle, i);
                candidates.push_back(cam);
            }catch(std::exception& e){
                std::cout << e.what() << std::endl;
            }
        }

        return candidates;
    }

    /*show user a list of avaible cameras. Return value is the camera handle of 
    the selected camera*/
    SgcCameraHandle* selectCamera(SgcBoardHandle* boardHandle){
        std::vector<std::pair<std::string, SgcCameraHandle*>> candidates = 
            discoverCameras(boardHandle);
        if(candidates.empty())
            throw std::runtime_error("No valid camera detected!");

        std::vector<std::string> candidateNames;
        for(uint32_t i = 0; i < candidates.size(); i++)
            candidateNames.push_back(candidates.at(i).first);

        uint32_t camIndex = mLog->selectDialog<std::string>(candidateNames, "camera");
        SgcCameraHandle * camera = candidates.at(camIndex).second;
        return camera;
    }

    SgcCameraHandle* mCamera;
};