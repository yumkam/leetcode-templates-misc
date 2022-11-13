#ifndef MANACHER_HPP
#define MANACHER_HPP
#include <vector>
// return palindrome radiuses, odd-sized palindrome for as odd indexes, even-sized palindrome as even indexes
template <typename size_type = size_t, typename String>
std::vector<size_type> manacher(const String &s) {
    const size_type n = s.size()*2 + 1;
    std::vector<size_type> p(n);
    size_type l1 = 1, r = 1;
    for(size_type i = 0; i < n; i++) {
	size_type pi = 0;
	if (r > i && l1 + (r - i) >= 1)
		pi = min(r - i, p[l1 + (r - i) - 1]);
	size_type limit = min(n - i - 1, i);
	// when i + pi is odd, i - pi is also odd
        while(pi <= limit && 
	      (!((i ^ pi) & 1) || s[(i - pi) >> 1] == s[(i + pi) >> 1])) {
            pi++;
        }
        if(i + pi > r) {
            l1 = i - pi + 1; r = i + pi;
        }
	p[i] = pi;
    }
    p.pop_back(); // remove dummy
    for (auto &pi: p) pi >>= 1; // fixup radius
    return p;
}
#endif
