#include <iostream>

#include "FFhelpers.h"

using namespace std;

/**
 * Set _PATH_DATA to the folder in which your images recide
 *
 */
#define _PATH_DATA "../../data/ImgD1/"
#define _PATH_ACCEPTED "./accepted.txt"
#define _PATH_DISCARDED "./discarded.txt"

int main() {
    vector<string> a;

    FF::get_files_sorted(a, _PATH_DATA, FF::FF_ONLY_FILE);

    FF::accept_or_reject(a, _PATH_DATA, _PATH_ACCEPTED, _PATH_DISCARDED);

    return 0;
}
