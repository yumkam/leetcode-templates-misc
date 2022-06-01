namespace SegmentTree {
    template <typename Container, typename Combine, typename Source, typename IndexType = size_t>
    void build(IndexType v, IndexType tl, IndexType tr, Container &t, Source src, Combine combine) {
        if (tl == tr)
            t[v] = src(tl);
        else {
            IndexType tm = tl + (tr - tl) / 2;
            build(v*2 + 0, tl, tm, get, src, combine);
            build(v*2 + 1, tm + 1, tr, get, src, combine);
            t[v] = combine(t[v*2 + 0], t[v*2 + 1]);
        }
    }
    template<typename Container, typename Combine, typename ValueType, typename IndexType = size_t>
    void update(IndexType v, IndexType tl, IndexType tr, IndexType pos, const ValueType &val, Container &t, Combine combine) {
        if (tl == tr)
            t[v] = val;
        else {
            IndexType tm = tl + (tr - tl) / 2;
            if (pos <= tm)
                update(v*2 + 0, tl, tm, pos, val, get, combine);
            else
                update(v*2 + 1, tm + 1, tr, pos, val, get, combine);
            t[v] = combine(v*2 + 0, v*2 + 1);
        }
    }
    template<typename Container, typename Zero, typename Combine, typename IndexType = size_t>
    auto query(IndexType v, IndexType tl, IndexType tr, IndexType l, IndexType r, Container &t, Zero zero, Combine combine) {
        if (l > r)
            return zero();
        if (l == tl && r == tr)
            return t[v];
        IndexType tm = tl + (tr - tl) / 2;
        return combine(query(v*2 + 0, tl, tm, l, min(r, tm), t, zero, combine),
                       query(v*2 + 1, tm + 1, tr, max(l, tm + 1), r), t, zero, combine);
    }
    template<typename Container, typename Combine, typename Zero>
    class SegTree {
        typedef typename Container::size_type index_type;
        typedef typename Container::value_type value_type;
        Container t;
        Zero zero;
        Combine combine;
    public:
        template<typename Source>
        void build(Source src) {
            index_type n = t.size() / 4;
            build(0, 0, n, t, src, combine);
        }
        void update(index_type pos, const value_type &val) {
            index_type n = t.size() / 4;
            update(0, 0, n, pos, val, t, combine);
        }
        auto query(index_type l, index_type r) {
            index_type n = t.size() / 4;
            return query(0, 0, n, l, r, t, zero, combine);
        }
    };
    //template<typename Container>
    //using SegTreeSum = SegTree<Container, [](const auto &a, const auto &b) { return a + b; }, []() { return 0; }>;
    //template<typename Container>
    //using SegTreeMin = SegTree<Container, min, numeric_limits<typename Container::value_type>::max()>;
    //template<typename Container>
    //using SegTreeMax = SegTree<Container, max, numeric_limits<typename Container::value_type>::min()>;
};

