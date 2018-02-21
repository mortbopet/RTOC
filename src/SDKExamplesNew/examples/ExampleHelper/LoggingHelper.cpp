#include <iostream>
#include <sstream>
#include <stdint.h>
#include <vector>

#define LINE_WIDTH 60
#define PRINT_STREAM(__msg) {\
        std::ostringstream __oss; \
        __oss << __msg; \
        println(__oss.str().c_str()); }\

#define PRINT_INFO(_logger, __msg) {\
        std::ostringstream __oss; \
        __oss << __msg; \
        _logger->printInfo(__oss.str().c_str()); }\

#define PRINT_ERROR(_logger, __msg) {\
        std::ostringstream __oss; \
        __oss << __msg; \
        _logger->printError(__oss.str().c_str()); }\

#define PRINT_KEY_VAL(_logger, __keymsg, __valmsg) {\
        std::ostringstream __kss; \
        __kss << __keymsg; \
        std::ostringstream __vss; \
        __vss << __valmsg; \
        _logger->printKeyVal(__kss.str().c_str(), __vss.str().c_str());}\

class LoggingHelper{

public:
    void println(std::string text){
        std::cout << text.c_str() << std::endl;
    };

    void print(std::string out){
        std::cout << out.c_str();
    };
    void printH1(std::string header){
        std::string devider(LINE_WIDTH,'#');
        std::string offsetLeft((LINE_WIDTH - header.length() - 2)/2,' ');
        std::string offsetRight((LINE_WIDTH - header.length() - 2)/2 + header.length()%2,' ');
        std::cout << std::endl;
        std::cout << devider << std::endl;
        std::cout << "#"<< offsetLeft << header << offsetRight<< "#" << std::endl;
        std::cout << devider << std::endl;
    };

    void printH2(std::string _header){
        std::string header = _header;
        if(_header.length()>LINE_WIDTH)
            header = _header.substr(0,LINE_WIDTH-2);
        std::string offsetLeft((LINE_WIDTH - header.length() - 2)/2,'_');
        std::string offsetRight((LINE_WIDTH - header.length() - 2)/2 + header.length()%2,'_');
        std::cout << std::endl;
        std::cout << offsetLeft<< " " << header << " " << offsetRight<< std::endl;
    };

    void printWarning(std::string msg){
        PRINT_STREAM("[WARNING]: " << msg);
    }

    void printInfo(std::string msg){
        PRINT_STREAM("[INFO]: " << msg);
    }

    void printError(std::string msg){
        PRINT_STREAM("[ERROR]: " << msg);
    }

    void printKeyVal(std::string key, double val){
        std::ostringstream strs;
        strs << val;
        printKeyVal(key, strs.str() );
    }

    void printKeyVal(std::string key, uint32_t val){
        printKeyVal(key, std::to_string((int64_t)val));
    }

    void printKeyVal(std::string key, int32_t val){
        printKeyVal(key, std::to_string((int64_t)val));
    }

    void printKeyVal(
        const std::string _key, 
        const std::string _val,
        uint32_t maxKeyLengh = 22,
        uint32_t maxValLength = 22){

        std::string key = _key;
        std::string val = _val;
        if(key.length()>maxKeyLengh){
            key = key.substr(key.length()-maxKeyLengh, maxKeyLengh);
        }

        if(val.length()>maxKeyLengh){
            val = val.substr(val.length()-maxKeyLengh, maxValLength);
        }
        
        std::string offset(LINE_WIDTH - key.length() - val.length() - 2,'.');
        PRINT_STREAM(key.c_str() << " " << offset.c_str() << " " << val.c_str())
    };

    template<typename T> 
    uint32_t selectDialog(std::vector<T> list,
        std::string chooseObjectName){

            if(list.empty()){
                std::stringstream ss;
                ss << std::string("No choice available for ") << chooseObjectName;
                throw std::runtime_error(ss.str());
            }

            if(list.size() == 1){
                PRINT_INFO(this, "Selected: " << list.at(0))
                return 0;
            }

            for(size_t i = 0; i < list.size(); ++i){
                std::string name(list[i]);
                
                std::stringstream ss;
                ss << "["<< i << "]";
                std::string key = ss.str();

                printKeyVal(key, name);
            }
            
            PRINT_STREAM("Please choose " << chooseObjectName <<" ["<< 0 << " - " << list.size()-1 << "] : "<< std::endl);
            int32_t index = -1;
            while(index < 0 || index >= static_cast<int32_t>(list.size())){
                std::cin >> index;
                if(index < 0 || index >= static_cast<int32_t>(list.size()))
                    PRINT_STREAM("Try again.");
            }
            return static_cast<uint32_t>(index);
    }
};
