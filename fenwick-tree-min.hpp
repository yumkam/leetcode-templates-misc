#ifndef FENWICK_TREE_MIN_HPP
#define FENWICK_TREE_MIN_HPP
#include <vector>
#include <cassert>
// basic subset
template <typename T, typename V = std::vector<T>>
class FenwickTree {
    // One-based Fenwick tree with a twist (A[0] saves nums[0])
    V A;
    // for all operations T must support 'a += b'
    // initial value of T assumed to be neutral (T{} + T{} == T{})
    public:
    typedef typename V::size_type size_type;
    typedef typename V::value_type value_type;
    private:
    static constexpr size_type LSB(const size_type i) {
        return i & -i;
    }

    public:
    FenwickTree(size_type size = 0):A(size) {
        static_assert([](){ T v {}; v += T{}; return v; }() == T{});
    }

    FenwickTree(const FenwickTree &) = default;
    FenwickTree(FenwickTree &&) = default;
    FenwickTree &operator = (const FenwickTree &) = default;   
    FenwickTree &operator = (FenwickTree &&) = default;

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

    constexpr size_type size() const { return A.size(); }

    constexpr bool empty() const { return A.empty(); }
};
#endif
// vim: sw=4 et
