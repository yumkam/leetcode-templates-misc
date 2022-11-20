#ifndef IDX_ITERATOR_HPP
#define IDX_ITERATOR_HPP 1
#include <iterator>
template <typename BaseIterator>
class basic_idx_iterator {
    public:
    typedef typename BaseIterator::iterator_category iterator_category;
    typedef typename BaseIterator::value_type::iterator::reference reference;
    typedef typename BaseIterator::value_type::iterator::value_type value_type;
    typedef typename BaseIterator::value_type::iterator::pointer pointer;
    typedef typename BaseIterator::difference_type difference_type;
    private:
    BaseIterator base;
    public:
    basic_idx_iterator(const BaseIterator &base): base{base} {}
    basic_idx_iterator(const basic_idx_iterator &other):basic_idx_iterator{other.base} {}
    
    //operator baseref () { return base; }
    //operator BaseIterator () { return base; }
//    reference operator *() const { return (*base)[idx]; }
//    pointer operator ->() const { return &(*base[idx]); }
//    reference operator [](difference_type n) const { return base[n][idx]; }
    
    const auto & operator ++() { ++base; return *this; }
    const auto & operator --() { --base; return *this; }
    auto operator ++(int) { auto temp = *(*this); ++(*this); return temp; }
    auto operator --(int) { auto temp = *(*this); --(*this); return temp; }
    const auto & operator +=(const difference_type n) { base += n; return *this; }
    const auto & operator -=(const difference_type n) { base -= n; return *this; }
    const basic_idx_iterator operator +(const difference_type n) const { auto temp = *this; return temp += n; }
    const auto operator -(const difference_type n) const { auto temp = *this; return temp -= n; }
    friend auto operator + (difference_type n, const basic_idx_iterator &a) { return a + n; }
    difference_type operator -(const basic_idx_iterator &other) const { return base - other.base; }

    bool operator == (const basic_idx_iterator &other) const { return base == other.base; }
    bool operator <  (const basic_idx_iterator &other) const { return base <  other.base; }
    bool operator >= (const basic_idx_iterator &other) const { return base >= other.base; }
    bool operator <= (const basic_idx_iterator &other) const { return base <= other.base; }
    bool operator >  (const basic_idx_iterator &other) const { return base >  other.base; }
    bool operator != (const basic_idx_iterator &other) const { return base != other.base; }
};

template <typename BaseIterator, size_t idx>
class constidx_iterator: public basic_idx_iterator {
    public:
    constidx_iterator(const BaseIterator &base): basic_idx_iterator{base} {}
    constidx_iterator(const constidx_iterator &other):constidx_iterator{other.base} {}
    
    //operator baseref () { return base; }
    //operator BaseIterator () { return base; }
    reference operator *() const { return (*base)[idx]; }
    pointer operator ->() const { return &(*base[idx]); }
    reference operator [](difference_type n) const { return base[n][idx]; }
};

template <typename BaseIterator, size_t idx>
class get_iterator: public basic_idx_iterator {
    public:
    get_iterator(const BaseIterator &base): basic_idx_iterator{base} {}
    get_iterator(const get_iterator &other):get_iterator{other.base} {}
    
    //operator baseref () { return base; }
    //operator BaseIterator () { return base; }
    reference operator *() const { return get<idx>(*base); }
    pointer operator ->() const { return &(get<idx>(*base)); }
    reference operator [](difference_type n) const { return get<idx>(base[n]); }
};
template <typename BaseIterator>
class idx_iterator: public basic_idx_iterator {
    private:
    const size_t idx;
    public:
    idx_iterator(const BaseIterator &base, size_t idx): basic_idx_iterator{base}, idx{idx} {}
    idx_iterator(const idx_iterator &other):idx_iterator{other.base, other.idx} {}

    reference operator *() const { return (*base)[idx]; }
    pointer operator ->() const { return &(*base[idx]); }
    reference operator [](difference_type n) const { return base[n][idx]; }
};
#endif

