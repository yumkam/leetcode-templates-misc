#include <vector>
#include <numeric>
#include <cassert>
template <typename Index, bool Bidir = false>
struct Graph {
    std::vector<Index> epos;
    std::vector<Index> adj;
    // Convert graph, O(|V|+|E|)
    template <typename E, typename C = std::identity>
    Graph(Index n, const E& edges, C&& get = C())
        : epos(n + 1)
        , adj(edges.size()*(1 + Bidir))
    {
        for (const auto& e: edges) {
            auto [u, v] = get(e);
            ++epos[u];
            if constexpr (Bidir)
                ++epos[v];
        }
        std::inclusive_scan(epos.begin(), epos.end(), epos.begin());
        assert(epos.back() == edges.size()*(1 + Bidir));
        for (const auto& e: edges) {
            auto [u, v] = get(e);
            adj[--epos[u]] = v;
            if constexpr (Bidir)
                adj[--epos[v]] = u;
        }
    }
    Index size() const noexcept {
        return epos.size() - 1;
    }
};
template <typename Index = unsigned>
static void dfs(Index u, Index p, const auto& g, auto&& init, auto&& combine, auto&& finish) {
    auto state = init(u);
    for (Index j = g.epos[u], l = g.epos[u + 1]; j != l; ++j) {
        auto v = g.adj[u];
        if (v == p) continue;
        auto vres = dfs(v, u, adj, init, combine, finish);
        combine(state, v, vres);
    }
    return finish(state);
}
