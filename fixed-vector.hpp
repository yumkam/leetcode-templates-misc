#define assert(X) ((void)0)
// Public domain till end-of-template, Signed-by: Yuriy Kaminskiy
#ifndef FIXED_VECTOR_HPP
#define FIXED_VECTOR_HPP
// #include "sized-integer.hpp"
#ifndef SIZED_INTEGER_HPP
#define SIZED_INTEGER_HPP
template <size_t> struct SizedInteger {
    using unsigned_type = uintmax_t;
    using signed_type = intmax_t;
};
#define SIZED_INTEGER(BITS) \
	template <> struct SizedInteger<BITS/8> { using signed_type = int##BITS##_t; using unsigned_type = uint##BITS##_t; }
SIZED_INTEGER(8);
SIZED_INTEGER(16);
#ifdef HAVE_INT24_T
SIZED_INTEGER(24);
#endif
SIZED_INTEGER(32);
SIZED_INTEGER(64);
#ifdef HAVE_INT128_T
SIZED_INTEGER(128);
#endif
#undef SIZED_INTEGER
#endif // SIZED_INTEGER_HPP

template <typename T, typename SizeType = typename SizedInteger<sizeof(T)>::unsigned_type>
class fixed_vector {
    // custom vector with allocated-once size
    // sequence of operation:
    //   fixed_vector<T> v;
    //   // 1st pass: dummy enumeration to count items
    //   for (const auto &_: sequence) { v.adjust_size(1); }
    //   v.allocate();
    //   // 2nd pass: populate array
    //   for (auto &x: sequence) { v.push_back(x); }
    // or
    //   fixed_vector<T> v;
    //   v.reserve(sequence.size());
    //   for (auto &x: sequence) { v.push_back(x); }
    // Restrictions:
    // 1) sizeof(T) >= sizeof(SizeType)
    //    (if sizeof(T) is (or *can be*) smaller than sizeof(size_t),
    //     you must specify SizeType too));
    //    e.g. beware about <int> or <unsigned>, they will work
    //    on 32-bit machines, but suddenly fail on 64-bit ones)
    // 2) you must not access items until they will be push_back()'d
    // 3) you must not push_back more items than was allocated
    // 4) if T has non-empty destructors,
    //    vector must be completely populated by the time it will be destructed
    //    (that is, you must push_back *exactly* the number of allocated items)
    // 5) no resizes after allocation, re-allocation, etc;
    //    you set size (maybe several times); allocate (once); completely populate; then use or destroy;
    //    no change of sequence is allowed;
    union u { SizeType idx; T t; u() {} };
    unique_ptr<u[]> a;
    SizeType sz;
public:
    static_assert(sizeof(u) == sizeof(T));
    typedef SizeType size_type;
    typedef T element_type;
    T* begin() { return &a[0].t; }
    T* end() { return &a[sz].t; }
    const T* cbegin() const { return &a[0].t; }
    const T* cend() const { return &a[sz].t; }
    SizeType size() const { return sz; }
    T& operator[] (SizeType i) { return a[i].t; }
    T& front() { return (*this)[0]; }
    T& back() { return (*this)[sz - 1]; }
    T& at(SizeType i) { assert(!!a); if (i < 0 || i >= sz) throw std::out_of_range; return (*this)[i]; }
    void adjust_size(SizeType inc = +1) {
	assert(!a);
	sz += inc;
    }
    void allocate() {
	assert(!a);
	a.reset(new u[sz]);
	if (sz)
	    a[sz - 1].idx = 0;
    }
    void reserve(SizeType size) {
	sz = 0;
	adjust_size(size);
	allocate();
    }
    SizeType current_size() const { // only valid before last item was pushed back!
	return a[sz - 1].idx;
    }
    template <class... Args>
    void emplace_back(Args&&...args) {
	SizeType idx = a[sz - 1].idx++;
	assert(idx >= 0 && idx < sz);
	new(&a[idx].t) T(forward<Args>(args)...);
    }
    void push_back(T&&x) {
	return emplace_back(move(x));
    }
    void push_back(const T& x) {
	return emplace_back(x);
    }
    SizeType capacity() const {
	return sz;
    }
    ~fixed_vector() {
	if (a)
	    for (SizeType i = 0; i < sz; i++)
		a[i].t.~T();
    }
};
// end-of-template
#endif
