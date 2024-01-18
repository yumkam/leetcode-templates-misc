#ifndef KMP_HPP
#define KMP_HPP
#include <vector>
#include <type_traits>
namespace kmp {
    static auto getlps(auto &&s) {
        const auto n = s.size();
        using index_type = std::decay_t<decltype(n)>;
        std::vector<index_type> lps(n);
        for (index_type i = 1, j = 0; i < n; ) {
            if (s[i] == s[j]) {
                lps[i++] = ++j;
            } else if (j > 0) {
                j = lps[j - 1];
            } else {
                ++i;
            }
        }
        return lps;
    }
    static auto next(auto &&s, auto &&lps, auto &&t, auto &i, auto &j) {
        for (const auto m = t.size(), n = s.size(); i != m; ) {
            if (t[i] == s[j]) {
                ++i; ++j;
                if (j == n) {
                    j = lps[j - 1];
                    return true;
                }
            } else if (j > 0) {
                j = lps[j - 1];
            } else {
                ++i;
            }
        }
        return false;
    }
    static auto search_all(auto &&s, auto &&lps, auto &&t, auto &&callback) {
        const auto m = t.size();
        const auto n = s.size();
        using index_type = std::decay_t<decltype(n)>;
        for (index_type i = 0, j = 0; next(s, lps, t, i, j); )
            if (callback(i - n))
                return true;
        return false;
    }
}
#endif // KMP_HPP
