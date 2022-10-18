#ifndef UNQIUE_COUNT_HPP
#define UNQIUE_COUNT_HPP
template <typename Iterator>
size_t unique_count(Iterator begin, Iterator end) {
    if (begin == end)
        return 0;
    size_t res = 1;
    for (auto prev = begin; ++begin != end; prev = begin)
        res += *prev != *begin;
    return res;
}
template <typename Iterator, typename Comp>
size_t unique_count(Iterator begin, Iterator end, Comp eq) {
    if (begin == end)
        return 0;
    size_t res = 1;
    for (auto prev = begin; ++begin != end; prev = begin)
        res += !eq(*prev, *begin);
    return res;
}
#endif
