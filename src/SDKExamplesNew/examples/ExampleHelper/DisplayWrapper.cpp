#include "DisplayWrapper.h"

std::shared_ptr<DisplayWrapper> DisplayWrapper::create(
        std::weak_ptr<DmaMemWrapper> dmaHandle,
        int32_t dmaPort,
        std::weak_ptr<FgWrapper> fgHandle){

    std::shared_ptr<DisplayWrapper> display = std::make_shared<DisplayWrapper>();
    display->setWidth(fgHandle.lock()->getWidth(dmaPort));
    display->setHeight(fgHandle.lock()->getHeight(dmaPort));
    display->setPixelBitCount(
        static_cast<int32_t>(fgHandle.lock()->getBitsPerPixel(dmaPort)));
    display->setDmaMemHandle(dmaHandle);
    display->setFgHandle(fgHandle);
    display->initialize();
    return display;
}

std::shared_ptr<DisplayWrapper> DisplayWrapper::create(uint32_t width, uint32_t height, uint32_t bits){
    std::shared_ptr<DisplayWrapper> display = std::make_shared<DisplayWrapper>();
    display->setWidth(width);
    display->setHeight(height);
    display->setPixelBitCount(bits);
    display->initialize();
    return display;
}

DisplayWrapper::DisplayWrapper():
mWidth(0),
mHeight(0),
mPixelBitCount(0),
mDmaPort(-1){
}

DisplayWrapper::~DisplayWrapper(){
    ::CloseDisplay(mDisplayId);
}

void DisplayWrapper::initialize(){
    mDisplayId = ::CreateDisplay(mPixelBitCount, mWidth, mHeight);
    SetBufferWidth(mDisplayId,mWidth,mHeight);
}

void DisplayWrapper::drawBuffer(int32_t bufferIndex){
    const void* imgPtr = Fg_getImagePtrEx(
        mFgHandle.lock()->getFgHandle(),
        bufferIndex,
        mDmaPort,
        mDmaHandle.lock()->getMemHandle());
    drawBuffer(imgPtr, bufferIndex);
}

void DisplayWrapper::drawBuffer(const void* imgPtr, int32_t bufferIndex){
   ::DrawBuffer(mDisplayId,
        imgPtr,
        (int)bufferIndex,"");
}

void DisplayWrapper::setWidth(uint32_t width){
    mWidth = width;
}

uint32_t DisplayWrapper::getWidth(){
    return mWidth;
}

void DisplayWrapper::setHeight(uint32_t height){
    mHeight =height;
}

uint32_t DisplayWrapper::getHeight(){
    return mHeight;
}

void DisplayWrapper::setPixelBitCount(uint32_t pixelBitCount){
    mPixelBitCount = pixelBitCount;
}

uint32_t DisplayWrapper::getPixelBitCount(){
    return mPixelBitCount;
}

void DisplayWrapper::setDmaMemHandle(std::weak_ptr<DmaMemWrapper> handle){
    mDmaHandle = handle;
}

void DisplayWrapper::setFgHandle(std::weak_ptr<FgWrapper> handle){
    mFgHandle =handle;
}

int32_t DisplayWrapper::getDisplayId(){
    return mDisplayId;
}
