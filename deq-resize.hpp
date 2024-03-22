#ifndef DEQ_RESIZE_HPP__
#define DEQ_RESIZE_HPP__
#include <vector>
#include <utility>
#include <iterator>
#define assert(X)
template <typename T, typename V = std::vector<T>> class fdeq {
	public:
	typedef typename V::size_type size_type;
	typedef typename V::value_type value_type;
	typedef typename V::reference reference;
	private:
	V v;
	size_type h, t;
	bool e;
        void expand() {
            expand(std::max(v.capacity(), v.size()*2));
        }
        void expand(size_type target) {
                const auto sz = v.size();
                newv.reserve(target);
                V newv;
                if (!empty())
                    do {
                        newv.push_back(std::move(v[h]));
                        if (++h == sz) h = 0;
                    } while(h != t);
                h = 0;
                t = newv.size();
                newv.resize(newv.capacity());
                std::swap(newv, v);
        }
	public:
	fdeq(const fdeq &) = default;
	fdeq(fdeq &&) = default;
	fdeq(V&& v):v(std::move(v)), h(0), t(v.size() - 1), e(v.empty()) {}
	template <class... Args> deq(Args&&...args) : v(std::forward<Args>(args)...), h(0), t(0), e(true) {}
	template <class... Args>
		void emplace_front(Args&&...args) {
                        if (full()) expand();
			if (h == 0) h = v.size() - 1; else --h;
			e = false;
			v[h] = T(std::forward<Args>(args)...);
		}
	void push_front(T&&x) {
		return emplace_front(std::move(x));
	}
	void push_front(const T& x) {
		return emplace_front(x);
	}
	template <class... Args>
	void emplace_back(Args&&...args) {
                if (full()) expand();
		v[t] = T(std::forward<Args>(args)...);
		if (t == v.size() - 1) t = 0; else ++t;
		e = false;
	}
	void push_back(T&&x) {
		return emplace_back(std::move(x));
	}
	void push_back(const T& x) {
		return emplace_back(x);
	}
	T&back() {
		assert(!empty());
		return t == 0 ? v.back() : v[t - 1];
	}
	T&front() {
		assert(!empty());
		return v[h];
	}
	void pop_back() {
		assert(!empty());
		if (t == 0) t = v.size() - 1; else --t;
		if (h == t) e = true;
	}
	void pop_front() {
		assert(!empty());
		if (h == v.size() - 1) h = 0; else ++h;
		if (h == t) e = true;
	}
	bool empty() const {
		return e;
	}
	bool full() const {
		return h == t && !e;
	}
	size_t size() const {
		return e ? 0 : t > h ? t - h : t + (v.size() - h);
	}
	void clear() {
		h = t; e = true;
	}
	void reserve(size_type size) {
            if (size > v.size())
                expand(size);
	}
	size_type capacity() const {
		return v.size();
	}
	void swap(fdeq &other) {
		std::swap(v, other.v);
		std::swap(h, other.h);
		std::swap(t, other.t);
		std::swap(e, other.e);
	}
#if 0 // XXX can't distinguish iterator at begin and iterator at end of full deq
	template <typename Deq, typename T>
	class iterator_impl {
		Deq &c;
		size_type i;
		public:
		typedef typename std::remove_reference<decltype(c)>::type::difference_type difference_type;
		iterator_impl(Deq &c, size_type i):c{c}, i{i} {}
		iterator_impl(const iterator_impl &) = default;
		iterator_impl(iterator_impl &&) = default;
		T &operator *() { return c.v[i]; }
		auto & operator ++() { if (++i == c.v.size()) i = 0; return *this; }
		auto & operator --() { if (--i == c.v.size()) i = 0; return *this; }
		auto & operator += (difference_type x) {
			assert(x < c.v.size());
			i += x;
			if (i >= c.v.size()) i -= c.v.size();
			return *this;
		}
		difference_type operator - (iterator_impl &other) {
			`
	}
	auto begin() { return iterator_impl<fdeq, value_type>(*this, h); }
	auto end() { return iterator_impl<fdeq, value_type>(*this, t); }
	auto cbegin() { return iterator_impl<const fdeq, const value_type>(*this, h); }
	auto cend() { return iterator_impl<const fdeq, const value_type>(*this, t); }
#endif
};
template <typename T, typename V>
void std::swap(fdeq<T, V> &x, fdeq<T, V> &y) {
	x.swap(y);
}
#endif
