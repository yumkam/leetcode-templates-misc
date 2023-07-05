#ifndef CONST_CONTAINER_WRAPPER_HPP
#define CONST_CONTAINER_WRAPPER_HPP 1
template <typename Container> class const_container_wrapper {
	typedef typename Container::value_type value_type;
	typedef typename Container::mapped_type mapped_type;
	typedef typename Container::key_type key_type;
	Container c;
	value_type zero {};
	public:
	const_container_wrapper(const Container &c) : c{c} {} 
	const_container_wrapper(Container &&c) : c{std::move(c)} {} 
	const_container_wrapper() = default;
	const_container_wrapper(const const_container_wrapper &) = default;
	const_container_wrapper(const_container_wrapper &&) = default;
	value_type &operator[](const key_type &key) {
		return c[key];
	}
	const value_type &operator[](const key_type &key) const {
		auto i = c.find(key);
		return i == c.end() ? as_const(zero) : i->second;
	}
	constexpr auto size() const { return c.size(); }
	constexpr auto empty() const { return c.empty(); }
	auto clear() { c.clear(); }
	auto resize(size_t size) {
	    if (size == 0) clear();
	    else zero = value_type {};
	}
	auto resize(size_t size, const value_type &v) {
	    if (size == 0) clear();
	    else zero = v;
	}
	auto begin() { return c.begin(); }
	auto end() { return c.end(); }
	auto cbegin() const { return c.cbegin(); }
	auto cend() const { return c.cend(); }
};
#endif // CONST_CONTAINER_WRAPPER_HPP
