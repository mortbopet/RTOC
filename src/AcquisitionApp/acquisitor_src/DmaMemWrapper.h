#pragma once

#include <memory>
#include <stdint.h>

#include <fgrab_prototyp.h>

#include "FgWrapper.h"
class DmaMemWrapper{
public:
    DmaMemWrapper();
    ~DmaMemWrapper();
    static std::shared_ptr<DmaMemWrapper> create(std::weak_ptr<FgWrapper> fg, int32_t dmaPort, bool initialize = true);
    void setFgHandle(std::weak_ptr<FgWrapper> handle);
    dma_mem* getMemHandle();
    dma_mem* initialize(size_t bufferCount, int32_t dmaPort);
private:
    int32_t mDmaPort;
    dma_mem* mMemory;
    std::weak_ptr<FgWrapper> mFgHandle;
};
