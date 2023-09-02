#ifndef MODARITH_HPP
#define MODARITH_HPP
#include <cinttypes>
#include <limits>
namespace modarith {
    static constexpr unsigned modulo = 1000000000+7;
    static_assert(modulo - 1 <= std::numeric_limits<unsigned>::max() - (modulo - 1));
    static void addmod(unsigned &a, unsigned b) { // a += b
        if ((a += b) >= modulo)
            a -= modulo;
    }
    static void submod(unsigned &a, unsigned b) { // a -= b
        if (a < b)
            a += modulo;
        a -= b;
    }
    static void mulmod(unsigned &a, unsigned b) { // a *= b
        uint64_t c = a;
        static_assert(modulo - 1 < std::numeric_limits<decltype(c)>::max()/(modulo - 1));
        a = (c*b) % modulo;
    }
    static unsigned powmod(unsigned a, unsigned n) { // a**n
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
    static unsigned invmod(unsigned a) { // (1/a)
        // static_assert(is_prime(modulo));
        return powmod(a, modulo - 2);
    }
    static unsigned mulpowmod(unsigned r, unsigned a, unsigned n) { // r*(a**n)
        for(;;) {
            if ((n & 1))
                mulmod(r, a);
            if ((n >>= 1) == 0)
                break;
            mulmod(a, a);
        }
        return r;
    }
    static unsigned divmod(unsigned a, unsigned b) { // a/b
        // static_assert(is_prime(modulo));
        return mulpowmod(a, b, modulo - 2);
    }
    static unsigned factmod(unsigned n) { // n!
        unsigned r = 1;
        for(; n > 1; n--)
            mulmod(r, n);
        return r;
    }
    static unsigned choosemod(unsigned n, unsigned k) { // n choose k
        if (k > n - k)
            k = n - k;
        unsigned num = 1, denum = 1;
        for (unsigned i = 1; i <= k; ++i) {
            mulmod(num, n - k + i);
            mulmod(denum, i);
        }
        return divmod(num, denum);
    }
}
#endif // MODARITH_HPP
