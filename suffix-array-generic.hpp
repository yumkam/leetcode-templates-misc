// https://github.com/e-maxx-eng/e-maxx-eng/issues/815
template <typename V, typename Size_Type = typename V::size_type>
struct SuffixArray { // https://cp-algorithms.com/string/suffix-array.html
  typedef Size_Type size_type;
  size_type n;
  vector < size_type > p;
  vector < vector < size_type > > C;
  SuffixArray(const V & s,
	const size_type alphabet = static_cast<size_type>(1) << (8*sizeof(typename V::value_type))) {
    n = s.size();
    static_assert(sizeof(typename V::value_type) < sizeof(size_type));
    vector < size_type > cnt(max(alphabet, n), 0), c(n);
    p.resize(n);
    // counter sort array p, O(n)
    for (size_type i = 0; i < n; i++) cnt[s[i]]++;
    for (size_type i = 1; i < alphabet; i++) cnt[i] += cnt[i - 1];
    for (size_type i = 0; i < n; i++) p[--cnt[s[i]]] = i;
    c[p[0]] = 0;
    size_type classes = 1;
    for (size_type i = 1; i < n; i++) {
      if (s[p[i]] != s[p[i - 1]]) classes++;
      c[p[i]] = classes - 1;
    }
    vector < size_type > pn(n);
    for (size_type two_pow_h = 1; two_pow_h < n; two_pow_h <<= 1) {
      vector < size_type > cn(n);
      for (size_type i = 0; i < n; i++) {
        pn[i] = p[i] + n - two_pow_h;
	if (pn[i] >= n) pn[i] -= n;
      }
      fill_n(cnt.begin(), classes, 0);
      for (size_type i = 0; i < n; i++) cnt[c[pn[i]]]++;
      for (size_type i = 1; i < classes; i++) cnt[i] += cnt[i - 1];
      for (size_type i = n; i-- > 0; ) p[--cnt[c[pn[i]]]] = pn[i];
      cn[p[0]] = 0;
      classes = 1;
      for (size_type i = 1; i < n; i++) {
	if (c[p[i]] != c[p[i - 1]])
          ++classes;
	else {
          auto pi_next = p[i] + two_pow_h;
          if (pi_next >= n) pi_next -= n;
          auto pi1_next = p[i - 1] + two_pow_h;
          if (pi1_next >= n) pi1_next -= n;
          if (c[pi_next] != c[pi1_next])
            ++classes;
        }
        cn[p[i]] = classes - 1;
      }
      C.push_back(move(c));
      c = move(cn);
    }
    C.push_back(move(c));
  }
  size_type lcp(size_type i, size_type j) {
    size_type ans = 0;
    for (size_type k = C.size(); k-- > 0 && i < n && j < n; ) {
      size_type b = static_cast<size_type>(1) << k;
      if (C[k][i] == C[k][j]) {
        ans += b;
        i += b;
        j += b;
      }
    }
    return ans;
  }
};
