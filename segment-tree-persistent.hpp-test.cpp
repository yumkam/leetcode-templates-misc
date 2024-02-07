#include "segment-tree-persistent.hpp"
#include "segment-tree-nrec.hpp"
#include <bits/stdc++.h>
using namespace std;
int main() {
    using namespace SegmentTree::PointUpdateRangeQuery;
    size_t len = 1000;
    SegTreeSum<vector<int>> st(len, 0);
    auto combine = [](auto &a, auto b) { a += b; };
    Persistent::SegTree<int, decltype(combine)> stp(len, combine);
    random_device rd;
    auto seed = rd();
    clog << seed << endl;
    mt19937 rng;
    rng.seed(seed);
    decltype(stp)::node_type root = nullptr;
    vector saveroot (10, root);
    vector savest(10, st);
    for (unsigned t = 0; t < 2000000; ++t) {
        if ((t % 1000) == 0) {
            auto idx = rng() % savest.size();
            st = savest[idx];
            root = saveroot[idx];
        }
        if ((t % 1000) == 500) {
            auto idx = rng() % savest.size();
            savest[idx] = st;
            saveroot[idx] = root;
        }
        auto i = rng() % len;
        int v = rng() % 2000 - 1000;
        st.update(i, v);
        size_t l = rng() % (len + 1);
        size_t r = rng() % (len + 1);
        if (l > r) swap(l, r);
        root = stp.update(root, i, v);
        int iv = rng() % len;
        auto r1 = st.query_halfopen(l, r, iv);
        int r2 = iv;
        stp.query_halfopen(root, l, r, r2);
        cout << i << ' ' << v << ' ' << l << ':' << r << ' ' << r1 - iv << ' ' << r2 - iv << '\n';
        assert(r1 == r2);
    }
    return 0;
}
