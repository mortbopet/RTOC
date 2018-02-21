/*This example show how to save the applet configuration*/
#include "FgDmaChannelExample.cpp"

class FgSaveConfigExample: public FgDmaChannelExample{
public:
    FgSaveConfigExample(){}

    ~FgSaveConfigExample(){}

    virtual int run(std::string path){
        try{
            //initializes internal structures of the library.
            uint32_t boardIndex = selectBoardDialog();
            std::string appletPath = selectAppletDialog(boardIndex);
            mFgHandle = FgWrapper::create(initializeFgWithDllPath(appletPath, boardIndex));
            
            /*FgDmaChannelExample shows dma initialization.*/
            int32_t dmaPort = selectDmaPortDialog(mFgHandle);
            mDmaHandle = DmaMemWrapper::create(mFgHandle, dmaPort);

            //Change width
            int width = 640;
            int32_t status = Fg_setParameter(mFgHandle->getFgHandle(), FG_WIDTH, &width, dmaPort);

            if(status != FG_OK)
                throwLastFgError();

            status = Fg_saveConfig(mFgHandle->getFgHandle(), path.c_str());
            if(status != FG_OK)
                throwLastFgError();

        }catch(std::exception& e){
            std::cout << "Example failed: " << e.what() << std::endl;
			return -1;
        }
		return 0;
    }
};