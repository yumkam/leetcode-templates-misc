#ifndef MODARITH_HPP
#define MODARITH_HPP
#include <cinttypes>
#include <limits>
#include <type_traits>
#include <tuple>
#include <array>
#include <vector>
#include <cassert>
namespace modarith {
    namespace detail {
        template <class T, std::size_t N>
        constexpr T mularray(const std::array<T, N> &a, std::size_t i) {
            return i == 0 ? T {1} : mularray(a, i - 1)*a[i - 1];
        }
        template <class T, std::size_t N>
        constexpr T mularray(const std::array<T, N> &a) {
            return mularray(a, N);
        }
    }
    static constexpr std::array<unsigned, 2> modprimes { 7, 191 };
#if 1
    static constexpr unsigned modulo = 1337;
    static_assert(detail::mularray(modprimes) == modulo);
#else
    static constexpr unsigned modulo = detail::mularray(modprimes);
#endif
    namespace detail {
        template <class T, std::size_t N>
        constexpr T mularray1(const std::array<T, N> &a, std::size_t i) {
            return i == 0 ? T{1} : mularray1(a, i - 1)*(a[i - 1] - (i == N || a[i] != a[i - 1]));
        }
        template <class T, std::size_t N>
        constexpr T mularray1(const std::array<T, N> &a) {
            return mularray1(a, N);
        }
    }
    // n*\prod(1-1/p) over unique p or \prod p_i^{k_i-1}(p_i - 1)
    static constexpr unsigned phi = detail::mularray1(modprimes);
    static_assert((modulo != 1337) || (phi == 1140));

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
    // a \in [0, modulo)
    static constexpr void negmod(unsigned &a) { // a = -a
        if (a != 0)
            a = modulo - a;
    }
    // a, b \in [0, modulo)
    static constexpr void mulmod(unsigned &a, unsigned b) { // a *= b
        uint64_t c = a;
        static_assert(modulo - 1 <= std::numeric_limits<decltype(c)>::max()/(modulo - 1));
        a = (c*b) % modulo;
    }
    static constexpr void addmulmod(unsigned &a, unsigned b, unsigned c) {
        static_assert(modulo - 1 <= (std::numeric_limits<uint64_t>::max() - (modulo - 1))/(modulo - 1));
        a = (a + b*static_cast<uint64_t>(c)) % modulo;
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
        return powmod(a, phi - 1);
    }
    namespace detail {
        template <typename T>
        static constexpr T gcd(T a, T b, std::make_signed_t<T> & x, std::make_signed_t<T> & y) {
            x = 1; y = 0;
            std::make_signed_t<T> x1 = 0, y1 = 1;
            static_assert(modulo <= std::numeric_limits<decltype(x1)>::max());
            while (b) {
                std::make_signed_t<T> q = a / b; // 0 <= q < a; q * b <= a
                std::tie(x, x1) = std::make_pair ( x1, x - q * x1 );
                std::tie(y, y1) = std::make_pair ( y1, y - q * y1 );
                std::tie(a, b)  = std::make_pair ( b,  a - q * b  );
            }
            // assert(abs(x) < modulo);
            return a;
        }
    }
    static constexpr unsigned invmod_alt(unsigned a) {
        std::make_signed_t<decltype(a)> x = 1, y = 0;
        auto g = detail::gcd(a, modulo, x, y);
        assert(g == 1);
        if (x < 0) x += modulo;
        return x;
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
        return mulpowmod(a, b, phi - 1);
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
    static constexpr unsigned reducemod(uint64_t A) {
        return A % modulo;
    }
    static constexpr unsigned reducemod(int64_t A) {
        int a = A % (int)modulo;
        if (a < 0) a += (int)modulo;
        return a;
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
        std::array<unsigned, modprimes.size()> pcnt {};
        for (unsigned i = 1; i <= k; ++i) {
            {
                unsigned v = n - i + 1;
                for (unsigned j = 0; j < modprimes.size(); ++j)
                    while(v % modprimes[j] == 0) {
                        v /= modprimes[j];
                        ++pcnt[j];
                    }
                mulmod(num, reducemod(v));
            }
            {
                unsigned v = i;
                for (unsigned j = 0; j < modprimes.size(); ++j)
                    while(v % modprimes[j] == 0) {
                        v /= modprimes[j];
                        assert(pcnt[j]);
                        --pcnt[j];
                    }
                mulmod(denum, reducemod(v));
            }
        }
        {
            unsigned v = num;
            for (unsigned j = 0; j < modprimes.size(); ++j) {
                v = mulpowmod(v, modprimes[j], pcnt[j]);
            }
            return divmod(v, denum);
        }
    }
    // return vector of choosemod(n, k)
    template <class Result = std::vector<unsigned>>
    Result pascalmod(unsigned rowIndex) {
        Result row(rowIndex + 1, 1);
        unsigned num = 1, denum = 1;
        std::array<unsigned, modprimes.size()> pcnt {};
        const unsigned half = rowIndex/2;
        // Note: when modulo is small, it is more efficient to pre-calculate invmod(range(modulo))
        std::array<unsigned, modulo < 1024 ? modulo : 0> inv;
        // It may be more efficient to pre-calculate pows
        std::array<std::vector<unsigned>, true ? modprimes.size() : 0> pows;
        for (unsigned j = 1; j < inv.size(); ++j)
            inv[j] = invmod(j);
        for (unsigned i = 1; i <= half; ++i) {
            // if gcd(denum, modulo) != 0 (denum is multiply of modprimes[i]), there are no inversion;
            // however, amount of multiplies of modprimes[i] in num is always greater-or-equal,
            // to denum (num is always divisible by denum);
            // hence, we can handle them separately
            {
                unsigned v = rowIndex - i + 1;
                for (unsigned j = 0; j < modprimes.size(); ++j)
                    while(v % modprimes[j] == 0) {
                        v /= modprimes[j];
                        ++pcnt[j];
                    }
                mulmod(num, reducemod(v));
            }
            {
                unsigned v = i;
                for (unsigned j = 0; j < modprimes.size(); ++j)
                    while(v % modprimes[j] == 0) {
                        v /= modprimes[j];
                        assert(pcnt[j]);
                        --pcnt[j];
                    }
                mulmod(denum, reducemod(v));
            }
            unsigned v = num;
            for (unsigned j = 0; j < modprimes.size(); ++j) {
                if (pcnt[j] == 0) continue;
                if (pows.empty())
                    v = mulpowmod(v, modprimes[j], pcnt[j]);
                else {
                    for (unsigned k = pows[j].size(), pow = k ? pows[j][k - 1] : 1; k < pcnt[j]; ++k)  {
                        mulmod(pow, modprimes[j]);
                        pows[j].push_back(pow);
                    }
                    mulmod(v, pows[j][pcnt[j] - 1]);
                }
            }
            if constexpr (inv.empty())
                row[i] = divmod(v, denum);
            else {
                mulmod(v, inv[denum]);
                row[i] = v;
            }
        }
        for (unsigned k = 1; k <= half; k++)
            row[rowIndex - k] = row[k];
        return row;
    }
}
#endif // MODARITH_HPP
// vim: sw=4 et
