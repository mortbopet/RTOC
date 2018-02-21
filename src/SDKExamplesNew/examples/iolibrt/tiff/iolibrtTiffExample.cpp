/*This example shows how to save image in tiff format.*/
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <vector>

#include "FgAcqBlockExample.cpp"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <sisoIo.h>
#include <sisoIoTiffErrorCodes.h>

class iolibrtTiffExample: public FgAcqBlockExample{
public:
    iolibrtTiffExample(){}

    int32_t run(std::string filePath){
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

            acquireAndSaveImage(
                mFgHandle,
                dmaPort,
                mDmaHandle->getMemHandle(),
                filePath);

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
    virtual void acquireAndSaveImage(std::shared_ptr<FgWrapper> fg, 
        uint32_t dmaPort, 
        dma_mem* dmaHandle,
        std::string filename){

            frameindex_t bufNr = 0;
            bool isError = false;
            const uint32_t waitDurationInMs = 10;
            frameindex_t MaxPics = 1;

            if(Fg_AcquireEx(fg->getFgHandle(),dmaPort,MaxPics,ACQ_BLOCK,dmaHandle) != FG_OK)
                throwLastFgError();

            bufNr = Fg_getImageEx(fg->getFgHandle(),
                SEL_ACT_IMAGE,
                0,
                dmaPort,
                waitDurationInMs,
                dmaHandle);

            isError = bufNr < 0;
            if (isError)
                throwLastFgError();

            const void* imgPtr = Fg_getImagePtrEx(
                fg->getFgHandle(),
                bufNr,
                dmaPort,
                dmaHandle);


            int ioStatus = IoWriteTiff(filename.c_str(), 
                (unsigned char*)(imgPtr),
                fg->getWidth(dmaPort), 
                fg->getHeight(dmaPort), 
                fg->getBitsPerSample(dmaPort),
                fg->getSamplePerPixel(dmaPort));
            if(ioStatus != ERR_TIF_NO_ERROR)
                throw std::runtime_error("cannot save tiff file");

            if(Fg_stopAcquireEx(fg->getFgHandle(),dmaPort,dmaHandle,0) != FG_OK)
                throwLastFgError();
    }
};