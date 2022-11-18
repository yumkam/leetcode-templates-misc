#ifndef COUNTING_SORT_INPLACE_HPP
#define COUNTING_SORT_INPLACE_HPP
#include <vector>
template <typename size_type, typename GetKey, typename V>
void counting_sort_inplace(V &words, GetKey getkey, size_type maxkey) {
#if 0
    { // in-place counting sort
	std::vector<size_type> cnt(maxkey + 1);
	for (const auto &w: words) // O(n)
	    ++cnt[getkey(w)];
	// change cnt to destination position
	size_type prev = 0;
	for (auto &c: cnt) { // O(k)
	    auto cur = c;
	    c = prev;
	    prev += cur;
	}
	for (size_type i = 0; i < words.size(); ) {
	    auto &dest = cnt[getkey(words[i])];
	    if (dest <= i) {
		if (dest < i)
		    // v belongs at an earlier position. Swap it into place
		    swap(words[i], words[dest]);
		else
		    // v belongs at the same position. don't visit it again
		    ++i;
		// increment the pointer for this value
		++dest;
	    } else {
		// v belongs at a later position.  Something later must belong here.
		// Skip it and let the later item swap in when we get there
		++i;
	    }
	} // O(n)
    } // time O(n + k), space = O(k)
#else
    { // in-place counting sort
	std::vector<size_type> cnt(maxkey + 1);
	for (const auto &w: words) // O(n)
	    ++cnt[getkey(w)];
	// change cnt to destination position
	size_type sum = 0;
	for (auto &c: cnt) // O(k)
	    sum = c += sum;
	for (size_type i = 0; i < words.size(); ++i) {
	    for (;;) {
		auto &dest = cnt[getkey(words[i])];
		auto pos = dest - 1;
		if (i >= pos)
			break;
		swap(words[i], words[dest = pos]);
	    }
	} // in-place cycle sort, O(n)
    } // time O(n + k), space = O(k)
#endif
}
#endif // COUNTING_SORT_INPLACE_HPP
