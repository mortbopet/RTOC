
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <vector>

#include "FgAppletIteratorExample.cpp"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>


class FgDmaChannelExample: public FgAppletIteratorExample{
public:
    FgDmaChannelExample(){}

    int32_t run(){
        //initializes internal structures of the library.
        int32_t status = Fg_InitLibraries(nullptr);
        if(status != FG_OK)
            throw std::runtime_error("Cannot initialize Fg libraries.");

        Fg_Struct* fgMasterHandle = nullptr;
        std::vector<std::shared_ptr<FgWrapper>> fgSlaveHandles;
        try{
            /*FgAppletIteratorExample shows how to load an applet.*/
            uint32_t boardIndex = selectBoardDialog();
            std::string appletPath = selectAppletDialog(boardIndex);
            mFgHandle = FgWrapper::create(appletPath, boardIndex);

            //count dma channels for every process
            uint32_t processNumber = getProcessNumber(mFgHandle->getFgHandle());
            uint32_t dmaNumber = 0;
            for(uint32_t i = 0; i < processNumber; ++i){
                uint32_t processDmaNumber = getDmaNumberForProcessIndex(mFgHandle->getFgHandle(), i);
                dmaNumber += processDmaNumber;
            }
            PRINT_INFO(mLog, "The applet has " << processNumber << 
                " processes with "<< dmaNumber <<" dma channels.");

            //allocate dma memory
            int32_t dmaPort = 0;
            uint32_t bufferCount = 3; //should not be less than 3
            size_t bufferPlayloadSize = mFgHandle->getPayloadSize(dmaPort);
            dma_mem* memory = Fg_AllocMemEx(mFgHandle->getFgHandle(),
                bufferPlayloadSize*bufferCount,
                bufferCount);
            Fg_FreeMemEx(mFgHandle->getFgHandle(), memory);

        //releases internal structures of the library
        Fg_FreeLibraries();
    }catch(std::exception& e){
        //releases internal structures of the library
        Fg_FreeLibraries();
        std::cout << "Example failed: " << e.what() << std::endl;
        return -1;
    }
    return 0;
    }

protected:
    //This function reads and returns the number of processes for the loaded applet.
    static uint32_t getProcessNumber(Fg_Struct* fgHandle){
        int32_t nrOfProcesses = -1;
        int32_t dmaIndex = 0; // no index needed
        int32_t status = Fg_getParameter(fgHandle, FG_NR_OF_PROCESSES, &nrOfProcesses, dmaIndex);
        if(status != FG_OK)
            throwLastFgError(fgHandle);
        return static_cast<uint32_t>(nrOfProcesses);
    }

    //This function reads the number of dma channels for a specific process index.
    static uint32_t getDmaNumberForProcessIndex(Fg_Struct* fgHandle, uint32_t processIndex){
        int32_t nrOfDmas = 0;
        int32_t status = Fg_getParameter(fgHandle, FG_NR_OF_DMAS, &nrOfDmas, processIndex);
        /*if(status != FG_OK)
        throwLastFgError(fgHandle);*/
        return static_cast<uint32_t>(nrOfDmas);
    }

    //To address a specific dma channel you have to use its dma port.
    static int32_t getDMAPort(Fg_Struct* fgHandle, int32_t processIndex, int32_t dmaIndex){
        int32_t dmaPort = dmaIndex; //This parameter is also used as input
        int32_t status = Fg_getParameter(fgHandle, FG_DMA_PORT, &dmaPort, processIndex);
        if(status != FG_OK)
            throwLastFgError(fgHandle);
        return dmaPort;
    }

    //This function prints the avaible dma port. And waits for an input from the user
    //which is bigger than zero and smaller than the size of the number of dma ports.
    int32_t selectDmaPortDialog(std::shared_ptr<FgWrapper> fg){
        return selectDmaPortDialog(fg->getFgHandle());
    }
    int32_t selectDmaPortDialog(Fg_Struct* fg){
        std::vector<int32_t> dmaPorts = createDmaPorts(fg);
        std::vector<std::string> dmaPortNames;
        for(size_t i = 0; i < dmaPorts.size(); i++){
            dmaPortNames.push_back(std::string("Dma Port ") + std::to_string(static_cast<uint64_t>(dmaPorts.at(i))));
        }
        return mLog->selectDialog<std::string>(dmaPortNames, "dma port");
    }

    //This is a helper function for further examples. It creates a vector with all dma ports.
    static std::vector<int32_t> createDmaPorts(Fg_Struct* fg){
        std::vector<int32_t> dmaPorts;
        uint32_t processNumber = getProcessNumber(fg);
        for(uint32_t procIndex = 0; procIndex < processNumber; ++procIndex){
            uint32_t processDmaNumber = getDmaNumberForProcessIndex(fg, procIndex);
            for(uint32_t dmaIndex = 0; dmaIndex < processDmaNumber; ++dmaIndex){
                int32_t dmaPort = getDMAPort(fg, procIndex, dmaIndex);
                dmaPorts.push_back(dmaPort);
            }
        }
        return dmaPorts;
    }
};
