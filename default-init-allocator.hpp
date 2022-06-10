// https://stackoverflow.com/a/21028912
// https://stackoverflow.com/questions/21028299/is-this-behavior-of-vectorresizesize-type-n-under-c11-and-boost-container/21028912#21028912
// Author: Casey https://stackoverflow.com/users/923854/casey
// Date: Jan 9, 2014 at 19:03
#ifndef DEFAULT_INIT_ALLOCATOR
#define DEFAULT_INIT_ALLOCATOR
#include <memory>
// Allocator adaptor that interposes construct() calls to
// convert value initialization into default initialization.
template <typename T, typename A=std::allocator<T>>
class default_init_allocator : public A {
  typedef std::allocator_traits<A> a_t;
public:
  template <typename U> struct rebind {
    using other =
      default_init_allocator<
        U, typename a_t::template rebind_alloc<U>
      >;
  };

  using A::A;

  template <typename U>
  void construct(U* ptr)
    noexcept(std::is_nothrow_default_constructible<U>::value) {
    ::new(static_cast<void*>(ptr)) U;
  }
  template <typename U, typename...Args>
  void construct(U* ptr, Args&&... args) {
    a_t::construct(static_cast<A&>(*this),
                   ptr, std::forward<Args>(args)...);
  }
};

#ifdef DEFAULT_INIT_ALLOCATOR_TEST
// https://stackoverflow.com/a/21028742
// Author: Howard Hinnant https://stackoverflow.com/users/576911/howard-hinnant
// Date: Jan 9, 2014 at 18:54
#include <vector>
#include <iostream>
using namespace std;
template <typename VecType>
void init_vec(VecType &v)
{
    // fill v with values [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
    v.resize(10);
    for (size_t i = 0; i < 10; ++i) v[i] = i;  // Note this change!!!
    // chop off the end of v, which now should be [1, 2, 3, 4, 5], but the other 5 values
    // should remain in memory
    v.resize(5);
}

template <typename VecType>
void print_vec(const char *label, VecType &v)
{
    cout << label << ": ";
    for (size_t i = 0; i < v.size(); ++i) {
        cout << v[i] << ' ';
    }
    cout << endl;
}

int main() {
  std::vector<int, default_init_allocator<int>> std_vec;
  init_vec(std_vec);
  std_vec.resize(10);
  print_vec("std", std_vec);
}
#endif
#endif
