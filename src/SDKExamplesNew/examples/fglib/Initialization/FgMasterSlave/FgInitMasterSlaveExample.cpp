
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <vector>

#include "FgDmaChannelExample.cpp"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>


class FgInitMasterSlaveExample: public FgDmaChannelExample{
public:
    FgInitMasterSlaveExample(){}

    void run(){
        std::shared_ptr<FgWrapper> fgHandle;
        std::vector<std::shared_ptr<FgWrapper>> fgSlaveHandles;
        try{
            uint32_t boardIndex = selectBoardDialog();
            std::string appletPath = selectAppletDialog(boardIndex);
            bool isSlave = selectMasterSlaveDialog();
            if(isSlave)
                fgHandle = std::shared_ptr<FgWrapper>(initializeSlaveFgHandle(appletPath, boardIndex));
            else
                fgHandle = std::shared_ptr<FgWrapper>(initializeFgWithDllPath(appletPath, boardIndex));
            uint32_t dmaPort = this->selectDmaPortDialog(fgHandle);
            
            mDisplay= DisplayWrapper::create(
                    std::weak_ptr<DmaMemWrapper>(mDmaHandle), 
                    dmaPort,
                    std::weak_ptr<FgWrapper>(mFgHandle));

            acquisitionLoop(
                mFgHandle->getFgHandle(),
                dmaPort,
                mDmaHandle->getMemHandle());
        }catch(std::exception& e){
            std::cout << "Example failed: " << e.what() << std::endl;
        }
    }


protected:
    bool selectMasterSlaveDialog(){
        std::vector<std::string> modes;
        modes.push_back("master");
        modes.push_back("slave");
        uint32_t mode = mLog->selectDialog<std::string>(modes, "mode");
        return static_cast<bool>(mode);
    }
    Fg_Struct* initializeSlaveFgHandle(std::string dllPath, uint32_t boardIndex, uint32_t slaveCount = 1){
        int isSlave = true;
        Fg_Struct* fg = Fg_InitEx(dllPath.c_str(), boardIndex, isSlave);
        if(!fg)
            this->throwLastFgError();
        return fg;
    }
};
