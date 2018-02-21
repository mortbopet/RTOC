/*This example demonstrates the acquisition with siso_genicam.*/
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

class SgcAcquisitionExample : public SgcAutoDiscoveryExample{

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

            /*FgDmaChannelExample shows dma initialization.*/
            int32_t dmaPort = selectDmaPortDialog(mFgHandle);
            mDmaHandle = DmaMemWrapper::create(mFgHandle, dmaPort);
            mDisplay= DisplayWrapper::create(
                std::weak_ptr<DmaMemWrapper>(mDmaHandle), 
                dmaPort,
                std::weak_ptr<FgWrapper>(mFgHandle));
            SgcBoardHandle* board = initialiseBoard(mFgHandle->getFgHandle());
            std::vector<std::pair<std::string, SgcCameraHandle*>> cameras = discoverCameras(board);

            acquisitionSgc(
                mFgHandle->getFgHandle(),
                dmaPort, 
                mDmaHandle->getMemHandle(),
                mCamera);

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
    void acquisitionSgc(Fg_Struct* fgHandle, uint32_t dmaPort, dma_mem* dmaHandle, SgcCameraHandle* cameraHandle){
        uint32_t MaxPics = 5000;

        /*FgAcqBlockExample shows the acquisition*/
        FgAcqBlockExample::acquisitionLoop(
            fgHandle,
            dmaPort,
            dmaHandle);



        frameindex_t bufNr = 0;
        bool isError = false;
        const uint32_t waitDurationInMs = 10;

        if(Fg_AcquireEx(fgHandle,dmaPort,MaxPics,ACQ_BLOCK,dmaHandle) != FG_OK)
            throwLastFgError();

        /*Start the camera acquisition*/
        Sgc_startAcquisition(cameraHandle, 0);

        while(!isError){
            bufNr = Fg_getImageEx(fgHandle,
                SEL_ACT_IMAGE,
                0,
                dmaPort,
                waitDurationInMs,
                dmaHandle);

            isError = bufNr < 0;
            if (isError)
                throwLastFgError();

            drawBuffer((int)bufNr);

            // Unblock the buffer
            Fg_setStatusEx(fgHandle,FG_UNBLOCK,bufNr,dmaPort,dmaHandle);
        }

        /*Stop framegrabber */
        if(Fg_stopAcquireEx(fgHandle,dmaPort,dmaHandle,0) != FG_OK)
            throwLastFgError();

        /*Stop the camera acquisition*/
        Sgc_stopAcquisition(cameraHandle, 0);
    }

};