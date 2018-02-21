#include "../../Initialization/FgInitDll/FgInitDllExample.cpp"

class FgParameterIteration: public FgInitDllExample{
public:
    FgParameterIteration(){}

    ~FgParameterIteration(){}

    virtual void run(){
        try{
            uint32_t boardIndex = selectBoardDialog();
            std::string appletPath = selectAppletDialog(boardIndex);
            mFg = std::make_shared<FgWrapper>(initializeFgWithDllPath(appletPath, boardIndex));
            printAppletParamter(mFg->getFgHandle());
        }catch(std::exception& e){
            std::cout << "Example failed: " << e.what() << std::endl;
        }
    }

protected:
    template<typename TYPE>
    void printValueInformation(Fg_Struct* fg, const int32_t paramterId){
        int32_t status = FG_OK;
        const int32_t charArrSize = 1024;
        int32_t dataSize = 1024;
        std::unique_ptr<char[]> dataTypeCharArr(new char[charArrSize]);
                            
        TYPE minVal = 0;
        dataSize = charArrSize;
        status = Fg_getParameterProperty(fg, 
                    paramterId, 
                    PROP_ID_MIN, 
                    dataTypeCharArr.get(), 
                    &dataSize);
        if(status != FG_OK)
            throwLastFgError();
        mLog.printKeyVal("Min Value:", dataTypeCharArr.get());

        TYPE maxVal = 0;
        dataSize = charArrSize;
        status = Fg_getParameterProperty(fg, 
                    paramterId, 
                    PROP_ID_MAX, 
                    dataTypeCharArr.get(), 
                    &dataSize);
        if(status != FG_OK)
            throwLastFgError();
        mLog.printKeyVal("Max Value:", dataTypeCharArr.get());

        TYPE stepSize = 0;
        dataSize = charArrSize;
        status = Fg_getParameterProperty(fg, 
                    paramterId, 
                    PROP_ID_STEP, 
                    dataTypeCharArr.get(), 
                    &dataSize);
        if(status != FG_OK)
            throwLastFgError();
        mLog.printKeyVal("Step size:", dataTypeCharArr.get());

    }

    void printValueInformation(Fg_Struct* fg, const int32_t paramterId){
        int32_t dataSize = 128;
        std::unique_ptr<char[]> dataTypeCharArr(new char[dataSize]);
        int32_t status = Fg_getParameterProperty(fg, 
                            paramterId, 
                            PROP_ID_DATATYPE, 
                            dataTypeCharArr.get(), 
                            &dataSize);
        if(status != FG_OK)
            throwLastFgError();

        int32_t dataType = atoi(dataTypeCharArr.get());
        switch(dataType){
            case FG_PARAM_TYPE_INT32_T:
                mLog.printKeyVal("Type:", "int32_t");
                printValueInformation<int32_t>(fg, paramterId);
                break;
            case FG_PARAM_TYPE_UINT32_T: 
                mLog.printKeyVal("Type:", "uint32_t");
                printValueInformation<uint32_t>(fg, paramterId);
                break;
            case FG_PARAM_TYPE_INT64_T: 
                mLog.printKeyVal("Type:", "int64_t");
                printValueInformation<int64_t>(fg, paramterId);
                break;
            case FG_PARAM_TYPE_UINT64_T: 
                mLog.printKeyVal("Type:", "uint64_t");
                printValueInformation<uint64_t>(fg, paramterId);
                break;
            case FG_PARAM_TYPE_SIZE_T: 
                mLog.printKeyVal("Type:", "size_t");
                printValueInformation<size_t>(fg, paramterId);
                break;
            default:
                mLog.printWarning("Parameter datatype is unknown.");
        }
    }
    void printAppletParamter(Fg_Struct* fg){
        int32_t parameterCount = Fg_getNrOfParameter(fg);
        if(parameterCount<0)
            throwLastFgError();

        int32_t status = FG_OK;
        for(int32_t paramterIndex = 0; paramterIndex < parameterCount; paramterIndex++){
            int32_t paramterId = Fg_getParameterId(fg, paramterIndex);
            if(paramterId < 0)
                throwLastFgError();

            const char * paramterName = Fg_getParameterName(fg, paramterIndex);
            if(paramterName == nullptr)
                throwLastFgError();

            mLog.printH2(paramterName);
            int32_t parameterIdByName = Fg_getParameterIdByName(fg, paramterName);
            if(parameterIdByName < 0 || parameterIdByName != paramterId)
                throwLastFgError();

            printValueInformation(fg, paramterId);

        }
    }
};