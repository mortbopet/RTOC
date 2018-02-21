
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <vector>

#include "FgAcqBlockExample.cpp"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <gbe.h>
#include <gbe_error.h>

class GbeCameraDetectionExample: public FgAcqBlockExample{
public:
    GbeCameraDetectionExample(){}

    int32_t run(){
        try{
            //initializes internal structures of the library.
            int32_t status = Fg_InitLibraries(nullptr);
            if(status != FG_OK)
                throw std::runtime_error("Cannot initialize Fg libraries.");

            /*FgAppletIteratorExample shows how to load an applet.*/
            uint32_t boardIndex = selectBoardDialog();


            BoardHandle* boardGbeHandle = createBoardGbeHandle(boardIndex);
            int32_t portIndex = selectPortDialog();
            scanNetwork(boardGbeHandle, portIndex);
            int32_t cameraIndex = selectCameraDialog(boardGbeHandle, portIndex);
            showCameraInfo(boardGbeHandle, portIndex, cameraIndex);

            /*free camera hanles*/
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
    /*This function lets the user select a gbe port*/
    int32_t selectPortDialog(){
        std::vector<std::string> ports;
        int64_t portCount = 4;
        for(int64_t i = 0; i < portCount; i++){
            ports.push_back(std::string("Port ") + std::to_string(i));
        }
        return mLog->selectDialog<std::string>(ports, "port");
    }

    /*This function initializes the gbe board handle*/
    BoardHandle* createBoardGbeHandle(int32_t boardIndex){
        int32_t status = GBE_OK;
        BoardHandle* handle = nullptr;
        status = Gbe_initBoard(boardIndex,0,&handle);
        if(status != GBE_OK)
            throw std::runtime_error(Gbe_getErrorDescription(status));
        return handle;
    }

    /*This function scans the network to find cameras.*/
    void scanNetwork(BoardHandle* boardHandle, int32_t portIndex){
        int32_t status = GBE_OK;
        int32_t timeout = 200;
        status = Gbe_scanNetwork(boardHandle,portIndex,200);
        if(status != GBE_OK)
            throw std::runtime_error(Gbe_getErrorDescription(status));
    }

    /*This function lets the user select a camera*/
    int32_t selectCameraDialog(BoardHandle* boardHandle,int32_t portIndex){
        std::vector<std::string> cameraCandidates = detectConnectedGECameras(boardHandle, portIndex);
        uint32_t index = mLog->selectDialog<std::string>(cameraCandidates, "camera");
        return static_cast<int32_t>(index);
    }

    /*This function detects cameras on a specified port and returns their names as a vector.*/
    std::vector<std::string> detectConnectedGECameras(BoardHandle* boardHandle,int32_t portIndex){
        std::vector<std::string> cameras;
        int32_t camCount = Gbe_getCameraCount(boardHandle, portIndex);
        int32_t status = GBE_OK;
        for(int32_t i = 0; i < camCount; i++){
            CameraHandle* cameraCandidat = createCameraHandle(boardHandle, portIndex, i);
            connectToCamera(cameraCandidat);
            CameraInfo *CamInfo = Gbe_getCameraInfo(cameraCandidat);
            cameras.push_back(CamInfo->model_name);
            status = Gbe_disconnectCamera(cameraCandidat);
            if(status != GBE_OK)
                throw std::runtime_error(Gbe_getErrorDescription(status));
            Gbe_freeCamera(cameraCandidat);
        }
        return cameras;
    }

    /*Connects to a camera*/
    void connectToCamera(CameraHandle* cameraHandle){
        int32_t status = GBE_OK;
        status = Gbe_connectCamera(cameraHandle);
        if(status != GBE_OK)
            throw std::runtime_error(Gbe_getErrorDescription(status));
    }

    /*Creates a camera handle*/
    CameraHandle* createCameraHandle(BoardHandle* boardHandle, int32_t portIndex, int32_t camIndex){
        CameraHandle *cameraCandidat = nullptr;
        int32_t status = Gbe_getCameraByIndex(boardHandle, portIndex, camIndex, &cameraCandidat);
        if(status != GBE_OK)
            throw std::runtime_error(Gbe_getErrorDescription(status));
        return cameraCandidat;
    }

    /*prints infos that indicates the connected camera.*/
    void showCameraInfo(BoardHandle* boardHandle,int32_t portIndex, int32_t camIndex){
        CameraHandle *cameraHandle = createCameraHandle(boardHandle, portIndex, camIndex);
        CameraInfo *CamInfo = Gbe_getCameraInfo(cameraHandle);
        mLog->printH2("Camera Information");
        PRINT_KEY_VAL(mLog, "Model", CamInfo->model_name);
        PRINT_KEY_VAL(mLog, "Manufacturer", CamInfo->manufactor_name);
        PRINT_KEY_VAL(mLog, "Serial number", CamInfo->serial_number);
        PRINT_KEY_VAL(mLog, "User name", CamInfo->user_name);
        PRINT_KEY_VAL(mLog, 
            "IP Address",
            (CamInfo->ipv4 >> 24) << "." <<
            ((CamInfo->ipv4 & 0xFFFFFF) >> 16) << "." <<
            ((CamInfo->ipv4 & 0xFFFF) >> 8) << "." <<
            ((CamInfo->ipv4 & 0xFF)))

            PRINT_KEY_VAL(mLog, 
            "MAC Address",
            CamInfo->mac[0] << "." <<
            CamInfo->mac[1] << "." <<
            CamInfo->mac[2] << "." <<
            CamInfo->mac[3] << "." <<
            CamInfo->mac[4] << "." <<
            CamInfo->mac[5])
            Gbe_freeCamera(cameraHandle);
    }

    /*Throws a gbe error as string.*/
    static void throwGbeException(int err){
        std::stringstream ss;
        ss << Gbe_getErrorDescription(err);
        throw std::runtime_error(ss.str());
    }
};