#ifndef MERGE_UNIQUE_HPP
#define MERGE_UNIQUE_HPP
#include <functional>
#include <forward_list>
#if 0
template <typename ...L>
static void merge_unique(std::forward_list<L...> &a, std::forward_list<L...> & b) {
    if (b.empty())
	return;
    if (a.empty()) {
	a.swap(b);
	return;
    }
    auto aprev = a.before_begin();
    auto acur = a.begin();
    auto bprev = b.before_begin();
    auto bcur = b.begin();
    while(!b.empty()) {
	if (*acur < *bcur) {
	    aprev = acur;
	    if (++acur == a.end()) {
		a.splice_after(aprev, move(b));
		return;
	    }
	} else if (*bcur < *acur) {
	    ++bcur;
	    a.splice_after(aprev, b, bprev);
	    ++aprev;
	    if (b.empty())
		return;
	} else {
	    b.pop_front();
	    if (b.empty())
		return;
	    bcur = b.begin();
	}
    }
}
#endif
template <typename ...L, typename Compare = std::less<>>
static void merge_unique(std::forward_list<L...> &a, std::forward_list<L...> & b, Compare comp = std::less<>()) {
    if (b.empty())
	return;
    if (a.empty()) {
	a.swap(b);
	return;
    }
    auto aprev = a.before_begin();
    auto acur = a.begin();
    auto bprev = b.before_begin();
    auto bcur = b.begin();
    while(!b.empty()) {
	if (comp(*acur, *bcur)) {
	    aprev = acur;
	    if (++acur == a.end()) {
		a.splice_after(aprev, move(b));
		return;
	    }
	} else if (comp(*bcur, *acur)) {
	    ++bcur;
	    a.splice_after(aprev, b, bprev);
	    ++aprev;
	    if (b.empty())
		return;
	} else {
	    b.pop_front();
	    if (b.empty())
		return;
	    bcur = b.begin();
	}
    }
}
#include <list>
#if 0
template <typename ...L>
static void merge_unique(std::list<L...> &a, std::list<L...> & b) {
    if (b.empty())
	return;
    if (a.empty()) {
	a.swap(b);
	return;
    }
    auto acur = a.begin();
    auto bcur = b.begin();
    for (;;) {
	if (*acur < *bcur) {
	    if (++acur == a.end()) {
		a.splice(acur, move(b));
		return;
	    }
	} else {
	    if (*bcur < *acur)
		a.splice(acur, b, bcur);
	    else
		b.pop_front();
	    if (b.empty())
		return;
	    bcur = b.begin();
	}
    }
}
#endif
template <typename ...L, typename Compare = std::less<>>
static void merge_unique(std::list<L...> &a, std::list<L...> & b, Compare comp = std::less<>()) {
    if (b.empty())
	return;
    if (a.empty()) {
	a.swap(b);
	return;
    }
    auto acur = a.begin();
    auto bcur = b.begin();
    for (;;) {
	if (comp(*acur, *bcur)) {
	    if (++acur == a.end()) {
		a.splice(acur, move(b));
		return;
	    }
	} else {
	    if (comp(*bcur, *acur))
		a.splice(acur, b, bcur);
	    else
		b.pop_front();
	    if (b.empty())
		return;
	    bcur = b.begin();
	}
    }
}
#endif
