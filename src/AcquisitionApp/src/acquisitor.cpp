#include "acquisitor.h"

#include "../SiliconSoftwareSrc/acquisitionhelpers.cpp"

Acquisitor::Acquisitor() {}

int32_t Acquisitor::run(char* path) {
    try {
        // initializes internal structures of the library.
        int32_t status = Fg_InitLibraries(nullptr);
        if (status != FG_OK)
            throw std::runtime_error("Cannot initialize Fg libraries.");

        /*Initialize framegrabber struct with default applet. Assume board is at index 0 (single
         * board in computer*/
        mFgHandle = FgWrapper::create("Acq_SingleCXP6x4AreaGray8.dll", 0);
        if (mFgHandle->getFgHandle() == NULL) {
            fprintf(stderr, "Error in Fg_Init(): %s\n", Fg_getLastErrorDescription(NULL));
            fprintf(stderr, "Press any key to continue...\n");
            _getch();
            return -1;
        }

        /*FgDmaChannelExample shows dma initialization.*/
        int32_t dmaPort = 0;
        mDmaHandle = DmaMemWrapper::create(mFgHandle, dmaPort);
        mDisplay = DisplayWrapper::create(std::weak_ptr<DmaMemWrapper>(mDmaHandle), dmaPort,
                                          std::weak_ptr<FgWrapper>(mFgHandle));
        SgcBoardHandle* board = initialiseBoard(mFgHandle->getFgHandle());
        std::vector<std::pair<std::string, SgcCameraHandle*>> cameras = discoverCameras(board);

        mCamera = selectCamera(board);

        acquisitionSgc(mFgHandle->getFgHandle(), dmaPort, mDmaHandle->getMemHandle(), mCamera);

        Sgc_freeBoard(board);

        // releases internal structures of the library
        Fg_FreeLibraries();
    } catch (std::exception& e) {
        // releases internal structures of the library
        Fg_FreeLibraries();
        std::cout << "Example failed: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

void Acquisitor::acquisitionSgc(Fg_Struct* fgHandle, uint32_t dmaPort, dma_mem* dmaHandle,
                                SgcCameraHandle* cameraHandle) {
    uint32_t MaxPics = 5000;

    frameindex_t bufNr = 0;
    bool isError = false;
    const uint32_t waitDurationInMs = 10;

    if (Fg_AcquireEx(fgHandle, dmaPort, MaxPics, ACQ_BLOCK, dmaHandle) != FG_OK)
        throwLastFgError();

    /*Start the camera acquisition*/
    Sgc_startAcquisition(cameraHandle, 0);

    while (!isError) {
        bufNr = Fg_getImageEx(fgHandle, SEL_ACT_IMAGE, 0, dmaPort, waitDurationInMs, dmaHandle);

        isError = bufNr < 0;
        if (isError)
            throwLastFgError();

        drawBuffer((int)bufNr);

        // Unblock the buffer
        Fg_setStatusEx(fgHandle, FG_UNBLOCK, bufNr, dmaPort, dmaHandle);
    }

    /*Stop framegrabber */
    if (Fg_stopAcquireEx(fgHandle, dmaPort, dmaHandle, 0) != FG_OK)
        throwLastFgError();

    /*Stop the camera acquisition*/
    Sgc_stopAcquisition(cameraHandle, 0);
}

void Acquisitor::throwLastFgError() {
    throwLastFgError(mFgHandle->getFgHandle());
}

void Acquisitor::throwLastFgError(Fg_Struct* fgHandle) {
    std::stringstream ss;
    ss << "Error (" << Fg_getLastErrorNumber(fgHandle) << ") "
       << Fg_getLastErrorDescription(fgHandle) << std::endl;
    throw std::runtime_error(ss.str());
}
