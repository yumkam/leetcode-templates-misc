#include <bits/stdc++.h>
namespace impl {
    template <class Size, class It1, class It2, class It3>
    static void dfs(Size u, It1 adj, It2 epos, It3 ecnt, std::vector<Size> &ret) {
	auto j = epos[u];
	auto &c = ecnt[u];
	while(c) {
	    --c;
	    auto v = adj[j + c];
	    if ((v & 1))
		continue;
	    v >>= 1;
	    auto ub = std::upper_bound(adj + epos[v], adj + epos[v] + ecnt[v], u<<1);
	    *--ub |= 1;
	    dfs(v, adj, epos, ecnt, ret);
	}
	ret.push_back(u);
    }
}
template <class Size>
std::vector<Size> euler_path(Size n, const std::vector<std::array<Size, 2>> &edges) {
    // time = O(|V|+|E|log|E|), space = O(|V|+|E|)
    const Size m = edges.size();
    std::vector<Size> adj(m*2 + 2);
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
    if (v1 != n) {
	if (v2 == n)
	    return {};
	++epos[v1];
	++epos[v2];
	//std::cout << "circuit " << v1 << ' ' << v2 << std::endl;
    }
    auto ecnt = epos;
    ecnt.pop_back();
    for (Size i = 1, s = epos[0]; i <= n; ++i)
	epos[i] = s += epos[i];
    assert(epos.back() == adj.size() - 2*(v1 == n));
    for (const auto &e: edges) {
	adj[--epos[e[0]]] = e[1] << 1;
	adj[--epos[e[1]]] = e[0] << 1;
    }
    if (v1 != n) {
	adj[--epos[v1]] = v2 << 1;
	adj[--epos[v2]] = v1 << 1;
    }
    for (Size i = 0; i < n; ++i)
	sort(adj.begin() + epos[i], adj.begin() + epos[i + 1]);
    std::vector<Size> ret;
    impl::dfs(v1 == n ? 0 : v1, adj.begin(), epos.cbegin(), ecnt.begin(), ret);
    for (Size i = 0; i != n; ++i) {
	auto j = epos[i];
	auto c = ecnt[i];
	while(c) {
	    --c;
	    if (!(adj[j + c] & 1)) {
		//std::cout << i << ' ' << adj[j + c] << std::endl;
		return {};
	    }
	}
    }
    if (v1 != n) {
	for (unsigned j = 0; j + 1 < ret.size(); ++j) {
	    if ((ret[j] == v1 && ret[j + 1] == v2) || (ret[j] == v2 && ret[j + 1] == v1)) {
		// remove fake v1->v2 or v2->v1 edge, start path from ret[j + 1]
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
