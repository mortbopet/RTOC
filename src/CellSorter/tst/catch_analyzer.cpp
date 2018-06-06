#include "catch.hpp"

#include "../lib/analyzer.h"
#include "../lib/datacontainer.h"

TEST_CASE("Analyzer init and setup", "[full], [analyzer]") {
    Analyzer* analyzer = new Analyzer();
    REQUIRE_NOTHROW(analyzer->loadSetup("/presets/LisaAagensen.pcs"));
    
}


