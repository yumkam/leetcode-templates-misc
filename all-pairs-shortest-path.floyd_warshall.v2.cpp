#include <bits/stdc++.h>
template <typename Weight = unsigned, typename Size = unsigned, Weight inf = std::numeric_limits<Weight>::max()/2>
void floyd_warshall(std::vector<std::vector<Weight>> &dist) {
    const auto n = dist.size();
    for (Size k = 0; k < n; ++k) {
        auto &dist_k = dist[k];
        assert(dist_k.size() == n);
        for (Size i = 0; i < n; ++i) {
            auto &dist_i = dist[i];
            auto dist_ik = dist_i[k];
            if (dist_ik == inf) continue;
            for (Size j = 0; j < n; ++j) {
                auto dist_kj = dist_k[j];
                if constexpr (inf > std::numeric_limits<Weight>::max() - inf)
                    if (dist_kj == inf) continue;
                dist_i[j] = std::min(dist_i[j], dist_ik + dist_kj);
            }
        }
    }
}
template <typename Weight = unsigned, typename Size = unsigned, Weight inf = std::numeric_limits<Weight>::max()/2>
void floyd_warshall_initpred(const std::vector<std::vector<Weight>> &dist, std::vector<std::vector<Size>> &pred) {
    const auto n = dist.size();
    assert(pred.size()  == n);
    for (Size i = 0; i < n; ++i) {
	auto &dist_i = dist[i];
        assert(dist_i.size() == n);
	auto &pred_i = dist[i];
        assert(pred_i.size() == n);
	for (Size j = 0; j < n; ++j)
	    pred_i[j] = dist_i[j] == inf ? n : j;
	pred_i[i] = i;
    }
}

