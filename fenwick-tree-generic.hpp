#ifndef FENWICK_TREE_GENERIC_HPP
#define FENWICK_TREE_GENERIC_HPP
#include <algorithm>
template <typename V, typename Op>
class FenwickGeneric {
	// V must be container type with
	// ```
	//	typedef ... value_type;
	//	typedef ... size_type;
	//	reference &operator[](size_type);
	//	size_type size();
	// ```
	//  and optionally
	// ```
	//	void reserve(size_type);
	//	size_type resize(size_type);
	//	size_type resize(size_type, const value_type &);
	//	value_type &emplace_back(Args... &&);
	//	void pop_back();
	// ```
	// (vector, array, deque, ...)
	// Op must be
	// 1) binary operation -- callable as `void operator() (value_type &, value_type)`
	// 2) commutative (a op b = b op a),
	//    `auto v = a; op(v, b);` and `auto v = b; op(v, a)` result `v` must be same
	// 3) associative (((a op b) op c) == (a op (b op c)))
	//    `auto v = a; op(v, b); op(v, c);` and `auto v = c; op(v, b); op(v, a);` result `v` must be same
	// e.g.
	// prefix max (note that new value in add() must be not lesser that current):
	//  Fenwick f(vector<int>(size, numeric_limits<int>::min()), [](auto &a, auto b) { if (a < b) a = b; }, true);
	// prefix min (note that new value in add() must be not greater that current):
	//  Fenwick f(vector<int>(size, numeric_limits<int>::max()), [](auto &a, auto b) { if (a > b) a = b; }, true);
	// prefix sum:
	//  Fenwick f(vector<int>(size, 0), [](auto &a, auto b) { a += b; }, true);
	// prefix sum with arbitrary init values:
	//  Fenwick f(vector<int>{1,2,3,4,5}, [](auto &a, auto b) { a += b; });
	// prefix product:
	//  Fenwick f(vector<int>(size, 1), [](auto &a, auto b) { a *= b; }, true);
	// prefix modulo product:
	//  Fenwick f(vector<int>(size, 1), [m](auto &a, auto b) { a = a*static_cast<int64_t>(b) % m; }, true);
	V A;
	[[no_unique_address]]
	Op op;
	public:
	typedef typename V::value_type value_type;
	typedef typename V::size_type size_type;
	private:
	// return least significant bit of i
	static size_type LSB(size_type i) {
		return i & -i;
	} 
	void init() { // time = O(n)
		for (size_type i = 1; i < A.size(); ++i) {
			auto j = i + LSB(i);
			if (j < A.size())
				op(A[j], A[i]);
		}
	}
	public:
	// set no_init to true if A consists of only neutral element
	// (0 for addition, 1 for multiplication, equal values for min/max)
	// time = no_init ? O(1) : O(n)
	FenwickGeneric(V &&A, Op op = Op{}, bool no_init = false):A{A}, op{op} { if (!no_init) init(); }
	// time = O(n)
	FenwickGeneric(const V &A, Op op = Op{}, bool no_init = false):A{A}, op{op} { if (!no_init) init(); }
	constexpr auto size() const { return A.size(); }
	constexpr auto empty() const { return A.empty(); }
	// requires `op(current_value, v), current_value == new_value`
	// for op == max v must be equal or greater than current value
	// for op == min v must be equal or less than current value
	// time = O(log n)
	void add(size_type idx, const value_type &v) {
		if (idx == 0) {
			op(A[0], v);
			return;
		}
		for (; idx < A.size(); idx += LSB(idx))
			op(A[idx], v);
	}
	// return op(A[0], op(A[1], op(A[2], ... op(A[idx - 1], A[idx]))))
	// time = O(log idx)
	value_type prefix_sum(size_type idx) const {
		auto v = A[0];
		for (; idx != 0; idx -= LSB(idx))
			op(v, A[idx]);
		return v;
	}
	// v = range_sum(i, j); r.first = op(r.second, op(...op(A[i + 1], A[i + 2]), ... A[j]));
	// if inverse() operation defined, we can calculate
	// op(...op(A[i + 1], A[i + 2), ... A[j]) as op(v.first, inverse(v.second))
	// a bit faster than op(prefix_sum(j), inverse(prefix_sum(i)))
	auto range_sum(size_type i, size_type j) const {
		auto sum = std::make_pair(A[0], A[0]);
		// assert(i < A.size());
		// assert(j < A.size());
		// assert(i < j);
		for (; j > i; j -= LSB(j))
			op(sum.first, A[j]);
		for (; i > j; i -= LSB(i))
			op(sum.second, A[i]);
		return sum;
	}

