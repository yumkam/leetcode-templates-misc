template <typename T, typename C>
static void forall_masks(T mask, C&& callback) {
    T m = 0;
    do {
        callback(m);
    } while((m = (m - 1) & mask));
}
#include <array>
#include <bit>
#include <numeric>
#include <cstdint>
#include <climits>
void generate_bitmasks(auto& masks, bool one_based = true) {
    const auto top = masks.size() + one_based;
    const auto n = std::countr_zero(top);
    using T = std::decay_t<decltype(masks)>::value_type;
    constexpr auto maxbits = sizeof(T)*CHAR_BIT;
    std::array<T, maxbits + 1> cntbit;
    { // pascal triangle
        T half = n/2;
        T acc = 1;
        for (T k = 1; k <= half; k++)
            cntbit[n - k] = cntbit[k] = (acc = acc * (uint64_t)(n - k + 1)/k);
        cntbit[0] = !one_based;
        cntbit[n] = 1;
    }
    //for (auto c:cntbit) clog << c << ' '; clog << '=' << n<< endl;
    std::partial_sum(cntbit.begin(), cntbit.end(), cntbit.begin());
    for (T i = one_based; i < top; ++i)
        masks[--cntbit[std::popcount(i)]] = i;
}

