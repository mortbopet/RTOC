/*This example show how to initialize a framegrabber using a configuration file.*/
#include <stdio.h>
#include <time.h>
#include <sstream>
#include <iostream>
#include <vector>

#include "FgExample.h"

#include <fgrab_struct.h>
#include <fgrab_prototyp.h>
#include <fgrab_define.h>
#include <SisoDisplay.h>


class FgInitConfigExample: public FgExample{
public:
    FgInitConfigExample(){}

    int32_t run(std::string configPath){
        try{
            //initializes internal structures of the library.
            int32_t status = Fg_InitLibraries(nullptr);
            if(status != FG_OK)
                throw std::runtime_error("Cannot initialize Fg libraries.");

            uint32_t boardIndex = selectBoardDialog();

            Fg_Struct* fgHandle = Fg_InitConfig(configPath.c_str(), boardIndex);
            if(fgHandle == nullptr)
                throwLastFgError(fgHandle);

            //destroy the fg handle
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

    /*This function creates a fg handle using a configuration file.*/
    Fg_Struct* initFGConfig(std::string configPath, uint32_t boardIndex){
        Fg_Struct* fgHandle = Fg_InitConfig(configPath.c_str(), boardIndex);
        if(fgHandle == nullptr)
            throwLastFgError(fgHandle);
        return fgHandle;
    }
};

