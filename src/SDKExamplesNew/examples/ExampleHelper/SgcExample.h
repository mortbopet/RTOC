#pragma once
#include <memory>
#include <stdint.h>
#include <vector>

#include <siso_genicam.h>

#include "FgExample.h"
#include "SgcWrapper.h"
#include "FgAcqBlockExample.cpp"


class SgcExample: public FgAcqBlockExample{
public:
    SgcExample();
    ~SgcExample(){};

    void throwSgcLastError();

protected:

};