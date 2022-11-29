#ifndef MIN_CYCLIC_ROTATION_HPP
#define MIN_CYCLIC_ROTATION_HPP
template <typename V>
size_t min_cyclic_rotation(const V &s) {
    auto n = s.size();
    using size_type = decltype(n);
    size_type res = 0;
    for (size_type l = 0; l < n; ) {
	res = l;
	size_type r = l, p = l + 1;
	size_type rmodn = r, pmodn = p;
	if (pmodn == n) pmodn = 0;
	for (; p < 2*n && s[rmodn] <= s[pmodn]; ++r, ++p) {
	    if (s[rmodn] < s[pmodn]) { r = l - 1; rmodn = r; }
	    if (++rmodn == n) rmodn = 0;
	    if (++pmodn == n) pmodn = 0;
	}
	while (l <= r) l += p - r;
    }
    return res;
}
#endif
