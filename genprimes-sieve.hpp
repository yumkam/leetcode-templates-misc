//#include <iostream>
#ifndef GENPRIMES_SIEVE_HPP
#define GENPRIMES_SIEVE_HPP 1
#include <algorithm>
#include <vector>
#include <numeric>
#include <cassert>
#include "isqrt.hpp"
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
    for (T n = 3, nsq = n*n, i = 0, l = 2*segsize + 3; nsq < l; ++i, nsq += 4*n + 4, n += 2)
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
    std::vector<T> primes;
    if (maxv >= 2)
        primes.push_back(2);
    if (maxv < 3)
        return primes;
    const T segsize = (maxv - 3)/2 + 1;
    std::vector<bool> segment(segsize);
    for (T n = 3, nsq = n*n, i = 0; nsq <= maxv; ++i, nsq += 4*n + 4, n += 2)
        if (!segment[i])
            for (T j = (nsq - 3)/2; j < segsize; j += n)
                segment[j] = true;
    for (T i = 0; i < segsize; ++i)
        if (!segment[i])
            primes.push_back(2*i + 3);
    return primes;
}
template<typename T>
static std::vector<T> genprimes_simple3(T maxv) {
    // time complexity = O(n log log n)
    // space complexity = O(n) [bits]
    // result size = O(n/ln n)
    const T segsize = (maxv + 2)/3;
    std::vector<bool> segment(segsize);
    std::vector<T> primes;
    if (maxv >= 2)
        primes.push_back(2);
    if (maxv >= 3)
        primes.push_back(3);
    for (T i = 1, i6 = 6, iq3 = 3, k = 5, kq = k*k;; iq3 += i6 + 3, i6 += 6) {
        if (kq > maxv)
            break;
        if (!segment[i]) {
            for (T j = iq3 + (i<<(1 + (i & 1))) + (i & 1); j < segsize; j += 2*k)
                segment[j] = true;
            for (T j = iq3 + i6 + (i & 1) + 1; j < segsize; j += 2*k)
                segment[j] = true;
        }
        ++i;
        kq += (k + 1 + (i & 1))<<(2 + (i & 1));
        k += 2<<(i & 1);
    }
    for (T i = 1, k = 4; (k | 1) <= maxv; ++i, k += 3)
        if (!segment[i])
            primes.push_back((k | 1));
    return primes;
}
// return lpd, where lpd[i] is index of least prime divisor of i*2 + 3 (equal to i if it is prime)
// (for even numbers, least prime divisor is always 2)
template<typename T>
static std::vector<T> gen_least_prime_divisors_simple2(T maxv) {
    // time complexity = O(n log log n)
    // result size = O(n)
    const T segsize = (std::max<T>(maxv, 3) - 3)/2 + 1;
    std::vector<T> segment(segsize);
    std::iota(segment.begin(), segment.end(), (T)0);
    for (T n = 3, nsq = n*n, i = 0; nsq <= maxv; ++i, nsq += 4*n + 4, n += 2)
        if (segment[i] == i) {
            for (T j = (nsq - 3)/2; j < segsize; j += n)
                if (auto &sj = segment[j]; sj == j)
                    sj = i;
        }
    return segment;
}
// return lpd, where
//      lpd[i] < i: if 2*i + 3 is not a prime, index of least prime divisor
//      lpd[i] > i: if 2*i + 3 is a prime, index of next prime
//      lpd[i] = i: if 2*i + 3 is the last prime in segment
template<typename T>
static std::vector<T> gen_least_prime_divisors_list_simple2(T maxv) {
    // time complexity = O(n log log n)
    // result size = O(n)
    const T segsize = (std::max<T>(maxv, 3) - 3)/2 + 1;
    std::vector<T> segment(segsize);
    std::iota(segment.begin(), segment.end(), (T)0);
    unsigned last = 0;
    for (T n = 3, nsq = n*n, i = 0; nsq <= maxv; ++i, nsq += 4*n + 4, n += 2)
        if (segment[i] == i) {
            segment[last] = i;
            last = i;
            for (T j = (nsq - 3)/2; j < segsize; j += n)
                if (auto &sj = segment[j]; sj == j)
                    sj = i;
        }
    return segment;
}
// return lpd, where lpd[i] is largest prime divisor of i*2 + 3 non-exceeding sqrt(i*2 + 3)
// (equal to i*2 + 3 if it is prime)
template<typename T>
static std::vector<T> gen_prime_divisors_simple2(T maxv) {
    // time complexity = O(n log log n)
    // result size = O(n)
    const T segsize = (std::max<T>(maxv, 3) - 3)/2 + 1;
    std::vector<T> segment(segsize);
    for (T i = 0, n = 3; i < segsize; ++i, n += 2)
        segment[i] = n;
    for (T i = 0, n = 3, nsq = n*n; nsq <= maxv; ++i, nsq += 4*n + 4, n += 2)
        if (segment[i] == n) {
            for (T j = (nsq - 3)/2; j < segsize; j += n)
                segment[j] = n;
        }
    return segment;
}
#endif // GENPRIMES_SIEVE_HPP
