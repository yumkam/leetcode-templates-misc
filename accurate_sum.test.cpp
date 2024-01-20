#include "accurate_sum.hpp"
#include <bits/stdc++.h>
#include <execution>
#include <utility>
template <typename D>
__attribute__((noinline))
auto KahanBabushkaStep(D sum, D x) {
	D t = sum + x;
	return std::pair { t, (std::abs(sum) >= std::abs(x) ? (sum - t) + x : (x - t) + sum) };
}
template <typename D, typename C, typename P = std::identity>
__attribute__((noinline))
D KahanBabushkaNeumaierSum(const C &A, P proj = {}) {
	D sum = 0.0;
	D c = 0.0;
	for (auto a: A) {
		auto x = proj(a);
		D dc;
		std::tie(sum, dc) = KahanBabushkaStep<D>(sum, x);
		c += dc;
	}
	return sum + c;
}
template <typename D, typename C, typename P>
D pairwise_sum_impl(const C &A, P proj, size_t i, size_t l) {
	auto mid = l/2;
#if 0
	if (mid == 0)
		return l == 0 ? 0 : proj(A[i]);
#else
	if (l < 16) {
		D res {};
		for (;l--;++i)
			res += proj(A[i]);
		return res;
	}
#endif
	return pairwise_sum_impl<D>(A, proj, i, mid) + pairwise_sum_impl<D>(A, proj, i + mid, l - mid);
}
template <typename D, typename C, typename P = std::identity>
__attribute__((noinline))
D pairwise_sum(const C &A, P proj =  {}) {
	return pairwise_sum_impl<D>(A, proj, 0, A.size());
}
static uint64_t rdtsc(void) {
	uint32_t a, d;
	asm volatile ("lfence;rdtsc":"=a"(a),"=d"(d));
	return a + (static_cast<uint64_t>(d)<<32);
}
using namespace std;
int main() {
	vector<float> v(4*3000*4000, 1.5);
	cout <<fixed<< setprecision(8);
	{
asm volatile("// accurate_sum");
	    auto t0 = rdtsc();
	    auto res = accurate_sum(v.cbegin(), v.cend(), 0.0f)/v.size();
	    auto t1 = rdtsc();
	    cout << res << '\t' << t1 - t0 << endl;	
	}
	{
asm volatile("// accurate_sum - 2");
	    auto t0 = rdtsc();
	    auto res = accurate_sum(v.cbegin(), v.cend(), 0.0f)/v.size();
	    auto t1 = rdtsc();
	    cout << res << '\t' << t1 - t0 << endl;	
	}
asm volatile("// pairwise");
	{
	    auto t0 = rdtsc();
	    auto res = pairwise_sum<float>(v)/v.size();
	    auto t1 = rdtsc();
	    cout << res << '\t' << t1 - t0 << endl;	
	}
asm volatile("// KahanBabushkaNeumaierSum");
	{
	    auto t0 = rdtsc();
	    auto res = KahanBabushkaNeumaierSum<float>(v)/v.size();
	    auto t1 = rdtsc();
	    cout << res << '\t' << t1 - t0 << endl;	
	}
asm volatile("// accumulate");
	{
	    auto t0 = rdtsc();
	    auto res = accumulate(v.cbegin(), v.cend(), 0.0f)/v.size();
	    auto t1 = rdtsc();
	    cout << res << '\t' << t1 - t0 << endl;	
	}
asm volatile("// reduce");
	{
	    auto t0 = rdtsc();
	    auto res = reduce(v.cbegin(), v.cend(), 0.0f)/v.size();
	    auto t1 = rdtsc();
	    cout << res << '\t' << t1 - t0 << endl;	
	}
asm volatile("// reduce par");
	{
	    auto t0 = rdtsc();
	    auto res = reduce(execution::par_unseq, v.cbegin(), v.cend(), 0.0f)/v.size();
	    auto t1 = rdtsc();
	    cout << res << '\t' << t1 - t0 << endl;	
	}
	return 0;
}
