#include <bits/stdc++.h>
using namespace std;
int main() {
	string p;
	string s;
	vector<unsigned> lps;
	while(cin >> p) {
		lps.clear(); lps.resize(p.length());
		for (size_t i = 1, j = 0; i < p.length(); )
			if (p[i] == p[j]) {
				lps[i++] = ++j;
			} else if (j > 0) {
				j = lps[j - 1];
			} else {
				++i;
			}
		while(cin >> s) {
			if (!s.length())
				break;
			for (unsigned i = 0, j = 0; i < s.length(); )
				if (s[i] == p[j]) {
					++i; ++j;
					if (j == p.length()) {
						cout << i - p.length() << endl;
						j = lps[j - 1];
					}
				} else if (j > 0) {
					j = lps[j - 1];
				} else {
					++i;
				}
			for (size_t pos = 0; pos <= s.length() - p.length(); pos++)
				if (0 == s.compare(pos, p.length(), p, 0, p.length()))
					cout << pos << endl;
		}
	}
	return 0;
}
