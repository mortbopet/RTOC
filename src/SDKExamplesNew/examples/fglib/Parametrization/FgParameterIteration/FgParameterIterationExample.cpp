#include "FgDmaChannelExample.cpp"

class FgParameterIterationExample: public FgDmaChannelExample{
public:
    FgParameterIterationExample(){}

    ~FgParameterIterationExample(){}

    virtual int32_t run(){
        try{
            //initializes internal structures of the library.
            int32_t status = Fg_InitLibraries(nullptr);
            if(status != FG_OK)
                throw std::runtime_error("Cannot initialize Fg libraries.");

            /*FgAppletIteratorExample shows how to load an applet.*/
            uint32_t boardIndex = selectBoardDialog();
            std::string appletPath = selectAppletDialog(boardIndex);
            mFgHandle = FgWrapper::create(initializeFgWithDllPath(appletPath, boardIndex));

            /*FgDmaChannelExample shows dma initialization.*/
            int32_t dmaPort = this->selectDmaPortDialog(mFgHandle);
            mDmaHandle = DmaMemWrapper::create(std::weak_ptr<FgWrapper>(mFgHandle),dmaPort);

            printAppletParameter(mFgHandle->getFgHandle());
        }catch(std::exception& e){
            std::cout << "Example failed: " << e.what() << std::endl;
            return -1;
        }
        return 0;
    }

protected:
    template<typename TYPE>
    void printIntegerValueInformation(Fg_Struct* fg, const int32_t parameterId){
        int32_t status = FG_OK;
        const int32_t charArrSize = 1024;
        int32_t dataSize = 1024;
        std::unique_ptr<char[]> dataTypeCharArr(new char[charArrSize]);

        dataSize = charArrSize;
        status = Fg_getParameterProperty(fg, 
            parameterId, 
            PROP_ID_MIN, 
            dataTypeCharArr.get(), 
            &dataSize);
        if(status != FG_OK)
            mLog->printError(Fg_getLastErrorDescription(fg));
        else
            mLog->printKeyVal("Min Value:", dataTypeCharArr.get());

        dataSize = charArrSize;
        status = Fg_getParameterProperty(fg, 
            parameterId, 
            PROP_ID_MAX, 
            dataTypeCharArr.get(), 
            &dataSize);
        if(status != FG_OK)
            mLog->printError(Fg_getLastErrorDescription(fg));
        else
            mLog->printKeyVal("Max Value:", dataTypeCharArr.get());

        dataSize = charArrSize;
        status = Fg_getParameterProperty(fg, 
            parameterId, 
            PROP_ID_STEP, 
            dataTypeCharArr.get(), 
            &dataSize);
        if(status != FG_OK)
            mLog->printError(Fg_getLastErrorDescription(fg));
        else
            mLog->printKeyVal("Step size:", dataTypeCharArr.get());

        dataSize = charArrSize;
        status = Fg_getParameterProperty(fg, 
            parameterId, 
            PROP_ID_VALUE, 
            dataTypeCharArr.get(), 
            &dataSize);
        if(status != FG_OK)
            mLog->printError(Fg_getLastErrorDescription(fg));
        else
            mLog->printKeyVal("Value: ", dataTypeCharArr.get());

    }

    void printStringValueInformation(Fg_Struct* fg, const int32_t parameterId){
        int32_t status = FG_OK;
        const int32_t charArrSize = 1024;
        int32_t dataSize = charArrSize;
        std::unique_ptr<char[]> dataTypeCharArr(new char[charArrSize]);

        dataSize = charArrSize;
        status = Fg_getParameterProperty(fg, 
            parameterId, 
            PROP_ID_VALUELLEN, 
            dataTypeCharArr.get(), 
            &dataSize);
        if(status != FG_OK)
            mLog->printError(Fg_getLastErrorDescription(fg));
        else
            mLog->printKeyVal("len: ", dataTypeCharArr.get());

        dataSize = charArrSize;
        status = Fg_getParameterProperty(fg, 
            parameterId, 
            PROP_ID_VALUE, 
            dataTypeCharArr.get(), 
            &dataSize);
        if(status != FG_OK)
            mLog->printError(Fg_getLastErrorDescription(fg));
        else
            mLog->printKeyVal("Value: ", dataTypeCharArr.get());
    }

