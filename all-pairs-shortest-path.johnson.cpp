#include <bits/stdc++.h>
using namespace std;
// time = O(|V|^2 log |E| + |V|*|E|)
// worst case = O(|V|^3 log |V|)
// temporary space = O(|V| + |E|) (fixed)
// output space = O(|V|^2)
// Johnson algorithm makes sense on sparse graphs, |E| << |V|^2;
// otherwise, Floyd-Warhsall is better
// return mindist[u][v] = minimum distance from u to v
template <typename T> static constexpr T saturated_add(T a, T b, T minv, T maxv) {
    if (a >= 0 && b >= 0 && b >= maxv - a)
	return maxv;
    if (a < 0 && b < 0 && b <= minv - a)
	return minv;
    return a + b;
}
template <typename Weight, typename Index, Weight minw = numeric_limits<Weight>::min(), Weight maxw = numeric_limits<Weight>::max(), typename E>
vector<vector<Weight>> apsp_johnson(Index n, const E &edges) {
    vector<Weight> qweight(n, 0);
    // Bellman-Ford, O(|V|*|E|)
    for (auto k = n; k--; ) { // ((n + 1) - 1) iterations (we added virtual n+1'th vertex q)
	bool relaxed = false;
	for (auto [w, u, v]: edges) {
	    auto u_dist = qweight[u];
	    auto uv_dist = saturated_add(u_dist, w, minw, maxw);
	    auto &v_dist = qweight[v];
	    if (uv_dist < v_dist || (w < 0 && u_dist == v_dist)) {
		v_dist = uv_dist;
		relaxed = true;
	    }
	}
	// there are also n virtual edges [0, q, v], but they are all no-op: 0 + 0 + qweight[v] < qweight[v] is never true
	if (!relaxed)
	    goto skip_test;
    }
    // for (auto u = n; u--; ) assert(qweight[u] <= 0);
    // Check for negative cycle
    for (auto [w, u, v]: edges) {
	auto u_dist = qweight[u];
	auto uv_dist = saturated_add(u_dist, w, minw, maxw);
	auto &v_dist = qweight[v];
	if (uv_dist < v_dist || (w < 0 && u_dist == v_dist))
	    return {};
    }
skip_test:
    // Convert graph, O(|V|+|E|) with re-weighting
    vector<Index> epos(n + 1);
    vector<pair<Weight, Index>> adj(edges.size());
    for (auto [w, u, v]: edges)
	++epos[u];
    partial_sum(epos.begin(), epos.end(), epos.begin());
    assert(epos.back() == edges.size());
    for (auto [w, u, v]: edges)
	adj[--epos[u]] = make_pair(w + qweight[u] - qweight[v], v);
    auto cmp = [](auto a, auto b) {
	return a.first > b.first;
    };
    vector<vector<Weight>> res(n, vector<Weight>(n, maxw));
    priority_queue<pair<Weight, Index>, vector<pair<Weight, Index>>, decltype(cmp)> q(cmp);
    for (Index s = 0; s < n; ++s) {
	auto &mindist = res[s];
	q.emplace(0, s);
	mindist[s] = 0;
	while(!q.empty()) {
	    auto [dist, u] = q.top();
	    q.pop();
	    if (dist > mindist[u])
		continue;
	    for (auto j = epos[u], l = epos[u + 1]; j != l; ++j) {
		auto [w, v] = adj[j];
		w += dist;
		if (w >= mindist[v])
		    continue;
		mindist[v] = w;
		q.emplace(w, v);
	    }
	}
	// undo re-weighting
	for (Index t = 0; t < n; ++t)
	    if (mindist[t] != maxw)
		mindist[t] -= qweight[s] - qweight[t];
    }
    return res;
}
// return mindist[u][v] = pair { minimum distance from u to v, predecessor on path from u to v }
// (it is possible to re-create (reversed) path to any vertex by following predecessor links)
template <typename Weight, typename Index, Weight minw = numeric_limits<Weight>::min(), Weight maxw = numeric_limits<Weight>::max(), typename E>
vector<vector<pair<Weight, Index>>> apsp_path_johnson(Index n, const E &edges) {
    vector<Weight> qweight(n, 0);
    // Bellman-Ford, O(|V|*|E|)
    // Bellman-Ford, O(|V|*|E|)
    for (auto k = n; k--; ) { // ((n + 1) - 1) iterations (we added virtual n+1'th vertex q)
	bool relaxed = false;
	for (auto [w, u, v]: edges) {
	    auto u_dist = qweight[u];
	    auto uv_dist = saturated_add(u_dist, w, minw, maxw);
	    auto &v_dist = qweight[v];
	    if (uv_dist < v_dist || (w < 0 && u_dist == v_dist)) {
		v_dist = uv_dist;
		relaxed = true;
	    }
	}
	// there are also n virtual edges [0, q, v], but they are all no-op: 0 + 0 + qweight[v] < qweight[v] is never true
	if (!relaxed)
	    goto skip_test;
    }
    // for (auto u = n; u--; ) assert(qweight[u] <= 0);
    // Check for negative cycle
    for (auto [w, u, v]: edges) {
	auto u_dist = qweight[u];
	auto uv_dist = saturated_add(u_dist, w, minw, maxw);
	auto &v_dist = qweight[v];
	if (uv_dist < v_dist || (w < 0 && u_dist == v_dist))
	    return {};
    }
skip_test:
#if 0
    for (Index u = 0; u < n; ++u) cout << (char)('w' + u) << '=' << qweight[u] << ' ';
    cout << '\n';
#endif
    // Convert graph, O(|V|+|E|) with re-weighting
    vector<Index> epos(n + 1);
    vector<pair<Weight, Index>> adj(edges.size());
    for (auto [w, u, v]: edges)
	++epos[u];
    partial_sum(epos.begin(), epos.end(), epos.begin());
    assert(epos.back() == edges.size());
    for (auto [w, u, v]: edges) {
	adj[--epos[u]] = make_pair(w + qweight[u] - qweight[v], v);
#if 0
	cout << (char)('w' + u) << (char)('w' + v) << '=' << w + qweight[u] - qweight[v] << '\n';
#endif
    }
    auto cmp = [](auto a, auto b) {
	return a.first > b.first;
    };
    vector<vector<pair<Weight, Index>>> res(n, vector<pair<Weight, Index>>(n, make_pair(maxw, n)));
    priority_queue<pair<Weight, Index>, vector<pair<Weight, Index>>, decltype(cmp)> q(cmp);
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
	    for (auto j = epos[u], l = epos[u + 1]; j != l; ++j) {
		auto [w, v] = adj[j];
		w += dist;
		if (w >= mindist[v].first)
		    continue;
		mindist[v] = make_pair(w, u);
		q.emplace(w, v);
	    }
	}
	// undo re-weighting
	for (Index t = 0; t < n; ++t)
	    if (auto &dist = mindist[t].first; dist != maxw)
		dist -= qweight[s] - qweight[t];
    } // = O(|V|^2+|V|*|E|log|E|)
    return res;
}
template <typename Index, typename D>
auto get_reversed_path(const D &mindist, Index v) {
    vector<Index> path;
    auto n = mindist.size();
    do {
	path.push_back(v);
	v = mindist[v].second;
    } while(v != n);
    return path;
}
//#ifdef TEST
int main() {
    size_t n = 4;
    // x y z w
    // 0 1 2 3
    typedef enum {w, x, y, z} Vertex;
    vector<tuple<int, unsigned,unsigned>> edges {{
	{+2, w, z},
	{+3, x, y}, {+6, x, w},
	{+4, y, w}, {+5, y, z},
	{-3, z, y}, {-7, z, x},
    }};
    auto res = apsp_path_johnson<int, unsigned>(n, edges);
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
