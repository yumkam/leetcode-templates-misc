void dfun(unsigned n, vector<vector<int>> &edges) {
    vector<unsigned> adj(edges.size());
    vector<unsigned> epos(n + 1);
    for (const auto &e: edges) ++epos[e[0]];
    for (unsigned i = 0, ep = 0; i <= n; ++i) epos[i] = ep += epos[i];
    assert(epos[n] == adj.size());
    for (const auto &e: edges) adj[--epos[e[0]]] = e[1];
    // adj[epos[u]] ... adj[epos[u + 1]]
}
void ufun(unsigned n, vector<vector<int>> &edges) {
    vector<unsigned> adj(edges.size()*2);
    vector<unsigned> epos(n + 1);
    for (const auto &e: edges) ++epos[e[0]], ++epos[e[1]];
    for (unsigned i = 0, ep = 0; i <= n; ++i) epos[i] = ep += epos[i];
    assert(epos[n] == adj.size());
    for (const auto &e: edges) {
	adj[--epos[e[0]]] = e[1];
	adj[--epos[e[1]]] = e[0];
    }
    // adj[epos[u]] ... adj[epos[u + 1]]
}
