#ifndef SQRT_DECOMP_VECTOR
#define SQRT_DECOMP_VECTOR
#include <vector>
#include <array>
#include <iterator>
#include <type_traits>
// vector with sqrt-decomposition
// supports O(1) access
// O(piece_size + n/piece_size) insert and erase in the middle, ammortized O(1) push_back and pop_back
// (assuming $piece_size \approx \sqrt n$, it's O(\sqrt n))
template <typename T, std::size_t PieceSize>
class insertable_vector {
    static constexpr std::size_t piece_size = PieceSize;
    std::vector<std::pair<std::size_t, std::array<T, piece_size>>> m_rep;
    std::size_t m_size = 0;
    public:
    typedef std::size_t size_type;
    typedef T value_type;
    typedef T &reference;
    insertable_vector(size_type s = 0):m_size(s) {
        m_rep.resize((m_size + piece_size - 1)/piece_size);
        for (auto &rep: m_rep) rep.first = 0;
    }
    insertable_vector(size_type s, const value_type &v):insertable_vector(s) {
        for (auto &rep: m_rep) { rep.second.fill(v); }
    }
    insertable_vector(const insertable_vector &other) = default;
    insertable_vector(insertable_vector &&other) = default;
    insertable_vector & operator = (const insertable_vector &other) = default;
    insertable_vector & operator = (insertable_vector &&other) = default;
    class iterator {
        insertable_vector &base;
        public:
        size_type position;
        iterator(insertable_vector &base, size_type position = 0):base{base}, position{position} {}
        typedef std::random_access_iterator_tag iterator_category; 
        typedef T value_type;
        typedef T &reference;
        typedef T *pointer;
        typedef std::make_signed_t<size_type> difference_type;
        iterator(const iterator &other):base{other.base}, position{other.position} {}
        auto &operator = (const iterator &other) noexcept {
	    // debug_assert(static_cast<uintptr_t>(&base) == static_cast<uintptr_t>(&other.base));
            position = other.position;
            return *this;
        }
        reference operator *() const { return base[position]; }
        pointer operator ->() const { return &base[position]; }
        reference operator [](difference_type n) const { return base[position + n]; }

        auto & operator ++() { ++position; return *this; }
        auto & operator --() { --position; return *this; }
        auto operator ++(int) { auto temp = *this; ++(*this); return temp; }
        auto operator --(int) { auto temp = *this; --(*this); return temp; }
        auto & operator +=(const difference_type n) { position += n; return *this; }
        auto & operator -=(const difference_type n) { position -= n; return *this; }
        auto operator +(const difference_type n) const { auto temp = *this; temp += n; return temp; }
        auto operator -(const difference_type n) const { auto temp = *this; temp -= n; return temp; }
        friend auto operator + (const difference_type n, const iterator &a) { return a + n; }
        difference_type operator - (const iterator &other) const { return position - other.position; }

