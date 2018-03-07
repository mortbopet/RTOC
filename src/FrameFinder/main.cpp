#include <stdio.h>
#include <iostream>

#include <thread>
#include "framefinder.h"
#include "timer.h"

using namespace std;
using namespace timer;

#define _PATH_DATA "../../../data/ImgD1/"
#define _PATH_ACCEPTED "./accepted.txt"
#define _PATH_DISCARDED "./discarded.txt"

int main() {
    Timer t;

    vector<string> a;

    remove(_PATH_ACCEPTED);
    remove(_PATH_DISCARDED);

    tic(t);
    std::cout << framefinder::get_files_sorted(a, _PATH_DATA, framefinder::FF_FULL_PATH) << std::endl;
    toc(t);
    std::cout << a.size() << std::endl;

    tic(t);
    framefinder::accept_or_reject(a, _PATH_DATA, _PATH_ACCEPTED, _PATH_DISCARDED);
    toc(t);

    return 0;
}
