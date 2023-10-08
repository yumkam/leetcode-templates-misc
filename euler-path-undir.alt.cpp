#include <bits/stdc++.h>
namespace impl {
    template <class Size, class It1, class It2, class It3>
    static void dfs(Size u, It1 adj, It2 epos, It3 ecnt, std::vector<Size> &ret) {
	auto j = epos[u];
	auto &c = ecnt[u];
	while(c) {
	    --c;
	    auto [v,vupos] = adj[j + c];
	    if ((v & 1))
		continue;
	    v >>= 1;
	    adj[vupos].first |= 1;
	    dfs(v, adj, epos, ecnt, ret);
	}
	ret.push_back(u);
    }
}
template <class Size>
std::vector<Size> euler_path(Size n, const std::vector<std::array<Size, 2>> &edges) {
    const Size m = edges.size();
    std::vector<std::pair<Size, Size>> adj(m*2 + 2); // { opposite_vertex<<1 | edge_visited, opposite_index }
    // (opposite_index = index of opposite edge in adj)
    std::vector<Size> epos(n + 1);
    for (const auto &e: edges) {
	++epos[e[0]];
	++epos[e[1]];
    }
    Size v1 = n, v2 = n;
    for (Size i = 0; i < n; ++i)
	if ((epos[i] & 1)) {
	    if (v1 == n)
		v1 = i;
	    else if (v2 == n)
		v2 = i;
	    else
		return {};
	}
    if (v1 != n) { // this must be euler path
	if (v2 == n) // .. except it is not
	    return {};
	++epos[v1]; // we will add artificial edge to convert euler path into euler circuit
	++epos[v2];
    }
    auto ecnt = epos;
    ecnt.pop_back();
    for (Size i = 1, s = epos[0]; i <= n; ++i)
	epos[i] = s += epos[i];
    assert(epos.back() == adj.size() - 2*(v1 == n));
    for (const auto &e: edges) {
	auto e0pos = --epos[e[0]];
	auto e1pos = --epos[e[1]];
	adj[e0pos] = std::make_pair(e[1] << 1, e1pos);
	adj[e1pos] = std::make_pair(e[0] << 1, e0pos);
    }
    if (v1 != n) {
	auto v1pos = --epos[v1];
	auto v2pos = --epos[v2];
	adj[v1pos] = std::make_pair(v2 << 1, v2pos);
	adj[v2pos] = std::make_pair(v1 << 1, v1pos);
    }
    std::vector<Size> ret;
    impl::dfs(v1 == n ? 0 : v1, adj.begin(), epos.cbegin(), ecnt.begin(), ret);
    for (Size i = 0; i != n; ++i) {
	auto j = epos[i];
	auto &c = ecnt[i];
	while(c) {
	    --c;
	    if (!(adj[j + c].first & 1))
		return {};
	}
    }
    if (v1 != n) {
	for (unsigned j = 0; j + 1 < ret.size(); ++j) {
	    if ((ret[j] == v1 && ret[j + 1] == v2) || (ret[j] == v2 && ret[j + 1] == v1)) {
		// remove fake v1->v2 or v2->v1 edge, start path from ret[j + 1]
		//std::cout << j << std::endl;
		std::rotate(ret.begin(), ret.begin() + j + 1, ret.end());
		break;
	    }
	}
	assert((ret.front() == v1 && ret.back() == v2) || (ret.front() == v2 && ret.back() == v1));
	ret.pop_back();
    }
    return ret;
}
using namespace std;
int main() {
    unsigned n, m;
    cin >> n >> m;
    vector<array<unsigned, 2>> edges(m);
    for (auto &e: edges)
	cin >> e[0] >> e[1];
    auto ret = euler_path(n, edges);
    if (ret.empty())
	cout << -1 << endl;
    else {
	cout << ret.size();
	for (auto r: ret)
	    cout << ' ' << r;
	cout << endl;
    }
    return 0;
}
