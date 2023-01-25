#ifndef MODINT_HPP
#define MODINT_HPP
#include <cstdint>
#include <limits>
template <typename Int> struct DoublePrec {};
template <> struct DoublePrec<int8_t> { using double_type = int16_t; };
template <> struct DoublePrec<uint8_t> { using double_type = uint16_t; };
template <> struct DoublePrec<int16_t> { using double_type = int32_t; };
template <> struct DoublePrec<uint16_t> { using double_type = uint32_t; };
template <> struct DoublePrec<int32_t> { using double_type = int64_t; };
template <> struct DoublePrec<uint32_t> { using double_type = uint64_t; };
template <typename Int, Int modulo>
class ModInt {
    // XXX assumes copy is faster/smaller than reference
    static_assert(modulo <= std::numeric_limits<Int>::max()/2);
    Int val;

    static Int mulpowmod(Int r, Int a, Int n) {
        for(;;) {
            if ((n & 1))
                r = mulmod(r, a);
            if ((n >>= 1) == 0)
                break;
            a = mulmod(a, a);
        }
        return r;
    }
    public:
    ModInt(const ModInt other):val(other.val) {}
    ModInt(const Int val):val(val) {}
    ModInt():val() {}
    static ModInt reduce(const Int val) {
	return val % modulo;
    }
    static ModInt reduce(const typename DoublePrec<Int>::double_type val) {
	return Int(val % modulo);
    }
    auto &operator += (const ModInt other) {
        if ((val += other.val) >= modulo)
            val -= modulo;
        return *this;
    }
    auto operator -= (const ModInt other) {
        if (val < other.val)
            val = val + modulo - other.val;
        else
            val = val - other.val;
        return *this;
    }
    auto &operator *= (const ModInt other) {
        typename DoublePrec<Int>::double_type temp = val;
        temp *= other.val;
        val = static_cast<Int>(temp % modulo);
	return *this;
    }
    auto operator * (const ModInt other) {
        typename DoublePrec<Int>::double_type temp = val;
        temp *= other.val;
        return ModInt(static_cast<Int>(temp % modulo));
    }

    template <typename AnyInt>
    ModInt operator ^= (AnyInt n) { // power
	if (!n)
	    return *this;
	auto a = *this;
	if (n < 0) {
	    a = ~a;
	    if (n == std::numeric_limits<AnyInt>::min()) {
		static_assert(0 == (std::numeric_limits<AnyInt>::min() & 1));
		a *= a;
		n >>= 1;
	    }
	    n = -n;
	}
	for (; (n & 1) == 0; n >>= 1)
	    a *= a;
	*this = a;
	while((n >>= 1)) {
	    a *= a;
	    if ((n & 1))
		*this *= a;
	}
	return *this;
    }

    auto operator / (ModInt b) {
	//static_assert(is_prime(modulo));
	return mulpowmod(val, b.val, modulo - 2);
    }

    auto operator /= (ModInt b) {
	//staic_assert(is_prime(modulo));
	return *this = mulpowmod(val, b.val, modulo - 2);
    }

    auto operator ~() { // modular invert
	//static_assert(is_prime(modulo));
	auto res = *this;
	res ^= modulo - 2;
    }

    auto 

    explicit operator Int() { return val; }
};
#endif // MODINT_HPP
