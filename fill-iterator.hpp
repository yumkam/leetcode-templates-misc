#ifndef FILL_ITERATOR_HPP
#define FILL_ITERATOR_HPP 1
#include <iterator>
template <typename T, typename SizeType = size_t>
class fill_iterator {
    T val;
    SizeType size;
    public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef T &reference, value_type;
    typedef const T *pointer;
    typedef SizeType size_type;
    typedef ssize_t difference_type;
    
    fill_iterator(): size {0} {}
    fill_iterator(size_type size): size {size} {}
    fill_iterator(const T &val, size_type size): val {val}, size {size} {}
    fill_iterator(T &&val, size_type size): val {val}, size {size} {}
    fill_iterator(const fill_iterator &other):val {other.val}, size {other.size} {}
    fill_iterator(fill_iterator &&other):val {std::move(other.val)}, size {other.size} {}
    
    explicit operator T() const { return val; }
    const T &operator *() const { return val; }
    pointer operator -> () const { return &val; }
    const T &operator [](size_type) const { return val; }
    
    const auto & operator ++() { --size; return *this; }
    const auto & operator --() { ++size; return *this; }
    auto operator ++(int) { auto copy = *this; ++*this; return copy; }
    auto operator --(int) { auto copy = *this; --*this; return copy; }
    const auto & operator +=(const size_type n) { size -= n; return *this; }
    const auto & operator -=(const size_type n) { size += n; return *this; }
    const auto operator +(const size_type n) const { auto temp = *this; return temp += n; }
    const auto operator -(const size_type n) const { auto temp = *this; return temp -= n; }
    friend auto operator + (size_type n, const fill_iterator &a) { return a + n; }

    difference_type operator -(const fill_iterator &other) const { return static_cast<ssize_t>(other.size) - static_cast<ssize_t>(size); }

    bool operator == (const fill_iterator &other) const { return size == other.size; }
    bool operator <  (const fill_iterator &other) const { return size >  other.size; }
    bool operator >= (const fill_iterator &other) const { return size <= other.size; }
    bool operator <= (const fill_iterator &other) const { return size >= other.size; }
    bool operator >  (const fill_iterator &other) const { return size <  other.size; }
    bool operator != (const fill_iterator &other) const { return size != other.size; }
};
template <typename T, typename SizeType>
auto make_fill_iterator(const T &val, SizeType size) {
	return fill_iterator<T, SizeType> {val, size};
}
template <typename T, typename SizeType>
auto make_fill_iterator_pair(const T &val, SizeType size) {
	return std::make_pair(fill_iterator<T, SizeType> {val, size}, fill_iterator<T, SizeType> {});
}
#ifdef TEST_FILL_ITERATOR
#include <iostream>
int main() {
	{
		auto begin = make_fill_iterator(5, 10u);
		decltype(begin) end;
		for (; begin != end; ++begin)
			std::cout << *begin << ' ';
		std::cout << '\n';
	}
	{
		auto [begin,end] = make_fill_iterator_pair(7, 10u);
		for (; begin != end; ++begin)
			std::cout << *begin << ' ';
		std::cout << '\n';
	}
	{
		struct foo { unsigned v; };
		auto begin = make_fill_iterator(foo{6}, 10u);
		auto b3 = 3 + begin;
		decltype(begin) end;
		std::cout << end - begin << '\n';
		std::cout << end - b3 << '\n';
		for (; begin != end; ++begin)
			std::cout << begin->v << ' ';
		std::cout << '\n';
	}
	return 0;
}
#endif
#endif
