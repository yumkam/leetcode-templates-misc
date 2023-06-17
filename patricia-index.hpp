#ifndef PATRICIA_INDEX_HPP
#define PATRICIA_INDEX_HPP 1
#include <vector>
template <typename K, typename GetKey>
class PatriciaIndex {
    // assumes K is random-access indexed container
    // with integer K::value_type (e.g. string, array<int,N>, vector<unsigned>)
    typedef typename K::value_type key_value_type;
    static constexpr auto key_value_bits = 8*sizeof(key_value_type);
    static constexpr auto topbit = static_cast<size_t>(1) << (8*sizeof(size_t) - 1);
    std::vector<pair<size_t,array<size_t,2>>> internal;
    [[no_unique_address]]
    GetKey getkey; // [&](size_t index) -> const K & { return words[index]; };
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
            std::cout << '"' << getkey(node) << '"';
            return;
        }
        std::cout << internal[node].first / key_value_bits << '+' << internal[node].first % key_value_bits << '(';
        dump(internal[node].second[0]); std::cout << ',';
        dump(internal[node].second[1]); std::cout << ')';
    }
    public:
    static constexpr auto nil = ~static_cast<size_t>(0);
    void dump() {
        dump(root);
    }
    PatriciaIndex(GetKey getkey):getkey(getkey), root {nil} {}
    ~PatriciaIndex() { clear(); }
    void reserve(size_t size) {
        internal.reserve(size - 1);
    }
    void clear() {
        internal.clear();
        root = nil;
    }
    bool empty() const {
        return root == nil;
    }
    size_t size() const {
        return root == nil ? 0 : 1 + internal.size();
    }
    void swap(PatriciaIndex<K, GetKey> &other) {
        std::swap(root, other.root);
        std::swap(getkey, other.getkey);
        // <<< you may need to use reference_wrapper<GetKey> and ref(getkey)
        internal.swap(other.internal);
    }
    size_t insert(size_t index) { // adds key at @index; return @index or index of existing element
        if (root == nil) {
            root = index ^ topbit;
            return index;
        }
        size_t pos;
        unsigned bit;
        const auto &key = getkey(index);
        {
            auto node = root;
            for (; !(node & topbit);) {
                auto &rec = internal[node];
                auto pos = rec.first / key_value_bits;
                auto bitv = pos >= key.size() ? 0 : (key[pos] >> (key_value_bits - 1 - (rec.first % key_value_bits))) & 1;
                node = rec.second[bitv];
            }
            const auto &rec = getkey(node ^ topbit);
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
                return node ^ topbit;
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
                    auto p = index ^ topbit;
                    auto bitv = pos >= key.size() ? 0 : (key[pos]>>(key_value_bits - 1 - bit)) & 1;
		    auto &rec = internal.back();
                    rec.second[bitv ^ 1] = node;
                    rec.second[bitv ^ 0] = p;
                    return index;
                }
                auto &rec = internal[node];
                auto bitv = rec.first / key_value_bits >= key.size() ? 0 :
                    (key[rec.first / key_value_bits] >> (key_value_bits - 1 - (rec.first % key_value_bits))) & 1;
                pnode = &(rec.second[bitv]);
                node = rec.second[bitv];
            }
        }
    }
    template <typename Key>
    size_t lookup(const Key& key) {
        if (root == nil)
            return nil;
        auto node = root;
        while(!(node & topbit)) {
            const auto &rec = internal[node];
            auto pos = rec.first / key_value_bits;
            auto bitv = pos >= key.size() ? 0 :
                (key[pos] >> (key_value_bits - 1 - (rec.first % key_value_bits))) & 1;
            node = rec.second[bitv];
        }
	node ^= topbit;
        if (key == getkey(node))
            return node;
        else
            return nil;
    }
    size_t end() { return nil; }
};
#endif
