#pragma once
#include <stdio.h>
#ifdef _WIN32
#include <windows.h>
#else
#include <unistd.h>
#endif
#include "FgDmaChannelExample.cpp"

class ApcListener{
public:
    virtual int32_t onImage(frameindex_t picNr) = 0;
    virtual int32_t onError(frameindex_t error) = 0;
};

class FgAcqAPCBlockExample : public FgDmaChannelExample, public ApcListener{
public:
    int32_t run();
protected:

    void acquisitionLoop(
        Fg_Struct* fgHandle, 
        uint32_t dmaIndex, 
        dma_mem* dmaHandle, 
        int32_t displayId);

    virtual int32_t onImage(frameindex_t picNr);
    virtual int32_t onError(frameindex_t error);
    int32_t mDmaPort;

};
