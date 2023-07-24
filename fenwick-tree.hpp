#ifndef FENWICK_TREE_HPP
#define FENWICK_TREE_HPP
#include <vector>
template <typename T, typename V = std::vector<T>>
class FenwickTree {
    // One-based Fenwick tree with a twist (A[0] saves nums[0])
    V A;
    // for all operations T must support 'a += b'
    // for get(), set(), range_sum(), cast to V, T must support 'a -= b'
    // for range_query(), lower_bound(), T must support 'a -= b' and 'a < b'
    public:
    typedef typename V::size_type size_type;
    typedef typename V::value_type value_type;
    private:
    static const size_type LSB(const size_type i) {
        return i & -i;
    }

    // Convert A[] in place to Fenwick tree form
    static void init(V &A) {
        for (size_type i = 1; i < A.size(); ++i) {
            auto j = i + LSB(i);
            if (j < A.size())
                A[j] += A[i];
        }
    }

    // Convert back to array of per-element counts
    static void fini(V &A) {
        for (size_type i = A.size(); i-- > 1;) {
            auto j = i + LSB(i);
            if (j < A.size())
                A[j] -= A[i];
        }
    }
    public:
    FenwickTree(size_type size = 0):A(size) {
    }

    FenwickTree(const V &nums):A(nums) {
        init(A);
    }

    FenwickTree(V &&nums):A(std::move(nums)) {
        init(A);
    }

    FenwickTree(const FenwickTree &) = default;
    FenwickTree(FenwickTree &&) = default;

    FenwickTree &operator = (const V & A) {
        this->A = A;
        init(this->A);
        return *this;
    }
    
    FenwickTree &operator = (V && A) {
        this->A = std::move(A);
        init(this->A);
        return *this;
    }
    
    FenwickTree &operator = (const FenwickTree &) = default;   
    FenwickTree &operator = (FenwickTree &&) = default;

    operator V () const & {
        V ret = A;
        fini(ret);
        return ret;
    }
    operator V () && {
        fini(A);
        return std::move(A);
    }

    // Returns the sum of the first i elements (indices 0 to i)
    // Equivalent to range_sum(0, i)
    T prefix_sum(size_type i) const {
        // assert(i < A.size());
        T sum = A[0];
        for (; i != 0; i -= LSB(i))
            sum += A[i];
        return sum;
    }

    // Add delta to element with index i (zero-based)
    void add(size_type i, T delta) {
        // assert(i < A.size());
        if (i == 0) {
            A[0] += delta;
            return;
        }
        for (; i < A.size(); i+= LSB(i))
            A[i] += delta;
    }

    // Returns sum of nums[i + 1] to nums[j].
    // Same as prefix_sum(j) - prefix_sum(i), but a bit faster
    T range_sum(size_type i, size_type j) const {
        T sum = 0;
        // assert(0 < i);
        // assert(i < j);
        // assert(j < A.size());
        for (; j > i; j -= LSB(j))
            sum += A[j];
        for (; i > j; i -= LSB(i))
            sum -= A[i];
        return sum;
    }
    
    //
    T get(size_type i) const {
        return i == 0 ? A[0] : range_sum(i - 1, i);
    }
    
    //
    void set(size_type i, T v) {
	v -= get(i);
        add(i, v);
    }
    
    template <class... Args>
    void emplace_back(Args&&...args) {
        auto cidx = A.size();
        auto &back = A.emplace_back(forward<Args>(args)...);
        if (!cidx)
            return;
        for (auto j = cidx - 1; j + LSB(j) == cidx; j -= LSB(j))
            back += A[j];
    }
    void push_back(T&&x) {
        return emplace_back(std::move(x));
    }
    void push_back(const T& x) {
        return emplace_back(x);
    }
    
    void pop_back() {
        A.pop_back();
    }

    private:
    void adjust_size(size_t osize) {
        auto i = osize <= 1 ? 1 : osize - 1;
        while(auto npow = i - LSB(i))
            i = npow;
        for (; i < A.size(); i++) {
            auto j = i + LSB(i);
            if (j >= osize && j < A.size())
                A[j] += A[i];
        }
    }
    public:
    template <class Iterator>
    void append(Iterator begin, Iterator end) {
        auto osize = A.size();
        for (; begin != end; ++begin)
            A.push_back(*begin);
        adjust_size(osize);
    }

    void reserve(size_type size) {
	A.reserve(size);
    }

    void resize(size_type size) {
        auto osize = A.size();
        A.resize(size);
        if (size > osize)
            adjust_size(osize);
    }

    void resize(size_type size, const T& default_value) {
        auto osize = A.size();
        A.resize(size, default_value);
        if (size > osize)
            adjust_size(osize);
    }

    // Find the largest idx (i <= idx < min(A.size(), (j<<1)) with prefix_sum(idx) <= value.
    // Requires: all values are non-negative (!(prefix_sum(idx) < prefix_sum(idx - 1)) for all idx within range)
    // Requires: j is power-of-two
    // O(log j)
    size_type upper_bound(T value, size_type i, size_type j) const {
        //assert("j is power of two" && j == LSB(j)));
        //assert(j < i);
        //assert((j<<1) >= i);
        auto s = A[0];
        if (value < s)
            return 0;
	size_type res = 0;
        for (; j > 0; j >>= 1)
	    if (auto p = res + j; p < i)
		if (auto t = s; !(value < (t += A[k]))) {
		    s = t;
		    res = p;
		    }
        return res + 1;
    }
    size_type upper_bound(T value, size_type i) const {
        size_type j = i;
        while(auto npow = j - LSB(j))
            j = npow;
        return upper_bound(value, i, j);
    }
    size_type upper_bound(T value) const {
	return upper_bound(value, size());
    }

