#include <vector>
#include <algorithm>
#include <utility>
#include <numeric>
#include <limits>
#include <cassert>
template <typename Weight, typename Index, bool Bidir = false>
struct WGraph {
    std::vector<Index> epos;
    std::vector<std::pair<Weight, Index>> adj;
    // Convert graph, O(|V|+|E|)
    template <typename E, typename C = std::identity>
    WGraph(Index n, const E& edges, C&& get = C())
        : epos(n + 1)
        , adj(edges.size()*(1 + Bidir))
    {
        for (auto& e: edges) {
            auto [w, u, v] = get(e);
            ++epos[u];
            if constexpr (Bidir)
                ++epos[v];
        }
        std::partial_sum(epos.begin(), epos.end(), epos.begin());
        assert(epos.back() == edges.size()*(1 + Bidir));
        for (auto& e: edges) {
            auto [w, u, v] = get(e);
            adj[--epos[u]] = std::make_pair(w, v);
            if constexpr (Bidir)
                adj[--epos[v]] = std::make_pair(w, u);
        }
    }
    Index size() const noexcept {
        return epos.size() - 1;
    }
};

#include "heap.hpp"
// Non-negative edges only
// time = O(|V|^2 log |E| + |E|)
// temporary space = O(|V| + |E|) (fixed)
// output space = O(|V|^2)
// return mindist[u][v] = minimum distance from u to v
template <typename Weight, typename Index, bool Bidir, Weight maxw = std::numeric_limits<Weight>::max(), typename E, typename C = std::identity>
std::vector<std::vector<Weight>> apsp_dijkstra(Index n, const E &edges, C&& get = std::identity()) {
    std::vector res(n, std::vector<Weight>(n, maxw));

    WGraph<Weight, Index, Bidir> g(n, edges, get);

    std::vector<Index> q;
    std::vector<Index> idx2heap(n, n);

    for (Index s = 0; s < n; ++s) {
	auto &mindist = res[s];
	auto cmp = [&q, &mindist](auto a, auto b) {
	    return mindist[q[b]].first < mindist[q[a]].first;
	};
	auto swapit = [&q, &idx2heap](auto a, auto b) {
	    std::swap(q[a], q[b]);
	    std::swap(idx2heap[q[a]], idx2heap[q[b]]);
	};

	assert(q.empty());
	q.emplace_back(0, s);
	idx2heap[s] = 0;
	mindist[s] = 0;

	while(!q.empty()) {
	    auto u = q.front();
	    auto dist = mindist[u];
	    idx2heap[u] = n;
	    q.front() = std::move(q.back());
	    idx2heap[q.front()] = 0;
	    q.pop_back();
	    binaryHeap::siftDown((Index)q.size(), (Index)0, cmp, swapit);
	    for (auto j = g.epos[u], l = g.epos[u + 1]; j != l; ++j) {
		auto [w, v] = g.adj[j];
		w += dist;
		if (w >= mindist[v])
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
    }
    return res;
}
// return mindist[u][v] = std::pair { minimum distance from u to v, predecessor on path from u to v }
// (it is possible to re-create (reversed) path to any vertex by following predecessor links)
template <typename Weight, typename Index, bool Bidir = false, Weight maxw = std::numeric_limits<Weight>::max(), typename E, typename C = std::identity>
std::vector<std::vector<std::pair<Weight, Index>>> apsp_path_dijkstra(Index n, const E &edges, C&& get = std::identity()) {
    std::vector res(n, std::vector<std::pair<Weight, Index>>(n, std::make_pair(maxw, n)));

    WGraph<Weight, Index, Bidir> g(n, edges, get);

    std::vector<Index> q;
    std::vector<Index> idx2heap(n, n);

    for (Index s = 0; s < n; ++s) {
	// Dijkstra, O(|V|+|E|log|V|)
	auto &mindist = res[s];
	auto cmp = [&q, &mindist](auto a, auto b) {
	    return mindist[q[b]] < mindist[q[a]];
	};
	auto swapit = [&q, &idx2heap](auto a, auto b) {
	    std::swap(q[a], q[b]);
	    std::swap(idx2heap[q[a]], idx2heap[q[b]]);
	};
	q.emplace_back(s);
	idx2heap[s] = 0;
	mindist[s].first = 0;
	while(!q.empty()) {
	    auto u = q.front();
	    auto dist = mindist[u].first;

	    idx2heap[u] = n;
	    q.front() = std::move(q.back());
	    idx2heap[q.front()] = 0;
	    q.pop_back();
	    binaryHeap::siftDown((Index)q.size(), (Index)0, cmp, swapit);

	    for (auto j = g.epos[u], l = g.epos[u + 1]; j != l; ++j) {
		auto [w, v] = g.adj[j];
		w += dist;
		if (w >= mindist[v].first)
		    continue;
		auto &spos = idx2heap[v];
		auto pos = spos;
		if (pos == n) {
		    spos = pos = q.size();
		    q.push_back(v);
		}
		mindist[v] = std::make_pair(w, u);
		binaryHeap::siftUp(pos, cmp, swapit);
	    }
	}
    } // = O(|V|^2+|V|*|E|log|E|)
    return res;
}
template <typename Index, typename D>
auto get_reversed_path(const D &mindist, Index v) {
    std::vector<Index> path;
    auto n = mindist.size();
    do {
	path.push_back(v);
	v = mindist[v].second;
    } while(v != n);
    return path;
}
#include <bits/stdc++.h>
using namespace std;
//#ifdef TEST
int main() {
    size_t n = 4;
    // x y z w
    // 0 1 2 3
    enum Vertex {w, x, y, z};
    vector<tuple<int, unsigned,unsigned>> edges {{
	{+2, w, z},
	{+3, x, y}, {+6, x, w},
	{+4, y, w}, {+5, y, z},
	{+3, z, y}, {+7, z, x},
    }};
    auto res = apsp_path_dijkstra<int, unsigned>(n, edges);
    for (unsigned u = 0; u < res.size(); u++) {
	auto &row = res[u];
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
