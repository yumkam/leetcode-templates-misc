template <typename T, size_t N>
class Vector {
    array<T, N> v;
public:
    T &operator[](size_t j) { return v[j]; }
};
template <typename T, size_t M, size_t N>
class Matrix {
    typedef Vector<T, N> rowtype;
    array<rowtype, M> m;
public:
    rowtype &operator[](size_t i) { return m[i]; }
    template <typename T, size_t M, size_t N, size_t K>
    friend Matrix<T, M, N> operator *(const Matrix<T, M, K> &a, const Matrix<T, K, N> &b) {
	Matrix<T, M, N> r;
        for (size_t i = 0; i < M; i++)
            for (size_t j = 0; j < N; j++) {
                size_t s = 0;
                for (size_t k = 0; k < K; k++)
                    s += a[i][k]*b[k][j];
                r[i][j] = s;
            }
	return r;
    }
    template <typename T, size_t M, size_t N>
    friend Vector<T, M> operator * (const Matrix<T, M, N> &a, const Vector<T, N> &v)
	Vector<T, M> r;
        for (size_t i = 0; i < M; i++) {
            size_t s = 0;
            for (size_t j = 0; j < N; j++)
                s += a[i][j]*v[j];
             r[i] = s;
        }
	return r;
    }
};
template <typename T, size_t N>
friend Matrix<T, N, N> MIdentity() {
    Matrix<T, N, N> r;
    for (size_t i = 0; i < N; i++)
	for (size_t j = 0; j < N; j++)
	    r[i][j] = i == j;
    return r;
}
template <typename T, size_t N>
Matrix<T, N, N> &operator *= (Matrix<T, N, N> &r, const Matrix<T, N, N> &b) {
	auto a = r;
        for (size_t i = 0; i < N; i++)
            for (size_t j = 0; j < N; j++) {
                size_t s = 0;
                for (size_t k = 0; k < N; k++)
                    s += a[i][k]*b[k][j];
                r[i][j] = s;
            }
	return r;
}
	
template <typename T, size_t N>
Matrix<T, N, N> MMulPow(Matrix<T, N, N> &r, const Matrix<T, N, N> &a, unsigned n) {
    auto t = a;
    for (;;) {
	if ((n & 1)) { r *= t; }
	if ((n >>= 1) == 0) break;
	auto w = t;
	t *= w;
    }
}

