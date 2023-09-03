#ifndef SEGMENT_TREE_NREC_HPP
#define SEGMENT_TREE_NREC_HPP
#include <iterator>
#include <limits>
#include <numeric>
// See https://codeforces.com/blog/entry/18051
// Title: "Efficient and easy segment trees"
// Author: Al.Cash <https://codeforces.com/profile/Al.Cash>
// Date: 2015-05-23 13:51
namespace SegmentTree {
    template <typename T>
        struct FunctionalMin {
            static constexpr T neutral = std::numeric_limits<T>::max();
            const T &operator() (const T&a, const T&b) const { return std::min(a, b); }
        };
    template <typename T>
        struct FunctionalMax {
            static constexpr T neutral = std::numeric_limits<T>::min();
            const T &operator() (const T&a, const T&b) const { return std::max(a, b); }
        };
    template <typename T>
        struct FunctionalGCD {
            static constexpr T neutral = 0;
            T operator() (const T a, const T b) { return std::gcd(a, b); }
        };
    namespace PointUpdateRangeQuery {
        template<typename Container, typename Combine>
            class SegTree {
                // combine should be callable as `value_type combine(const value_type &, const value_type &)`
                // container should support `value_type &operator[](size_type)` and `size_type size() const`
                typedef typename Container::size_type size_type;
                typedef typename Container::size_type index_type;
                typedef typename Container::value_type value_type;
                Container t;
                [[no_unique_address]]
                Combine combine;
                public:
                SegTree(index_type size, Combine combine = Combine()) : t(size*2), combine{combine} {
                    if (combine(t[0], t[0]) != t[0])
                        init();
                }
                template <typename Source>
                    SegTree(index_type size, Source src, Combine combine = Combine()) : t(size*2), combine(combine) {
                        init(src);
                    }
                SegTree(index_type size, const value_type &initv, Combine combine = Combine()) : t(size*2, initv), combine{combine} {
                    if (combine(initv, initv) != initv)
                        init();
                }
                template <typename Iterator>
                    SegTree(Iterator begin, Iterator end, Combine combine = Combine()) : SegTree(std::distance(begin, end), [begin]() mutable { return *begin++; }, combine) {
                    }
                SegTree(const Container &c, Combine combine = Combine()) : SegTree(c.cbegin(), c.cend(), combine) {
                }
                SegTree(Container &&c, Combine combine = Combine()) : SegTree(c.size(), [begin = c.begin()]() mutable { return std::move(*begin++); }, combine) {
                }
                SegTree(const SegTree &) = default;
                SegTree(SegTree &&) = default;
                constexpr auto size () const noexcept { return t.size() >> 1; }
                template<typename Source>
                    void init(Source src) {
                        index_type n = size();
                        for (index_type i = 0; i < n; i++)
                            t[i + n] = src();
                        init();
                    }
                void reset(const value_type &initv) {
                    index_type n = size();
                    if (combine(initv, initv) == initv)
                        fill(t.begin(), t.end(), initv);
                    else {
                        fill(t.begin() + n, t.end(), initv);
                        init();
                    }
                }
                private:
                void init() {
                    index_type n = size();
                    for (index_type i = n; i--; )
                        t[i] = combine(t[i<<1], t[(i<<1)^1]);
                }
                public:
                void update(index_type pos, const value_type &val) {
                    index_type n = size();
                    pos += n;
                    t[pos] = val;
                    while(pos > 1) {
                        auto next = pos >> 1;
                        t[next] = combine(std::as_const(t)[pos], std::as_const(t)[pos ^ 1]);
                        pos = next;
                    }
                }
                // `updater(value_type &v)` is supposed to be able to change
                // any result of combine(a_i,...combine(a_j^old, combine(...)))
                // to result of combine(a_i,...combine(a_j^new, combine(...)))
                // this makes sense when combine is expensive, but update is not,
                // e.g. for `multiset`
                template <typename Updater>
                void replace(index_type pos, Updater updater) {
                    index_type n = size();
                    pos += n;
                    updater(t[pos]);
                    while(pos > 1) {
                        auto next = pos >> 1;
                        updater(t[next]);
                        pos = next;
                    }
                }
                const auto &query(index_type index) const {
                    index_type n = size();
                    return t[n + index];
                }
                auto query_halfopen(index_type l, index_type r, value_type res) const { // [ l; r )
                    index_type n = size();
                    for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
                        if ((l & 1))
                            res = combine(res, t[l++]);
                        if ((r & 1))
                            res = combine(res, t[--r]);
                    }
                    return res;
                }
                // search for first index in [l; r) so that pred(query_halfopen(l, r, res)) is true
                // expects tree partitioned in a way there exists index k in [l;r]
                // such that for all i in [l;r) `pred(query_halfopen(l, i, res))` is same as `i >= k`
                template <typename Pred>
                auto search_index_halfopen(index_type l, index_type r, value_type res, Pred pred) const { // [ l; r )
#if 0
                    if ((size() & (size() - 1)) == 0) {
                        // O(log n)
                        auto ll = l + n;
                        auto rr = r + n;
                        vector<pair<index_type, index_type>> stack;
                        for (;;) {
                            if (pred(res))
                                break;
                            if (ll >= rr) {
                                if (stack.empty())
                                    break;
                                tie(ll, rr) = stack.back();
                                stack.pop_back();
                                continue;
                            }
                            if ((ll & 1)) {
                                res = combine(res, t[ll++]);
                                continue;
                            }
                            if (ll < n) {
                                auto tmp = combine(res, t[(ll<<1)]);
                                if (pred(tmp)) {

                                }
                            }
                        }
                    }
#endif
                    // O(log^2 n)
                    auto lo = l;
                    auto hi = r;
                    while(lo < hi) {
                        auto mid = lo + (hi - lo)/2;
                        auto midval = query_halfopen(lo, mid + 1, res);
                        if (pred(midval))
                            hi = mid;
                        else {
                            res = std::move(midval);
                            lo = mid + 1;
                        }
                    }
                    return lo;
#endif
                }
                // extras
                template <typename Result, typename Reduce>
                Result query_reduce_halfopen(index_type l, index_type r, Result res, Reduce reduce) const { // [ l; r )
                    index_type n = size();
                    for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
                        if ((l & 1))
                            res = reduce(res, t[l++]);
                        if ((r & 1))
                            res = reduce(res, t[--r]);
                    }
                    return res;
                }
                /* e.g. ```
                auto combine = [](const auto &a, const auto &b) {
                        decltype(a) res;
                        std::merge(a.begin(), a.end(), b.begin(), b.end(), std::back_inserter(res));
                        return res;
                };
                vector<int> src { 5, 1, 5, 3 1, 5 };
                auto srcgen = [i = src.cbegin()]() mutable { return *i++; };
                SegTree<vector<vector<int>>, decltype(combine)> st(src.size(), srcgen, combine);
                auto query_min_greater = [](const auto &st, auto l, auto h, auto x) {
                    auto reduce = [x](auto res, const auto &l) {
                            auto lb = lower_bound(l.cbegin(), l.cend());
                            if (lb == l.cend()) return res;
                            return min(res, *lb);
                    };
                    return st.query_reduce_halfopen(1, 3, std::numeric_limits<decltype(x)>::max(), reduce);
                };
                std::cout << query_min_greater(1, 3, 2) << '\n';
                ```
                */
                // for non-commutative combine
                auto query_halfopen(index_type l, index_type r, value_type resl, value_type resr) const { // [ l; r )
                    index_type n = size();
                    for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
                        if ((l & 1))
                            resl = combine(resl, t[l++]);
                        if ((r & 1))
                            resr = combine(t[--r], resr);
                    }
                    return combine(resl, resr);
                }
                auto query(index_type l, index_type r, value_type res) const { // [ l; r]
                    const index_type n = size();
                    for (l += n, r += n; l <= r; l >>= 1, r >>= 1) {
                        if (l & 1) {
                            res = combine(res, t[l]); ++l;
                        }
                        if (!(r & 1)) {
                            res = combine(t[r], res); --r;
                        }
                    }
                    return res;
                }
                // for non-commutative combine
                auto query(index_type l, index_type r, value_type resl, value_type resr) const { // [ l; r]
                    index_type n = size();
                    for (l += n, r += n; l <= r; l >>= 1, r >>= 1) {
                        if ((l & 1)) {
                            resl = combine(resl, t[l]); ++l;
                        }
                        if (!(r & 1)) {
                            resr = combine(t[r], resr); --r;
                        }
                    }
                    return combine(resl, resr);
                }
                auto query(index_type l, index_type r) { // [ l; r]
                    return query_halfopen(l, r, query(r));
                }
                private:
                struct reference {
                    SegTree &f;
                    size_type index;
                    reference(SegTree &f, size_type index) : f(f), index(index) {}
                    reference(const reference &) = default;
                    operator value_type() const {
                        return f.query(index);
                    }
                    reference &operator =(value_type v) {
                        f.update(index, v);
                        return *this;
                    }
#if 0
                    reference &operator = (const reference &fr) {
                        return *this = value_type(fr);
                    }
                    bool operator == (const reference &fr) const {
                        return value_type(*this) == value_type(fr);
                    }
#endif
                };
                public:
                reference operator [](size_type index) {
                    return reference(*this, index);
                }
                auto operator [](size_type index) const {
                    return query(index);
                }
                reference front() { return (*this)[0]; }
                reference back() { return (*this)[size() - 1]; }
                auto front() const { return (*this)[0]; }
                auto back() const { return (*this)[size() - 1]; }
            };
        template<typename Container>
        using SegTreeSum = SegTree<Container, std::plus<typename Container::value_type>>;
        template<typename Container>
        using SegTreeMin = SegTree<Container, FunctionalMin<typename Container::value_type>>;
        template<typename Container>
        using SegTreeMax = SegTree<Container, FunctionalMax<typename Container::value_type>>;
    }
    namespace PointUpdateRangeQuery2D {
        template<typename Container, typename Combine>
            class SegTree {
                typedef typename Container::size_type size_type;
                typedef typename Container::size_type index_type;
                typedef typename Container::value_type value_type;
                Container t;
                size_type m_height;
                size_type m_width;
                [[no_unique_address]]
                Combine combine;
                public:
                SegTree(index_type height, index_type width, Combine combine = Combine()) : t(width*height*2*2), m_height(height), m_width(width), combine{combine} {
                    if (combine(t[0], t[0]) != t[0])
                        init();
                }
                template <typename Source>
                    SegTree(index_type height, index_type width, Source src, Combine combine = Combine()) : t(width*height*2*2), m_height(height), m_width(width), combine(combine) {
                        init(src);
                    }
                SegTree(index_type height, index_type width, const value_type &initv, Combine combine = Combine()) : t(width*height*2*2, initv), m_height(height), m_width(width), combine(combine) {
                    if (combine(initv, initv) != initv)
                        init();
                }
#if 0
                template <typename Iterator>
                    SegTree(index_type width, Iterator begin, Iterator end, Combine combine = Combine()) : SegTree(std::distance(begin, end)/width, width, [begin]() mutable { return *begin++; }, combine) {
                    }
#endif
                SegTree(index_type width, const Container &c, Combine combine = Combine()) : SegTree(width, c.cbegin(), c.cend(), combine) {
                }
                SegTree(index_type width, Container &&c, Combine combine = Combine()) : SegTree(c.size()/width, width, [begin = c.begin()]() mutable { return std::move(*begin++); }, combine) {
                }
                template <typename VofV>
                    SegTree(const VofV& c, Combine combine = Combine()) : SegTree(c.size(), c[0].size(), [ii=c.cbegin(), i=c[0].cbegin(), e=c[0].cend()] () mutable { if (i == e) { i = (++ii)->cbegin(); e = ii->cend(); } return *i++; }, combine) {} // VofV is expected to be non-empty vector<vector<T>>-alike type with same c[i].size()
                template <typename VofV>
                    SegTree(VofV&& c, Combine combine = Combine()) : SegTree(c.size(), c[0].size(), [ii=c.begin(), i=c[0].cbegin(), e=c[0].cend()] () mutable { if (i == e) { i = (++ii)->cbegin(); e = ii->cend(); } return std::move(*i++); }, combine) {} // VofV is expected to be non-empty vector<vector<T>>-alike type with same c[i].size()
                SegTree(const SegTree &) = default;
                SegTree(SegTree &&) = default;
                constexpr auto width () const noexcept { return m_width; }
                constexpr auto height () const noexcept { return m_height; }
                private:
                template<typename Source>
                    void init(Source src) {
                        const auto n = width();
                        //const auto h = height();
                        const auto m = t.size() >> 1;
                        // assign raw values for unmerged trees
                        for (index_type i = m + n; i < t.size(); i += n)
                            for (index_type j = 0; j < n; ++j, ++i)
                                t[i] = src();
                        init();
                    }
                void init() {
                    // time = O(n*m)
                    const auto n = width();
                    //const auto h = height();
                    const auto n2 = n*2;
                    const auto m = t.size() >> 1;
                    // make low-layer trees
                    for (index_type i = m; i < t.size(); i += n2)
                        for (index_type j = n; j--; )
                            t[i + j] = combine(std::as_const(t)[i + (j<<1)], std::as_const(t)[i + ((j<<1)^1)]);
                    // make upper-layer merged trees
                    for (index_type i = m; i; ) {
                        i -= n2;
                        auto a = i<<1;
                        auto b = a + n2;
                        for (index_type j = 0; j < n2; ++j)
                            t[i + j] = combine(std::as_const(t)[a + j], std::as_const(t)[b + j]);
                    }
                }
                public:
                void update(index_type row, index_type col, const value_type &val) {
                    // time = O(log n*log m)
                    const auto n = width();
                    const auto h = height();
                    const auto n2 = n<<1; 
                    //index_type m = t.size() >> 1;
                    row += h;
                    col += n;
                    auto rp = row*n2; // start of row
                    t[rp + col] = val;
                    while(row > 1) {
                        auto nextr = row >> 1;
                        auto nextrp = (row & 1 ? rp - n2 : rp) >> 1; // = nextr*n2;
                        auto altrp = row & 1 ? rp - n2 : rp + n2; // = (row ^ 1)*n2;
                        t[nextrp + col] = combine(std::as_const(t)[rp + col], std::as_const(t)[altrp + col]);
                        for (auto c = col; c > 1; ) {
                            auto nextc = c >> 1;
                            t[rp + nextc] = combine(std::as_const(t)[rp + c], std::as_const(t)[rp + (c^1)]);
                            t[nextrp + nextc] = combine(std::as_const(t)[altrp + nextc], std::as_const(t)[rp + nextc]);
                            c = nextc;
                        }
                        row = nextr;
                        rp = nextrp;
                    }
                }
                auto query(index_type i, index_type j) const {
                    const auto n = width();
                    //const auto h = height();
                    const auto n2 = n<<1; 
                    const auto m = t.size() >> 1;
                    return t[m + i*n2 + j];
                }
                auto query_halfopen(index_type u, index_type ll, index_type b, index_type rr, value_type res = value_type {}) const { // [ upper, left; bottom, right )
                    // time = O(log n*log m)
                    const auto n = width();
                    const auto h = height();
                    const auto n2 = n*2;
                    for (u += h, b += h; u < b; u >>= 1, b >>= 1) {
                        if ((u & 1)) {
                            auto rp = (u++)*n2;
                            for (auto l = ll + n, r = rr + n; l < r; l >>= 1, r >>= 1) {
                                if ((l & 1))
                                    res = combine(res, t[rp + l++]);
                                if ((r & 1))
                                    res = combine(res, t[rp + --r]);
                            }
                        }
                        if ((b & 1)) {
                            auto rp = (--b)*n2;
                            for (auto l = ll + n, r = rr + n; l < r; l >>= 1, r >>= 1) {
                                if ((l & 1))
                                    res = combine(res, t[rp + l++]);
                                if ((r & 1))
                                    res = combine(res, t[rp + --r]);
                            }
                        }
                    }
                    return res;
                }
            };
        template<typename Container>
        using SegTreeSum = SegTree<Container, std::plus<typename Container::value_type>>;
        template<typename Container>
        using SegTreeMin = SegTree<Container, FunctionalMin<typename Container::value_type>>;
        template<typename Container>
        using SegTreeMax = SegTree<Container, FunctionalMax<typename Container::value_type>>;
    }
    namespace RangeUpdatePointQuery {
        template<typename Container, typename Combine, typename Container::value_type Init>
            class SegTree {
                typedef typename Container::size_type size_type;
                typedef typename Container::size_type index_type;
                typedef typename Container::value_type value_type;
                Container t;
                [[no_unique_address]]
                Combine combine;
                public:
                SegTree(index_type size, Combine combine = Combine()) : t(size*2, Init), combine{combine} {
                }
                template <typename Source>
                    SegTree(index_type size, Source src, Combine combine = Combine()) : t(size*2, Init), combine(combine) {
                        init(src);
                    }
                SegTree(index_type size, const value_type &init, Combine combine = Combine()) : SegTree(size, [&init]() { return init; }, combine) {
                }
                template <typename Iterator>
                    SegTree(Iterator begin, Iterator end, Combine combine = Combine()) : SegTree(std::distance(begin, end), [begin]() mutable { return *begin++; }, combine) {
                    }
                SegTree(const Container &c, Combine combine = Combine()) : SegTree(c.cbegin(), c.cend(), combine) {
                }
                SegTree(Container &&c, Combine combine = Combine()) : SegTree(c.size(), [begin = c.begin()]() mutable { return std::move(*begin++); }, combine) {
                }
                SegTree(const SegTree &) = default;
                SegTree(SegTree &&) = default;
                constexpr auto size () const noexcept { return t.size() >> 1; }
                private:
                template<typename Source>
                    void init(Source src) {
                        index_type n = size();
                        for (index_type i = 0; i < n; i++)
                            t[i + n] = src();
                        init();
                    }
                void init() {}
                public:
                auto query(index_type index, value_type res) const {
                    // O(log n)
                    const index_type n = size();
                    for (index += n; index > 0; index >>= 1)
                        res = combine(res, t[index]);
                    return res;
                }
                auto query(index_type index) const {
                    return query(index, Init);
                }
                void update(index_type l, index_type r, const value_type &val) { // [ l; r]
                    // O(log n)
                    const index_type n = size();
                    for (l += n, r += n; l <= r; l >>= 1, r >>= 1) {
                        if ((l & 1)) {
                            t[l] = combine(std::as_const(t)[l], val); ++l;
                        }
                        if (!(r & 1)) {
                            t[r] = combine(val, std::as_const(t)[r]); --r;
                        }
                    }
                }
                void update_halfopen(index_type l, index_type r, const value_type &val) { // [ l; r )
                    // O(log n)
                    const index_type n = size();
                    for (l += n, r += n; l < r; l >>= 1, r >>= 1) {
                        if ((l & 1)) {
                            t[l] = combine(val, std::as_const(t)[l]); ++l;
                        }
                        if ((r & 1)) {
                            --r; t[r] = combine(std::as_const(t)[r], val);
                        }
                    }
                }
                void push() { // O(n)
                    const index_type n = size();
                    for (index_type i = 1; i < n; ++i) {
                        auto root = i<<1;
                        t[root] = combine(std::as_const(t)[root], std::as_const(t)[i]);
                        t[root ^ 1] = combine(std::as_const(t)[root ^ 1], std::as_const(t)[i]);
                        t[i] = Init;
                    }
                }
                auto &query_raw(index_type index) { // must call push() first
                    return t[size() + index];
                }
                auto operator [](size_type index) const {
                    return query(index);
                }
            };
        template<typename Container>
        using SegTreeSum = SegTree<Container, std::plus<typename Container::value_type>, 0>;
        template<typename Container>
        using SegTreeMin = SegTree<Container, FunctionalMin<typename Container::value_type>, FunctionalMin<typename Container::value_type>::neutral>;
        template<typename Container>
        using SegTreeMax = SegTree<Container, FunctionalMax<typename Container::value_type>, FunctionalMax<typename Container::value_type>::neutral>;
    }
    namespace RangeUpdateRangeQueryLazy {
        template<typename Container, typename CombineU, typename CombineQ, typename Container::value_type Init>
            class SegTree {
                // requires:
                // combineU(combineQ(a, b), c) == combineQ(combineU(a, c), combineU(b, c))
                // combineU(Init, a) == Init
                // combineQ(Init, a) == Init
                typedef typename Container::size_type size_type;
                typedef typename Container::size_type index_type;
                typedef typename Container::value_type value_type;
                Container t;
                Container d;
                [[no_unique_address]]
                CombineU combineU;
                [[no_unique_address]]
                CombineQ combineQ;
                index_type h;
                static unsigned ilog2(unsigned i) { return sizeof(i)*8 - __builtin_clz(i); }
                static unsigned ilog2(unsigned long i) { return sizeof(i)*8 - __builtin_clzl(i); }
                static unsigned ilog2(unsigned long long i) { return sizeof(i)*8 - __builtin_clzll(i); }
                public:
                SegTree(index_type size, CombineU combineU = CombineU(), CombineQ combineQ = CombineQ()) : t(size*2, Init), d(size, Init), combineU{combineU}, combineQ{combineQ}, h{ilog2(size)} {
                    if (combineQ(Init, Init) != Init)
                        init();
                }
                template <typename Source>
                    SegTree(index_type size, Source src, CombineU combineU = CombineU(), CombineQ combineQ = CombineQ()) : t(size*2, Init), d(size, Init), combineU{combineU}, combineQ{combineQ}, h{ilog2(size)} {
                        init(src);
                    }
                SegTree(index_type size, const value_type &init, CombineU combineU = CombineU(), CombineQ combineQ = CombineQ()) : SegTree(size, [&init]() { return init; }, combineU, combineQ) {
                    if (combineQ(init, init) != init)
                        init();
                }
                template <typename Iterator>
                    SegTree(Iterator begin, Iterator end, CombineU combineU = CombineU(), CombineQ combineQ = CombineQ()) : SegTree(std::distance(begin, end), [begin]() mutable { return *begin++; }, combineU, combineQ) {
                    }
                SegTree(const Container &c, CombineU combineU = CombineU(), CombineQ combineQ = CombineQ()) : SegTree(c.cbegin(), c.cend(), combineU, combineQ) {
                }
                SegTree(Container &&c, CombineU combineU = CombineU(), CombineQ combineQ = CombineQ()) : SegTree(c.size(), [begin = c.begin()]() mutable { return std::move(*begin++); }, combineU, combineQ) {
                }
                SegTree(const SegTree &) = default;
                SegTree(SegTree &&) = default;
                template<typename Source>
                    void init(Source src) {
                        index_type n = size();
                        for (index_type i = 0; i < n; i++)
                            t[i + n] = src();
                        init();
                    }
                void reset(const value_type &initv) {
                    index_type n = size();
                    if (combineQ(initv, initv) == initv)
                        fill(t.begin(), t.end(), initv);
                    else {
                        fill(t.begin() + n, t.end(), initv);
                        init();
                    }
                }
                private:
                void init() {
                    const index_type n = size();
                    for (index_type i = n; i--; )
                        t[i] = combineQ(std::as_const(t)[i<<1], std::as_const(t)[(i<<1)^1]);
                }
                void update_t(index_type index) {
                    const index_type n = size();
                    while(index > 0) {
                        auto next = index >> 1;
                        t[next] = combineU(combineQ(std::as_const(t)[index], std::as_const(t)[index ^ 1]), d[next]);
                        index = next;
                    }
                }
                void apply(index_type index, const value_type &val) {
                    const index_type n = size();
                    t[index] = combineU(std::as_const(t)[index], val);
                    if (index < n)
                        d[index] = combineU(d[index], val);
                }
                void push(index_type p) {
                    const index_type n = size();
                    for (index_type s = h; s > 0; --s) {
                        index_type i = p >> s;
                        if (d[i] != Init) {
                            apply((i << 1), d[i]);
                            apply((i << 1) ^ 1, d[i]);
                            d[i] = Init;
                        }
                    }
                }
                public:
                auto query_halfopen(index_type l, index_type r, value_type res) const { // [ l; r )
                    const index_type n = size();
                    l += n;
                    r += n;
                    push(l);
                    push(r - 1);
                    for (; l < r; l >>= 1, r >>= 1) {
                        if ((l & 1))
                            res = combineQ(res, t[l++]);
                        if ((r & 1))
                            res = combineQ(t[--r], res);
                    }
                    return res;
                }
                auto query(index_type index) const {
                    const index_type n = size();
                    index += n;
                    //push(index);
                    return t[index];
                }
                void update_halfopen(index_type l, index_type r, const value_type &val) { // [ l; r )
                    const index_type n = size();
                    l += n, r += n;
                    auto l0 = l;
                    auto r0 = r;
                    for (; l < r; l >>= 1, r >>= 1) {
                        if ((l & 1))
                            apply(l++, val);
                        if ((r & 1))
                            apply(--r, val);
                    }
                    update_t(l0);
                    update_t(r0 - 1);
                }
                constexpr auto size () noexcept { return t.size() >> 1; }
                auto operator [](size_type index) const {
                    return query(index);
                }
            };
        //template<typename Container>
        //using SegTreeMinInc = SegTree<Container, FunctionalMin<typename Container::value_type>, std::plus<typename Container::value_type>, 0>;
        //template<typename Container>
        //using SegTreeMaxInc = SegTree<Container, FunctionalMax<typename Container::value_type>, std::plus<typename Container::value_type>, 0>;
    }
};
#endif // SEGMENT_TREE_NREC_HPP
// vim: sw=4 et
