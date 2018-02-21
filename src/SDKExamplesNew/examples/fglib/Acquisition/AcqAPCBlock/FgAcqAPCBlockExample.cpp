#include "FgAcqAPCBlockExample.h"


struct fg_apc_data {
    Fg_Struct *fg;
    uint32_t port;
    dma_mem *mem;
    ApcListener* listener;
};

int ApcCallback(frameindex_t picNr, struct fg_apc_data *data)
{
    if(picNr < 0 ) //negative picNr indicates Error
        return data->listener->onError(picNr);
    else
        return data->listener->onImage(picNr);

    //return value != 0 unregisters the apc handle
    return 0;
}

int32_t FgAcqAPCBlockExample::run(){
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
        mDmaPort = this->selectDmaPortDialog(mFgHandle);
        mDmaHandle = DmaMemWrapper::create(std::weak_ptr<FgWrapper>(mFgHandle),mDmaPort);
        mDisplay = DisplayWrapper::create(mDmaHandle,mDmaPort,mFgHandle);

        acquisitionLoop(
            mFgHandle->getFgHandle(),
            mDmaPort, 
            mDmaHandle->getMemHandle(),
            mDisplay->getDisplayId());

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


void FgAcqAPCBlockExample::acquisitionLoop(Fg_Struct* fgHandle, uint32_t dmaPort, dma_mem* dmaHandle, int32_t displayId){
    struct fg_apc_data apcdata;
    apcdata.port = dmaPort;
    apcdata.fg = fgHandle;
    apcdata.mem = dmaHandle;
    apcdata.listener = this;

    struct FgApcControl ctrl;
    ctrl.version = 0;
    ctrl.data = &apcdata;
    ctrl.func = ApcCallback;
    ctrl.flags = FG_APC_DEFAULTS | FG_APC_DELIVER_ERRORS;
    ctrl.timeout = 5;

    if (Fg_registerApcHandler(fgHandle, dmaPort, &ctrl, FG_APC_CONTROL_BASIC) != FG_OK)
        throwLastFgError();

    if (Fg_AcquireEx(fgHandle, dmaPort, GRAB_INFINITE, ACQ_BLOCK, dmaHandle) != FG_OK)
        throwLastFgError();

#ifdef _WIN32
    Sleep(30000);
#else
    sleep(30);
#endif

    Fg_registerApcHandler(fgHandle, dmaPort, nullptr, FG_APC_CONTROL_BASIC);
    Fg_stopAcquireEx(fgHandle,dmaPort,dmaHandle,0);
}

/*callback for the arrival of a new image*/
int32_t FgAcqAPCBlockExample::onImage(frameindex_t picNr){
    PRINT_INFO(mLog,"Image " << picNr << "received.");

    uint32_t timeoutInMilliseconds = 200;
    frameindex_t bufferImageNumber = Fg_getImageEx(
        mFgHandle->getFgHandle(), 
        SEL_NEXT_IMAGE, 
        picNr, 
        mDmaPort, 
        timeoutInMilliseconds, 
        mDmaHandle->getMemHandle());

    mDisplay->drawBuffer(static_cast<int32_t>(bufferImageNumber));
    int ret = Fg_setStatusEx(mFgHandle->getFgHandle(), 
        FG_UNBLOCK, 
        bufferImageNumber, 
        mDmaPort, 
        mDmaHandle->getMemHandle());

    return 0;
}

/*callback for the arrival of an error*/
int32_t FgAcqAPCBlockExample::onError(frameindex_t error){
    return 0;
}