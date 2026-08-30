#ifndef MODARITH_HPP
#define MODARITH_HPP
#include <cinttypes>
#include <limits>
#include <vector>
#include <type_traits>
#include <tuple>
#include <cassert>
namespace modarith {
    static constexpr unsigned modulo = 1000000000+7;
    //static constexpr unsigned modulo = 998244353;
    // a, b \in [0, modulo)
    static constexpr void addmod(unsigned &a, unsigned b) { // a += b
        static_assert(modulo - 1 <= std::numeric_limits<decltype(b)>::max() - (modulo - 1));
        if ((a += b) >= modulo)
            a -= modulo;
    }
    // a, b \in [0, modulo)
    static constexpr void submod(unsigned &a, unsigned b) { // a -= b
        static_assert(modulo <= std::numeric_limits<decltype(b)>::max() - (modulo - 1));
        if (a < b)
            a += modulo;
        a -= b;
    }
    // a, b \in [0, modulo)
    static constexpr void mulmod(unsigned &a, unsigned b) { // a *= b
        uint64_t c = a;
        static_assert(modulo - 1 <= std::numeric_limits<decltype(c)>::max()/(modulo - 1));
        a = (c*b) % modulo;
    }
    // a \in [0, modulo), n integer; time = O(log n)
    template <typename N>
    static constexpr unsigned powmod(unsigned a, N n) { // a**n
        if (!n)
            return 1;
        for (; (n & 1) == 0; n >>= 1)
            mulmod(a, a);
        unsigned r = a; n >>= 1;
        for(; n; n >>= 1) {
            mulmod(a, a);
            if ((n & 1))
                mulmod(r, a);
        }
        return r;
    }
    // a \in [0, modulo); time = O(log modulo)
    static constexpr unsigned invmod(unsigned a) { // (1/a)
        // static_assert(is_prime(modulo));
        return powmod(a, modulo - 2);
    }
    // a \in [0, modulo)
    static constexpr void negmod(unsigned &a) { // a = -a
        if (a != 0)
            a = modulo - a;
    }
    static constexpr void addmulmod(unsigned &a, unsigned b, unsigned c) {
        static_assert(modulo - 1 <= (std::numeric_limits<uint64_t>::max() - (modulo - 1))/(modulo - 1));
        a = (a + b*static_cast<uint64_t>(c)) % modulo;
    }
    // a, b, r \in [0, modulo), n integer; time = O(log n)
    template <typename N>
    static constexpr unsigned mulpowmod(unsigned r, unsigned a, N n) { // r*(a**n)
        for(;;) {
            if ((n & 1))
                mulmod(r, a);
            if ((n >>= 1) == 0)
                break;
            mulmod(a, a);
        }
        return r;
    }
    // a, b \in [0, modulo); time = O(log modulo)
    static constexpr unsigned divmod(unsigned a, unsigned b) { // a/b
        // static_assert(is_prime(modulo));
        return mulpowmod(a, b, modulo - 2);
    }
    // n integer; time = O(n)
    static constexpr unsigned factmod(unsigned n) { // n!
        unsigned r = 1;
        for(; n > 1; n--)
            mulmod(r, n);
        return r;
    }
    // time = O(min(k, n - k) + log modulo)
    static constexpr unsigned choosemod(unsigned n, unsigned k) { // n choose k
        if (k > n - k)
            k = n - k;
        unsigned num = 1, denum = 1;
        for (unsigned i = 1; i <= k; ++i) {
            mulmod(num, n - k + i);
            mulmod(denum, i);
        }
        return divmod(num, denum);
    }
    // return vector of choosemod(n, k)
    template <bool do_half = false, class Result = std::vector<unsigned>>
    Result pascalmod(unsigned rowIndex) {
        unsigned num = 1, denum = 1;
        const unsigned half = rowIndex/2;
        Result row((do_half ? half : rowIndex) + 1, 1);
        for (unsigned i = 1; i <= half; ++i) {
            mulmod(num, rowIndex - i + 1);
            mulmod(denum, i);
            row[i] = divmod(num, denum);
        }
        if (!do_half) {
            for (unsigned k = 1; k <= half; k++)
                row[rowIndex - k] = row[k];
        }
        return row;
    }
    auto choosefromhalf(auto n, auto k, const auto& v) {
        if (auto k1 = n - k; k1 < k)
            k = k1;
        return v[k];
    }
    // n integer; time = O(n)
    template <class Result = std::vector<unsigned>>
    static constexpr Result invfactmodv(unsigned n) { // { (0!)^-1,...,(n!)^-1 }
        Result res(n + 1);
        auto invfactmod = res[n] = invmod(factmod(n));
        for (auto i = n; i--; ) {
            mulmod(invfactmod, i + 1); // inv(i!) = inv((i + 1)!)*(i + 1)
            res[i] = invfactmod;
        }
        unsigned r = 1;
        for(; n > 1; n--)
            mulmod(r, n);
        return r;
    }
    static constexpr unsigned reducemod(unsigned a) {
        return a % modulo;
    }
    static constexpr unsigned reducemod(int a) {
        static_assert(modulo <= std::numeric_limits<int>::max());
        a %= static_cast<int>(modulo);
        if (a < 0) a += static_cast<int>(modulo);
        return a;
    }
    static constexpr unsigned reducemod(unsigned long A) {
        if (__builtin_constant_p(A < modulo) && A < modulo) return A;
        return A % modulo;
    }
    static constexpr unsigned reducemod(unsigned long long A) {
        if (__builtin_constant_p(A < modulo) && A < modulo) return A;
        return A % modulo;
    }
    static constexpr unsigned reducemod(long A) {
        int a = A % (int)modulo;
        if (a < 0) a += (int)modulo;
        return a;
    }
    static constexpr unsigned reducemod(long long A) {
        int a = A % (int)modulo;
        if (a < 0) a += (int)modulo;
        return a;
    }
    // alternative implementation of invmod
    // extended gcd
    template <typename T>
    static constexpr T extended_gcd(T a, T b, std::make_signed_t<T> & x, std::make_signed_t<T> & y) {
        x = 1; y = 0;
        std::make_signed_t<T> x1 = 0, y1 = 1;
        static_assert(modulo <= std::numeric_limits<decltype(x1)>::max());
        while (b) {
            std::make_signed_t<T> q = a / b; // 0 <= q <= a; q * b <= a
            std::tie(x, x1) = std::make_pair ( x1, x - q * x1 );
            std::tie(y, y1) = std::make_pair ( y1, y - q * y1 );
            std::tie(a, b)  = std::make_pair ( b,  a - q * b  );
        }
        // assert(abs(x) < modulo);
        return a;
    }
    namespace alt {
        static constexpr unsigned invmod(unsigned a) {
            std::make_signed_t<decltype(a)> x = 1, y = 0;
            auto g = extended_gcd(a, modulo, x, y);
            assert(g == 1);
            if (x < 0) x += modulo;
            return x;
        }
    }
}
#endif // MODARITH_HPP
// vim: sw=4 et
