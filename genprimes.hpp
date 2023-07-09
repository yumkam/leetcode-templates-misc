#ifndef GENPRIMES_HPP
#define GENPRIMES_HPP 1
#include <algorithm>
#include <limits>
#include <vector>
// generate primes for checking up to maxv
// last value is senitel (any odd value so that v*v > maxv)
template<typename T>
static std::vector<T> genprimes(T maxv) {
	// time = O(sqrt(maxv))
	std::vector<T> primes;
	// primes.reserve(sqrt(maxv)/(log2(maxv)/2));
	primes.push_back(2);
        for (T p = 3, pq = p*p;; pq += 4*p + 4, p += 2) {
            if (p > maxv) {
                primes.push_back(p); // senitel
                break;
            }
            for (auto pp: primes) {
                if (pp*pp > p)
                    break;
                if ((p % pp) == 0)
                    goto prime_not_prime;
            }
            primes.push_back(p);
            prime_not_prime:;
        }
	return primes;
}
#endif
