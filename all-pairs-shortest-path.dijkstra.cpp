#include <vector>
#include <queue>
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
    WGraph(Index n, const E& edges, C&& get = std::identity())
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

// Non-negative edges only
// time = O(|V|^2 log |E| + |V|*|E|)
// temporary space = O(|V| + |E|) (fixed)
// output space = O(|V|^2)
// return mindist[u][v] = minimum distance from u to v
template <typename Weight, typename Index, bool Bidir = false, Weight maxw = std::numeric_limits<Weight>::max(), typename E, typename Convert = std::identity>
std::vector<std::vector<Weight>> apsp_dijkstra(Index n, const E &edges, auto&& get = std::identity()) {
    std::vector res(n, std::vector<Weight>(n, maxw));

    WGraph<Weight, Index, Bidir> g(n, edges, get);

    auto cmp = [](auto a, auto b) {
	return a.first > b.first;
    };
    std::priority_queue<std::pair<Weight, Index>, std::vector<std::pair<Weight, Index>>, decltype(cmp)> q(cmp);
    for (Index s = 0; s < n; ++s) {
	auto &mindist = res[s];
	q.emplace(0, s);
	mindist[s] = 0;
	while(!q.empty()) {
	    auto [dist, u] = q.top();
	    q.pop();
	    if (dist > mindist[u])
		continue;
	    for (auto j = g.epos[u], l = g.epos[u + 1]; j != l; ++j) {
		auto [w, v] = g.adj[j];
		w += dist;
		if (w >= mindist[v])
		    continue;
		mindist[v] = w;
		q.emplace(w, v);
	    }
	}
    }
    return res;
}
// return mindist[u][v] = std::pair { minimum distance from u to v, predecessor on path from u to v }
// (it is possible to re-create (reversed) path to any vertex by following predecessor links)
template <typename Weight, typename Index, bool Bidir = false, Weight maxw = std::numeric_limits<Weight>::max(), typename E, typename Convert = std::identity>
std::vector<std::vector<std::pair<Weight, Index>>> apsp_path_dijkstra(Index n, const E &edges, Convert&& get = std::identity()) {
    std::vector res(n, std::vector<std::pair<Weight, Index>>(n, std::make_pair(maxw, n)));

    WGraph<Weight, Index, Bidir> g(n, edges, get);

    auto cmp = [](auto a, auto b) {
	return a.first > b.first;
    };
    std::priority_queue<std::pair<Weight, Index>, std::vector<std::pair<Weight, Index>>, decltype(cmp)> q(cmp);
    for (Index s = 0; s < n; ++s) {
	// Dijkstra, O(|V|+|E|log|V|)
	auto &mindist = res[s];
	q.emplace(0, s);
	mindist[s].first = 0;
	while(!q.empty()) {
	    auto [dist, u] = q.top();
	    q.pop();
	    if (dist > mindist[u].first)
		continue;
	    for (auto j = g.epos[u], l = g.epos[u + 1]; j != l; ++j) {
		auto [w, v] = g.adj[j];
		w += dist;
		if (w >= mindist[v].first)
		    continue;
		mindist[v] = std::make_pair(w, u);
		q.emplace(w, v);
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
    std::vector<std::tuple<int, unsigned,unsigned>> edges {{
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
