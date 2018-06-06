#ifndef PRETTYPRINT_H
#define PRETTYPRINT_H

#include <iostream>
#include <string>

namespace PP {

#define PRINTWIDTH 80
using namespace std;

void header(string in) {
    int fillChars = PRINTWIDTH - in.length() - 2;
    for (int i = 0; i < fillChars / 2; i++) {
        cout << '-';
    }
    cout << ' ';
    cout << in;
    cout << ' ';
    for (int i = 0; i < fillChars / 2; i++) {
        cout << '-';
    }
    cout << endl;
}
}  // namespace PP

#endif
