// Public domain
#include <bits/stdc++.h>
using namespace std;
template <typename T> static constexpr T saturated_add(T a, T b, T minv, T maxv) {
    assert(a >= minw); assert(a <= maxw);
    assert(b >= minw); assert(b <= maxw);
    if (a >= 0 && b >= 0 && b >= maxv - a)
        return maxv;
    if (a < 0 && b < 0 && b <= minv - a)
        return minv;
    return a + b;
}
// Bellman-Ford
// time = O(|V|*|E|)
// worst case = O(|V|^3)
// temporary space = O(1) (fixed)
// output space = O(|V|)
// n number of vertexes |V|
// e list of directed edges, e.g. vector<tuple<weight,source vertex,destination vertex>>
// requires: sum of any subset of edges must fit into [minw:maxw]:
// accumulate(e.begin(), e.end(), (intmax_t)0, [](auto s, const auto &e) { return s + min(get<0>(e), 0); }) >= minw &&
// accumulate(e.begin(), e.end(), (intmax_t)0, [](auto s, const auto &e) { return s + max(get<0>(e), 0); }) <= maxw
// return pair { mindist, loopv }
// 	mindist[v] = minimum distance from s to v,
// 	loopv = vertex that is part of negative loop or n if no negative loop found
template <typename Weight, typename Index, Weight minw = numeric_limits<Weight>::min(), Weight maxw = numeric_limits<Weight>::max(), typename E>
pair<vector<Weight>, Index> sssp_bellman_ford(Index n, const E &edges, Index s) {
    auto ret = make_pair(vector<Weight>(n, maxw), n);
    auto &[mindist, loopv] = ret;
    mindist[s] = 0;
    // note on saturation: [{-1e6,0,1}, {-1e6,1,2},...,{-1e6,999,1000},{-1e6,1000,0}]
    // even though sum(weight) > INT_MIN, it will enter saturation
    for (auto k = n; k > 1; ) { // (n - 1) iterations
        bool relaxed = false;
        for (auto [w, u, v]: edges) {
            auto u_dist = mindist[u];
            auto uv_dist = saturated_add(u_dist, w, minw, maxw);
            auto &v_dist = mindist[v];
            if (uv_dist < v_dist || w < 0 && u_dist == v_dist) {
                v_dist = uv_dist;
                relaxed = true;
            }
        }
        if (!relaxed)
            goto skip_test;
    }
    if constexpr (minw < 0) // Check for negative cycle
        for (auto [w, u, v]: edges) {
            auto u_dist = mindist[u];
            auto uv_dist = saturated_add(u_dist, w, minw, maxw);
            auto &v_dist = mindist[v];
            if (uv_dist < v_dist || w < 0 && u_dist == v_dist) { // found negative cycle
                loopv = v;
                break;
            }
        }
skip_test:
    return ret;
}
// n number of vertexes |V|
// e list of directed edges, e.g. vector<tuple<weight,source vertex,destination vertex>>
// requires: sum of any subset of edges must fit into [minw:maxw]:
// accumulate(e.begin(), e.end(), (intmax_t)0, [](auto s, const auto &e) { return s + min(get<0>(e), 0); }) >= minw &&
// accumulate(e.begin(), e.end(), (intmax_t)0, [](auto s, const auto &e) { return s + max(get<0>(e), 0); }) <= maxw
// return pair { mindist, loopv }
// 	mindist[v] = pair { minimum distance from s to v, predecessor on path from s to v }
// 	loopv = vertex that is part of negative loop or `n` if no negative loop found
// (it is possible to re-create (reversed) path to any vertex by following predecessor links)
template <typename Weight, typename Index, Weight minw = numeric_limits<Weight>::min(), Weight maxw = numeric_limits<Weight>::max(), typename E>
pair<vector<pair<Weight, Index>>,Index> sssp_path_bellman_ford(Index n, const E &edges, Index s) {
    // Bellman-Ford, O(|V|*|E|)
    auto ret = make_pair(vector<pair<Weight, Index>>(n, pair { maxw, n }), n);
    auto &[mindist, loopv] = ret;
    mindist[s].first = 0;
    for (Index k = n; k-- > 1; ) { // ((n + 1) - 1) iterations
        bool relaxed = false;
        for (auto [w, u, v]: edges) {
            auto u_dist = mindist[u].first;
            auto uv_dist = saturated_add(u_dist, w, minw, maxw);
            auto &[v_dist, path] = mindist[v];
            if (uv_dist < v_dist || (w < 0 && u_dist == v_dist)) {
                v_dist = uv_dist;
                path = u;
                relaxed = true;
            }
        }
        if (!relaxed) goto skip_test;
    }
    if constexpr (minw < 0) // negative weights are possible
        for (auto [w, u, v]: edges) {
            auto u_dist = mindist[u].first;
            auto uv_dist = saturated_add(u_dist, w, minw, maxw);
            auto &[v_dist, path] = mindist[v];
            if (uv_dist < v_dist || (w < 0 && u_dist == v_dist)) {
                path = u;
                loopv = v;
                break;
            }
        }
skip_test:
    return ret;
}
template <typename Index, typename D>
auto get_reversed_path(const D &mindist, Index v, Index senitel) {
    vector<Index> path;
    do {
        path.push_back(v);
        v = mindist[v].second;
    } while(v != senitel);
    return path;
}
template <typename Index, typename D>
auto get_reversed_path(const D &mindist, Index v) {
    return get_reversed_path(mindist, v, (Index)mindist.size());
}
//#ifdef TEST
int main() {
#if 0
    size_t n = 4;
    // x y z w
    // 0 1 2 3
    typedef enum {w, x, y, z} Vertex;
    vector<tuple<int, unsigned,unsigned>> edges {{
        {+2, w, z},
        {+1, x, y}, {+6, x, w},
        {+4, y, w}, {+5, y, z},
        {-3, z, y}, {-7, z, x},
    }};
#else
    size_t n = 1000;
    vector<tuple<int, unsigned,unsigned>> edges(n - 1);
    for (unsigned e = 0; e < edges.size(); ++e)
        edges[e] = make_tuple(-2'000'000,e%n,(e+1)%n);
    constexpr int minw = -1'000'000'000;
    constexpr int maxw = +1'000'000'000;
#endif
    for (unsigned u = 0; u < n; u++) {
        auto [row,loopv] = sssp_path_bellman_ford<int, unsigned,minw,maxw>(n, edges, u);
        cout << (char)(u + 'w');
        if (loopv != n) {
            cout << '@' << (char)(loopv + 'w') << flush;
            auto v = loopv;
            for (unsigned i = 0; i < n; ++i) {
                assert(v != n);
                v = row[v].second;
            }
            cout << '@';
            for (auto t: get_reversed_path(row, v, v))
                cout << (char)('w'+t) << '<';
            cout << (char)('w'+v);
        } else {
            for (unsigned v = 0; v < n; ++v) {
                cout << ' ' << row[v].first;
                for (auto t: get_reversed_path(row, v))
                    cout << '<' << (char)('w'+t);
            }
        }
        cout << '\n';
    }
    return 0;
}
//#endif
