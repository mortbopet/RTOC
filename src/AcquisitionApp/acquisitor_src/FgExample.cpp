#include "FgExample.h"
#include <iostream>

#define FG_CHECK(func, fg)                                                                     \
    if (func != FG_OK) {                                                                       \
        std::stringstream ss;                                                                  \
        ss << "Error (" << Fg_getLastErrorNumber(fg) << ") " << Fg_getLastErrorDescription(fg) \
           << std::endl;                                                                       \
        throw std::invalid_argument(ss.str());                                                 \
    }

std::shared_ptr<DmaMemWrapper> FgExample::setupDma(uint32_t dmaIndex, uint32_t bufferCount) {
    mDmaHandle = DmaMemWrapper::create(mFgHandle, dmaIndex);
    mDmaHandle->initialize(10, dmaIndex);
    return mDmaHandle;
}

void FgExample::throwLastFgError() {
    throwLastFgError(mFgHandle->getFgHandle());
}

void FgExample::throwLastFgError(Fg_Struct* fgHandle) {
    std::stringstream ss;
    ss << "Error (" << Fg_getLastErrorNumber(fgHandle) << ") "
       << Fg_getLastErrorDescription(fgHandle) << std::endl;
    throw std::runtime_error(ss.str());
}

int32_t FgExample::selectBoardDialog() {
    int32_t boardType = 0;
    int32_t nrOfBoardsPresent = FgWrapper::getNrOfBoards();

    std::vector<std::string> boardNames;
    for (int32_t i = 0; i < nrOfBoardsPresent; i++) {
        try {
            boardType = Fg_getBoardType(i);
            if (boardType < 0)
                continue;
            const char* name = Fg_getBoardNameByType(boardType, true);
            if (name == nullptr)
                continue;
            std::string boardName(name);
            boardNames.push_back(boardName);
        } catch (...) {
        }
    }

    return mLog->selectDialog(boardNames, "board");
}

uint32_t FgExample::getDmaNumber() {
    return getDmaNumber(mFgHandle->getFgHandle());
}

uint32_t FgExample::getDmaNumber(Fg_Struct* fgHandle) {
    int32_t dataSize = 1024;
    std::unique_ptr<char[]> dataCharArr(new char[dataSize]);
    int32_t status = Fg_getParameterProperty(fgHandle, FG_NR_OF_DMAS, PROP_ID_VALUE,
                                             dataCharArr.get(), &dataSize);
    if (status != FG_OK)
        throwLastFgError(fgHandle);

    return (uint32_t)atoi(dataCharArr.get());
}
