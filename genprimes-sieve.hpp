#include <iostream>
#ifndef GENPRIMES_SIEVE_HPP
#define GENPRIMES_SIEVE_HPP 1
#include <algorithm>
#include <limits>
#include <vector>
#include <cmath>
#include <cassert>
template<typename T>
static T floor_isqrt(T v) {
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
// generate primes up to maxv
template<typename T>
static std::vector<T> genprimes(T maxv, T segsize) {
    // time complexity = O(n log log n)
    // space complexity = O(sqrt(n)) [bits]
    // result size = O(n/ln n)
    if (maxv < 3)
        segsize = 0;
    else {
        if (2*segsize + 3 > maxv)
            segsize = (maxv - 3)/2 + 1;
    }
#if 0
    if (maxv > 3) {
        auto logn = sizeof(int)*8 - 1 - __builtin_clz(maxv);
        auto lnn = logn*77/111; // 77/111 =~ ln 2
        primes.reserve(maxv/lnn);
    }
#endif
    std::vector<bool> segment(segsize);
    //clog << "segs " << segsize << endl;
    std::vector<T> primes;
    if (maxv >= 2)
        primes.push_back(2);
    // regular sieve for first segment
    for (T n = 3, nsq = n*n, i = 0; i < segsize; ++i, nsq += 4*n + 4, n += 2)
        if (!segment[i])
            for (T j = (nsq - 3)/2; j < segsize; j += n)
                segment[j] = true;
    for (T i = 0; i < segsize; ++i)
        if (!segment[i])
            primes.push_back(2*i + 3);
    std::vector<T> idx;
    idx.reserve(primes.size());
    //std::clog << primes.size() << '\n';
    idx.push_back(-1);
    // generate next segments
    for (T segstart = segsize; 2*segstart + 3 <= maxv; ) {
        segsize = std::min(segsize, (maxv - (2*segstart + 3))/2 + 1);
        segment.clear();
        segment.resize(segsize);
        auto segend = segstart + segsize;
        // segment[i] represent number 2*(segstart+i) + 3
        // number n has index (n - 3)/2 - segstart
        for (size_t i = 1; i < primes.size(); ++i) {
            auto p = primes[i];
            if (i == idx.size()) {
                auto k = (p*p - 3)/2;
                if (k >= segend)
                    break;
                if (k < segstart)
                    k = k + (segstart - k + p - 1)/p*p;
                idx.push_back(k);
            }
            auto j = idx[i] - segstart;
            for (; j < segsize; j += p)
                segment[j] = true;
            idx[i] = segstart + j;
        }
        // minimum sq = ((2*ss + 3)^2 - 3) / 2 = (2^2*ss^2+2*2*ss*3+3^2-3)/2 = 
        // = 2*ss^2 + 2*ss*3 + 3
        // ss = k*size, k >= 1
        // send = (k+1)*size = k*size + size
        // sq = 2*ss^2 + 2*ss*3 + 3 = 2*(k*size)^2 + 6 * (k*size) + 3
        //    = 2*k^2*size*size + 6*k*size + 3 = (5*k*size + 3 + (2*k^2*size - 1)*size) + size + k*size > send
        //assert(((2*segstart + 3)*(2*segstart + 3) - 3)/2 >= segend);
        for (T i = 0; i < segsize; ++i)
            if (!segment[i])
                primes.push_back(2*(i + segstart) + 3);
        segstart = segend;
    }
    //std::clog << segment.capacity() << ' ' << idx.size() << '\n';
    return primes;
}
template<typename T>
static auto genprimes(T maxv) {
    return genprimes(maxv, (std::max<T>(3, floor_isqrt(maxv)) - 3)/2 + 1);
}
template<typename T>
static std::vector<T> genprimes_simple(T maxv) {
    // time complexity = O(n log log n)
    // space complexity = O(n) [bits]
    // result size = O(n/ln n)
    const T segsize = (std::max<T>(maxv, 3) - 3)/2 + 1;
    std::vector<bool> segment(segsize);
    std::vector<T> primes;
    if (maxv >= 2)
        primes.push_back(2);
    for (T n = 3, nsq = n*n, i = 0; i < segsize; ++i, nsq += 4*n + 4, n += 2)
        if (!segment[i])
            for (T j = (nsq - 3)/2; j < segsize; j += n)
                segment[j] = true;
    for (T i = 0; i < segsize; ++i)
        if (!segment[i])
            primes.push_back(2*i + 3);
    return primes;
}
#endif // GENPRIMES_SIEVE_HPP
