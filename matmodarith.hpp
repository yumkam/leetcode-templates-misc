#include <vector>
#include <array>
#include <limits>
#include <cstdint>
namespace matmodarith {
    static constexpr unsigned modulo = 1000000000+7;
    static void addmod(unsigned &a, unsigned b) {
        static_assert(modulo - 1 <= std::numeric_limits<unsigned>::max() - (modulo - 1));
        if ((a += b) >= modulo)
            a -= modulo;
    }
    static unsigned mulmod(unsigned a, unsigned b) {
        static_assert(modulo - 1 < std::numeric_limits<uint64_t>::max()/(modulo - 1));
        return a*static_cast<uint64_t>(b) % modulo;
    }
    static void addmulmod(unsigned &a, unsigned b, unsigned c) {
        static_assert(modulo - 1 < (std::numeric_limits<uint64_t>::max() - (modulo - 1))/(modulo - 1));
        a = (a + b*static_cast<uint64_t>(c)) % modulo;
    }
    typedef std::array<unsigned, 5> mvec;
    typedef std::array<mvec, 5> matrix;
    static void mmmul(matrix &r, const matrix &a, const matrix &b) {
        for (unsigned i = 0; i < a.size(); i++) {
            const auto &ai = a[i];
            auto &ri = r[i];
	    std::fill(ri.begin(), ri.end(), 0);
            for (unsigned j = 0; j < a.size(); j++) {
                auto aij = ai[j];
                const auto &bj = b[j];
                for (unsigned k = 0; k < a.size(); k++)
                    addmulmod(ri[k], aij, bj[k]);
            }
        }
    }
    static void mvmul(mvec &r, const matrix &a, const mvec &v) {
        for (unsigned i = 0; i < a.size(); i++) {
            unsigned s = 0;
            for (unsigned j = 0; j < a.size(); j++)
                addmulmod(s, a[i][j], v[j]);
             r[i] = s;
        }
    }
    static void midentity(matrix &r) {
        for (unsigned i = 0; i < r.size(); i++)
            for (unsigned j = 0; j < r.size(); j++)
                r[i][j] = i == j;
    }
    static void mpow(matrix &r, const matrix &a, unsigned n) {
        auto t = a;
        auto tp = &t;
        auto rp = &r;
        matrix w1s, w2s;
        auto w1p = &w1s;
        auto w2p = &w2s;
        for (;;) {
            if ((n & 1)) { mmmul(*w1p, *rp, *tp); swap(rp, w1p); }
            if ((n >>= 1) == 0) break;
            mmmul(*w2p, *tp, *tp);
            swap(w2p, tp);
        }
        if (rp != &r)
            r = *rp;
    }
}
