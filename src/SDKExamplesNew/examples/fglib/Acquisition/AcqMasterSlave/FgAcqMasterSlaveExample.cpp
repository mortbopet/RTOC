#include "FgAcqMasterSlaveExample.h"



int32_t FgAcqMasterSlaveExample::run(){
    try{
        bool isSlave = selectMasterSlaveDialog();

        //initializes internal structures of the library.
        uint32_t flags = FG_INIT_LIBRARIES_AUTOSTART_ON_INIT;
        if(isSlave)
            flags |= FG_INIT_LIBRARIES_SLAVE;
        else
            flags |= FG_INIT_LIBRARIES_MASTER;
        std::string id = "master-slave-example";
        uint32_t timeoutInMilliseconds = 1000*60; //timeout for slave to wait for master
        Fg_InitLibrariesEx(nullptr, flags, id.c_str(), timeoutInMilliseconds);
        int32_t status = Fg_InitLibraries(nullptr);
        if(status != FG_OK)
            throw std::runtime_error("Cannot initialize Fg libraries.");

        uint32_t boardIndex = selectBoardDialog();
        std::string appletPath = selectAppletDialog(boardIndex);
        
        Fg_Struct* fgHandle = Fg_InitEx(appletPath.c_str(), boardIndex, isSlave);
        if(!fgHandle)
            throw std::runtime_error("Cannot initialize frame grabber!");
        mFgHandle = FgWrapper::create(fgHandle);

        /*FgDmaChannelExample shows dma initialization.*/
        uint32_t dmaPort = this->selectDmaPortDialog(mFgHandle);
        mDmaHandle = DmaMemWrapper::create(mFgHandle, dmaPort);

        mDisplay= DisplayWrapper::create(
            std::weak_ptr<DmaMemWrapper>(mDmaHandle), 
            dmaPort,
            std::weak_ptr<FgWrapper>(mFgHandle));

        /*FgAcqBlockExample shows dma initialization.*/
        bool printAllowed = false;
        uint32_t frameCount = 500000;
        acquisitionLoop(
            mFgHandle->getFgHandle(),
            dmaPort,
            mDmaHandle->getMemHandle(),
            printAllowed,
            frameCount);
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

bool FgAcqMasterSlaveExample::selectMasterSlaveDialog(){
    std::vector<std::string> modes;
    modes.push_back("master"); // index 0 stands for false
    modes.push_back("slave"); // index 1 stands for true
    uint32_t mode = mLog->selectDialog<std::string>(modes, "mode");
    return mode != 0;
}