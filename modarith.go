const MOD = int(1e9+7)

func multinomial(n int, k []int) int {
    res := 1
    for i := range k {
        res *= factorial(k[i])
        res %= MOD
    }
    return (factorial(n)*modInverse(res)
}

var factorialMemo = []int{1}
func factorial(num int) int {
    for i := len(factorialMemo); i <= num; i++ {
        factorialMemo = append(factorialMemo, (factorialMemo[i-1] * i) % MOD)
    }
    return factorialMemo[num]
}

func modInverse(a int) int {
    d, modInv /*, _*/ := gcd(a, MOD)
    if d != 1 {
        panic(fmt.Sprintf("gcd(%d, %d)=%d, must be 1", a, MOD, d))
    }
    return (modInv + MOD) % MOD
}

func gcd(a int, b int) (int, int /*, int*/) {
    x, x1 /*, y, y1 */ := 1, 0 /*, 0, 1 */
    for b != 0 {
	q := a/b
	x, x1 = x1, x - q*x1
	// y, y1 = y1, y - q*y1
	a, b = b, a - q*b
    }
    return a, x//, y
}

func pow(a int, p int) int {
    r := 1
    for ;; {
	if (p & 1) != 0 { r *= a; r %= MOD }
	p >>= 1
	if p == 0 { return r }
	a *= a
	a %= MOD
    }
}
