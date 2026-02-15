#ifndef ABSDIFF_HPP
#define ABSDIFF_HPP
#include <type_traits>
template <typename T>
static auto absdiff(T a, T b) {
    std::make_unsigned_t<T> ua = a, ub = b;
    return a >= b ? ua - ub : ub - ua;
}
#endif
