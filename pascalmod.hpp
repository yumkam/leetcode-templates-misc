#include "modarith.hpp"
#include <vector>
namespace modarith {
    // return vector of choosemod(n, k)
    template <class Result = std::vector<unsigned>>
    Result pascalmod(unsigned rowIndex) {
        Result row(rowIndex + 1, 1);
        unsigned num = 1, denum = 1;
        const unsigned half = rowIndex/2;
        for (unsigned i = 1; i <= half; ++i) {
            mulmod(num, reducemod(rowIndex - i + 1));
            mulmod(denum, reducemod(i));
            assert(denum != 0);
            row[i] = divmod(num, denum);
        }
        for (unsigned k = 1; k <= half; k++)
            row[rowIndex - k] = row[k];
        return row;
    }
}
