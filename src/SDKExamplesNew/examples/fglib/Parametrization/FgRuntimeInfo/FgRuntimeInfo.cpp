#include "FgExample.h"

class FgRuntimeInfo: public FgExample{
public:
    FgRuntimeInfo(){}

    ~FgRuntimeInfo(){}

    virtual void run(){
        try{
            printRuntimeInformation();
        }catch(std::exception& e){
            mLog.printError(e.what());
        }
    }

protected:

    std::string getInformation(const enum Fg_Info_Selector selector, int dataType, int param){
        uint32_t buflen = 256;
        std::unique_ptr<char[]> buffer(new char[buflen]);
        std::string result;
        int32_t err = FG_OK;
        
        //check data type
        err = Fg_getSystemInformation(nullptr, selector, PROP_ID_DATATYPE, param, buffer.get(), &buflen);
        int32_t type = atoi(buffer.get());

        if(type != dataType)
            throw std::runtime_error("Parameter is not the specified type!");

        //get data value
        buflen = 256;
        err = Fg_getSystemInformation(nullptr, selector, PROP_ID_VALUE, param, buffer.get(), &buflen);
        result = std::string(buffer.get());
        if(err != FG_OK)
            throw std::runtime_error("Cannot get System Information");
        return result;
    }

    std::string getInformationString(const enum Fg_Info_Selector selector){
        return getInformationString(selector, 0);
    }

    
    int32_t getInformationInt32(const enum Fg_Info_Selector selector, uint32_t boardIndex){
        std::string value = getInformation(selector, FG_PARAM_TYPE_INT32_T, boardIndex);
        return atoi(value.c_str());
    }

    std::string getInformationString(const enum Fg_Info_Selector selector, uint32_t boardIndex){
        return getInformation(selector, FG_PARAM_TYPE_CHAR_PTR, boardIndex);
    }

    void printBoardInformation(uint32_t boardIndex){
        mLog.printKeyVal("Board interface",getInformationString(INFO_CAMERA_INTERFACE, boardIndex));
        mLog.printKeyVal("Board status", getInformationInt32(INFO_BOARDSTATUS, boardIndex));
        mLog.printKeyVal("Board name", getInformationString(INFO_BOARDNAME, boardIndex));
        mLog.printKeyVal("Board serialNo", getInformationString(INFO_BOARDSERIALNO, boardIndex));

        int boardType = Fg_getBoardType(boardIndex);
        mLog.printKeyVal("Board type", boardType);
        int useShortName = 1;
        mLog.printKeyVal("Board short name", Fg_getBoardNameByType(boardType, useShortName));
    }

    void printRuntimeInformation(){
        mLog.printH1("Runtime");
		
        mLog.printKeyVal("Runtime version", Fg_getSWVersion());
        mLog.printKeyVal("Firmware version", getInformationString(INFO_FIRMWAREVERSION));
        mLog.printKeyVal("Driver version", getInformationString(INFO_DRIVERVERSION));
        mLog.printKeyVal("DriverArch", getInformationString(INFO_DRIVERARCH));
        uint32_t boardCount = getInformationInt32(INFO_NR_OF_BOARDS,0);
        mLog.printKeyVal("Board count", boardCount);

        for(uint32_t i = 0; i < boardCount; i++){
            mLog.printH2(std::string("Board: ") + std::to_string((int64_t)i));
            printBoardInformation(i);
        }
    }
};


