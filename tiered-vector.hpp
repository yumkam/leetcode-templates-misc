#ifndef TIERED_VECTOR_HPP
#define TIERED_VECTOR_HPP
#include <vector>
#include <array>
#include <iterator>
#include <type_traits>
#include <algorithm>
// vector with sqrt-decomposition
// supports O(1) access
// O(piece_size + n/piece_size) insert and erase in the middle, ammortized O(1) push_back and pop_back
// (assuming $piece_size \approx \sqrt n$, it's O(\sqrt n))
template <typename T>
class tiered_vector {
    public:
    typedef std::size_t size_type;
    typedef T value_type;
    typedef T &reference;
    typedef const T &const_reference;
    private:
    static constexpr size_type default_shift = 4;
    public:
    size_type m_shift;
    size_type m_size = 0;
    std::vector<value_type> m_rep;
    std::vector<size_type> m_pos;
    constexpr size_type piece_size() const noexcept { return (static_cast<size_type>(1)<<m_shift); }
    constexpr size_type mask() const noexcept { return piece_size() - 1; }
    static size_type size2shift(size_type s) noexcept {
        size_type shift = 1;
        for (size_type mask = 1; ((s + mask)>>shift) > mask; ++shift)
            mask = (mask << 1) | 1;
        return shift;
    }
    public:
    tiered_vector(size_type s = 0, const value_type &v = {})
        : m_shift(size2shift(s)), m_size(s), m_pos((m_size + mask()) >> m_shift) {
        m_rep.reserve(m_pos.size() << m_shift);
        m_rep.resize(s, v);
        m_rep.resize(m_pos.size() << m_shift);
    }
    tiered_vector(const tiered_vector &other) = default;
    tiered_vector(tiered_vector &&other) = default;
    tiered_vector & operator = (const tiered_vector &other) = default;
    tiered_vector & operator = (tiered_vector &&other) = default;
    void swap(tiered_vector &other) {
        swap(m_rep, other.m_rep);
        swap(m_pos, other.m_pos);
        swap(m_size, other.m_size);
    }
    private:
    template <typename IV, typename IT>
    class Iterator {
        IV *base;
        public:
        size_type pos;
        Iterator(IV *base, size_type pos = 0):base{base}, pos{pos} {}
        typedef std::random_access_iterator_tag iterator_category; 
        typedef IT value_type;
        typedef value_type &reference;
        typedef value_type *pointer;
        typedef std::make_signed_t<size_type> difference_type;
        Iterator(const Iterator &other) = default;
        Iterator &operator = (const Iterator &other) = default;
        reference operator *() const {
            return (*base)[pos];
        }
        pointer operator ->() const { return &*(*this); }
        reference operator [](difference_type n) const {
            return *(*this + n);
        }

        auto & operator ++() { ++pos; return *this; }
        auto & operator --() { --pos; return *this; }
        auto operator ++(int) { auto temp = *this; ++(*this); return temp; }
        auto operator --(int) { auto temp = *this; --(*this); return temp; }
        auto & operator +=(difference_type n) { pos += n; return *this; }
        auto operator +(const difference_type n) const { auto temp = *this; temp += n; return temp; }
        auto operator -(const difference_type n) const { auto temp = *this; temp -= n; return temp; }
        friend auto operator + (const difference_type n, const Iterator &a) { return a + n; }
        difference_type operator - (const Iterator &other) const {
            return pos - other.pos;
        }

