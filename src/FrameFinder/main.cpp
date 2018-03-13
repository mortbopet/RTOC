#include <cstdio>
#include <iostream>

#include "framefinder.h"
#include "timer.h"

#define _PATH_DATA "/Users/jl/Dropbox/DTU/04_Semester4/fagprojekt/data/ImgD1/"

int main() {
    timer::Timer t;

    std::vector<Frame> a, b, c;

    tic(t);
    get_files(a, _PATH_DATA);
    std::cout << "Found: " << a.size() << std::endl;
    toc(t);

    tic(t);
    accept_or_reject(a, _PATH_DATA, (double)0.0352);
    std::cout << "Checked: " << a.size() << std::endl;
    toc(t);

    tic(t);
    get_accepted(a, b);
    std::cout << "Accepted: " << b.size() << std::endl;
    toc(t);

    tic(t);
    get_rejected(a, c);
    std::cout << "Rejected: " << c.size() << std::endl;
    toc(t);

    std::cout << "Total: " << b.size() + c.size() << std::endl;
    return 0;
}
