#ifndef HELPER_H
#define HELPER_H

#define NDEBUG
#include "../external/readerwriterqueue/readerwriterqueue.h"

// Clear function for std::queue
template <typename T>
void clearQueue(std::queue<T>& q) {
    std::queue<T> empty;
    std::swap(q, empty);
}

template <typename T>
void clearCamel(moodycamel::BlockingReaderWriterQueue<T>& q) {
    // clear queue
    bool r;
    do {
        r = q.pop();
    } while (r);
}

#endif  // HELPER_H
