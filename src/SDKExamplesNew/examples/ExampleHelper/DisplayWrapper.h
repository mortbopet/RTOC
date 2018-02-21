#pragma once

#include <stdint.h>

#include <fgrab_prototyp.h>
#include "DmaMemWrapper.h"
#include "FgWrapper.h"


class DisplayWrapper{
public:
    static std::shared_ptr<DisplayWrapper> create(
        std::weak_ptr<DmaMemWrapper> dmaHandle,
        int32_t dmaPort,
        std::weak_ptr<FgWrapper> fgHandle);
    static std::shared_ptr<DisplayWrapper> create(uint32_t width, uint32_t height, uint32_t bits);

    DisplayWrapper();
    ~DisplayWrapper();
    void initialize();

    void drawBuffer(int32_t bufferIndex);
    void drawBuffer(const void* imgPtr, int32_t bufferIndex);
    
    int32_t getDisplayId();
    void setWidth(uint32_t);
    uint32_t getWidth();
    void setHeight(uint32_t);
    uint32_t getHeight();
    void setPixelBitCount(uint32_t);
    uint32_t getPixelBitCount();
    void setDmaMemHandle(std::weak_ptr<DmaMemWrapper>);
    void setFgHandle(std::weak_ptr<FgWrapper>);
private:
    int32_t mDmaPort;
    std::weak_ptr<DmaMemWrapper> mDmaHandle;
    std::weak_ptr<FgWrapper> mFgHandle;
    int32_t mDisplayId;
    uint32_t mWidth;
    uint32_t mHeight;
    uint32_t mPixelBitCount;
};