        bool operator == (const iterator &other) const { return position == other.position; }
        bool operator <  (const iterator &other) const { return position <  other.position; }
        bool operator >= (const iterator &other) const { return position >= other.position; }
        bool operator <= (const iterator &other) const { return position <= other.position; }
        bool operator >  (const iterator &other) const { return position >  other.position; }
        bool operator != (const iterator &other) const { return position != other.position; }
    };
    typedef iterator const_iterator;
    // time O(1)
    reference operator[](size_type pos) noexcept {
        auto idx = pos / piece_size;
        auto sub = pos % piece_size;
        auto &rep = m_rep[idx];
        if ((sub += rep.first) >= piece_size)
            sub -= piece_size;
        return rep.second[sub];
    }
    reference at(size_type pos) {
	if (pos >= size())
	    throw std::out_of_range();
	return (*this)[pos];
    }
    iterator begin() noexcept {
        return iterator(*this, 0);
    }
    iterator end() noexcept {
        return iterator(*this, m_size);
    }
    const_iterator cbegin() noexcept {
	return begin();
    }
    const_iterator cend() noexcept {
	return end();
    }
    size_type size() const {
        return m_size;
    }
    bool empty() const {
        return size() == 0;
    }
    iterator erase(iterator cpos) {
	auto pos = cpos.position;
	auto idx = pos / piece_size;
	auto sub = pos % piece_size;
	if (idx + 1 == m_rep.size()) {
	    auto &rep = m_rep[idx];
	    if (sub == 0) {
		if (++rep.first == piece_size) rep.first = 0;
	    } else {
		auto eop = (rep.first + m_size) % piece_size;
		sub += rep.first;
		if (sub >= piece_size)
		    sub -= piece_size;
		for (;;) {
		    auto next = sub;
		    if (++next == piece_size)
			next = 0;
		    if (next == eop)
			break;
		    rep.second[sub] = std::move(rep.second[next]);
		    sub = next;
		}
	    }
	} else {
	    auto &rep = m_rep[idx];
	    sub += rep.first;
	    if (sub >= piece_size)
		sub -= piece_size;
	    for (;;) {
		auto next = sub;
		if (++next == piece_size)
		    next = 0;
		if (next == rep.first)
		    break;
		rep.second[sub] = std::move(rep.second[next]);
		sub = next;
	    }
	    while(idx + 1 < m_rep.size()) {
		auto &rep = m_rep[idx];
		auto &nrep = m_rep[++idx];
		auto last = rep.first;
		if (last == 0)
		    last = piece_size;
		--last;
		rep.second[last] = std::move(nrep.second[nrep.first]);
	    }
	    auto &rep = m_rep[idx];
	    if (++rep.first == piece_size)
		rep.first = 0;
	}
	if ((--m_size) % piece_size == 0)
	    m_rep.pop_back();
	return cpos;
    }
    void pop_back() {
	auto last = end();
	erase(--last);
    }
    // time = O(piece_size)
    iterator insert(iterator cpos, const value_type &v) {
        auto pos = cpos.position;
        if (m_size % piece_size == 0) {
            m_rep.emplace_back();
            m_rep.back().first = 0;
        }
        if (pos == m_size) {
            (*this)[m_size++] = v;
            return cpos;
        }
        ++m_size;
        auto idx = pos / piece_size;
        auto sub = pos % piece_size;
        if (idx == m_rep.size() - 1) {
            if (sub == 0) {
                if (m_rep[idx].first == 0)
                    m_rep[idx].first = piece_size;
                --m_rep[idx].first;
                m_rep[idx].second[m_rep[idx].first] = v;
                return cpos;
            }
	    #if 0
            for (auto t = m_size; --t > pos; )
                (*this)[t] = std::move((*this)[t - 1]);
            (*this)[pos] = v;
	    #else
	    auto &rep = m_rep[idx];
	    auto eop = rep.first + (m_size % piece_size);
	    if (eop >= piece_size)
		eop -= piece_size;
	    if (eop == 0)
		eop = piece_size;
	    --eop;
	    auto cv = rep.first + sub;
	    if (cv >= piece_size)
		cv -= piece_size;
	    while(eop != cv) {
		auto next = eop;
		if (next == 0)
		    next = piece_size;
		--next;
		rep.second[eop] = std::move(rep.second[next]);
		eop = next;
	    }
	    rep.second[cv] = v;
	    #endif
            return cpos;
        }
        auto endidx = m_rep.size();
        while(--endidx > idx) {
	    auto &rep = m_rep[endidx];
            if (rep.first == 0)
                rep.first = piece_size;
            --rep.first;
	    auto &prev = m_rep[endidx - 1];
            auto prevlast = prev.first;
            if (prevlast == 0)
                prevlast = piece_size;
            --prevlast;
           rep.second[rep.first] = std::move(prev.second[prevlast]);
        }
        #if 0
        auto eop = pos - sub;
        for (auto t = eop + piece_size; --t > pos; )
            (*this)[t] = std::move((*this)[t - 1]);
        (*this)[pos] = v;
        #else
	auto &rep = m_rep[idx];
        auto eop = rep.first;
	if (eop == 0)
	    eop = piece_size;
	--eop;
	auto cv = rep.first + sub;
	if (cv >= piece_size)
	    cv -= piece_size;
	while(eop != cv) {
	    auto next = eop;
	    if (next == 0)
		next = piece_size;
	    --next;
	    rep.second[eop] = std::move(rep.second[next]);
	    eop = next;
	}
	rep.second[cv] = v;
        #endif
        return cpos;
    }
    auto push_back(const value_type &v) {
        return insert(end(), v);
    }
    void reserve(size_type s) {
	m_rep.reserve((s + piece_size - 1)/piece_size);
    }
    void resize(size_type s) {
	if (s == m_size)
	    return;
	if (s < m_size) {
	    m_size = s;
	    m_rep.resize((m_size + piece_size - 1) / piece_size);
	    //m_rep_size = (m_size + piece_size - 1) / piece_size;
	    //m_last_size = m_size % piece_size;
	    return;
	}
	reserve(s);
	auto i = m_rep.size();
	m_rep.resize((s + piece_size - 1) / piece_size);
	for (; i < m_rep.size(); ++i)
	    m_rep[i].first = 0;
	//auto new_rep_size = (s + piece_size - 1) / piece_size;
	//auto new_last_size = s % piece_size;
    }
    void resize(size_type s, const value_type &v) {
	if (s <= m_size)
	    return resize(s);
	reserve(s);
	while(m_size < s)
	    push_back(v);
    }
    void shrink_to_fit() {
	m_rep.shrink_to_fit();
    }
};
#endif
// vim: et sw=4 ts=4
