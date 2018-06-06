
#ifndef RTOC_SETUP_H
#define RTOC_SETUP_H

#include <string>

class Setup {
public:
    bool runProcessing;
    bool extractData;
    bool storeRaw;
    bool storeProcessed;
    bool asyncImageWrite;
    long dataFlags = 0;
    std::string rawPrefix;
    std::string processedPrefix;
    std::string outputPath;
    std::string experimentName;

private:
};

#endif  // RTOC_SETUP_H
