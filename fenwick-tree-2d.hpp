#ifndef FENWICK_TREE_2D_HPP
#define FENWICK_TREE_2D_HPP
#include <vector>
template <typename T, typename V = std::vector<std::vector<T>>>
class FenwickTree2D {
    // One-based Fenwick tree with a twist (A[0] saves nums[0])
    V A;
    public:
    typedef typename V::size_type size_type;
    typedef T value_type;
    private:
    static const size_type LSB(const size_type i) {
        return i & -i;
    }

    // Convert A[] in place to Fenwick tree form
    static void init(V &A) {
	for (size_type yi = 1; yi < A[0].size(); ++yi) {
            auto yj = yi + LSB(yi);
            if (yj < A[0].size())
                A[0][yj] += A[0][yi];
	}
        for (size_type xi = 1; xi < A.size(); ++xi) {
	    auto xj = xi + LSB(xi);
	    if (xj < A.size()) {
		A[xj][0] += A[xi][0];
		for (size_type yi = 1; yi < A[0].size(); ++yi) {
		    auto yj = yi + LSB(yi);
		    if (yj < A[0].size())
			A[xj][yj] += A[xi][yi];
		}
	    }
        }
    }

    // Convert back to array of per-element counts
    static void fini(V &A) {
        for (size_type yi = A[0].size(); yi-- > 1; ) {
            auto yj = yi + LSB(yi);
            if (yj < A[0].size())
                A[0][yj] -= A[0][yi];
        }
        for (size_type xi = A.size(); xi-- > 1; ) {
            auto xj = xi + LSB(xi);
            if (xj < A.size()) {
                A[xj] -= A[xi];
		for (size_type yi = A[0].size(); yi-- > 1; ) {
		    auto yj = yi + LSB(yi);
		    if (yj < A[0].size())
			A[xj][yj] -= A[xi][yi];
		}
	    }
        }
    }
    public:
    FenwickTree2D(size_type m, size_type n):A(m, typename V::value_type(n)) {}
    FenwickTree2D(FenwickTree2D &&) = default;
    FenwickTree2D(const FenwickTree2D &) = default;

    FenwickTree2D(const V &other) : A(other.A) { init(A); }
    FenwickTree2D(V &&other) : A(move(other.A)) { init(A); }
    operator V () const & {
        V ret = A;
        fini(ret);
        return ret;
    }
    operator V () && {
        fini(A);
        return A;
    }
#if 0
    FenwickTree2D &operator =(FenwickTree2D &&) = default;
    FenwickTree2D &operator =(const FenwickTree2D &) = default;

    FenwickTree2D &operator =(const V &other) { A = other; init(A); }
    FenwickTree2D &operator =(V &&other) { A = move(other); init(A); }
#endif

    // Returns the sum of the first i elements (indices 0 to i)
    // Equivalent to range_sum(0, i)
    T prefix_sum(size_type x, size_type y) const {
        // assert(i < A.size());
        T sum = A[0][0];
	#if 0
	for (auto i = x; i != 0; i -= LSB(i))
	    sum += A[i][0];
	#endif
	for (auto j = y; j != 0; j -= LSB(j))
	    sum += A[0][j];
        for (auto i = x; i != 0; i -= LSB(i)) {
	    sum += A[i][0];
            for (auto j = y; j != 0; j -= LSB(j))
                sum += A[i][j];
	}
        return sum;
    }

    // Add delta to element with index i (zero-based)
    void add(size_type x, size_type y, T delta) {
        // assert(i < A.size());
        if (y == 0) {
	    if (x == 0) {
		A[0][0] += delta;
		return;
	    }
	    for (auto i = x; i < A.size(); i += LSB(i))
                A[i][0] += delta;
            return;
        }
	if (x == 0) {
            for (auto j = y; j < A[0].size(); j += LSB(j))
                A[0][j] += delta;
            return;
	}
        for (auto i = x; i < A.size(); i += LSB(i)) {
            for (auto j = y; j < A[0].size(); j += LSB(j))
                A[i][j] += delta;
	}
    }

