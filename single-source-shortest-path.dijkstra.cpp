// Public domain
#include <bits/stdc++.h>
#include "heap.hpp"
#include "wgraph.hpp"
using namespace std;
template <typename Weight, typename Index>
struct sssp_dijkstra {
    std::vector<Index> q;
    std::vector<Index> idx2heap;

    // Dijkstra
    // time = O(|V| + |E| log |V|)
    // temporary space = O(|V| + |E|)
    // output space = O(|V|)
    // n number of vertexes |V|
    // requires monotonic weightes:
    //    either
    //       maxw <= weight <= minw == 0 and comp is std::greater<Weight>
    //    or 
    //       0 == minw <= weight <= maxw and comp is std::less<Weight>
    // All possible path length must fit into minw:maxw range
    // e list of directed edges, e.g. vector<tuple<weight,source vertex,destination vertex>>
    // return mindist
    //  mindist[v] = minimum distance from s to v,
    template <Weight minw = std::numeric_limits<Weight>::min(), Weight maxw = std::numeric_limits<Weight>::max(), typename Comp = std::less<Weight>, typename Visit = std::function<bool(Index, Weight)>>
    bool run(const WGraph<Weight, Index>& g, Index s, std::vector<Weight>& mindist, Comp&& comp = std::less<Weight> {}, Visit&& visit = [] (Index, Weight) { return false; }) {
        const auto n = g.size();
        mindist.clear();
        mindist.resize(n, maxw);
        q.clear();
        idx2heap.clear();
        idx2heap.resize(n, n);
        auto cmp = [this, &mindist, &comp](auto a, auto b) {
            return comp(mindist[q[b]], mindist[q[a]]);
        };
        auto swapit = [this](auto a, auto b) {
            std::swap(q[a], q[b]);
            std::swap(idx2heap[q[a]], idx2heap[q[b]]);
        };
        mindist[s] = minw;
        q.push_back(s);
        idx2heap[s] = 0;
        while(!q.empty()) {
            auto u = q.front();
            auto dist = mindist[u];
            if (visit(u, dist)) {
                return true;
            }
            idx2heap[u] = n;
            q.front() = std::move(q.back());
            idx2heap[q.front()] = 0;
            q.pop_back();
            binaryHeap::siftDown((Index)q.size(), (Index)0, cmp, swapit);
            for (auto [w, v]: g[u]) {
                w += dist;
                if (!comp(w, mindist[v]))
                    continue;
                auto &spos = idx2heap[v];
                auto pos = spos;
                if (pos == n) {
                    spos = pos = q.size();
                    q.push_back(v);
                }
                mindist[v] = w;
                binaryHeap::siftUp(pos, cmp, swapit);
            }
        }
        return false;
    }
    // return mindist
    //  mindist[v] = pair { minimum distance from s to v, predecessor on path from s to v }
    // (it is possible to re-create (reversed) path to any vertex by following predecessor links)
    template <Weight minw = std::numeric_limits<Weight>::min(), Weight maxw = std::numeric_limits<Weight>::max(), typename Comp = std::less<Weight>, typename Visit = std::function<bool(Index, Weight)>>
    bool run(const WGraph<Weight, Index>& g, Index s, std::vector<std::pair<Weight, Index>>& mindist, Comp&& comp = std::less<Weight> {}, Visit&& visit = [] (Index, Weight) { return false; }) {
        const auto n = g.size();
        mindist.clear();
        mindist.resize(n, pair { maxw, n } );
        q.clear();
        idx2heap.clear();
        idx2heap.resize(n, n);
        auto cmp = [this, &mindist, &comp](auto a, auto b) {
            return comp(mindist[q[b]].first, mindist[q[a]].first);
        };
        auto swapit = [this](auto a, auto b) {
            std::swap(q[a], q[b]);
            std::swap(idx2heap[q[a]], idx2heap[q[b]]);
        };
        mindist[s] = pair { minw, n };
        q.push_back(s);
        idx2heap[s] = 0;
        while(!q.empty()) {
            auto u = q.front();
            auto [dist, _] = mindist[u];
            if (visit(u, dist)) {
                return true;
            }
            idx2heap[u] = n;
            q.front() = std::move(q.back());
            idx2heap[q.front()] = 0;
            q.pop_back();
            binaryHeap::siftDown((Index)q.size(), (Index)0, cmp, swapit);
            for (auto [w, v]: g[u]) {
                w += dist;
                if (!comp(w, mindist[v].second))
                    continue;
                auto &spos = idx2heap[v];
                auto pos = spos;
                if (pos == n) {
                    spos = pos = q.size();
                    q.push_back(v);
                }
                mindist[v] = pair { w, u };
                binaryHeap::siftUp(pos, cmp, swapit);
            }
        }
        return false;
    }
};
    
#if 0
template <typename Weight, typename Index, Weight minw = numeric_limits<Weight>::min(), Weight maxw = numeric_limits<Weight>::max(), typename E, typename Comp>
vector<Weight> sssp_dijkstra(Index n, const E &edges, Index s, Comp comp = std::less<Weight> {}) {
    auto [epos, adj] = edge_list_to_adj(edges);
    return sssp_dijkstra<Weight, Index, minw, maxw>(n, adj.cbegin(), epos.cbegin(), s, comp);
}
#endif
// Dijkstra
// time = O(|V| + |E| log |V|)
// temporary space = O(|V|)
// output space = O(|V|)
// n number of vertexes |V|
// adj adjacency list, adj.size() == |E|
// epos positions of edges in adjacency list, epos.size() = n + 1
//      all edges for vertex u lays in half-closed range adj[epos[u]:epos[u + 1]]
// return mindist
//      mindist[v] = minimum distance from s to v
#if 0
template <typename Weight, typename Index, Weight minw = numeric_limits<Weight>::min(), Weight maxw = numeric_limits<Weight>::max(), typename ADJ, typename EP, typename Comp>
vector<Weight> sssp_dijkstra(Index n, const ADJ adj, const EP epos, Index s, Comp comp = std::less<Weight> {}) {
    std::vector<Weight> mindist(n, maxw);
    std::vector<Index> q;
    std::vector<Index> idx2heap(n, n);
}
#endif
template <typename Index, typename D>
auto get_reversed_path(const D &mindist, Index v, Index senitel) {
    std::vector<Index> path;
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
    size_t n = 26;
    vector<tuple<int, unsigned,unsigned>> edges(n - 1);
    for (unsigned e = 0; e < edges.size(); ++e)
        edges[e] = make_tuple(+2'000'000,e%n,(e+1)%n);
    constexpr int minw = 0;
    constexpr int maxw = +1'000'000'000;
#endif
    WGraph<int, unsigned> g(n, edges);
    sssp_dijkstra<int, unsigned> d;
    for (unsigned u = 0; u < n; u++) {
        std::vector<std::pair<int, unsigned>> row;
        d.run<minw,maxw>(g, u, row);
        cout << (char)(u + 'w');
        for (unsigned v = 0; v < n; ++v) {
            cout << ' ' << row[v].first;
            for (auto t: get_reversed_path(row, v))
                cout << '<' << (char)('w'+t);
        }
        cout << '\n';
    }
    return 0;
}
//#endif
