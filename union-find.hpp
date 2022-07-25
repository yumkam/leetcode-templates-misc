#ifndef UNION_FIND_HPP
#define UNION_FIND_HPP 1
namespace union_find {
    template <typename V>
    static I find_root(V &parent, typename V::value_type i) {
        auto j = i;
        while(parent[j] != j)
            j = parent[j];
        for (;;) {
            auto next = parent[i];
            if (next == j)
                return j;
            parent[i] = j;
            i = next;
        }
    }
    template <typename V>
    static I union_roots(V &parent, V&rank/*, V&size*/, typename V::value_type  i, typename V::value_type j) {
        i = find_root(parent, i);
        j = find_root(parent, j);
        if (i == j)
            return i;
        if (rank[i] < rank[j])
            swap(i, j);
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
