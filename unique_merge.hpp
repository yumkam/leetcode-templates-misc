#ifndef UNQIUE_COPY_HPP
#define UNQIUE_COPY_HPP
template <typename It, typename Out>
Out unique_copy_ordered(It it, It end, Out out) {
    for (; it != end; ++out) {
	auto old = it;
	while(++it != end && !(*old < *it))
	    ;
	*out = *old;
    }
    return out;
}
template <typename It, typename Out, typename Comp>
Out unique_copy_ordered(It it, It end, Out out, Comp comp) {
    for (; it != end; ++out) {
	auto old = it;
	while(++it != end && !comp(*old, *it))
	    ;
	*out = *old;
    }
    return out;
}
#endif
#ifndef UNQIUE_MERGE_HPP
#define UNQIUE_MERGE_HPP
template <typename It1, typename It2, typename Out>
Out unique_merge(It1 it1, It1 end1, It2 it2, It2 end2, Out out) {
    for (; it1 != end1; ++out) {
	if (it2 == end2)
	    return unique_copy_ordered(it1, end1, out);
	if (*it1 < *it2) {
	    auto it = it1;
	    while(++it1 != end1 && !(*it < *it1))
		;
	    *out = *it;
    	} else if (*it2 < *it1) {
	    auto it = it2;
	    while(++it2 != end2 && !(*it < *it2))
		;
	    *out = *it;
	} else {
	    auto it = it2;
	    while(++it1 != end1 && !(*it < *it1))
		;
	    while(++it2 != end2 && !(*it < *it2))
		;
	    *out = *it;
	}
    }
    return unique_copy_ordered(it2, end2, out);
}
template <typename It1, typename It2, typename Out, typename Comp>
Out unique_merge(It1 it1, It1 end1, It2 it2, It2 end2, Out out, Comp comp) {
    for (; it1 != end1; ++out) {
	if (it2 == end2)
	    return unique_copy_ordered(it1, end1, out, comp);
	if (comp(*it1, *it2)) {
	    auto it = it1;
	    while(++it1 != end1 && !comp(*it, *it1))
		;
	    *out = *it;
    	} else if (*it2 < *it1) {
	    auto it = it2;
	    while(++it2 != end2 && !comp(*it, *it2))
		;
	    *out = *it;
	} else {
	    auto it = it2;
	    while(++it1 != end1 && !(comp(*it, *it1)))
		;
	    while(++it2 != end2 && !(comp(*it, *it2)))
		;
	    *out = *it;
	}
    }
    return unique_copy_ordered(it2, end2, out, comp);
}
#endif // UNQIUE_MERGE_HPP
#ifndef UNQIUE_MERGE_STRICT_HPP
#define UNQIUE_MERGE_STRICT_HPP
#include <algorithm>
// assumes values in it1 and it2 are already unique
template <typename It1, typename It2, typename Out>
Out unique_merge_strict(It1 it1, It1 end1, It2 it2, It2 end2, Out out) {
    for (; it1 != end1; ++out) {
	if (it2 == end2)
	    return std::copy(it1, end1, out);
	if (*it1 < *it2) {
	    *out = *it1; ++it1;
    	} else if (*it2 < *it1) {
	    *out = *it2; ++it2;
	} else {
	    *out = *it1; ++it1; ++it2;
	}
    }
    return std::copy(it2, end2, out);
}
template <typename It1, typename It2, typename Out, typename Comp>
Out unique_merge_strict(It1 it1, It1 end1, It2 it2, It2 end2, Out out, Comp comp) {
    for (; it1 != end1; ++out) {
	if (it2 == end2)
	    return std::copy(it1, end1, out);
	if (comp(*it1, *it2)) {
	    *out = *it1; ++it1;
    	} else if (*it2 < *it1) {
	    *out = *it2; ++it2;
	} else {
	    *out = *it1; ++it1; ++it2;
	}
    }
    return std::copy(it2, end2, out);
}
#endif
