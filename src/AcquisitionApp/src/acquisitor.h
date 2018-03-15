#pragma once

/*This example demonstrates the acquisition with siso_genicam.*/
#include <stdio.h>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <unistd.h>
#endif

#include <SisoDisplay.h>
#include <fgrab_define.h>
#include <fgrab_prototyp.h>
#include <fgrab_struct.h>
#include <siso_genicam.h>
#include <stdexcept>

#include "../SiliconSoftwareSrc/DisplayWrapper.h"
#include "../SiliconSoftwareSrc/DmaMemWrapper.h"

class Acquisitor {
public:
    Acquisitor();

    int32_t run(char* path);

    void drawBuffer(int32_t bufferIndex) { mDisplay->drawBuffer(bufferIndex); }

protected:
    void acquisitionSgc(Fg_Struct* fgHandle, uint32_t dmaPort, dma_mem* dmaHandle,
                        SgcCameraHandle* cameraHandle);

private:
    void throwLastFgError(Fg_Struct* fgHandle);
    void throwLastFgError();

    std::shared_ptr<FgWrapper> mFgHandle;
    std::shared_ptr<DmaMemWrapper> mDmaHandle;
    std::shared_ptr<DisplayWrapper> mDisplay;
    SgcCameraHandle* mCamera;
};