template <typename Weight = unsigned, typename Size = unsigned, Weight inf = std::numeric_limits<Weight>::max()/2>
void floyd_warshall(std::vector<std::vector<Weight>> &dist, std::vector<std::vector<Size>> &pred) {
    const auto n = dist.size();
    assert(pred.size() == n);
    for (Size k = 0; k < n; ++k) {
        auto &dist_k = dist[k];
        assert(dist_k.size() == n);
        auto &pred_k = pred[k];
        assert(pred_k.size() == n);
        for (Size i = 0; i < n; ++i) {
            auto &dist_i = dist[i];
            auto &pred_i = pred[i];
            auto dist_ik = dist_i[k];
            if (dist_ik == inf) continue;
            for (Size j = 0; j < n; ++j) {
                auto dist_kj = dist_k[j];
                if constexpr (inf > std::numeric_limits<Weight>::max() - inf)
                    if (dist_kj == inf) continue;
                if (auto dist_ikj = dist_ik + dist_kj; dist_ikj < dist_i[j]) {
                    dist_i[j] = dist_ikj;
                    pred_i[j] = pred_k[j];
                }
            }
        }
    }
}
// flat matrix: a00 a01 a02 ... a10 a11 a12 ... ...
template <typename Weight = unsigned, typename Size = unsigned, Weight inf = std::numeric_limits<Weight>::max()/2>
void floyd_warshall(std::vector<Weight> &dist, Size n) {
    assert(dist.size() == n*n);
    for (Size k = 0; k < n; ++k) {
        for (Size i = 0; i < n; ++i) {
            auto dist_ik = dist[i*n + k];
            if (dist_ik == inf) continue;
            for (Size j = 0; j < n; ++j) {
                auto dist_kj = dist[k*n + j];
                if constexpr (inf > std::numeric_limits<Weight>::max() - inf)
                   if (dist_kj == inf) continue;
                auto &dist_ij = dist[i*n + j];
                dist_ij = std::min(dist_ij, dist_ik + dist_kj);
            }
        }
    }
}
template <typename Weight = unsigned, typename Size = unsigned, Weight inf = std::numeric_limits<Weight>::max()/2>
void floyd_warshall_initpred(const std::vector<Weight> &dist, Size n, std::vector<Size> &pred) {
    assert(dist.size() == n*n);
    assert(pred.size() == dist.size());
    for (Size i = 0; i < n; ++i) {
	for (Size j = 0; j < n; ++j)
	    pred[i*n + j] = dist[i*n + j] == inf ? n : j;
	pred[i*n + i] = i;
    }
}
template <typename Weight = unsigned, typename Size = unsigned, Weight inf = std::numeric_limits<Weight>::max()/2>
void floyd_warshall(std::vector<Weight> &dist, Size n, std::vector<Size> &pred) {
    assert(dist.size() == n*n);
    for (Size k = 0; k < n; ++k) {
	for (Size i = 0; i < n; ++i) {
	    auto dist_ik = dist[i*n + k];
	    if (dist_ik == inf) continue;
	    for (Size j = 0; j < n; ++j) {
		auto dist_kj = dist[k*n + j];
		if constexpr (inf > std::numeric_limits<Weight>::max() - inf)
                   if (dist_kj == inf) continue;
		auto &dist_ij = dist[i*n + j];
		if (auto dist_ikj = dist_ik + dist_kj; dist_ikj < dist_ij) {
                    dist_ij = dist_ikj;
                    pred[i*n + j] = pred[k*n + j];
		}
	    }
	}
    }
}
template <typename Weight = unsigned, typename Size = unsigned, Weight inf = std::numeric_limits<Weight>::max()/2>
void floyd_warshall_undirected(std::vector<std::vector<Weight>> &dist) {
    const auto n = dist.size();
    for (Size k = 0; k < n; ++k) {
	auto &dist_k = dist[k];
	assert(dist_k.size() == k + 1);
	for (Size i = 0; i < n; ++i) {
	    auto &dist_i = dist[i];
	    auto dist_ik = i < k ? dist_k[i] : dist_i[k];
	    if (dist_ik == inf) continue;
	    for (Size j = 0; j < i; ++j) {
		auto dist_kj = j < k ? dist_k[j] : dist[j][k];
		if constexpr (inf > std::numeric_limits<Weight>::max() - inf)
                    if (dist_kj == inf) continue;
		dist_i[j] = std::min(dist_i[j], dist_ik + dist_kj);
	    }
	}
    }
}
template <typename Weight = unsigned, typename Size = unsigned, Weight inf = std::numeric_limits<Weight>::max()/2>
void floyd_warshall_undirected_initpred(const std::vector<std::vector<Weight>> &dist, Size n, std::vector<std::vector<Size>> &pred) {
    assert(pred.size() == dist.size());
    for (Size i = 0; i < n; ++i) {
        auto &pred_i = pred[i];
        auto &dist_i = dist[i];
        assert(dist_i.size() == i + 1);
        assert(pred_i.size() == dist_i.size());
	for (Size j = 0; j < i; ++j)
	    pred[i][j] = dist[i][j] == inf ? n : j;
	pred[i][i] = i;
    }
}
template <typename Weight = unsigned, typename Size = unsigned, Weight inf = std::numeric_limits<Weight>::max()/2>
void floyd_warshall_undirected(std::vector<std::vector<Weight>> &dist, Size n, std::vector<std::vector<Size>> &pred) {
    // all sizes verified in initpred
    for (Size k = 0; k < n; ++k) {
        auto &dist_k = dist[k];
        auto &pred_k = pred[k];
	for (Size i = 0; i < n; ++i) {
            auto &dist_i = dist[i];
            auto &pred_i = pred[i];
	    auto dist_ik = i < k ? dist_k[i] : dist_i[k];
	    if (dist_ik == inf) continue;
	    for (Size j = 0; j < i; ++j) {
		auto dist_kj = k < j ? dist[j][k] : dist_k[j];
		if constexpr (inf > std::numeric_limits<Weight>::max() - inf)
                    if (dist_kj == inf) continue;
		auto &dist_ij = dist[i][j];
		if (auto dist_ikj = dist_ik + dist_kj; dist_ikj < dist_ij) {
		    dist_ij = dist_ikj;
		    pred_i[j] = k < j ? pred_k[j] : pred[j][k];
		}
	    }
	}
    }
}
// flat compact matrix: a00 a10 a11 a20 a21 a22 a30 ...
// index of [i][j] is `j < i ? i*(i + 1) + j : j*(j + 1) + i`
template <typename Weight = unsigned, typename Size = unsigned, Weight inf = std::numeric_limits<Weight>::max()/2>
void floyd_warshall_undirected(std::vector<Weight> &dist, Size n) {
    assert(dist.size() == n*(n + 1)/2);
    for (Size k = 0; k < n; ++k) {
	for (Size i = 0; i < n; ++i) {
	    auto dist_ik = dist[i < k ? k*(k + 1)/2 + i : i*(i + 1)/2 + k];
	    if (dist_ik == inf) continue;
	    for (Size j = 0; j < i; ++j) {
		auto dist_kj = dist[k < j ? j*(j + 1)/2 + k : k*(k + 1)/2 + j];
		if constexpr (inf > std::numeric_limits<Weight>::max() - inf)
                   if (dist_kj == inf) continue;
		dist[i*(i + 1)/2 + j] = std::min(dist[i*(i + 1)/2 + j], dist_ik + dist_kj);
	    }
	}
    }
}
template <typename Weight = unsigned, typename Size = unsigned, Weight inf = std::numeric_limits<Weight>::max()/2>
void floyd_warshall_undirected_initpred(const std::vector<Weight> &dist, Size n, std::vector<Size> &pred) {
    assert(dist.size() == n*(n + 1)/2);
    assert(pred.size() == dist.size());
    for (Size i = 0; i < n; ++i) {
	for (Size j = 0; j < i; ++j)
	    pred[i*(i + 1)/2 + j] = dist[i*(i + 1)/2 + j] == inf ? n : j;
        pred[i * (i + 1)/2 + i] = i;
    }
}
template <typename Weight = unsigned, typename Size = unsigned, Weight inf = std::numeric_limits<Weight>::max()/2>
void floyd_warshall_undirected(std::vector<Weight> &dist, Size n, std::vector<Size> &pred) {
    // all sizes verified in initpred
    for (Size k = 0; k < n; ++k) {
	for (Size i = 0; i < n; ++i) {
	    auto dist_ik = dist[i < k ? k*(k + 1)/2 + i : i*(i + 1)/2 + k];
	    if (dist_ik == inf) continue;
	    for (Size j = 0; j < i; ++j) {
		auto idx_kj = k < j ? j*(j + 1)/2 + k : k*(k + 1)/2 + j;
		auto dist_kj = dist[idx_kj];
		if constexpr (inf > std::numeric_limits<Weight>::max() - inf)
                    if (dist_kj == inf) continue;
		auto &dist_ij = dist[i*(i + 1)/2 + j];
		if (auto dist_ikj = dist_ik + dist_kj; dist_ikj < dist_ij) {
		    dist_ij = dist_ikj;
		    pred[i*(i + 1)/2 + j] = pred[idx_kj];
		}
	    }
	}
    }
}
using namespace std;
int main() {
    constexpr int inf = INT_MAX/2;
    vector<tuple<unsigned, unsigned, int>> edges {{
	tuple { 1, 3, -2 },
	tuple { 2, 1, 4 },
	tuple { 2, 3, 3 },
	tuple { 3, 4, 2 },
 	tuple { 4, 2, -1 },
    }};	
    constexpr unsigned n = 4;
    vector<vector<int>> dist(n, vector<int>(n, inf));
    for (unsigned i = 0; i < n; ++i) dist[i][i] = 0;
    for (auto &edge: edges)
	dist[get<0>(edge) - 1][get<1>(edge) - 1] = get<2>(edge);
    for (unsigned i = 0; i < n; ++i) {
	for (unsigned j = 0; j < n; ++j) dist[i][j] == inf ? cout << "inf " : cout << dist[i][j] << ' ';
	cout << endl;
    }
    cout << endl;
    floyd_warshall<int,unsigned,inf>(dist);
    for (unsigned i = 0; i < n; ++i) {
	for (unsigned j = 0; j < n; ++j) dist[i][j] == inf ? cout << "inf " : cout << dist[i][j] << ' ';
	cout << endl;
    }
    cout << endl;
    return 0;
}
