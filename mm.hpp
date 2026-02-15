#ifndef MINIMAX_HPP
#define MINIMAX_HPP
#include <utility>
template <typename T>
static void minimize(T& a, T b) {
    if (a > b) a = b;
}
#define MMI minimize
template <typename T>
static void maximize(T& a, T b) {
    if (a < b) a = b;
}
#define MMA maximize
template <typename T>
static void minimax(T& a, T& b) {
    if (a > b) std::swap(a, b);
}
#endif
