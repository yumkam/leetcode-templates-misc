#ifndef IOTA_ITERATOR_HPP
#define IOTA_ITERATOR_HPP 1
#include <iterator>
template <typename T>
class iota_iterator {
    T val;
    public:
    typedef std::random_access_iterator_tag iterator_category;
    typedef T reference, value_type;
    typedef ssize_t difference_type;
    typedef const T *pointer;
    
    iota_iterator(T val): val {val} {}
    iota_iterator(const iota_iterator &other):val(other.val) {}
    
    operator T() const { return val; }
    const T &operator *() const { return val; }
    pointer operator ->() const { return &val; }
    const T operator [](T n) const { return val + n; }
    
    const auto & operator ++() { ++val; return *this; }
    const auto & operator --() { --val; return *this; }
    auto operator ++(int) { return val++; }
    auto operator --(int) { return val--; }
    const auto & operator +=(const T n) { val += n; return *this; }
    const auto & operator -=(const T n) { val -= n; return *this; }
    const auto operator +(const T n) const { auto temp = *this; return temp += n; }
    const auto operator -(const T n) const { auto temp = *this; return temp -= n; }
    friend auto operator + (T n, const iota_iterator a) { return a + n; }

    difference_type operator -(const iota_iterator other) const { return val - other.val; }

    bool operator == (const iota_iterator other) const { return val == other.val; }
    bool operator <  (const iota_iterator other) const { return val <  other.val; }
    bool operator >= (const iota_iterator other) const { return val >= other.val; }
    bool operator <= (const iota_iterator other) const { return val <= other.val; }
    bool operator >  (const iota_iterator other) const { return val >  other.val; }
    bool operator != (const iota_iterator other) const { return val != other.val; }
};
template <typename T>
auto make_iota_iterator(const T a) {
	return iota_iterator<T>{a};
}
template <typename T>
auto make_iota_iterator(const T a, const T b) {
	return std::make_pair(iota_iterator<T>{a}, iota_iterator<T>{b});
}
#ifdef TEST_IOTA_ITERATOR
#include <iostream>
int main() {
	{
		auto [begin, end] = make_iota_iterator(1u, 10u);
		std::cout << end - begin << std::endl;
		std::cout << std::distance(begin, end) << std::endl;
		for (; begin != end; ++begin)
			std::cout << *begin << ' ';
		std::cout << '\n';
	}
	{
		struct foo { unsigned v; };
		foo seq[] = { 1, 2, 3 };
		auto [begin, end] = make_iota_iterator(std::begin(seq), std::end(seq) );
		std::cout << end - begin << std::endl;
		std::cout << std::distance(begin, end) << std::endl;
		for (; begin != end; ++begin)
			std::cout << (*begin)->v << ' ';
		std::cout << '\n';
	}
	return 0;
}
#endif
#endif
