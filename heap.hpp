#ifndef BINARY_HEAP_HPP
#define BINARY_HEAP_HPP 1
namespace binaryHeap {
    // Maintains max heap of size k; sifts element at index idx down
    // time = O(log n)
    // cmp(size_t a, size_t b) -> bool
    //    return true if element at index a is less than element at index b
    //    use greater-than comparator for min-heap
    // swapit(size_t a, size_t b) -> void
    //    called to swap elements at indexes a and b
    template <typename Size, typename Comp, typename Swap>
    static void siftDown(Size k, Size idx, Comp cmp, Swap swapit) {
        if (!k)
            return;
        while (idx < (k - 1)/2) {
            auto idx_left = 2*idx + 1;
            // assert(idx_left < k);
            auto idx_right = idx_left + 1;
            // assert(idx_right < k);
            if (!cmp(idx_right, idx_left))
                idx_left = idx_right;
            if (!cmp(idx, idx_left))
                return;
            swapit(idx, idx_left);
            idx = idx_left;
        }
        if (idx < k/2) {
            auto idx_left = 2*idx + 1;
            // assert(idx_left + 1 >= k);
            if (!cmp(idx, idx_left))
                return;
            swapit(idx, idx_left);
        }
    }
    // Maintains max heap; sifts element at index idx up
    // time = O(log n)
    template <typename Size, typename Comp, typename Swap>
    static void siftUp(Size idx, Comp cmp, Swap swapit) {
        while(idx > 0) {
            auto root = (idx - 1)/2;
            if (!cmp(root, idx))
                break;
            swapit(idx, root);
            idx = root;
        }
    }
    // Makes max heap of size k
    // time = O(n)
    template <typename Size, typename Comp, typename Swap>
    static void makeHeap(Size k, Comp cmp, Swap swap) {
        for (auto idx = k/2; idx--; ) {
            siftDown(k, idx, cmp, swap);
        }
    }
};
#endif
