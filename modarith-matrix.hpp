#ifndef MODARITH_MATRIX_HPP
#define MODARITH_MATRIX_HPP
#ifndef MODARITH_HPP
#include "modarith.hpp"
#endif
#include <array>
#include <vector>
#include <algorithm>
#include <type_traits>
namespace modarith {
    // variable-size 
    template <typename T = unsigned, typename MVec = std::vector<T>, typename MMat = std::vector<MVec>>
    struct modvmatrix {
        typedef MVec mvec;
        typedef MMat matrix;
        typedef typename mvec::size_type size_type;
        typedef typename mvec::value_type value_type;
        static_assert(std::is_same<T, value_type>::value);
        static_assert(std::is_same<typename matrix::value_type, mvec>::value);
        static void mmmul(matrix &r, const matrix &a, const matrix &b) {
            // `r` must not alias `a` or `b`
            const auto m = a.size();
            const auto n = b.size();
            // assert(n != 0);
            // assert(r.size() == m);
            // for (const auto &rv: r) assert(rv.size() == b[0].size());
            // for (const auto &av: a) assert(av.size() == n);
            // for (const auto &bv: b) assert(bv.size() == b[0].size());
            for (size_type i = 0; i < m; i++) {
                auto &ri = r[i];
                const auto p = ri.size();
                const auto &ai = a[i];
                std::fill(ri.begin(), ri.end(), T{});
                for (size_type j = 0; j < n; j++) {
                    const auto &bj = b[j];
                    const auto aij = ai[j];
                    for (size_type k = 0; k < p; k++)
                        addmulmod(ri[k], aij, bj[k]);
                }
            }
        }
        static void mvmul(mvec &r, const matrix &a, const mvec &v) {
            // `r` must not alias `v` or contained in `a`
            const auto m = a.size();
            const auto n = v.size();
            // assert(r.size() == m);
            // for (const auto &av: a) assert(av.size() == n);
            for (size_type i = 0; i < m; i++) {
                const auto &ai = a[i];
                T s {};
                for (size_type j = 0; j < n; j++)
                    addmulmod(s, ai[j], v[j]);
                r[i] = s;
            }
        }
        static void midentity(matrix &r) {
            const auto d = r.size();
            // for (const auto &rv: v) assert(rv.size() == d);
            for (size_type i = 0; i < d; i++)
                for (size_type j = 0; j < d; j++)
                    r[i][j] = i == j;
        }
        template <typename N>
        static void mpow(matrix &r, const matrix &a, N n) {
            // `r` must not alias `a`
            // assert(r.size() == a.size());
            // for (const auto &rv: r) assert(rv.size() == a.size());
            // for (const auto &av: a) assert(av.size() == a.size());
            auto t = a;
            auto tp = &t;
            auto rp = &r;
            matrix w1s, w2s;
            if (w1s.size() != a.size()) { w1s = a; w2s = a; } // compile-time no-op for std::array
            auto w1p = &w1s;
            auto w2p = &w2s;
            for (;;) {
                if ((n & 1)) { mmmul(*w1p, *rp, *tp); std::swap(rp, w1p); }
                if ((n >>= 1) == 0) break;
                mmmul(*w2p, *tp, *tp);
                std::swap(w2p, tp);
            }
            if (rp != &r)
                r = std::move(*rp);
        }
    };
    // fixed-size std::array based matrix
    template<size_t sz, typename T = unsigned>
        using modmatrix = modvmatrix<T, std::array<T, sz>, std::array<std::array<T, sz>, sz> >;
}
#endif // MODARITH_MATRIX_HPP
// vim: sw=4 et
