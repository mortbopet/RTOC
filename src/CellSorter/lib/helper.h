#ifndef HELPER_H
#define HELPER_H

// Clear function for std::queue
template <typename T>
void clearQueue(std::queue<T>& q) {
    std::queue<T> empty;
    std::swap(q, empty);
}

#endif // HELPER_H
