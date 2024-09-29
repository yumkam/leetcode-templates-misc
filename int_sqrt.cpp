#include <bits/stdc++.h>
using namespace std;
template <typename Int>
Int int_floor_sqrt(Int a) { // maximum X such that X*X <= a
    Int lo = 0;
    static const Int limit = std::floor(std::sqrt(std::numeric_limits<Int>::max()));
    Int hi = std::max((Int)1, std::min(a/2, limit));
    while(lo < hi) {
	Int mid = lo + (hi - lo + 1)/2;
	if (mid*mid > a)
	    hi = mid - 1;
	else
	    lo = mid;
    }
    return lo;
}
template <typename Int>
Int int_ceil_sqrt(Int a) { // minimum X such that X*X >= a
    Int lo = 0;
    static const Int limit = std::ceil(std::sqrt(std::numeric_limits<Int>::max()));
    Int hi = std::max((Int)1, std::min(a/2, limit));
    while(lo < hi) {
	Int mid = lo + (hi - lo)/2;
	if (mid*mid < a)
	    lo = mid + 1;
	else
	    hi = mid;
    }
    return lo;
}
int main() {
    cout << ceil(sqrt(INT_MAX)) << endl;
    cout << int_ceil_sqrt(INT_MAX) << endl;
    cout << floor(sqrt(INT_MAX)) << endl;
    cout << int_floor_sqrt(INT_MAX) << endl;
}
