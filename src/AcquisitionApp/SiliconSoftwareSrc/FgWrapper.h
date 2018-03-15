#pragma once
#include <memory>
#include <stdint.h>
#include <vector>

#include <fgrab_prototyp.h>


class FgWrapper{
public:
    FgWrapper();
    ~FgWrapper();
    static std::shared_ptr<FgWrapper> create(Fg_Struct* fgHandle);
    static std::shared_ptr<FgWrapper> create(std::string appletPath, uint32_t boardIndex);

    void throwLastError();

    static int32_t getNrOfBoards();

    /* Fg Parametrization Functions */
    size_t getBitsPerPixel(int32_t dmaPort);


    uint32_t getWidth(uint32_t dmaPort);
    void setWidth(uint32_t val, uint32_t dmaPort);
    static uint32_t getWidth(Fg_Struct* fgHandle, uint32_t dmaPort);
    static void setWidth(Fg_Struct* fgHandle, uint32_t val, uint32_t dmaPort);

    uint32_t getHeight(uint32_t dmaPort);
    void setHeight(uint32_t val, uint32_t dmaPort);
    static uint32_t getHeight(Fg_Struct* fgHandle, uint32_t dmaPort);
    static void setHeight(Fg_Struct* fgHandle, uint32_t val, uint32_t dmaPort);

    uint32_t getBitalignment(uint32_t dmaPort);
    void setBitalignment(uint32_t val, uint32_t dmaPort);
    static uint32_t getBitalignment(Fg_Struct* fgHandle, uint32_t dmaPort);
    static void setBitalignment(Fg_Struct* fgHandle, uint32_t val, uint32_t dmaPort);

    //FG_FORMAT
    int32_t getFormat(int32_t dmaPort);
    static int32_t getFormat(Fg_Struct* fgHandle, int32_t dmaPort);
    uint32_t getBitsPerSample(int32_t dmaPort);
    uint32_t getSamplePerPixel(int32_t dmaPort);

    size_t getPayloadSize(int32_t dmaPort);

    void IsCameraSimulatorActive(bool isActive, uint32_t dmaPort);
    static void IsCameraSimulatorActive(Fg_Struct* fgHandle, bool isActive, uint32_t dmaPort);

    Fg_Struct* getFgHandle();
    void setFgHandle(Fg_Struct*);

    Fg_Struct* FgInit(std::string applet, uint32_t boardIndex);

private:


    Fg_Struct* mFgHandle;

};