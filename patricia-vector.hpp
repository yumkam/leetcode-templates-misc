#ifndef PATRICIA_VECTOR_HPP
#define PATRICIA_VECTOR_HPP 1
#include <vector>
template <typename K, typename V>
class PatriciaTree {
    // assumes K is random-access indexed container
    // with integer K::value_type (e.g. string, array<int,N>, vector<unsigned>)
    typedef typename K::value_type key_value_type;
    typedef V value_type;
    static constexpr auto key_value_bits = 8*sizeof(key_value_type);
    static constexpr auto topbit = static_cast<size_t>(1) << (8*sizeof(size_t) - 1);
    typedef pair<const K,V> entry_type;
    std::vector<entry_type> external;
    std::vector<pair<size_t,array<size_t,2>>> internal;
    static constexpr auto nil = ~static_cast<size_t>(0);
    template <typename T>
    static unsigned clz(T i) {
        extern unsigned linktime_error_clz_unknown_size();
        return
            sizeof(i) <= sizeof(unsigned) ? __builtin_clz(i) - (sizeof(unsigned) - sizeof(i))*8u:
            sizeof(i) <= sizeof(unsigned long) ? __builtin_clzl(i)  - (sizeof(unsigned long) - sizeof(i))*8u:
            sizeof(i) <= sizeof(unsigned long long) ? __builtin_clzll(i)  - (sizeof(unsigned long long) - sizeof(i))*8u:
            linktime_error_clz_unknown_size();
    }
    size_t root;
    void dump(size_t node) {
        if (node == nil) { std::cout << "null"; return; }
        if (node & topbit) {
            node ^= topbit;
            std::cout << '"' << external[node].first << '"';
            return;
        }
        std::cout << internal[node].first / key_value_bits << '+' << internal[node].first % key_value_bits << '(';
        dump(internal[node].second[0]); std::cout << ',';
        dump(internal[node].second[1]); std::cout << ')';
    }
    public:
    void dump() {
        dump(root);
    }
    PatriciaTree():root {nil} {}
    PatriciaTree(size_type size):PatriciaTree { reserve(size); }
    ~PatriciaTree() { clear(); }
    void reserve(size_t size) {
        external.reserve(size);
        internal.reserve(size - 1);
    }
    void clear() {
        external.clear();
        internal.clear();
	root = nil;
    }
    void insert(const K& key, const V& value) {
        if (root == nil) {
            root = external.size() ^ topbit;
            external.emplace_back(key, value);
            return;
        }
        size_t pos;
        unsigned bit;
        {
            auto node = root;
            for (; !(node & topbit);) {
                auto &rec = internal[node];
                auto pos = rec.first / key_value_bits;
                auto bitv = pos >= key.size() ? 0 : (key[pos] >> (key_value_bits - 1 - (rec.first % key_value_bits))) & 1;
                node = rec.second[bitv];
            }
            auto idx = node ^ topbit;
            auto &rec = external[idx].first;
            auto l = min(key.size(), rec.size());
            pos = 0;
            for (; pos < l; pos++)
                if (key[pos] != rec[pos])
                    break ;
            auto diff = 0;
            if (pos < key.size()) diff ^= key[pos];
            if (pos < rec.size()) diff ^= rec[pos];
            //if (!diff) std::cout << key << ' ' << rec << std::endl; 
            if (!diff)
                return;
            bit = clz(diff) % key_value_bits;
        }
        // ins_between
        size_t bitpos = pos * key_value_bits + bit;
        {
            auto node = root;
            auto pnode = &root;
            for (;;) {
                if ((node & topbit) || bitpos < internal[node].first) {
                    *pnode = internal.size();
                    internal.emplace_back(bitpos, array<size_t,2> {}); // invalidates pnode
                    auto p = external.size() ^ topbit;
                    external.emplace_back(key, value);
                    auto bitv = pos >= key.size() ? 0 : (key[pos]>>(key_value_bits - 1 - bit)) & 1;
                    internal.back().second[bitv ^ 1] = node;
                    internal.back().second[bitv ^ 0] = p;
                    return;
                }
                auto &rec = internal[node];
                auto bitv = rec.first / key_value_bits >= key.size() ? 0 :
                    (key[rec.first / key_value_bits] >> (key_value_bits - 1 - (rec.first % key_value_bits))) & 1;
                pnode = &(rec.second[bitv]);
                node = rec.second[bitv];
            }
        }
    }
    entry_type* lookup(const K& key) {
        return lookup(key, key.size() + 1);
    }
    entry_type* lookup(const K& key, size_t skippos) {
        if (root == nil)
            return nullptr;
        auto node = root;
        while(!(node & topbit)) {
            const auto &rec = internal[node];
            auto pos = rec.first / key_value_bits;
            if (pos >= skippos)
                ++pos;
            auto bitv = pos >= key.size() ? 0 :
                (key[pos] >> (key_value_bits - 1 - (rec.first % key_value_bits))) & 1;
            node = rec.second[bitv];
        }
        auto & rec = external[node ^ topbit];
        if (skippos < key.size()) {
            if (rec.first.size() == key.size() - 1 &&
                rec.first.compare(0, skippos, key, 0, skippos) == 0 &&
                rec.first.compare(skippos, rec.first.size() - skippos, key, skippos + 1) == 0)
                return &rec;
            else
                return nullptr;
        }
        if (rec.first == key)
            return &rec;
        else
            return nullptr;
    }
    entry_type *end() { return nullptr; }
};
#endif
