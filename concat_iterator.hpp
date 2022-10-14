#ifndef CONCAT_ITERATOR_HPP
#define CONCAT_ITERATOR_HPP
#include <iterator>
template <typename I>
class concat_iterator {
    I i1, i1end, i2;
    public:
    typedef std::input_iterator_tag iterator_category;
    // XXX can also satisfy forward_iterator_tag, as long as I::iterator_category is also forward_iterator_tag; but does not satisfy other tags!
    typedef typename I::reference reference;
    typedef typename I::value_type value_type;
    typedef typename I::difference_type difference_type;
    typedef typename I::pointer pointer;
    concat_iterator(const I &i1, const I &i1end, const I &i2):i1{i1}, i1end{i1end}, i2{i2} {}
    concat_iterator(const I &i1end, const I &i2):i1{i1end},i1end{i1end},i2{i2} {}
    concat_iterator(const concat_iterator& other): i1{other.i1}, i1end{other.i1end}, i2{other.i2} {}
    concat_iterator(concat_iterator&& other): i1{std::move(other.i1)}, i1end{std::move(other.i1end)}, i2{std::move(other.i2)} {}
    auto operator = (const concat_iterator &other ) {
        i1 = other.i1; i1end = other.i1end; i2 = other.i2;
        return *this;
    }
    reference operator *() const { return i1 == i1end ? *i2 : *i1; }
    pointer operator->() { return i1 == i1end ? i2.operator ->() : i1.operator->(); }
    const auto & operator ++() { return i1 == i1end ? ++i2 : ++i1; }
    auto operator ++(int) { return i1 == i1end ? i2++ : i1++; }

    bool operator == (const concat_iterator &other) const { 
	// assert (i1end == other.i1end);
	return i1 == other.i1 && i2 == other.i2;
	// return (i1 == i1end ? i2 : i1) == (other.i1 == other.i1end ? other.i2 : other.i1 );
    }
    bool operator != (const concat_iterator &other) const { return !(*this == other); }
};
template <typename Iterator> auto make_concat_iterator(const Iterator &i1, const Iterator &i1end, const Iterator &i2) {
	return concat_iterator<Iterator>{i1, i1end, i2};
}
template <typename Iterator> auto make_concat_iterator(const Iterator &i1end, const Iterator &i2) {
	return concat_iterator<Iterator>{i1end, i2};
}
template <typename Container> 
auto make_concat_iterator_pair(Container &a, Container &b) {
	using std::begin;
	using std::end;
	return make_pair(make_concat_iterator(begin(a), end(a), begin(b)),
			make_concat_iterator(end(a), end(b)));
}
#if 0
template <typename T, size_t N, size_t M> 
auto make_concat_iterator_pair(T (&a)[N], T (&b)[M]) {
	return make_pair(make_concat_iterator(std::begin(a), std::end(a), std::begin(b)),
			make_concat_iterator(std::end(a), std::end(b)));
}
#endif
#ifdef TEST_CONCAT_ITERATOR
#include <iostream>
#include <string>
#include <map>
int main() {
	{
		std::string a = "abc";
		std::string b = "defgh";
		auto begin = make_concat_iterator(a.begin(), a.end(), b.begin());
		auto end =  make_concat_iterator(a.end(), b.end());
		for (; begin != end; ++begin)
			std::cout << *begin;
		std::cout << std::endl;
	}
	{
		std::map<int,int> a, b;
		a[1]=2;
		a[2]=3;
		b[3]=4;
		b[4]=5;
		b[5]=6;
		auto begin = make_concat_iterator(a.begin(), a.end(), b.begin());
		auto end =  make_concat_iterator(a.end(), b.end());
		for (; begin != end; ++begin)
			std::cout << begin->first << '=' << begin->second << ' ';
		std::cout << std::endl;
	}
#if 0 // not implemented
	{
		char a[] = "abc";
		char b[] = "defgh";
		auto [begin, end] = make_concat_iterator_pair(a, b);
		for (; begin != end; ++begin)
			std::cout << *begin;
		std::cout << std::endl;
	}
#endif
	return 0;
}
#endif
#endif
