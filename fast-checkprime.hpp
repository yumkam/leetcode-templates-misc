static void mulmod(unsigned &a, unsigned b, unsigned mod) {
    uint64_t c = a;
    a = (c*b) % mod;
}
static unsigned mulpowmod(unsigned r, unsigned a, unsigned n, unsigned mod) {
    for(;;) {
	if ((n & 1))
	    mulmod(r, a, mod);
	if ((n >>= 1) == 0)
	    break;
	mulmod(a, a, mod);
    }
    return r;
}
static bool probably_prime(unsigned p) {
    return p == 2 || mulpowmod(1, 2, p - 1, p) == 1;
}
