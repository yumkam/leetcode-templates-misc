#ifndef UNION_FIND_HPP
#define UNION_FIND_HPP 1
#include <algorithm>
#include <utility>
namespace union_find {
    template <typename IndexType = unsigned, typename RankType = unsigned>
        struct Vertex {
            typedef IndexType index_type;
            typedef RankType rank_type;
            IndexType parent;
            RankType rank;
            Vertex(IndexType parent = 0):parent{parent}, rank{0} {}
            void operator ++() {
                ++rank;
            }
            void operator += (Vertex &other) {
                other.parent = parent;
            }
            void operator = (IndexType index) {
                parent = index;
            }
            operator IndexType () {
                return parent;
            }
        };
    template <typename IndexType = unsigned, typename RankType = unsigned, typename SizeType = unsigned>
        struct SizedVertex: public Vertex<IndexType, RankType> {
            typedef SizeType size_type;
            SizeType size;
            SizedVertex(IndexType parent = 0, SizeType size = 1):Vertex<IndexType,RankType>{parent}, size{size} {}
            void operator += (SizedVertex &other) {
                this->Vertex<IndexType, RankType>::operator +=(other);
                size += other.size;
            }
        };
    template <typename V, typename I>
    static I find_root(V &v, I i) {
        auto j = i;
        while(v[j].parent != j)
            j = v[j].parent;
        while (i != j)
            i = std::exchange(v[i].parent, j);
        return j;
    }
    template <bool canonize = true, typename V, typename I>
    static I union_roots(V &v, I i, I j) {
        if constexpr (canonize) {
            i = find_root(v, i);
            j = find_root(v, j);
        }
        if (i == j)
            return i;
        if (v[j].rank > v[i].rank)
            std::swap(i, j);
        else if (v[i].rank == v[j].rank)
            ++v[i].rank;
        v[i] += v[j];
        return i;
    }
};
#endif
