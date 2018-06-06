#include "FgWrapper.h"
#include <sstream>
#include "fg_pixel.h"

/**
   @note All thrown exceptions are caught in the Acquisitor initialization stage, and handled
   through writing the error message to the log
*/

#define FG_CHECK(func, fg)                                                                     \
    if (func != FG_OK) {                                                                       \
        std::stringstream ss;                                                                  \
        ss << "Error (" << Fg_getLastErrorNumber(fg) << ") " << Fg_getLastErrorDescription(fg) \
           << std::endl;                                                                       \
        throw std::invalid_argument(ss.str());                                                 \
    }

FgWrapper::FgWrapper() : mFgHandle(nullptr) {}

FgWrapper::~FgWrapper() {
    if (mFgHandle)
        Fg_FreeGrabber(mFgHandle);
}

std::shared_ptr<FgWrapper> FgWrapper::create(Fg_Struct* fg) {
    std::shared_ptr<FgWrapper> fgHandle = std::make_shared<FgWrapper>();
    fgHandle->setFgHandle(fg);
    return fgHandle;
}

std::shared_ptr<FgWrapper> FgWrapper::create(std::string appletPath, uint32_t boardIndex) {
    std::shared_ptr<FgWrapper> fgHandle = std::make_shared<FgWrapper>();
    fgHandle->FgInit(appletPath, boardIndex);
    return fgHandle;
}

size_t FgWrapper::getPayloadSize(int32_t dmaPort) {
    uint32_t width = getWidth(dmaPort);
    uint32_t height = getHeight(dmaPort);
    int32_t format = getFormat(dmaPort);
    return width * height * bytesPerPixel(format);
}

Fg_Struct* FgWrapper::getFgHandle() {
    return mFgHandle;
}

void FgWrapper::setFgHandle(Fg_Struct* fgHandle) {
    mFgHandle = fgHandle;
}

Fg_Struct* FgWrapper::FgInit(std::string applet, uint32_t boardIndex) {
    mFgHandle = Fg_Init(applet.c_str(), boardIndex);
    if (!mFgHandle)
        throw std::runtime_error(
            "Cannot initialize frame grabber.\nError in external dependencies.\nAre the "
            "framegrabber drivers installed or currently busy?");
    return mFgHandle;
}

void FgWrapper::throwLastError() {
    std::stringstream ss;
    ss << "Error (" << Fg_getLastErrorNumber(mFgHandle) << ") "
       << Fg_getLastErrorDescription(mFgHandle) << std::endl;
    throw std::runtime_error(ss.str());
}

int32_t FgWrapper::getNrOfBoards() {
    int nrOfBoards = 0;
    char buffer[256];
    unsigned int buflen = 256;
    buffer[0] = 0;

    // availability : starting with RT 5.2
    if (Fg_getSystemInformation(NULL, INFO_NR_OF_BOARDS, PROP_ID_VALUE, 0, buffer, &buflen) ==
        FG_OK) {
        nrOfBoards = atoi(buffer);
    }
    return nrOfBoards;
}

size_t FgWrapper::getBitsPerPixel(int32_t dmaPort) {
    return bitsPerPixel(getFormat(dmaPort));
}

uint32_t FgWrapper::getWidth(uint32_t dmaPort) {
    return getWidth(mFgHandle, dmaPort);
}

void FgWrapper::setWidth(uint32_t val, uint32_t dmaPort) {
    setWidth(mFgHandle, val, dmaPort);
}

uint32_t FgWrapper::getWidth(Fg_Struct* fgHandle, uint32_t dmaPort) {
    uint32_t result = 0;
    FG_CHECK(Fg_getParameter(fgHandle, FG_WIDTH, &result, dmaPort), fgHandle);
    return result;
}

void FgWrapper::setWidth(Fg_Struct* fgHandle, uint32_t val, uint32_t dmaPort) {
    FG_CHECK(Fg_setParameter(fgHandle, FG_WIDTH, &val, dmaPort), fgHandle);
}

