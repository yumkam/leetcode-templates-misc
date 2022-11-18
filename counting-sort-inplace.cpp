#include "counting-sort-inplace.hpp"
#include <bits/stdc++.h>
using namespace std;
int main() {
	mt19937 gen;
	vector<pair<size_t, uint8_t>> v(50);
	for (size_t i = 0; i < v.size(); i++) {
		v[i] = make_pair(i, (uint8_t)gen() % 5);
		cout << v[i].first << '/' << (int)v[i].second << ' ';
	}
	cout << endl;
	counting_sort_inplace(v, [](auto v) { return v.second; }, numeric_limits<decltype(v[0].second)>::max());
	for (size_t i = 0; i < v.size(); i++)
		cout << v[i].first << '/' << (int)v[i].second << ' ';
	assert(is_sorted_until(v.begin(), v.end(), [](const auto a, const auto b) { return a.second < b.second; }) == v.end());
	cout << endl;
	return 0;
}
