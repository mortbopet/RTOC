#pragma once
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <time.h>
#include <iostream>
#include <memory>
#include <sstream>
#include <vector>

#include "DmaMemWrapper.h"
#include "FgWrapper.h"
#include "LoggingHelper.cpp"

class FgExample {
public:
    FgExample() { Fg_InitLibraries(nullptr); }

    ~FgExample() { Fg_FreeLibraries(); }

protected:
    std::shared_ptr<FgWrapper> mFgHandle;
    std::shared_ptr<DmaMemWrapper> mDmaHandle;
    std::shared_ptr<LoggingHelper> mLog;

    std::shared_ptr<DmaMemWrapper> setupDma(uint32_t dmaIndex, uint32_t bufferCount);

    int selectBoardDialog();

    void drawBuffer(int32_t bufferIndex);

    /* Fg Parametrization Functions */
    void throwLastFgError();
    static void throwLastFgError(Fg_Struct* fgHandle);

    uint32_t getDmaNumber();
    static uint32_t getDmaNumber(Fg_Struct* fgHandle);

private:
    std::vector<std::string> getDllFrom(std::string path);
};