uint32_t FgWrapper::getHeight(uint32_t dmaPort) {
    return getHeight(mFgHandle, dmaPort);
}

void FgWrapper::setHeight(uint32_t val, uint32_t dmaPort) {
    setHeight(mFgHandle, val, dmaPort);
}

uint32_t FgWrapper::getHeight(Fg_Struct* fgHandle, uint32_t dmaPort) {
    uint32_t result = 0;
    FG_CHECK(Fg_getParameter(fgHandle, FG_HEIGHT, &result, dmaPort), fgHandle);
    return result;
}

void FgWrapper::setHeight(Fg_Struct* fgHandle, uint32_t val, uint32_t dmaPort) {
    FG_CHECK(Fg_setParameter(fgHandle, FG_HEIGHT, &val, dmaPort), fgHandle);
}

uint32_t FgWrapper::getBitalignment(uint32_t dmaPort) {
    return getBitalignment(mFgHandle, dmaPort);
}

void FgWrapper::setBitalignment(uint32_t val, uint32_t dmaPort) {
    setBitalignment(mFgHandle, val, dmaPort);
}

uint32_t FgWrapper::getBitalignment(Fg_Struct* fgHandle, uint32_t dmaPort) {
    uint32_t result = 0;
    FG_CHECK(Fg_getParameter(fgHandle, FG_BITALIGNMENT, &result, dmaPort), fgHandle);
    return result;
}

void FgWrapper::setBitalignment(Fg_Struct* fgHandle, uint32_t val, uint32_t dmaPort) {
    FG_CHECK(Fg_setParameter(fgHandle, FG_BITALIGNMENT, &val, dmaPort), fgHandle);
}

void FgWrapper::IsCameraSimulatorActive(bool isActive, uint32_t dmaPort) {
    IsCameraSimulatorActive(mFgHandle, isActive, dmaPort);
}

void FgWrapper::IsCameraSimulatorActive(Fg_Struct* fgHandle, bool isActive, uint32_t dmaPort) {
    uint32_t val = 0;
    if (isActive)
        val = 1;
    FG_CHECK(Fg_setParameter(fgHandle, FG_CAMERASIMULATOR_ENABLE, &val, dmaPort), fgHandle);
}

int32_t FgWrapper::getFormat(int32_t dmaPort) {
    return getFormat(mFgHandle, dmaPort);
}

int32_t FgWrapper::getFormat(Fg_Struct* fgHandle, int32_t dmaPort) {
    int32_t result = 0;
    FG_CHECK(Fg_getParameter(fgHandle, FG_FORMAT, &result, dmaPort), fgHandle);
    return result;
}

uint32_t FgWrapper::getBitsPerSample(int32_t dmaPort) {
    int32_t format = getFormat(dmaPort);
    switch (format) {
        case FG_GRAY:
            return 8;
        case FG_GRAY16:
            return 16;
        case FG_COL24:
            return 8;
        case FG_COL48:
            return 16;
        default:
            throw std::invalid_argument("Pixel type is not known");
    };
}

uint32_t FgWrapper::getBytesPerSample(int32_t dmaPort) {
    int32_t format = getFormat(dmaPort);
    switch (format) {
        case FG_GRAY:
            return 1;
        case FG_GRAY16:
            return 2;
        case FG_COL24:
            return 1;
        case FG_COL48:
            return 2;
        default:
            throw std::invalid_argument("Pixel type is not known");
    };
}

uint32_t FgWrapper::getSamplePerPixel(int32_t dmaPort) {
    int32_t format = getFormat(dmaPort);
    switch (format) {
        case FG_GRAY:
        case FG_GRAY16:
            return 1;
        case FG_COL24:
        case FG_COL48:
            return 3;
        default:
            throw std::invalid_argument("Pixel type is not known");
    };
}
