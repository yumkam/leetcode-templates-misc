#ifndef MODARITH_V2_HPP
#define MODARITH_V2_HPP
#include <cinttypes>
#include <limits>
#include <type_traits>
#include <tuple>
#include <cassert>
namespace std { // XXX HACK WARNING: altering std:: namespace is generally bad thing
    template <> struct make_signed<unsigned __int128> { typedef __int128 type; };
    template <> struct make_unsigned<__int128> { typedef unsigned __int128 type; };
}
namespace modarith {
    namespace {
        template <typename T> struct double_int {};
        template <> struct double_int<uint8_t> { typedef uint16_t type; };
        template <> struct double_int<uint16_t> { typedef uint32_t type; };
        template <> struct double_int<uint32_t> { typedef uint64_t type; };
        template <> struct double_int<uint64_t> { typedef unsigned __int128 type; };
        template <typename T> using make_double_int_t = std::conditional_t<std::is_signed_v<T>, std::make_signed_t<typename double_int<std::make_unsigned_t<T>>::type>, typename double_int<T>::type>;
    };

    template <typename T = uint32_t, T MOD = 1'000'000'007>
    struct mod {
        typedef T type;
	static constexpr T modulo = MOD;
        // a, b \in [0, modulo)
        static constexpr T add(T a, T b) { // a += b
            static_assert(modulo - 1 <= std::numeric_limits<decltype(b)>::max() - modulo);
            if ((a += b) >= modulo)
                a -= modulo;
            return a;
        }
        // a, b \in [0, modulo)
        static constexpr T sub(T a, T b) { // a -= b
            static_assert(modulo <= std::numeric_limits<decltype(b)>::max() - (modulo - 1));
            if (a < b)
                a += modulo;
            return a - b;
        }
        // a \in [0, modulo)
        static constexpr T neg(T a) { // a = -a
            return a == 0 ? a : modulo - a;
        }
        // a, b \in [0, modulo)
        static constexpr T mul(T a, T b) { // a *= b
            make_double_int_t<T> c = a;
            static_assert(modulo - 1 <= std::numeric_limits<decltype(c)>::max()/(modulo - 1));
            return (c*b) % modulo;
        }
        static constexpr T addmul(T a, T b, T c) {
            using double_type = make_double_int_t<T>;
            static_assert(modulo - 1 <= (std::numeric_limits<double_type>::max() - (modulo - 1))/(modulo - 1));
            return (a + b*static_cast<double_type>(c)) % modulo;
        }
        // a \in [0, modulo), n integer; time = O(log n)
        template <typename N>
        static constexpr T pow(T a, N n) { // a**n
            if (!n)
                return 1;
            for (; (n & 1) == 0; n >>= 1)
                a = mul(a, a);
            T r = a; n >>= 1;
            for(; n; n >>= 1) {
                a = mul(a, a);
                if ((n & 1))
                    r = mul(r, a);
            }
            return r;
        }
        // a \in [0, modulo); time = O(log modulo)
        static constexpr T inv(T a) { // (1/a)
            // static_assert(is_prime(modulo));
            return pow(a, modulo - 2);
        }
        private:
            static constexpr T gcd(T a, T b, std::make_signed_t<T> & x, std::make_signed_t<T> & y) {
                x = 1; y = 0;
                std::make_signed_t<T> x1 = 0, y1 = 1;
                while (b) {
                    std::make_signed_t<T> q = a / b; // 0 <= q < a; q * b <= a
                    std::tie(x, x1) = std::make_pair ( x1, x - q * x1 );
                    std::tie(y, y1) = std::make_pair ( y1, y - q * y1 );
                    std::tie(a, b)  = std::make_pair ( b,  a - q * b  );
                }
                // assert(abs(x) < modulo);
                return a;
            }
        public:
        static constexpr T inv_alt(T a) {
            std::make_signed_t<decltype(a)> x = 1, y = 0;
            static_assert(modulo <= std::numeric_limits<decltype(x)>::max());
            auto g = gcd(a, modulo, x, y);
            assert(g == 1);
            if (x < 0) x += modulo;
            return x;
        }
        // a, b, r \in [0, modulo), n integer; time = O(log n)
        template <typename N>
        static constexpr T mulpow(T r, T a, N n) { // r*(a**n)
            for(;;) {
                if ((n & 1))
                    r = mul(r, a);
                if ((n >>= 1) == 0)
                    break;
                a = mul(a, a);
            }
            return r;
        }
        // a, b \in [0, modulo); time = O(log modulo)
        static constexpr T divmod(T a, T b) { // a/b
            // static_assert(is_prime(modulo));
            return mulpow(a, b, modulo - 2);
        }
        // n integer; time = O(n)
        template <typename N>
        static constexpr T fact(N n) { // n!
            T r = 1;
            for(; n > 1; n--)
                r = mul(r, n);
            return r;
        }
        // time = O(min(k, n - k) + log modulo)
        template <typename N>
        static constexpr T choose(N n, N k) { // n choose k
            if (k > n - k)
                k = n - k;
            N num = 1, denum = 1;
            for (N i = 1; i <= k; ++i) {
                num = mul(num, n - k + i);
                denum = mul(denum, i);
            }
            return div(num, denum);
        }
        static constexpr T reduce(T a) {
            return a % modulo;
        }
        static constexpr T reduce(std::make_signed_t<T> a) {
            static_assert(modulo <= std::numeric_limits<decltype(a)>::max());
            a %= static_cast<decltype(a)>(modulo);
            if (a < 0) a += static_cast<decltype(a)>(modulo);
            return a;
        }
        static constexpr T reduce(make_double_int_t<T> A) {
            return A % modulo;
        }
        static constexpr T reduce(std::make_signed_t<make_double_int_t<T>> A) {
            std::make_signed_t<T> a = A % (std::make_signed_t<T>)modulo;
            if (a < 0) a += (std::make_signed_t<T>)modulo;
            return a;
        }
    };
    // 2**61 - 1 is meersene prime
    // optimized versions (branchless & no 128-bit division/modulo)
    static constexpr uint64_t prime61 = (static_cast<uint64_t>(1)<<61) - 1;
    template <>
    constexpr uint64_t mod<uint64_t, prime61>::add(uint64_t a, uint64_t b) {
        // requires: a < modulo && b <= modulo || a <= modulo && b < modulo
        a += b + 1;
        a = (a & modulo) + (a>>61);
        a = (a & modulo) + (a>>61);
        return a - 1;
    }
    template <>
    constexpr uint64_t mod<uint64_t, prime61>::reduce(uint64_t A) {
        A = (A & modulo) + (A>>61) + 1; // [1:modulo + 1<<3]
        A = (A & modulo) + (A>>61); // [1:modulo]
        return A - 1; // [0:modulo)
    }
    template <>
    constexpr uint64_t mod<uint64_t, prime61>::reduce(make_double_int_t<uint64_t> A) {
        A = (A & modulo) + (A>>61); // [0:1<<67)
        return reduce(
                (uint64_t)((uint64_t)(A & modulo) + (uint64_t)(A>>61) // [0:modulo+1<<6)
                    ));
    }
    template <>
    constexpr uint64_t mod<uint64_t, prime61>::sub(uint64_t a, uint64_t b) {
        return add (a, 2*modulo - b);
    }
    template <>
    constexpr uint64_t mod<uint64_t, prime61>::mul(uint64_t a, uint64_t b) {
        //assert(a < (1ull<<62));
        //assert(b < (1ull<<62));
#ifdef __SIZEOF_INT128__
        auto x = ((unsigned __int128)a)*b; // 1<<124
        auto ret = (uint64_t)(x & modulo) + (uint64_t)(x >> 61); // 1<<63 + modulo
#else
        uint64_t l1 = (uint32_t)a, h1 = a>>32, l2 = (uint32_t)b, h2 = b>>32;
        uint64_t l = l1*l2, m = l1*h2 + l2*h1, h = h1*h2;
        uint64_t ret = (l & modulo) + (l >> 61) + (h << 3) + (m >> 29) + (m << 35 >> 3);
#endif
        return reduce(ret);
    }
    typedef mod<> m1x7;
    typedef mod<uint32_t, 998244353> m9x3;
    typedef mod<uint64_t, prime61> mod61;
}
#endif // MODARITH_V2_HPP
// vim: sw=4 et