	template <class... Args>
	void emplace_back(Args&&...args) {
		auto cidx = A.size();
		auto &back = A.emplace_back(std::forward<Args>(args)...);
		if (!cidx)
			return;
		for (auto j = cidx - 1; j + LSB(j) == cidx; j -= LSB(j))
			op(back, A[j]);
	}

	void push_back(value_type&&x) {
		return emplace_back(std::move(x));
	}
	void push_back(const value_type& x) {
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
		for (; i < A.size(); ++i) {
			auto j = i + LSB(i);
			if (j >= osize && j < A.size())
				op(A[j], A[i]);
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

	void resize(size_type size, const value_type& default_value) {
		auto osize = A.size();
		A.resize(size, default_value);
		if (size > osize)
			adjust_size(osize);
	}

	void resize(size_type size) {
		auto osize = A.size();
		A.resize(size);
		if (size > osize)
			adjust_size(osize);
	}

	void reset(size_type size, const value_type& default_value, bool no_init = false) {
		auto n = A.size();
		A.clear();
		A.resize(n, default_value);
		if (!no_init)
			init(A);
	}

	// search for first index such that prefix_sum(idx) > key and idx < j,
	// return j prefix_sum(size() - 1)) <= key)
	template <typename Key, typename Comp = std::less<Key>>
	size_type upper_bound(const Key &key, size_type j, size_type jpow2, Comp comp = Comp()) const {
		// jpow2 is must be maximum power of two such that jpow2 <= j
		// e.g. (static_cast<size_type>(1)<<(sizeof(size_type)*8 - 1 - clz(j)))
		auto acc = A[0];
		if (comp(key, acc))
			return 0;
		size_type res = 0;
		for (auto k = jpow2; k > 0; k >>= 1) {
			if (auto p = res + k; p < j) {
				auto temp = acc;
				op(temp, A[p]);
				if (!comp(key, temp)) {
					acc = std::move(temp);
					res = p;
				}
			}
		}
		return res + 1;
	}

	template <typename Key, typename Comp = std::less<Key>>
	size_type upper_bound(const Key &key, size_type j, Comp comp = Comp()) const {
		auto jpow2 = j;
		while(auto npow = jpow2 & (jpow2 - 1))
			jpow2 = npow;
		return upper_bound(key, j, jpow2, comp);
	}

	template <typename Key, typename Comp = std::less<Key>>
	size_type upper_bound(const Key &key, Comp comp = Comp()) const {
		return upper_bound(key, A.size(), comp);
	}

	template <typename Key, typename Comp = std::less<Key>>
	size_type lower_bound(const Key &key, size_type j, size_type jpow2, Comp comp = Comp()) const {
		// jpow2 is must be maximum power of two such that jpow2 <= j
		auto acc = A[0];
		if (!comp(acc, key))
			return 0;
		size_type res = 0;
		for (auto k = jpow2; k > 0; k >>= 1) {
			if (auto p = res + k; p < j) {
				auto temp = acc;
				op(temp, A[p]);
				if (comp(temp, key)) {
					acc = std::move(temp);
					res = p;
				}
			}
		}
		return res + 1;
	}

	template <typename Key, typename Comp = std::less<Key>>
	size_type lower_bound(const Key &key, size_type j, Comp comp = Comp()) const {
		auto jpow2 = j;
		while(auto npow = jpow2 & (jpow2 - 1))
			jpow2 = npow;
		return lower_bound(key, j, jpow2, comp);
	}

	template <typename Key, typename Comp = std::less<Key>>
	size_type lower_bound(const Key &key, Comp comp = Comp()) const {
		return lower_bound(key, A.size(), comp);
	}
};
#endif // FENWICK_TREE_GENERIC_HPP
