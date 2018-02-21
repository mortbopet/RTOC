
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <vector>

#include "GbeAcquisitionExample.cpp"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <gbe.h>
#include <gbe_error.h>

//interface class to receive events from camera
class EventListener{
public:
    virtual void onEvent(CameraHandle *cameraHandle, const GbeEventInfo *eventInfo) = 0;
};

//this structure type is handed over to the callback function
struct gbe_callback_data {
    EventListener* observer;
};

//this function is called by the gbelib when an event accures
static int CameraEventCallback(CameraHandle *cameraHandle, const GbeEventInfo *eventInfo, const void* privateData)
{
    const gbe_callback_data* dataPtr = static_cast<const gbe_callback_data*>(privateData);
    dataPtr->observer->onEvent(cameraHandle, eventInfo);
    return 0;
}

class GbeEventExample: public GbeAcquisitionExample, public EventListener{
public:
    GbeEventExample(){}

    int32_t run(){
        try{
            //initializes internal structures of the library.
            int32_t status = Fg_InitLibraries(nullptr);
            if(status != FG_OK)
                throw std::runtime_error("Cannot initialize Fg libraries.");

            /*FgAppletIteratorExample shows how to load an applet.*/
            uint32_t boardIndex = selectBoardDialog();

            BoardHandle* boardGbeHandle = createBoardGbeHandle(boardIndex);

            /*CameraDetectionExample shows camera detection strategy.*/
            int32_t portIndex = selectPortDialog();
            scanNetwork(boardGbeHandle, portIndex);
            int32_t cameraIndex = selectCameraDialog(boardGbeHandle, portIndex);
            CameraHandle* cameraHandle = createCameraHandle(boardGbeHandle, portIndex, cameraIndex);

            /*GbeCameraConnectionExample*/
            connectToCamera(cameraHandle);

            gbe_callback_data privateData;
            privateData.observer = this;
            int err = Gbe_registerCameraEventCallback(cameraHandle, CameraEventCallback, &privateData);
            if(err)
                throw std::runtime_error("Failed to register event callback.");

            /*FgAppletIteratorExample shows how to load an applet.*/
            std::string appletPath = selectAppletDialog(boardIndex);
            mFgHandle = FgWrapper::create(appletPath, boardIndex);

            /*FgDmaChannelExample shows dma initialization.*/
            mDmaHandle = DmaMemWrapper::create(mFgHandle, portIndex);
            mDisplay= DisplayWrapper::create(
                std::weak_ptr<DmaMemWrapper>(mDmaHandle), 
                portIndex,
                std::weak_ptr<FgWrapper>(mFgHandle));

            /*GbeAcquisitionExample shows the acquisition with gbe*/
            acquisitionGbe(
                mFgHandle->getFgHandle(),
                portIndex,
                mDmaHandle->getMemHandle(),
                cameraHandle);

            /*free camera hanles*/
            Gbe_freeCamera(cameraHandle);
            Gbe_freeBoard(boardGbeHandle);

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

    virtual void onEvent(CameraHandle *cameraHandle, const GbeEventInfo *eventInfo){
        try {
            switch(eventInfo->type)
            {
            case GBE_EVENT_CAMERA_CONNECTION_LOST:
                PRINT_INFO(mLog,"Camera connection lost detected!");
                break;
            case GBE_EVENT_GEVEVENT:
                mLog->printH2("GEV Event");
                PRINT_KEY_VAL(mLog,"ReqId", std::hex << std::showbase << eventInfo->data.gevevent.reqId << std::dec);
                PRINT_KEY_VAL(mLog,"Index", eventInfo->data.gevevent.index);
                PRINT_KEY_VAL(mLog,"EventId", std::hex << std::showbase << eventInfo->data.gevevent.eventId << std::dec);
                PRINT_KEY_VAL(mLog,"StreamChannelIndex",eventInfo->data.gevevent.streamChannelIndex);
                PRINT_KEY_VAL(mLog,"BlockId",std::hex << std::showbase << eventInfo->data.gevevent.blockId << std::dec);
                PRINT_KEY_VAL(mLog,"Timestamp",eventInfo->data.gevevent.timestamp);
                PRINT_KEY_VAL(mLog,"DataLength", eventInfo->data.gevevent.dataLen);
                break;
            }
        } catch(std::runtime_error& e) {
            PRINT_ERROR(mLog, e.what());
        }
    }
};