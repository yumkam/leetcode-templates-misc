#include "fenwick-tree-2d.hpp"
#include <bits/stdc++.h>
using namespace std;
int main() {
    auto print = [](auto &ft, auto m, auto n) {
	for (unsigned i = 0; i < m; ++i) { for (unsigned j = 0; j < n; ++j) cout << ft.prefix_sum(i, j) << ' '; cout << '\n'; }
    };
    {
	const unsigned m = 5;
	const unsigned n = 5;
	FenwickTree2D<int, vector<vector<int>>> ft(m, n);
	print(ft,m,n); cout << endl;
	ft.add(0, 3, 3);
	print(ft,m,n); cout << endl;
	ft.add(4, 0, 5);
	print(ft,m,n); cout << endl;
	ft.add(1, 2, 7);
	print(ft,m,n); cout << endl;
    }
    mt19937 rng;
    for (unsigned t = 1; t < 100; ++t) {
	unsigned m = rng() % 200;
	unsigned n = rng() % 200;
	if (n*m > 10000) {
	    if (rng() & 1)
		m = 10000/n;
	    else
		n = 10000/m;
	}
	FenwickTree2D<int> ft(m, n);
	vector<vector<int>> mat(m, vector<int>(n));
	const unsigned vr = (INT_MAX/(n*m*1000));
	vector<int> temp(m);
	cout << m << 'x' << n << endl;
	for (unsigned subt = 0; subt < 1000; ++subt) {
	    unsigned ti = rng() % m;
	    unsigned tj = rng() % n;
	    unsigned tv = rng() % vr;
	    clog << m << 'x' << n << ' ' << ti << ',' << tj << '+' << tv << '\r' << flush;
	    ft.add(ti, tj, tv);
	    mat[ti][tj] += tv;
	    temp.clear();
	    temp.resize(n);
#if DEBUG
	    cout << ti << ',' << tj << '+' << tv << endl;
	    print(ft,m,n); cout << endl;
#endif
	    for (unsigned i = 0; i < m; ++i) {
		int rsum = 0;
		for (unsigned j = 0; j < n; ++j) {
		    temp[j] += (rsum += mat[i][j]);
#if DEBUG
		    cout << ' ' << temp[j] << flush;
#endif
		    assert(temp[j] == ft.prefix_sum(i, j));
		}
#if DEBUG
		cout << endl;
#endif
	    }
	}
    }
    return 0;
}
