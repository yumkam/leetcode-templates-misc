// Public domain
#include <bits/stdc++.h>
#include "heap.hpp"
using namespace std;
template <typename Index, typename E>
auto edge_list_to_adj(const Index n, const E &e) {
    using Weight = std::decay_t<decltype(std::get<0>(e[0]))>;
    pair ret { std::vector<Index>(n + 1), std::vector<std::pair<Weight, Index>>(e.size()) };
    auto &[epos, adj] = ret;
    for (auto [w,u,v]: e)
	++epos[u];
    std::inclusive_scan(epos.begin(), epos.end(), epos.begin());
    assert(epos.back() == adj.size());
    for (auto [w,u,v]: e)
	adj[--epos[u]] = std::pair { w, v };
    return ret;
}
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
// 	mindist[v] = minimum distance from s to v,
template <typename Weight, typename Index, Weight minw = numeric_limits<Weight>::min(), Weight maxw = numeric_limits<Weight>::max(), typename E>
vector<Weight> sssp_dijkstra(Index n, const E &edges, Index s, Comp comp = std::greater<Weight> {}) {
    auto [epos, adj] = edge_list_to_adj(edges);
    return sssp_dijkstra<Weight, Index, minw, maxw>(n, adj.cbegin(), epos.cbegin(), s, comp);
}
// Dijkstra
// time = O(|V| + |E| log |V|)
// temporary space = O(|V|)
// output space = O(|V|)
// n number of vertexes |V|
// adj adjacency list, adj.size() == |E|
// epos positions of edges in adjacency list, epos.size() = n + 1
// 	all edges for vertex u lays in half-closed range adj[epos[u]:epos[u + 1]]
// return mindist
// 	mindist[v] = minimum distance from s to v
template <typename Weight, typename Index, Weight minw = numeric_limits<Weight>::min(), Weight maxw = numeric_limits<Weight>::max(), typename ADJ, typename EP, typename Comp>
vector<Weight> sssp_dijkstra(Index n, const ADJ adj, const EP epos, Index s, Comp comp = std::greater<Weight> {}) {
    vector<Weight> mindist(n, maxw);
    std::vector<Index> q;
    std::vector<Index> idx2heap(n, n);
    auto cmp = [&q,&mindist,&comp](auto a, auto b) {
    	return comp(mindist[q[b]], mindist[q[a]]);
    };
    auto swapit = [&q,&idx2heap](auto a, auto b) {
	std::swap(q[a], q[b]);
	std::swap(idx2heap[q[a]], idx2heap[q[b]]);
    };
    mindist[s] = minw;
    q.push_back(s);
    idx2heap[s] = 0;
    while(!q.empty()) {
	auto u = q.front();
	auto dist = mindist[u];
	idx2heap[u] = n;
	q.front() = std::move(q.back());
	idx2heap[q.front()] = 0;
	q.pop_back();
	binaryHeap::siftDown((Index)q.size(), (Index)0, cmp, swapit);
	for (Index j = epos[u], l = epos[u + 1]; j != l; ++j) {
	    auto [w, v] = adj[j];
	    w += dist;
	    if (!comp(w, mindist[v]))
		continue;
	    auto &pos = idx2heap[v];
	    if (pos == n) {
	       pos = q.size();
	       q.push_back(v);
	    }
	    mindist[v] = w;
	    binaryHeap::siftUp(pos, cmp, swapit);
	}
    }
    return mindist;
}
// return mindist
// 	mindist[v] = pair { minimum distance from s to v, predecessor on path from s to v }
// (it is possible to re-create (reversed) path to any vertex by following predecessor links)
template <typename Weight, typename Index, Weight minw = numeric_limits<Weight>::min(), Weight maxw = numeric_limits<Weight>::max(), typename ADJ, typename EP, typename Comp>
vector<pair<Weight, Index>> sssp_path_dijkstra(Index n, const ADJ adj, const EP epos, Index s, Comp comp = std::greater<Weight> {}) {
    vector<pair<Weight, Index>> mindist(n, pair { maxw, n } );
    std::vector<Index> q;
    std::vector<Index> idx2heap(n, n);
    auto cmp = [&q,&mindist,&comp](auto a, auto b) {
    	return comp(mindist[q[b]].first, mindist[q[a]].first);
    };
    auto swapit = [&q,&idx2heap](auto a, auto b) {
	std::swap(q[a], q[b]);
	std::swap(idx2heap[q[a]], idx2heap[q[b]]);
    };
    q.push_back(s);
    mindist[s] = pair { minw, n };
    idx2heap[s] = 0;
    while(!q.empty()) {
	auto u = q.front();
	auto dist = mindist[u];
	idx2heap[u] = n;
	q.front() = std::move(q.back());
	idx2heap[q.front()] = 0;
	q.pop_back();
	binaryHeap::siftDown((Index)q.size(), (Index)0, cmp, swapit);
	for (Index j = epos[u], l = epos[u + 1]; j != l; ++j) {
	    auto [w, v] = adj[j];
	    w += dist;
	    if (!comp(w, mindist[v]))
		continue;
	    auto &pos = idx2heap[v];
	    if (pos == n) {
	       pos = q.size();
	       q.push_back(v);
	    }
	    mindist[v] = pair { w, u };
	    binaryHeap::siftUp(pos, cmp, swapit);
	}
    }
    return mindist;
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
    constexpr int minw = 0;
    constexpr int maxw = +1'000'000'000;
#endif
    for (unsigned u = 0; u < n; u++) {
        auto [row,loopv] = sssp_path_dijkstra<int, unsigned,minw,maxw>(n, edges, u);
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
