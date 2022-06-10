#ifndef NO_INIT_ALLOCATOR
#define NO_INIT_ALLOCATOR
// https://stackoverflow.com/a/25922811
// https://stackoverflow.com/users/298661/puppy
// Sep 18, 2014 at 21:39
#include <memory>
template<typename T> struct no_init_allocator : std::allocator<T> {
	void construct(T* p) {}
	template<typename... Args> void construct(T* p, Args&&... args) {
		new (p) T(std::forward<Args>(args)...);
	}
};
#endif
