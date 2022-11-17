// XXX
// #include <type_traits> 
// make_unsigned<foo>::type
#ifndef INTEGER_ALTERNATIVE_HPP
#define INTEGER_ALTERNATIVE_HPP
#include <cstdint>
template <typename> struct IntegerAlternative {};
#define INTEGER_ALTERNATIVE(BASE,SIGNED,UNSIGNED) \
	template <> struct IntegerAlternative<BASE> { \
		    using signed_type = SIGNED; \
		    using unsigned_type = UNSIGNED; \
	}
#define INTEGER_ALTERNATIVE_DUAL(BITS) \
	INTEGER_ALTERNATIVE(int##BITS##_t,int##BITS##_t,uint##BITS##_t); \
	INTEGER_ALTERNATIVE(uint##BITS##_t,int##BITS##_t,uint##BITS##_t)
INTEGER_ALTERNATIVE_DUAL(8);
INTEGER_ALTERNATIVE_DUAL(16);
INTEGER_ALTERNATIVE_DUAL(32);
INTEGER_ALTERNATIVE_DUAL(64);
INTEGER_ALTERNATIVE(char, signed char, unsigned char);
#undef INTEGER_ALTERNATIVE
#undef INTEGER_ALTERNATIVE_DUAL
#endif
