#ifndef ACCURATE_SUM
#include <array>
#include <functional>
#include <climits>
#include <cassert>
#if __cplusplus > 201703L
#include <bit>
#endif
namespace {
template <typename D, typename A>
D accurate_sum_tail(const A &stack, size_t used, D init) {
#if __cpp_lib_bitops
	while(used) {
		auto bit = std::countr_zero(used);
		used ^= (size_t)1<<bit;
		init += stack[bit];
	}
#else
	for (size_t bit = 0, mask = 1; used; ++bit, mask <<= 1) {
		if (used & mask) {
			used ^= mask;
			init += stack[bit];
		}
	}
#endif
	return init;
}
template <typename D, typename It, typename P, typename Tag>
D accurate_sum(It begin, It end, D init, P proj, Tag) {
	constexpr size_t bits = sizeof(size_t)*CHAR_BIT;
	std::array<D, bits> stack; // *uninitialized*
	size_t used = 0;
	for (; begin != end; ++begin) {
		auto cur = proj(*begin);
		++used;
#if __cpp_lib_bitops
		size_t bit = std::countr_zero(used);
		for (size_t i = 0; i < bit; ++i)
			cur += stack[i];
#else
		size_t bit = 0, mask = 1;
		for (; (used & mask) == 0; ++bit, mask <<= 1)
			cur += stack[bit];
#endif
		stack[bit] = cur;
	}
	return accurate_sum_tail(stack, used, init);
}
template <typename D, typename It, typename P>
D accurate_sum(It begin, It end, D init, P proj, std::random_access_iterator_tag) {
	constexpr size_t bits = sizeof(size_t)*CHAR_BIT;
	std::array<D, bits> stack; // *uninitialized*
	size_t used = 0;
	size_t len = end - begin;
	for (; used + 8 <= len; ) {
		auto a0 = proj(begin[used+0]);
		auto a1 = proj(begin[used+1]);
		auto a2 = proj(begin[used+2]);
		auto a3 = proj(begin[used+3]);
		auto a4 = proj(begin[used+4]);
		auto a5 = proj(begin[used+5]);
		auto a6 = proj(begin[used+6]);
		auto a7 = proj(begin[used+7]);
		a0 += a4;
		a1 += a5;
		a2 += a6;
		a3 += a7;
		a0 += a2;
		a1 += a3;
		auto cur = a0 + a1;
		used += 8;
#if __cpp_lib_bitops
		size_t bit = std::countr_zero(used);
		for (size_t i = 3; i < bit; ++i)
			cur += stack[i];
#else
		size_t bit = 3, mask = 8;
		for (; (used & mask) == 0; ++bit, mask <<= 1)
			cur += stack[bit];
#endif
		stack[bit] = cur;
	}
	begin += used;
	if ((len & 4)) {
		auto a0 = proj(*begin++);
		auto a1 = proj(*begin++);
		auto a2 = proj(*begin++);
		auto a3 = proj(*begin++);
		a0 += a2;
		a1 += a3;
		stack[2] = a0 + a1;
	}
	if ((len & 2)) {
		auto a0 = proj(*begin++);
		auto a1 = proj(*begin++);
		stack[1] = a0 + a1;
	}
	if ((len & 1)) {
		auto a0 = proj(*begin++);
		stack[0] = a0;
	}
	return accurate_sum_tail(stack, len, init);
}
}
template <typename D, typename It, typename P = std::identity>
__attribute__((noinline))
D accurate_sum(It begin, It end, D init = {}, P proj = {}) {
	return accurate_sum(begin, end, init, proj, typename It::iterator_category {});
}
#endif // ACCURATE_SUM
