#ifndef STEP_ITERATOR_HPP
#define STEP_ITERATOR_HPP 1
#include <iterator>
template <typename BaseIterator>
class step_iterator {
    public:
    typedef typename BaseIterator::iterator_category iterator_category;
    typedef typename BaseIterator::reference reference;
    typedef typename BaseIterator::value_type value_type;
    typedef typename BaseIterator::difference_type difference_type;
    typedef typename BaseIterator::pointer pointer;
    private:
    BaseIterator base;
    typename BaseIterator::difference_type step;
    typedef const BaseIterator &baseref;
    public:
    step_iterator(const BaseIterator & base, difference_type step): base{base}, step {step} {}
    step_iterator(const step_iterator<BaseIterator> &other):step_iterator{other.base, other.step} {}
    
    operator baseref () { return base; }
    operator BaseIterator () { return base; }
    reference operator *() const { return *base; }
    pointer operator ->() const { return base; }
    reference operator [](difference_type n) const { return base[n*step]; }
    
    const auto & operator ++() { base += step; return *this; }
    const auto & operator --() { base -= step; return *this; }
    auto operator ++(int) { auto temp = *base; ++(*this); return temp; }
    auto operator --(int) { auto temp = *base; --(*this); return temp; }
    const auto & operator +=(const difference_type n) { base += n*step; return *this; }
    const auto & operator -=(const difference_type n) { base -= n*step; return *this; }
    const step_iterator<BaseIterator> operator +(const difference_type n) const { auto temp = *this; return temp += n; }
    const auto operator -(const difference_type n) const { auto temp = *this; return temp -= n; }
    friend auto operator + (difference_type n, const step_iterator<BaseIterator> &a) { return a + n; }

    difference_type operator -(const baseref other) const { return (base - other) / step; }

    bool operator == (const baseref other) const { return base == other; }
    bool operator <  (const baseref other) const { return base <  other; }
    bool operator >= (const baseref other) const { return base >= other; }
    bool operator <= (const baseref other) const { return base <= other; }
    bool operator >  (const baseref other) const { return base >  other; }
    bool operator != (const baseref other) const { return base != other; }
};
#endif

