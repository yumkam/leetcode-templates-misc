#ifndef VBITSET_HPP
#define VBITSET_HPP 1
#include <vector>
#include <algorithm>
#include <utility>
#include <cassert>
// combined bitset and vector, for semi-sparse set of reasonably small integers;
// bitset efficiently answers to "number is present",
// vector efficiently enumerates numbers;
class vbitset {
    std::vector<bool> bits;
    std::vector<unsigned> vals;
    public:
    using iterator = decltype(vals)::iterator;
    using const_iterator = decltype(vals)::const_iterator;
    vbitset() = default;
    bool insert(unsigned v) { // O(1)
        if (bits.size() <= v)
            bits.resize(v + 1);
        if (bits[v])
            return false;
        bits[v] = true;
        vals.push_back(v);
        return true;
    }
    bool contains(unsigned v) const { // O(1)
        return v < bits.size() && bits[v];
    }
    auto begin() const {
        return vals.cbegin();
    }
    auto cbegin() const {
        return vals.cbegin();
    }
    auto end() const {
        return vals.cend();
    }
    auto cend() const {
        return vals.cend();
    }
    auto erase_if(auto&& pred) { // O(size())
        size_t removed = 0;
        vals.erase(
                std::remove_if(vals.begin(), vals.end(),
                    [&pred, &bits = bits, &removed](auto val) {
                        if (pred(val)) {
                            bits[val] = false;
                            ++removed;
                            return true;
                        }
                        return false;
                    }),
                vals.end());
        return removed;
    }
    auto erase(const_iterator it) { // O(1) // XXX invalidates iterators???
        auto last = std::prev(vals.end());
        auto index = it - vals.cbegin();
        bits[std::exchange(vals[index], *last)] = false;
        vals.pop_back();
        return vals.cbegin() + index;
    }
    bool erase(unsigned v) { // O(size())
        if (v >= bits.size() || !bits[v]) {
            return false;
        }
        erase(std::find(vals.cbegin(), vals.cend(), v));
        return true;
    }
    void clear() { // O(size())
        for (auto v: vals) bits[v] = false;
        vals.clear();
    }
    void sort(auto pred = std::less<>()) { // O(size() * log size())
        sort(vals.begin(), vals.end(), pred);
    }
    void reconstruct() { // O(max())
        vals.clear();
        for (unsigned i = 0; i < bits.size(); ++i)
            if (bits[i])
                vals.push_back(i);
    }
    auto size() const { // O(1)
        return vals.size();
    }
    auto max() const { // O(1)
        return bits.size();
    }
    auto empty() const { // O(1)
        return vals.empty();
    }
};
#endif // vbitset
