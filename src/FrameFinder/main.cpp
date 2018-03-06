#include <iostream>
#include <stdio.h>

#include "framefinder.h"
#include "timer.h"
#include <thread>

using namespace std;
using namespace timer;

/**
 * Set _PATH_DATA to the folder in which your images recide
 *
 */
#define _PATH_DATA "../../data/ImgD1/"
#define _PATH_ACCEPTED "./accepted.txt"
#define _PATH_DISCARDED "./discarded.txt"

int main() {
    Timer t;

    vector<string> a;

    remove(_PATH_ACCEPTED);
    remove(_PATH_DISCARDED);

    tic(t);
    framefinder::get_files_sorted(a, _PATH_DATA, framefinder::FF_ONLY_FILE);
    toc(t);

    tic(t);
    framefinder::accept_or_reject(a, _PATH_DATA, _PATH_ACCEPTED,_PATH_DISCARDED);
    toc(t);


    return 0;
}
