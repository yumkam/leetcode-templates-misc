//#include <iostream>
#ifndef ISQRT_FAST_HPP
#define ISQRT_FAST_HPP 1
#include <algorithm>
#include <limits>
#include <climits>
#include <cmath>
#include <cassert>
#include <bit>
template<typename T>
static T floor_isqrt_fast(T v) {
    assert(v >= 0);
    if (v == 0)
        return 0;
    auto shift = (sizeof(T)*CHAR_BIT - 1 - std::countl_zero(std::make_unsigned_t<T>{v}))/2;
    T lo = 1;
    lo <<= shift;
    T hi = lo<<1;
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
static T ceil_isqrt_fast(T v) {
    assert(v >= 0);
    if (v == 0)
        return 0;
    auto shift = (sizeof(T)*CHAR_BIT - 1 - std::countl_zero(std::make_unsigned_t<T>{v}))/2;
    T lo = 1;
    lo <<= shift;
    T hi = lo<<1;
    while(lo < hi) {
        T mid = lo + (hi - lo)/2;
        if (mid*mid < v)
            lo = mid + 1;
        else
            hi = mid;
    }
    return lo;
}
#endif // ISQRT_FAST_HPP
