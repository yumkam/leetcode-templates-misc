#ifndef TRYGET_HPP
#define TRYGET_HPP
#include <type_traits>
namespace {
    const auto &tryget(const auto &u, const auto &key) {
        // lookup key in map u, return const ref if found, or ref to const default value if not
        const auto j = u.find(key);
        static const std::decay_t<decltype(j->second)> r{};
        return j == u.end() ? r : j->second;
    }
}
#endif
