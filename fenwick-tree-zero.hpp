template <typename T, typename V = vector<T>>
class FenwickTree {
    // Zero-based Fenwick tree
    V A;
    public:
    typedef typename V::size_type size_type;
    private:

    // Convert A[] in place to Fenwick tree form
    static void init(V &A) {
        for (size_type i = 0; i < A.size(); ++i) {
            auto j = i | (i + 1);
            if (j < A.size())
                A[j] += A[i];
        }
    }

    // Convert back to array of per-element counts
    static void fini(V &A) {
        for (size_type i = A.size(); i-- > 0; ) {
            auto j = i | (i + 1);
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

    FenwickTree(V &&nums):A(move(nums)) {
        init(A);
    }

    operator V () const & {
        V ret = A;
        fini(ret);
        return ret;
    }
    operator V () && {
        fini(A);
        return A;
    }

    // Returns the sum of the first i elements (indices 0 to i)
    // Equivalent to range_sum(0, i)
    T prefix_sum(size_type i) const {
        // assert(i < A.size());
        T sum = 0;
	cout << i << endl;
	for (;;) {
	    sum += A[i];
	    if ((i &= i + 1) == 0)
 		break;
	    --i;
	}	    
		cout << "ps" << sum << endl;
        return sum;
    }

    // Add delta to element with index i (zero-based)
    void add(size_type i, T delta) {
        // assert(i < A.size());
        for (; i < A.size(); i |= i + 1)
            A[i] += delta;
    }

    // Returns sum of nums[i + 1] to nums[j].
    // Same as prefix_sum(j) - prefix_sum(i), but a bit faster
    T range_sum(size_type i, size_type j) const {
	    cout << "XXX" << i << ' ' << j << endl;
#if 1 // TODO
		T sum = 0;
	if (j <= i) return 0;
	auto leni = i == 0 ? 32 : __builtin_clz(i);
	auto lenj = j == 0 ? 32 : __builtin_clz(j);
	cout << leni << ' ' << lenj <<endl;
	if (leni != lenj) {
	    for (;;) {
		sum += A[j];
		if ((j &= j + 1) == 0)
		    break;
		--j;
	    }
	    for (;;) {
		sum -= A[i];
		if ((i &= i + 1) == 0)
		    break;
		--i;
	    }
	    cout << 'a' << prefix_sum(j) - prefix_sum(i) << " vs " << sum <<endl;
	    return sum;
	}
	for (;;) {
	    if (i < j) {
		sum += A[j];
		if ((j &= j + 1) == 0)
		    break;
		--j;
	    } else if (j < i) {
		sum -= A[i];
		if ((i &= i + 1) == 0)
		    break;
		--i;
	    } else {
		break;
	    }
	}
	cout << 'b' << prefix_sum(j) - prefix_sum(i) << " vs " << sum <<endl;
        return prefix_sum(j) - prefix_sum(i);
        return sum;
#else
	return prefix_sum(j) - prefix_sum(i);
#endif 
    }
    
    //
    T get(size_type i) const {
        return i == 0 ? A[0] : range_sum(i - 1, i);
    }
    
    //
    void set(size_type i, const T v) {
        add(i, v - get(i));
    }
    
    template <class... Args>
    void emplace_back(Args&&...args) {
	auto osize = A.size();
        A.emplace_back(forward<Args>(args)...);
	auto i = osize;
	auto j = osize & (osize + 1);
	while (j < i) {
	    A.back() += A[i - 1];
	    i &= i - 1;
	}
    }
    void push_back(T&&x) {
        return emplace_back(move(x));
    }
    void push_back(const T& x) {
        return emplace_back(x);
    }

    template <class Iterator>
    void append(Iterator begin, Iterator end, size_type hint = 0) {
	A.reserve(A.size() + hint);
	auto osize = A.size();
	for (; begin != end; ++begin)
	    A.push_back(*begin);
	auto i = osize;
	while((i & (i - 1)) != 0)
		i &= i - 1;
	if (i) --i;
	for (; i < A.size(); i++) {
	    auto j = i | (i + 1);
	    if (j >= osize && j < A.size())
		A[j] += A[i];
	}
    }
    
    void pop_back() {
        A.pop_back();
    }

    void clear() {
	A.clear();
    }
    
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
};
