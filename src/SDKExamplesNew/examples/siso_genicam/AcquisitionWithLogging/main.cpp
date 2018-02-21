#include "../Acquisition/SgcAcquisitionExample.cpp"

#include "siso_log.h"

// ============================================================================
// Helper functions
// ============================================================================

const std::string levelName(unsigned int level)
{
    switch (level) {
    case SISOLOG_LOGLEVEL_WARN:
        return "WARN ";
    case SISOLOG_LOGLEVEL_ERROR:
        return "ERROR";
    case SISOLOG_LOGLEVEL_FATAL:
        return "FATAL";
    default:
        return "-----";
    }
}

// ============================================================================
// Log Message Callback
// ============================================================================

void logMsgCallback(tProcessId pid, tThreadId tid, const char* const logger, unsigned int level, const char* const msg, unsigned int tagcount, const tSisoLogTag* const tags, void* user_ptr)
{
    std::cout << "Received Log Message from process " << pid << " thread " << tid << ": [" << levelName(level).c_str() << "] " << msg << " [Tags: " << tagcount << ", Logger: " << logger << "]" << std::endl;

    for (unsigned int i = 0; i < tagcount; ++i) {
        std::cout << "... Log Tag " << i << ": " << tags[i].name << "=" << tags[i].value << std::endl;
    }
}

// ============================================================================
// MAIN
// ============================================================================

int main(int argc, char* argv[], char* envp[])
{
    // --- Init the SisoLog library

    (void) SisoLog_InitLibrary();

    // --- Set the SisoLog library to defauklt mode and register log event callback

    (void) SisoLog_SetMode(SISOLOG_MODE_DEFAULT);
    (void) SisoLog_RegisterLogMsgCallback(&logMsgCallback, 0);

    // --- Init the Fglib library (use default runtime paths)

    (void) Fg_InitLibraries(0);

    // --- Execute the acquisition example

    try {
        SgcAcquisitionExample example;
        example.run();
    } catch(std::exception& e) {
        std::cout << "Caught exception: " << e.what() << std::endl;
    }

    // --- Release the Fglib library

    (void) Fg_FreeLibraries();

    // --- Release the SisoLog library

    (void) SisoLog_FreeLibrary();

    return FG_OK;
}
