
#ifndef CELLSORTER_SETUP_H
#define CELLSORTER_SETUP_H

#include <string>

class Setup {
public:
    bool runProcessing;
    bool extractData;
    bool storeRaw;
    bool storeProcessed;
    long dataFlags = 0;
    std::string rawPrefix;
    std::string processedPrefix;
    std::string outputPath;
    std::string experimentName;

private:
};


#endif //CELLSORTER_SETUP_H
