#ifndef VHASHOP_HPP
#define VHASHOP_HPP
template <size_t hpow=10007> // hmodulo implicit 2**(sizeof(size_t)*8)
struct vhashop {
    auto operator() (const auto &v) const {
        size_t h = 1; // avoids { 0 }, { 0, 0 }, {0, 0, 0},... to have same zero hash
        for (const auto x: v) // assumes v::value_type is integer type
            h = h*hpow + x;
        return h;
    }
};
#endif
