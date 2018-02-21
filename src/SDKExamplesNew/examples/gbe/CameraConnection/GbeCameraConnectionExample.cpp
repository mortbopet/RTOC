
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <vector>

#include "GbeCameraDetectionExample.cpp"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <gbe.h>
#include <gbe_error.h>

class GbeCameraConnectionExample: public GbeCameraDetectionExample{
public:
    GbeCameraConnectionExample(){}

    int32_t run(){
        try{
            //initializes internal structures of the library.
            int32_t status = Fg_InitLibraries(nullptr);
            if(status != FG_OK)
                throw std::runtime_error("Cannot initialize Fg libraries.");

            /*FgAppletIteratorExample shows how to load an applet.*/
            uint32_t boardIndex = selectBoardDialog();
            BoardHandle* boardGbeHandle = createBoardGbeHandle(boardIndex);

            /*CameraDetectionExample shows how to search and connect to cameras.*/
            int32_t portIndex = selectPortDialog();
            scanNetwork(boardGbeHandle, portIndex);
            int32_t cameraIndex = selectCameraDialog(boardGbeHandle, portIndex);
            CameraHandle* cameraHandle = createCameraHandle(boardGbeHandle, portIndex, cameraIndex);

            /*connect to cameras and print some genicam parameter*/
            connectToCamera(cameraHandle);
            printGenApiParameter(cameraHandle);

            /*free camera handles*/
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
protected:


    void printGenApiParameter(CameraHandle* cameraHandle){
        mLog->printH1("GenApi Parameter");

        std::string name = "DeviceVendorName";
        std::string vendorName = getStringValue(cameraHandle, name);
        PRINT_KEY_VAL(mLog, name, vendorName);

        name = "DeviceModelName";
        std::string modelName = getStringValue(cameraHandle, name);
        PRINT_KEY_VAL(mLog, name, modelName);

        name = "DeviceVersion";
        std::string deviceVersion = getStringValue(cameraHandle, name);
        PRINT_KEY_VAL(mLog, name, deviceVersion);

        name = "Width";
        int64_t width = getIntegerValue(cameraHandle, name);
        PRINT_KEY_VAL(mLog, name, std::to_string(width));

        name = "Height";
        int64_t height = getIntegerValue(cameraHandle, name);
        PRINT_KEY_VAL(mLog, name, std::to_string(height));

    }

    int64_t getIntegerValue(CameraHandle* cameraHandle, std::string name){
        int32_t status = GBE_OK;
        int64_t value = 0;
        status = Gbe_getIntegerValue(cameraHandle,name.c_str(),&value);
        if(status != GBE_OK)
            throw std::runtime_error(Gbe_getErrorDescription(status));
        return value;

    }

    void setIntegerValue(CameraHandle* cameraHandle, std::string name, int64_t val){
        int32_t status = GBE_OK;
        status = Gbe_setIntegerValue(cameraHandle,name.c_str(),val);
        if(status != GBE_OK)
            throw std::runtime_error(Gbe_getErrorDescription(status));
    }

    std::string getStringValue(CameraHandle* cameraHandle, std::string name){
        int32_t status = GBE_OK;
        const char* value = nullptr;
        status = Gbe_getStringValue(cameraHandle,name.c_str(),&value);
        if(status != GBE_OK)
            throw std::runtime_error(Gbe_getErrorDescription(status));
        std::string result(value);
        return result;

    }
};