    void printIntFieldValueInformation(Fg_Struct* fg, const int32_t parameterId){
        int32_t status = FG_OK;

        //FieldParameterInt field;
        char field[256];
        int32_t dataSize = sizeof(field);

        status = Fg_getParameterProperty(fg, 
            parameterId, 
            PROP_ID_VALUE, 
            &field, 
            &dataSize);
        if(status != FG_OK)
            mLog->printError(Fg_getLastErrorDescription(fg));
        else
            mLog->printKeyVal("Value:", field);
    }

    void printDoubleFieldValueInformation(Fg_Struct* fg, const int32_t parameterId){
        int32_t status = FG_OK;

        FieldParameterDouble field;
        int32_t dataSize = sizeof(field);

        status = Fg_getParameterProperty(fg, 
            parameterId, 
            PROP_ID_VALUE, 
            &field, 
            &dataSize);
        if(status != FG_OK)
            throwLastFgError();

        mLog->printKeyVal("Index:", field.index);
        mLog->printKeyVal("Value:", field.value);
    }

    void printValueInformation(Fg_Struct* fg, const int32_t parameterId){
        int32_t dataSize = 128;
        std::unique_ptr<char[]> dataTypeCharArr(new char[dataSize]);
        int32_t status = Fg_getParameterProperty(fg, 
            parameterId, 
            PROP_ID_DATATYPE, 
            dataTypeCharArr.get(), 
            &dataSize);
        if(status != FG_OK)
            throwLastFgError();

        int32_t dataType = atoi(dataTypeCharArr.get());
        switch(dataType){
        case FG_PARAM_TYPE_INT32_T:
            mLog->printKeyVal("Type:", "int32_t");
            printIntegerValueInformation<int32_t>(fg, parameterId);
            break;
        case FG_PARAM_TYPE_UINT32_T: 
            mLog->printKeyVal("Type:", "uint32_t");
            printIntegerValueInformation<uint32_t>(fg, parameterId);
            break;
        case FG_PARAM_TYPE_INT64_T: 
            mLog->printKeyVal("Type:", "int64_t");
            printIntegerValueInformation<int64_t>(fg, parameterId);
            break;
        case FG_PARAM_TYPE_UINT64_T: 
            mLog->printKeyVal("Type:", "uint64_t");
            printIntegerValueInformation<uint64_t>(fg, parameterId);
            break;
        case FG_PARAM_TYPE_SIZE_T: 
            mLog->printKeyVal("Type:", "size_t");
            printIntegerValueInformation<size_t>(fg, parameterId);
            break;
        case FG_PARAM_TYPE_DOUBLE: 
            mLog->printKeyVal("Type:", "double");
            printIntegerValueInformation<double>(fg, parameterId);
            break;
        case FG_PARAM_TYPE_CHAR_PTR:
            mLog->printKeyVal("Type:", "char*");
            printStringValueInformation(fg, parameterId);
            break;
        case FG_PARAM_TYPE_STRUCT_FIELDPARAMINT:
            mLog->printKeyVal("Type:", "int field parameter");
            printIntFieldValueInformation(fg, parameterId);
            break;
        case FG_PARAM_TYPE_STRUCT_FIELDPARAMDOUBLE:
            mLog->printKeyVal("Type:", "double field parameter");
            printDoubleFieldValueInformation(fg, parameterId);
            break;
        case FG_PARAM_TYPE_INVALID:
            mLog->printError("invalid data type");
            break;
        default:
            mLog->printWarning("Parameter datatype is unknown.");
        }
    }
    void printAppletParameter(Fg_Struct* fg){
        int32_t parameterCount = Fg_getNrOfParameter(fg);
        if(parameterCount<0)
            throwLastFgError();

        int32_t status = FG_OK;
        for(int32_t parameterIndex = 0; parameterIndex < parameterCount; parameterIndex++){
            int32_t parameterId = Fg_getParameterId(fg, parameterIndex);
            if(parameterId < 0)
                throwLastFgError();

            const char * parameterName = Fg_getParameterName(fg, parameterIndex);
            if(parameterName == nullptr)
                throwLastFgError();

            mLog->printH2(parameterName);
            int32_t parameterIdByName = Fg_getParameterIdByName(fg, parameterName);
            if(parameterIdByName < 0 || parameterIdByName != parameterId)
                throwLastFgError();

            printValueInformation(fg, parameterId);

        }
    }
};