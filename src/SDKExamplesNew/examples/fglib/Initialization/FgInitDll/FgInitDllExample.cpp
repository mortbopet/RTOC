/*
This example show how to initialize a framegrabber and load an applet.
*/
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <vector>

#include "FgExample.h"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>


class FgInitDllExample: public FgExample{
public:
    FgInitDllExample(){}

    void run(){
        Fg_Struct* fgHandle = nullptr;
        try{
            uint32_t boardIndex = selectBoardDialog();
            std::string appletPath = selectAppletDialog(boardIndex);
            fgHandle = initializeFgWithDllPath(appletPath, boardIndex);
        }catch(std::exception& e){
            std::cout << "Example failed: " << e.what() << std::endl;
        }
        DestroyFg(fgHandle);
    }

protected:

    Fg_Struct* initializeFgWithDllPath(std::string dllPath, uint32_t boardIndex){
        Fg_Struct* fgHandle = Fg_Init(dllPath.c_str(), boardIndex);
        if(!fgHandle)
            throw std::runtime_error("Cannot initialize frame grabber!");
        return fgHandle;
    }

    void DestroyFg(Fg_Struct*& fgHandle){
        int err = Fg_FreeGrabber(fgHandle);
        fgHandle = nullptr;
        if(err != FG_OK)
            std::cout << "Example failed: " << err << std::endl;
    }
};
