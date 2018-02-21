#include "../../Initialization/FgInitDll/FgInitDllExample.cpp"

class FgXMLParamterInfo: public FgInitDllExample{
public:
    FgXMLParamterInfo(){}

    ~FgXMLParamterInfo(){}

    virtual void run(){
        Fg_Struct* fgHandle = nullptr;
        try{
            uint32_t boardIndex = selectBoardDialog();
            std::string appletPath = selectAppletDialog(boardIndex);
            fgHandle = initializeFgWithDllPath(appletPath, boardIndex);
            std::string xmlParamterInfo = getXMLParamterInfo(fgHandle, 0);
        }catch(std::exception& e){
            std::cout << "Example failed: " << e.what() << std::endl;
        }
        DestroyFg(fgHandle);
    }

protected:
    std::string getXMLParamterInfo(Fg_Struct* fgHandle, int32_t port){
        size_t xmlBufferSize = 0;

        if (Fg_getParameterInfoXML(fgHandle, port, 0, &xmlBufferSize) != FG_OK)
            throwLastFgError();

        char* xmlBuffer = new char[xmlBufferSize];

        if ((xmlBuffer == nullptr)) {
            throw std::runtime_error("Cannot allocate memory");
        }

        if (Fg_getParameterInfoXML(fgHandle, port, xmlBuffer, &xmlBufferSize) != FG_OK) {
            delete[] xmlBuffer;
            throwLastFgError();
        }

        std::string result(xmlBuffer);
        delete[] xmlBuffer;
        return result;
    }
};