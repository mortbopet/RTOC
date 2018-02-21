#include "FgAcqBlockExample.cpp"

class FgAcqMasterSlaveExample : public FgAcqBlockExample{

public:
    int32_t run();

protected:
    bool selectMasterSlaveDialog();
};