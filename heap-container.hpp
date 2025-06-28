#ifndef BINARY_HEAP_IT_HPP
#define BINARY_HEAP_IT_HPP 1
#include <algorithm>
namespace binaryHeapIt {
    // Maintains max heap of size (@end - @begin); sifts element at index (@mid - @begin) down
    // (toward the leafs of heap)
    // cmp(const T& a, const T& b) -> bool
    //    return true if @a is less than @b
    //    use greater-than comparator for min-heap
    // Expects:
    // *) It is random-accessible iteratror
    // *) @begin, @end, @mid belong to same container
    // *) @begin <= @mid <= @end
    // *) with exception of item @mid, [@begin, @end] is a valid heap
    // *) element at @mid is not greater than parent in heap
    // time = O(log (@end - @mid))
    template <typename It, typename Comp>
    static void siftDown(It begin, It end, It mid, Comp cmp) {
        using namespace std;
        const auto k = end - begin;
        auto idx = mid - begin;
        if (!k)
            return;
        while (idx < (k - 1)/2) {
            auto idx_left = 2*idx + 1;
            // assert(idx_left < k);
            auto idx_right = idx_left + 1;
            // assert(idx_right < k);
            if (!cmp(begin[idx_right], begin[idx_left]))
                idx_left = idx_right;
            if (!cmp(begin[idx], begin[idx_left]))
                return;
            swap(begin[idx], begin[idx_left]);
            idx = idx_left;
        }
        if (idx < k/2) {
            auto idx_left = 2*idx + 1;
            // assert(idx_left + 1 >= k);
            if (!cmp(begin[idx], begin[idx_left]))
                return;
            swap(begin[idx], begin[idx_left]);
        }
    }

    // Maintains max heap; sifts element at @end up (toward root of heap)
    // time = O(log n)
    template <typename It, typename Comp>
    static void siftUp(It begin, It end, Comp cmp) {
        using namespace std;
        auto idx = end - begin;
        while(idx > 0) {
            auto root = (idx - 1)/2;
            if (!cmp(begin[root], begin[idx]))
                break;
            swap(begin[idx], begin[root]);
            idx = root;
        }
    }

    // Maintains max heap; replace element at @mid with @val and fix heap
    template <typename It, typename T, typename Comp>
    static void adjustHeap(It begin, It end, It mid, T&& val, Comp cmp) {
        if (cmp(*mid, val)) {
            *mid = std::move(val);
            siftUp(begin, mid);
        } else if (cmp(val, *mid)) {
            *mid = std::move(val);
            siftDown(begin, end, mid, cmp);
        } else {
            *mid = std::move(val);
        }
    }

    // Makes max heap of size k
    // time = O(n)
    template <typename It, typename Comp>
    static void makeHeap(It begin, It end, Comp cmp) {
        auto k = end - begin;
        for (auto idx = k/2; idx--; ) {
            siftDown(begin, end, begin + idx, cmp);
        }
    }
};
#endif
