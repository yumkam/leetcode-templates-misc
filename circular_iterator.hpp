#ifndef CIRCULAR_ITERATOR_HPP
#define CIRCULAR_ITERATOR_HPP
#include <iterator>
namespace detail {
	template <typename T> struct tag_map { };
	template <> struct tag_map<std::input_iterator_tag> { typedef std::input_iterator_tag tag; };
	template <> struct tag_map<std::output_iterator_tag> { typedef std::output_iterator_tag tag; };
	template <> struct tag_map<std::forward_iterator_tag> { typedef std::forward_iterator_tag tag; };
	template <> struct tag_map<std::bidirectional_iterator_tag> { typedef std::bidirectional_iterator_tag tag; };
	template <> struct tag_map<std::random_access_iterator_tag> { typedef std::bidirectional_iterator_tag tag; };
#if __cplusplus > 201703L
	template <> struct tag_map<std::contiguous_iterator_tag> { typedef std::bidirectional_iterator_tag tag; };
#endif
}
template <typename BaseIterator>
class CircularIterator {
	BaseIterator begin, end, cur;
	public:
	typedef typename detail::tag_map<typename BaseIterator::iterator_category>::tag iterator_category;
	typedef typename BaseIterator::reference reference;
	typedef typename BaseIterator::value_type value_type;
	typedef typename BaseIterator::difference_type difference_type;
	typedef typename BaseIterator::pointer pointer;
	CircularIterator(const BaseIterator &begin,
			const BaseIterator &end,
			const BaseIterator &cur)
		: begin {begin}, end {end}, cur {cur} { }
	CircularIterator(const CircularIterator &) = default;
	CircularIterator(CircularIterator &&) = default;
	CircularIterator &operator = (const CircularIterator & ) = default;
	CircularIterator &operator = (CircularIterator && ) = default;
	void swap (const CircularIterator &other ) {
		swap(begin, other.begin); swap(end, other.end); swap(cur, other.cur);
	}
	auto &operator ++() {
		if (++cur == end)
			cur = begin;
		return *this;
	}
	auto operator ++(int) { auto copy = *this; ++*this; return copy; }
	auto &operator --() {
		if (cur == begin)
			cur = end;
		--cur;
		return *this;
	}
	auto &operator --(int) { auto copy = *this; --*this; return copy; }
	reference operator *() {
		return *cur;
	}
	pointer operator->() { return cur->operator -> (); }
	bool operator == (const CircularIterator &other) const { 
		return cur == other.cur;
	}
	bool operator != (const CircularIterator &other) const { return !(*this == other); }
};
#endif
