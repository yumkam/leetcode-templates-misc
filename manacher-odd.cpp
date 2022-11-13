#include <bits/stdc++.h>
using namespace std;
vector<int> manacher_odd(string s) {
    int n = s.size();
    s = "$" + s + "^";
    vector<int> p(n + 2);
    int l = 1, r = 1;
    for(int i = 1; i <= n; i++) {
        p[i] = max(0, min(r - i, p[l + (r - i)]));
        while(s[i - p[i]] == s[i + p[i]]) {
            p[i]++;
        }
        if(i + p[i] > r) {
            l = i - p[i], r = i + p[i];
        }
    }
    return vector<int>(begin(p) + 1, end(p) - 1);
}
vector<int> manacher_odd2(const string &s) {
    int n = s.size();
    vector<int> p(n + 1);
    int l = 0, r = 1;
    for(int i = 0; i < n; i++) {
        p[i] = max(0, min(r - i, l + (r - i) >= 0 ? p[l + (r - i)] : 0));
	int limit = min(n - i - 1, i);
        while(p[i] <= limit && s[i - p[i]] == s[i + p[i]]) {
            p[i]++;
        }
        if(i + p[i] > r) {
            l = i - p[i]; r = i + p[i];
        }
    }
    p.pop_back();
    return p;
}
vector<int> manacher_all(const string &s) {
    int n = s.size()*2 + 1;
    vector<int> p(n);
    int l1 = 1, r = 1;
    for(int i = 0; i < n; i++) {
	    //clog << l << ' ' << r << ' ' << i << endl;
        p[i] = max(0, min(r - i, l1 + (r - i) >= 1 ? p[l1 + (r - i) - 1] : 0));
	int limit = min(n - i - 1, i);
#if 0
        while(p[i] <= limit && 
	      ((i - p[i]) & 1) ?
	      	(((i + p[i]) & 1) ? s[(i - p[i]) >> 1] == s[(i + p[i]) >> 1] : false)
	       : !((i + p[i]) & 1)) {
            p[i]++;
        }
#else
        while(p[i] <= limit && 
	      (!((i ^ p[i]) & 1) || s[(i - p[i]) >> 1] == s[(i + p[i]) >> 1])) {
            p[i]++;
        }
#endif
        if(i + p[i] > r) {
            l1 = i - p[i] + 1; r = i + p[i];
        }
    }
    return p;
}
// return odd-sized palindrome sizes for as odd indexes, even-sized palindrome sizes as even indexes
// both - doubled
template <typename size_type = size_t, typename String>
vector<size_type> manacher_all2(const String &s) {
    size_type n = s.size()*2 + 1;
    vector<size_type> p(n);
    size_type l1 = 1, r = 1;
    for(size_type i = 0; i < n; i++) {
	size_type pi = 0;
	if (r > i && l1 + (r - i) >= 1)
		pi = min(r - i, p[l1 + (r - i) - 1]);
	size_type limit = min(n - i - 1, i);
	// when i + pi is odd, i - pi is also odd
        while(pi <= limit && 
	      (!((i ^ pi) & 1) || s[(i - pi) >> 1] == s[(i + pi) >> 1])) {
            pi++;
        }
        if(i + pi > r) {
            l1 = i - pi + 1; r = i + pi;
        }
	p[i] = pi;
    }
    return p;
}
vector<int> manacher_al(string s) {
	    string t;
    for(auto c: s) {
        t += string("#") + c;
    }
    return manacher_odd(t + "#");
}
template <typename T>
ostream &operator << (ostream & os, const vector<T> &v) {
	os << '[';
	for (auto &a: v) os << ' ' << a;
	os << ']';
	return os;
}
int main() {
	string s;
#define TEST 1
#if TEST
	mt19937_64 rng;
	clog << setw(10);
	for (size_t l = 1; l < 4096; l++) {
		string s(l, ' ');
		uint64_t r = 0;
		size_t b = 0;
		for (size_t t = 0; t < 100; t++) {
			clog << '\r' << setw(10) <<l << ' ' << setw(10) <<t << flush;
			for (auto &c: s) {
				if (b < 5) { r = rng(); b = sizeof(r)*8; }
				c = (r % 4) + 'a';
				r >>= 5;
				b -= 5;
			}
			{
				auto r1 = manacher_odd(s);
				auto r2 = manacher_odd2(s);
				assert(equal(r1.begin(), r1.end(), r2.begin()));
				auto mm = minmax_element(r1.begin(), r1.end());
				clog << ' ' << setw(10) << *mm.first << ' ' << setw(10) <<*mm.second;
			}
			{
				auto r1 = manacher_al(s);
				auto r2 = manacher_all2<unsigned>(s);
				assert(equal(r1.begin(), r1.end(), r2.begin()));
				auto mm = minmax_element(r1.begin(), r1.end());
				clog << ' ' << setw(10) << *mm.first << ' ' << setw(10) <<*mm.second;
			}
			clog << flush;
		}
	}
#else
	cin >> s;
	cout << manacher_odd(s) << endl;
	cout << manacher_odd2(s) << endl;
	cout << manacher_al(s) << endl;
	cout << manacher_all(s) << endl;
	cout << manacher_all2(s) << endl;
#endif
	return 0;
}
