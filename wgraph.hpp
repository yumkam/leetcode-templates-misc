#include <vector>
#include <numeric>
#include <cassert>
#include <span>
#include <functional>
#include <ostream>
template <typename Weight, typename Index>
struct WGraph {
    using index_type = Index;
    using weight_type = Weight;
    std::vector<Index> epos;
    std::vector<std::pair<Weight, Index>> adj;
    // Convert graph, O(|V|+|E|)
    template <typename E, typename C = std::identity>
    WGraph(Index n, const E& edges, C&& get = C(), bool bidir = false)
        : epos(n + 1)
    {
        for (auto& e: edges) {
            auto [w, u, v] = get(e);
            ++epos[u];
            if (bidir)
                ++epos[v];
        }
        std::inclusive_scan(epos.begin(), epos.end(), epos.begin());
        assert(epos.back() == (edges.size()<<bidir));
        adj.resize(epos.back());
        for (auto& e: edges) {
            auto [w, u, v] = get(e);
            adj[--epos[u]] = std::make_pair(w, v);
            if (bidir)
                adj[--epos[v]] = std::make_pair(w, u);
        }
    }
    WGraph(Index n, const std::vector<std::vector<int>>& el, bool bidir = false)
        : WGraph(n, el, [](const auto& e) { return std::tuple { e[2], e[0], e[1] }; }, bidir)
        {}
    Index size() const noexcept { // return number of vertexes
        return epos.size() - 1;
    }
    auto operator[](Index v) const { // return span that consists of adjacent edges of vertex v
        return std::span { adj.cbegin() + epos[v], adj.cbegin() + epos[v + 1] };
    }
    friend std::ostream& operator<<(std::ostream& os, const WGraph& g) {
        os << "graph = [";
        for (unsigned u = 0; u < g.size(); ++u) {
            os << u << " ->";
            for (auto [w, v]: g[u])
                os << ' ' << w <<'/' << v;
            os << ";\n";
        }
        return os << "]\n";
    }
};
