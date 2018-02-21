/*This example demonstrates how to iterate through applets using
the applet iterator and how to load them.*/

#include <stdio.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <vector>

#include "FgExample.h"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>


class FgAppletIteratorExample: public FgExample{
public:
    FgAppletIteratorExample(){}

    int32_t run(){
        try{
            //initializes internal structures of the library.
            int32_t status = Fg_InitLibraries(nullptr);
            if(status != FG_OK)
                throw std::runtime_error("Cannot initialize Fg libraries.");

            uint32_t boardIndex = selectBoardDialog();
            std::string appletPath = selectAppletDialog(boardIndex);

            Fg_Struct* fgHandle = Fg_Init(appletPath.c_str(), boardIndex);
            if(!fgHandle)
                throw std::runtime_error("Cannot initialize frame grabber!");
            Fg_FreeGrabber(fgHandle);

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

    /*This function prints loadable Applets and lets the user select one.
    The path of the selected Applet is returned.*/
    std::string selectAppletDialog(int32_t boardIndex){
        bool showIncompatibility = false;
        std::vector<std::string> applets = determineLoadableApplets(boardIndex, showIncompatibility);
        uint32_t index = mLog->selectDialog<std::string>(applets, "applet");
        return applets[index];
    }


    /*This function determinesr loadable applets using applet iterator.*/
    std::vector<std::string> determineLoadableApplets(int32_t boardIndex, bool showIncompatibility = false){
        //Fetch the iterator.
        Fg_AppletIteratorType iter = nullptr;
        int32_t appletCount = Fg_getAppletIterator(boardIndex, FG_AIS_FILESYSTEM, &iter, 0);
        bool isError = appletCount < 0;
        if(isError){
            this->throwLastFgError();
        }

        //Iterate through the available applets.
        std::vector<std::string> applets;
        try{
            Fg_AppletIteratorItem entry = nullptr;
            for(int32_t i = 0; (entry = Fg_getAppletIteratorItem(iter, i)) != nullptr; i++){
                int64_t flags = Fg_getAppletIntProperty(entry, FG_AP_INT_FLAGS);
                //Check if the applet is loadable
                if ((flags & FG_AF_IS_LOADABLE) == FG_AF_IS_LOADABLE){
                    const char * appletPath = Fg_getAppletStringProperty(entry, FG_AP_STRING_APPLET_PATH);
                    applets.push_back(appletPath);
                }else if(showIncompatibility){
                    const char* appletName = Fg_getAppletStringProperty(entry, FG_AP_STRING_APPLET_NAME);
                    mLog->printH2(appletName);
                    printAPInverseErrorFlag(flags);
                }
            }
        }catch(std::exception& e){
            Fg_freeAppletIterator(iter);
            throw e;
        }
        Fg_freeAppletIterator(iter);
        return applets;
    }

    /*Based on the given FG_AP_INT_FLAGS flag incompatiblity reasons are printed.*/
    void printAPInverseErrorFlag(int64_t flags){
        if ((flags & FG_AF_IS_AVAILABLE) != FG_AF_IS_AVAILABLE)
            mLog->printWarning("Applet is not avaible.");
        if ((flags & FG_AF_IS_CORRECT_PLATFORM) != FG_AF_IS_CORRECT_PLATFORM)
            mLog->printWarning("Applet has wrong platform.");
        if ((flags & FG_AF_IS_VALID_LICENSE) != FG_AF_IS_VALID_LICENSE)
            mLog->printWarning("License is invalid");
        if ((flags & FG_AF_IS_LOADABLE) != FG_AF_IS_LOADABLE)
            mLog->printWarning("Applet is not loadable.");
        if ((flags & FG_AF_IS_COMPATIBLE) != FG_AF_IS_COMPATIBLE)
            mLog->printWarning("Applet is incompatible.");
    }

    /*Helper function to load an applet from the given path.*/
    Fg_Struct* initializeFgWithDllPath(std::string dllPath, uint32_t boardIndex, int isSlave = 0){
        Fg_Struct* fgHandle = Fg_InitEx(dllPath.c_str(), boardIndex, isSlave);
        if(!fgHandle)
            throw std::runtime_error("Cannot initialize frame grabber!");
        return fgHandle;
    }

    /*Helper function to destroy the given fgHandle.*/
    void DestroyFg(Fg_Struct*& fgHandle){
        int err = Fg_FreeGrabber(fgHandle);
        fgHandle = nullptr;
        if(err != FG_OK)
            std::cout << "Example failed: " << err << std::endl;
    }
};
