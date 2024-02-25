//#include <iostream>
#ifndef ISQRT_HPP
#define ISQRT_HPP 1
#include <algorithm>
#include <limits>
#include <cmath>
#include <cassert>
template<typename T>
static T floor_isqrt(T v) {
    assert(v >= 0);
    T lo = 0;
    static const T limit = std::floor(std::sqrt(static_cast<long double>(std::numeric_limits<T>::max())));
    T hi = std::min(static_cast<T>(v/2 + (v&1)), limit);
    while(lo < hi) {
        T mid = lo + (hi - lo)/2 + ((hi - lo) & 1);
        if (mid*mid > v)
            hi = mid - 1;
        else
            lo = mid;
    }
    return lo;
}
template<typename T>
static T ceil_isqrt(T v) {
    assert(v >= 0);
    T lo = 0;
    static const T limit = std::ceil(std::sqrt(static_cast<long double>(std::numeric_limits<T>::max())));
    T hi = std::min(static_cast<T>(v/2 + 1), limit);
    while(lo < hi) {
        T mid = lo + (hi - lo)/2;
        if (mid*mid < v)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}
#endif // ISQRT_HPP
