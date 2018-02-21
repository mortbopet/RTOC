#include "DmaMemWrapper.h"

DmaMemWrapper::DmaMemWrapper():
mDmaPort(-1){
}

DmaMemWrapper::~DmaMemWrapper(){
    if(mMemory 
        && mFgHandle.lock()){
            Fg_FreeMemEx(mFgHandle.lock()->getFgHandle(), mMemory);
    }
}

std::shared_ptr<DmaMemWrapper> DmaMemWrapper::create(std::weak_ptr<FgWrapper> fg, int32_t dmaPort, bool initialize){
    std::shared_ptr<DmaMemWrapper> dma = std::make_shared<DmaMemWrapper>();
    dma->setFgHandle(fg);
    if(initialize)
        dma->initialize(10, dmaPort );
    return dma;
}


void DmaMemWrapper::setFgHandle(std::weak_ptr<FgWrapper> handle){
    mFgHandle = handle;
}

dma_mem* DmaMemWrapper::getMemHandle(){
    return mMemory;
}

dma_mem* DmaMemWrapper::initialize(size_t bufferCount, int32_t dmaPort){
    Fg_Struct* fg = mFgHandle.lock()->getFgHandle();
    size_t totalBufSize = mFgHandle.lock()->getPayloadSize(dmaPort);
    mMemory = Fg_AllocMemEx(fg,
        totalBufSize*bufferCount,
        bufferCount);
    if(!mMemory)
        mFgHandle.lock()->throwLastError();
    return mMemory;
}
