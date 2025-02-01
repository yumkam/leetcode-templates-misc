#ifndef UNION_FIND_HPP
#define UNION_FIND_HPP 1
#include <utility>
namespace union_find {
    template <typename V>
    static auto find_root(V &parent, typename V::value_type i) {
        auto j = i;
        while (parent[j] != j)
            j = parent[j];
        while (i != j)
            i = std::exchange(parent[i], j);
        return j;
    }
    template <bool canonize = true, typename V>
    static auto union_roots(V &parent, V&rank/*, V&size*/, typename V::value_type  i, typename V::value_type j) {
        if constexpr (canonize) {
            i = find_root(parent, i);
            j = find_root(parent, j);
        }
        if (i == j)
            return i;
        if (rank[i] < rank[j])
            std::swap(i, j);
        else if (rank[i] == rank[j])
            ++rank[i];
        parent[j] = i;
        #if 0
        size[i] += size[j];
        #endif
        return i;
    }
};
#endif
//using namespace union_find;
