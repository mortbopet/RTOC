
#ifndef RTOC_SETUP_H
#define RTOC_SETUP_H

#include <string>

#include <utility>

class Setup {
public:
    bool runProcessing;
    bool extractData;
    bool storeRaw;
    bool storeProcessed;
    bool asyncImageWrite;
    unsigned long dataFlags = 0;
    unsigned long conditionFlags = 0;
    std::pair<int, int> inlet;
    std::pair<int, int> outlet;
    std::string rawPrefix;
    std::string processedPrefix;
    std::string outputPath;
    std::string experimentName;

private:
};

#endif  // RTOC_SETUP_H