    // Find the minimum idx (i <= idx < min(A.size(), (j<<1)) with prefix_sum(idx) >= value.
    // Requires: all values are non-negative
    // Requires: j is maximum power-of-two that is not greater than i
    // O(log j)
    size_type lower_bound(T value, size_type i, size_type j) const {
        auto s = A[0];
        if (!(s < value))
            return 0;
	size_type res = 0;
        for (; j > 0; j >>= 1)
	    if (auto p = res + j; p < i)
		if (auto t = s; (t += A[k]) < value) {
		    s = t;
		    res = p;
		}
        return res + 1;
    }
    size_type lower_bound(T value, size_type i) const {
        size_type j = i;
        while(auto npow = j - LSB(j))
            j = npow;
        return lower_bound(value, i, j);
    }
    size_type lower_bound(T value) const {
        return lower_bound(value, size());
    }
    
    constexpr size_type size() const { return A.size(); }
    
    constexpr bool empty() const { return A.empty(); }

    private:
    struct FenwickReference {
        FenwickTree &f;
        size_type index;
        FenwickReference(FenwickTree &f, size_type index) : f(f), index(index) {}
        FenwickReference(const FenwickReference &) = default;
        operator T() const {
            return f.get(index);
        }
        FenwickReference &operator =(T v) {
            f.set(index, v);
            return *this;
        }
	FenwickReference &operator +=(T v) {
	    f.add(index, v);
	    return *this;
	}
	FenwickReference &operator -=(T v) {
	    f.add(index, -v); // XXX possible signed overflow if (v < 0 && v == numeric_limits<T>::min())
	    return *this;
	}
        FenwickReference &operator = (const FenwickReference &fr) {
            return *this = T(fr);
        }
        bool operator == (const FenwickReference &fr) const {
            return T(*this) == T(fr);
        }
    };
    public:
    FenwickReference operator [](size_type index) {
        return FenwickReference(*this, index);
    }
    FenwickReference front() { return (*this)[0]; }
    FenwickReference back() { return (*this)[size() - 1]; }
};
namespace V1 {
    template <typename T, typename V = std::vector<T>>
    class FenwickTreeRangeUpdateRangeQuery {
	// T is expected to support 'a += b', 'a -= b', 'a + b', 'a - b', 'a * b', '-a', a cast from size_type
	FenwickTree<T, V> t1;
	FenwickTree<T, V> t2;
	public:
	typedef typename V::size_type size_type;
	typedef typename V::value_type value_type;
	FenwickTreeRangeUpdateRangeQuery(size_type size):t1(size), t2(size) {}
	FenwickTreeRangeUpdateRangeQuery(const FenwickTreeRangeUpdateRangeQuery &) = default;
	FenwickTreeRangeUpdateRangeQuery(FenwickTreeRangeUpdateRangeQuery &&) = default;
	auto size() const { return t1.size(); }
	// adds val to nums[l] ... nums[r]
	void add_range(size_type l, size_t r, const value_type&val) {
	    const auto n = size();
	    t1.add(l, val);
	    t2.add(l, val*static_cast<value_type>(l) - val);
	    if (r + 1 != size) {
		t1.add(r + 1, -val);
		t2.add(r + 1, -val*static_cast<value_type>(r));
	    }
	}
	// return 
	value_type prefix_sum(size_t i) {
	    return t1.prefix_sum(i)*static_cast<value_type>(i) - t2.prefix_sum(i);
	}
	// Returns sum of nums[i + 1] to nums[j].
	// Same as prefix_sum(j) - prefix_sum(i), but a bit faster
	value_type range_sum(size_t l, size_t r) {
	    return t1.sum(r)*static_cast<value_type>(r) - t1.prefix_sum(l)*static_cast<value_type>(l) - t2.range_sum(l, r);
	}
    };
} // V1
namespace V2 {
    template <typename T>
    class FenwickTreeRangeUpdateRangeQuery {
	// T is expected to support 'a += b', 'a -= b', 'a + b', 'a - b', 'a * b', '-a', a cast from size_type
	struct vpair {
	    T v1;
	    T v2;
	    void operator += (const T& other) { v1 += other.v1; v2 += other.v2; }
	    void operator -= (const T& other) { v1 -= other.v1; v2 -= other.v2; }
	};
	FenwickTree<vpair>> t;
	public:
	typedef decltype(t)::size_type size_type;
	typedef T value_type;
	FenwickTreeRangeUpdateRangeQuery(size_type size):t(size) {}
	FenwickTreeRangeUpdateRangeQuery(const FenwickTreeRangeUpdateRangeQuery &) = default;
	FenwickTreeRangeUpdateRangeQuery(FenwickTreeRangeUpdateRangeQuery &&) = default;
	auto size() const { return t.size(); }
	// adds val to nums[l] ... nums[r]
	void add_range(size_type l, size_t r, const value_type&val) {
	    const auto n = size();
	    t.add(l, vpair{val, val*static_cast<value_type>(l) - val});
	    if (r + 1 != size)
		t.add(r + 1, vpair {-val, -val*static_cast<value_type>(r)});
	}
	value_type prefix_sum(size_t i) {
	    auto vp = t.prefix_sum(i);
	    return vp.v1*static_cast<value_type>(i) - vp.v2;
	}
	value_type range_sum(size l, size_r) {
	    return prefix_sum(r) - prefix_sum(l);
	}
	void push_back(const value_type &val) {
	    t.push_back(vpair {val, val*static_cast<value_type>(size()) - val });
    };
} // V2
#endif
