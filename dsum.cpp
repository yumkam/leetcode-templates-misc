#include <functional>
template <typename D>
pair<D,D> fast2sum(D a, D b) {
	D s = a + b;
	D z = s - a;
	D t = b - z;
	return pair { s, t };
}
template <typename D, typename C, typename P>
D Kahan2sum(const C &A, P &&proj) {
	D sum = 0.0;
	D c = 0.0;
	for (auto a: A) {
		D y = proj(a) + c;
		tie(sum, c) = fast2sum(sum, y);
	}
	return sum + c;
}
template <typename D, typename C, typename P>
D pairwise_sum(const C &A, P &&proj, size_t i, size_t l) {
	auto mid = l/2;
	if (mid == 0)
		return l == 0 ? 0 : proj(A[i]);
	return pairwise_sum(A, proj, i, mid) + pairwise_sum(A, proj, i + mid, l - mid);
}
template <typename D, typename C, typename P = std::identity>
D pairwise_sum(const C &A, P &&proj =  {}) {
	return pairwise_sum(A, proj, 0, A.size());
}
template <typename D, typename C, typename P>
auto KahanBabushkaStep(D sum, D &c, D x) {
	D t = sum + x;
	return pair { t, (std::abs(sum) >= std::abs(x) ? (sum - t) + x : (x - t) + sum) };
}
template <typename D, typename C, typename P = std::identity>
D KahanBabushkaNeumaierSum(const C &A, P &&proj {}) {
	D sum = 0.0;
	D c = 0.0;
	for (auto a: A) {
		auto x = proj(a);
		D dc;
		tie(sum, dc) = KahanBabushkaStep(sum, x);
		c += dc;
	}
	return sum + c;
}
		 
template <typename D, typename C, typename P>
D KahanBabushkaKleinSum(const C &A, P &&proj) {
	D sum = 0.0;
	D cs = 0.0;
	D ccs = 0.0;
	for (auto a: A) {
		auto x = proj(a);
		D c, cc;
		tie(sum, c ) = KahanBabushkaStep(sum, x);
		tie( cs, cc) = KahanBabushkaStep(cs, c); 
		ccs += cc;
	}
	return sum + (cs + ccs);
}
#include <array>
#include <functional>
template <typename D, typename It, typename P = std::identity<D>>
D accurate_sum(It begin, It end, D init = {}, P &&proj = {}) {
	constexpr size_t bits = sizeof(size_t)*CHAR_BIT;
	std::array<D, bits> stack {};
	size_t used = 0;
	for (; begin != end; ++begin) {
		auto cur = proj(*begin);
		size_t bit = 0, mask = 1;
		for (; (used & mask); ++bit, mask <<= 1) {
			used ^= mask;
			cur += stack[bit];
		}
		used ^= mask;
		stack[bit] = cur;
	}
	while(used) {
		auto bit = countr_zero(used);
		used ^= (size_t)1<<bit;
		init += stack[bit];
	}
	return init;
}