    // returns sum of rectangle (xi + 1, xi + 1) to (xj, yj)
    T range_sum(size_type xi, size_type yi, size_type xj, size_type yj) const {
#if 0
        T sum = 0;
        // assert(i < A.size());
        // assert(j < A.size());
        // assert(i < j);
        for (; j > i; j -= LSB(j))
            sum += A[j];
        for (; i > j; i -= LSB(i))
            sum -= A[i];
        return sum;
#else
	auto sum = prefix_sum(xj, yj);
	sum -= prefix_sum(xi, yj);
	sum += prefix_sum(xi, yi);
	sum -= prefix_sum(xj, yi);
#endif
	return sum;
    }
    // range sum of (0:yi + 1) to (0:yj)
    T range_sum_x0(size_type yi, size_type yj) const {
        T sum = 0;
        // assert(yi < A[0].size());
        // assert(yj < A[0].size());
        // assert(yi < yj);
        for (; yj > yi; yj -= LSB(yj))
            sum += A[0][yj];
        for (; yi > yj; yi -= LSB(yi))
            sum -= A[0][yi];
        return sum;
    }
    // range sum of (0:yi + 1) to (0:yj)
    T range_sum_y0(size_type xi, size_type xj) const {
        T sum = 0;
        // assert(xi < A.size());
        // assert(xj < A.size());
        // assert(xi < xj);
        for (; xj > xi; xj -= LSB(xj))
            sum += A[xj][0];
        for (; xi > xj; xi -= LSB(xi))
            sum -= A[xi][0];
        return sum;
    }

    T range_sum_closed(size_type xi, size_type yi, size_type xj, size_type yj) const {
	auto sum = prefix_sum(xj, yj);
	if (xi > 0) {
	    sum -= prefix_sum(xi - 1, yj);
	    if (yi > 0)
		sum += prefix_sum(xi - 1, yi - 1);
	}
	if (yi > 0)
	    sum -= prefix_sum(xj, yi);
    }
#if 0 
    //
    T get(size_type i) const {
        return i == 0 ? A[0] : range_sum(i - 1, i);
    }
#endif
    
#if 0 
    //
    void set(size_type i, const T v) {
        add(i, v - get(i));
    }
#endif
    
    #if 0 // broken
    template <class... Args>
    void emplace_back(Args&&...args) {
        A.emplace_back(forward<Args>(args)...);
    }
    void push_back(T&&x) {
        return emplace_back(move(x));
    }
    void push_back(const T& x) {
        return emplace_back(x);
    }
    #endif
    
    void pop_back() {
        A.pop_back();
    }
    
#if 0
    // Find the largest idx (i <= idx <= j) with prefix_sum(i) <= value.
    // Requires: all values are non-negative
    // Regular binary search: O(log^2 j)
    size_type range_query_npow2(T value, size_type i, size_type j) const {
        while(j > i) {
            size_type m = i + (j - i + 1) / 2;
            if (prefix_sum(m) > value) {
                j = m - 1;
            } else {
                i = m;
            }
        }
        return i; 
    }
    size_type range_query_npow2(T value, size_type i = 0) const {
        return range_query_npow2(value, i, size() - 1);
    }

    // Find the largest idx (i <= idx <= j) with prefix_sum(idx) <= value.
    // Requires: all values are non-negative
    // Requires: j is power-of-two
    // O(log j)
    size_type range_query_pow2(T value, size_type i, size_type j) const {
        value -= A[0];
        assert("j is power of two" && (j & (j - 1)) == 0);
        for (; j > 0; j >>= 1) {
            if (i + j < A.size() && A[i + j] <= value) {
                value -= A[i + j];
                i += j;
            }
        }
        return i;
    }
    size_type range_query_pow2(T value, size_type i = 0) const {
        return range_query_pow2(value, i, size() - 1);
    }
    
    size_type range_query(T value, size_type i, size_type j) const {
        return (j & (j - 1)) ? range_query_npow2(i, j) : range_query_pow2(i, j);
    }
    size_type range_query(T value, size_type i = 0) const {
        return range_query(value, i, size() - 1);
    }
    
    size_type size() const {
        return A.size();
    }
    
    bool empty() const {
        return A.empty();
    }

    private:
    struct FenwickReference {
        FenwickTree &f;
        size_type index;
        FenwickReference(FenwickTree &f, size_type index) : f(f), index(index) {}
        FenwickReference(const FenwickReference & fr) : FenwickReference(fr.f, fr.index) {}
        operator T() const {
            return f.get(index);
        }
        FenwickReference &operator =(T v) {
            f.set(index, v);
            return *this;
        }
        T operator = (const FenwickReference &fr) {
            return *this = T(fr);
        }
        bool operator == (const FenwickReference &fr) {
            return T(*this) == T(fr);
        }
        bool operator < (const FenwickReference &fr) {
            return T(*this) < T(fr);
        }
    };
    public:
    FenwickReference operator [](size_type index) {
        return FenwickReference(*this, index);
    }
    FenwickReference front() { return (*this)[0]; }
    FenwickReference back() { return (*this)[size() - 1]; }
#endif
};
#endif // FENWICK_TREE_2D_HPP
