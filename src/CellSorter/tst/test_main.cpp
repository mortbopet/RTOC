#include <cassert>

#include "../lib/prettyprint.h"

int test_parameterClass();
int test_framefinder();
int test_datacontainer();

int main() {
    PP::header("parameterClass Test");
    assert(test_parameterClass() == 0);
    PP::header("FrameFinder Test");
    assert(test_framefinder() == 0);
    PP::header("DataContainer Test");
    assert(test_datacontainer() == 0);
    return 0;
}
