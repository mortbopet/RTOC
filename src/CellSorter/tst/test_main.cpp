#include <cassert>

int tst_parameterClass();
int test_framefinder();

int main() {
    assert(tst_parameterClass() == 0);
    assert(test_framefinder() == 0);
    return 0;
}
