#include <iostream>

#include "FFhelpers.h"
#include "timer.h"

using namespace std;

/**
 * Set _PATH_DATA to the folder in which your images recide
 *
 */
#define _PATH_DATA      "/Users/jl/Downloads/fagprojekt/data/ImgD1/"
#define _PATH_ACCEPTED  "./accepted.txt"
#define _PATH_DISCARDED "./discarded.txt"

int main() {
    chrono::steady_clock::time_point t0;

    vector<string> a;

    timer::tic(t0);
    FF::get_files_sorted(a, _PATH_DATA, FF::FF_ONLY_FILE);
    timer::tocp(t0);

    timer::tic(t0);
    FF::accept_or_reject(a, _PATH_DATA, _PATH_ACCEPTED,_PATH_DISCARDED);
    timer::tocp(t0);

    return 0;

}
