#include "FgEventExample.h"


struct fg_event_data{
    EventListener* listener;
};

int EventCallback(uint64_t events, struct fg_event_data *data, const struct fg_event_info *info){
    return data->listener->onEvent(events, info);
};
    FgEventExample::FgEventExample(){}

    int32_t FgEventExample::run(){
        try{
            //initializes internal structures of the library.
            int32_t status = Fg_InitLibraries(nullptr);
            if(status != FG_OK)
                throw std::runtime_error("Cannot initialize Fg libraries.");

            /*FgAppletIteratorExample shows how to load an applet.*/
            uint32_t boardIndex = selectBoardDialog();
            std::string appletPath = selectAppletDialog(boardIndex);
            mFgHandle = FgWrapper::create(appletPath, boardIndex);

            /*FgDmaChannelExample shows dma initialization.*/
            int32_t dmaPort = selectDmaPortDialog(mFgHandle);
            mDmaHandle = DmaMemWrapper::create(mFgHandle, dmaPort);

            std::string eventName = selectEvent(mFgHandle->getFgHandle());
            uint64_t eventMask = Fg_getEventMask(mFgHandle->getFgHandle(),eventName.c_str());

            fg_event_data data;
            data.listener = this;
            fg_event_info info;
            status = Fg_registerEventCallback(mFgHandle->getFgHandle(), eventMask, EventCallback, &data, FG_EVENT_DEFAULT_FLAGS, &info);
            if(status != FG_OK)
                throwLastFgError();

            const unsigned int enable = 1;
            status = Fg_activateEvents(mFgHandle->getFgHandle(), eventMask,enable);
            if(status != FG_OK)
                throwLastFgError();

            mDisplay= DisplayWrapper::create(
                std::weak_ptr<DmaMemWrapper>(mDmaHandle), 
                dmaPort,
                std::weak_ptr<FgWrapper>(mFgHandle));

            /*FgAcqBlockExample shows the acquisition*/
            bool printAllowed = false;
            acquisitionLoop(
                mFgHandle->getFgHandle(),
                dmaPort,
                mDmaHandle->getMemHandle(),
                printAllowed);


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

    /*this function interprets the event, and prints its information.
	This example function should not be adopted as it is. Because it
	consumes a lot of performance with the logging. The callback function 
	blocks the execution of the fglib and should therefore take as 
	little time as possible.*/
    int32_t FgEventExample::onEvent(uint64_t events, const struct fg_event_info *info){

        uint64_t mask = 0;
        for(int32_t i = 0; i<64; ++i){
			mask = uint64_t(1)<<i;
            if((mask & events) == 0)
                continue;

            mLog->printH2("Events Received");

            PRINT_KEY_VAL(mLog, "Event: ", Fg_getEventName(mFgHandle->getFgHandle(), mask));

            if(info->notify[i] & FG_EVENT_NOTIFY_PAYLOAD)
                PRINT_INFO(mLog, "the event has payload");

            if(info->notify[i] & FG_EVENT_NOTIFY_TIMESTAMP)
                PRINT_KEY_VAL(mLog, "Timestamp: ", info->timestamp[i]);

            if(info->notify[i] & FG_EVENT_NOTIFY_LOST)
                PRINT_INFO(mLog, "there was data lost before this event");

            if(info->notify[i] & FG_EVENT_NOTIFY_JOINED)
                PRINT_INFO(mLog, "events were joined in mode FG_EVENT_BATCHED");
        }
        return 0;
    }
	
    /*This function determines all avaible events and returns their names*/
    std::vector<std::string> FgEventExample::determineEvents(Fg_Struct* fgHandle){
        std::vector<std::string> eventNames;
        for(int32_t i = 0; i<64; ++i){
            uint64_t mask = 1ull<<i;
            const char * name = Fg_getEventName(fgHandle, mask);
            if(name == nullptr)
                continue;
            eventNames.push_back(name);
        }
        return eventNames;
    }

    std::string FgEventExample::selectEvent(Fg_Struct* fgHandle){
        std::vector<std::string> names = determineEvents(fgHandle);
        uint32_t index = mLog->selectDialog<std::string>(names, "event");
        return names.at(index);
    }