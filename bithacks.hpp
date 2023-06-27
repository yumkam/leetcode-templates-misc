#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-function"
static auto popcount(unsigned x) { return __builtin_popcount(x); }
static auto popcount(int x) { return __builtin_popcount(x); }
static auto popcount(unsigned long x) { return __builtin_popcountl(x); }
static auto popcount(long x) { return __builtin_popcountl(x); }
static auto popcount(unsigned long long x) { return __builtin_popcountll(x); }
static auto popcount(long long x) { return __builtin_popcountll(x); }

static auto ctz(unsigned x) { return __builtin_ctz(x); }
static auto ctz(int x) { return __builtin_ctz(x); }
static auto ctz(unsigned long x) { return __builtin_ctzl(x); }
static auto ctz(long x) { return __builtin_ctzl(x); }
static auto ctz(unsigned long long x) { return __builtin_ctzll(x); }
static auto ctz(long long x) { return __builtin_ctzll(x); }

static auto clz(unsigned x) { return __builtin_clz(x); }
static auto clz(int x) { return __builtin_clz(x); }
static auto clz(unsigned long x) { return __builtin_clzl(x); }
static auto clz(long x) { return __builtin_clzl(x); }
static auto clz(unsigned long long x) { return __builtin_clzll(x); }
static auto clz(long long x) { return __builtin_clzll(x); }

template <typename T>
static auto LSB(T x) { return x & -x; }
template <typename T>
static bool is_power_of_two(T x) { return (x & (x - 1)) == 0; }

template <typename T>
static auto floor_ilog2(T x) { return sizeof(x)*8 - 1 - clz(x); }
template <typename T>
static auto ceil_ilog2(T x) { return floor_ilog2(x) + !is_power_of_two(x); }
#pragma GCC diagnostic pop
