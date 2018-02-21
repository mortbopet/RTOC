#include <stdio.h>
#include <time.h>

#include "FgDmaChannelExample.cpp"


class FgAcqStandardExample : public FgDmaChannelExample{
public:
    int32_t run(){
        try{
            //initializes internal structures of the library.
            int32_t status = Fg_InitLibraries(nullptr);
            if(status != FG_OK)
                throw std::runtime_error("Cannot initialize Fg libraries.");

            /*FgAppletIteratorExample shows the initialization procedure.*/
            uint32_t boardIndex = selectBoardDialog();
            std::string appletPath = selectAppletDialog(boardIndex);
            mFgHandle = FgWrapper::create(appletPath, boardIndex);

            /*FgDmaChannelExample shows dma initialization.*/
            int32_t dmaPort = this->selectDmaPortDialog(mFgHandle);
            mDmaHandle = DmaMemWrapper::create(std::weak_ptr<FgWrapper>(mFgHandle),dmaPort);

            mDisplay = DisplayWrapper::create(mDmaHandle,dmaPort,mFgHandle);

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
    //This function acquires images using the standard strategy
    void acquisitionLoop(Fg_Struct* fgHandle, uint32_t dmaIndex, dma_mem* dmaHandle, frameindex_t MaxPics = 10){

        //acquisition start
        if(Fg_AcquireEx(fgHandle,dmaIndex,GRAB_INFINITE,ACQ_STANDARD,dmaHandle) != FG_OK)
            throwLastFgError();

        frameindex_t lastPicNr = 0;
        while(lastPicNr < MaxPics) {
            lastPicNr = Fg_getLastPicNumberBlockingEx(fgHandle,lastPicNr+1,dmaIndex,10,dmaHandle);

            //a negative index indicates an error
            if(lastPicNr <0)
                throwLastFgError();

            drawBuffer(static_cast<int32_t>(lastPicNr));
        }

        //acquisition stop
        Fg_stopAcquireEx(fgHandle,dmaIndex,dmaHandle,0);
    }
};