        bool operator == (const Iterator &other) const { return pos == other.pos; }
        bool operator <  (const Iterator &other) const { return pos <  other.pos; }
        bool operator >= (const Iterator &other) const { return pos >= other.pos; }
        bool operator <= (const Iterator &other) const { return pos <= other.pos; }
        bool operator >  (const Iterator &other) const { return pos >  other.pos; }
        bool operator != (const Iterator &other) const { return pos != other.pos; }
    };
    public:
    typedef Iterator<tiered_vector, value_type> iterator;
    typedef Iterator<const tiered_vector, const value_type> const_iterator;
    typedef std::reverse_iterator<iterator> reverse_iterator;
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    // time O(1)
    reference operator[](size_type pos) noexcept {
        auto idx = pos >> m_shift;
        auto sub = pos & mask();
        return m_rep[(pos & ~mask()) + ((m_pos[idx] + sub) & mask())];
    }
    const_reference operator[](size_type pos) const noexcept {
        auto idx = pos >> m_shift;
        auto sub = pos & mask();
        return m_rep[(pos & ~mask()) + ((m_pos[idx] + sub) & mask())];
    }
    reference at(size_type pos) {
        if (pos >= size())
            throw std::out_of_range("index is out of range");
        return (*this)[pos];
    }
    const_reference at(size_type pos) const {
        if (pos >= size())
            throw std::out_of_range("index is out of range");
        return (*this)[pos];
    }
    auto begin() noexcept {
        return iterator(this, 0);
    }
    auto end() noexcept {
        return iterator(this, m_size);
    }
    auto cbegin() const noexcept {
        return const_iterator(this, 0);
    }
    auto cend() const noexcept {
        return const_iterator(this, m_size);
    }
    auto rbegin() noexcept { return reverse_iterator(end()); }
    auto rend() noexcept { return reverse_iterator(begin()); }
    auto crbegin() const noexcept { return const_reverse_iterator(cend()); }
    auto crend() const noexcept { return const_reverse_iterator(cbegin()); }
    size_type size() const {
        return m_size;
    }
    bool empty() const {
        return m_pos.empty();
    }
    size_t capacity() const {
        return m_rep.capacity();
    }
    void resize(size_t new_size) {
        reserve(new_size);
        m_size = new_size;
        m_pos.resize((m_size + mask()) >> m_shift);
        auto new_rep_size = m_pos.size() << m_shift;
        if (new_rep_size > m_rep.size())
            m_rep.resize(new_rep_size);
    }
    void reserve(size_t new_size) {
        if (new_size <= m_size)
            return;
        if (auto new_shift = size2shift(new_size); new_shift > m_shift) {
            flatten();
            m_shift = new_shift;
            m_pos.clear();
            m_pos.resize((m_size + mask()) >> m_shift);
            if (auto new_size = (m_pos.size() << m_shift); new_size > m_rep.size())
                m_rep.resize(new_size);
        }
        auto new_rep_size = (new_size + mask()) >> m_shift;
        m_pos.reserve(new_rep_size);
        m_rep.reserve(new_rep_size << m_shift);
    }
    void shrink_to_fit() {
        if (auto new_shift = size2shift(m_size); new_shift < m_shift) {
            flatten();
            m_shift = new_shift;
            m_pos.clear();
            m_pos.resize((m_size + mask()) >> m_shift);
        }
        m_rep.resize(m_pos.size() << m_shift);
        m_rep.shrink_to_fit();
        m_pos.shrink_to_fit();
    }
    iterator erase(iterator cpos) {
        auto idx = cpos.pos >> m_shift;
        auto sub = cpos.pos & mask();
        auto spos = cpos.pos - sub;
        auto &pos = m_pos[idx];
        // TODO choose either move-to-front or move-to-back, depending on which is smaller
        if (sub == 0) {
            pos = (pos + 1) & mask();
        } else {
            auto eop = idx + 1 == m_pos.size() ? (pos + m_size) & mask() : pos;
            sub = (sub + pos) & mask();
            for (;;) {
                auto next = (sub + 1) & mask();
                if (next == eop)
                    break;
                m_rep[spos + sub] = std::move(m_rep[spos + next]);
                sub = next;
            }
        }
        auto last = (m_pos[idx] - 1) & mask();
        while(++idx != m_pos.size()) {
            auto &npos = m_pos[idx];
            m_rep[spos + last] = std::move(m_rep[spos + piece_size() + npos]);
            last = npos;
            npos = (npos + 1) & mask();
            spos += piece_size();
        }
        if ((--m_size & mask()) == 0)
            m_pos.pop_back();
        return cpos;
    }
    void pop_back() {
        auto last = end();
        erase(--last);
    }
    // time = O(piece_size)
    iterator insert(iterator cpos, const value_type &v) {
        if ((size() & (size() - 1)) == 0)
            reserve(size()*2);
        auto idx = cpos.pos >> m_shift;
        auto sub = cpos.pos & mask();
        auto spos = cpos.pos - sub;
        if ((m_size & mask()) == 0) {
            m_pos.emplace_back(0);
            if (auto new_size = (m_pos.size() << m_shift); new_size > m_rep.size())
                m_rep.resize(new_size);
        }
        if (spos + sub == m_size++) { // special case: push_back
            m_rep[spos + ((sub + m_pos[idx]) & mask())] = v;
            return cpos;
        }
        auto endidx = m_pos.size();
        auto endpos = (m_pos.size() - 1) << m_shift;
        while(--endidx > idx) {
            auto &pos = m_pos[endidx];
            pos = (pos - 1) & mask();
            auto prevpos = endpos - piece_size();
            auto prevlast = (m_pos[endidx - 1] - 1) & mask();
            m_rep[endpos + pos] = std::move(m_rep[prevpos + prevlast]);
            endpos = prevpos;
        }
#if 0
        auto eop = pos - sub;
        for (auto t = eop + piece_size; --t > pos; )
            (*this)[t] = std::move((*this)[t - 1]);
        (*this)[pos] = v;
#else
        auto &pos = m_pos[idx];
        // TODO choose either move-to-front or move-to-back, depending on which is smaller
        if (sub == 0) {
            pos = (pos - 1) & mask();
            m_rep[spos + pos] = v;
        } else {
            auto eop = idx + 1 == m_pos.size() ? pos + m_size : pos;
            eop = (eop - 1) & mask();
            auto cv = (pos + sub) & mask();
            while(eop != cv) {
                auto next = (eop - 1) & mask();
                m_rep[spos + eop] = std::move(m_rep[spos + next]);
                eop = next;
            }
            m_rep[spos + cv] = v;
        }
        #endif
        return cpos;
    }
    void flatten() {
        if (empty()) return;
        size_t idx = 0;
        auto spos = m_rep.begin();
        for(; idx != m_pos.size(); ++idx) {
            auto next = spos + piece_size();
            if (m_pos[idx]) {
                std::rotate(spos, spos + m_pos[idx], next);
                m_pos[idx] = 0;
            }
            spos = next;
        }
    }
    auto &push_back(const value_type &v) {
        return *insert(end(), v);
    }
    auto &back() { return (*this)[size() - 1]; }
    const auto &back() const { return (*this)[size() - 1]; }
    auto &front() { return (*this)[0]; }
    const auto &front() const { return (*this)[0]; }
};
#endif // TIERED_VECTOR_HPP
// vim: et sw=4 ts=4
