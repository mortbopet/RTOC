
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <vector>

#include "FgDmaChannelExample.cpp"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>


class FgAcqBlockExample: public FgDmaChannelExample{
public:
    FgAcqBlockExample(){}

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
            int32_t dmaPort = this->selectDmaPortDialog(mFgHandle);
            mDmaHandle = DmaMemWrapper::create(mFgHandle, dmaPort);

            mDisplay= DisplayWrapper::create(
                std::weak_ptr<DmaMemWrapper>(mDmaHandle), 
                dmaPort,
                std::weak_ptr<FgWrapper>(mFgHandle));

            acquisitionLoop(
                mFgHandle->getFgHandle(),
                dmaPort,
                mDmaHandle->getMemHandle());
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

    virtual void acquisitionLoop(Fg_Struct* fgHandle, 
        uint32_t dmaPort, 
        dma_mem* dmaHandle,
        bool printAllowed = true, 
        uint32_t MaxPics = 5000){

            frameindex_t bufNr = 0;
            bool isError = false;
            const uint32_t waitDurationInMs = 10;

            if(Fg_AcquireEx(fgHandle,dmaPort,MaxPics,ACQ_BLOCK,dmaHandle) != FG_OK)
                throwLastFgError();

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

                // Read the number of blocked images
                frameindex_t nbi = Fg_getStatusEx(fgHandle, NUMBER_OF_BLOCKED_IMAGES, 0, dmaPort, dmaHandle);
                if(printAllowed)
                    PRINT_INFO(mLog,"Number of blocked images: " << nbi << std::endl);

                // Unblock the buffer
                Fg_setStatusEx(fgHandle,FG_UNBLOCK,bufNr,dmaPort,dmaHandle);
            }

            if(Fg_stopAcquireEx(fgHandle,dmaPort,dmaHandle,0) != FG_OK)
                throwLastFgError();
    }


};
