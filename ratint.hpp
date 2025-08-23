#ifndef RATINT_HPP
#define RATINT_HPP
#include <iostream>
#include <numeric>
#include <cassert>
template <class base = int>
struct rat {
    // XXX assumes overflow never happens
    base num = 0, den = 1;
    rat() = default;
    rat(base v): num(v) {}
    rat(base mul, base div): num(mul), den(div) {}
    rat(const rat&) = default;
    rat& operator = (const rat&) = default;
    auto reduce() const {
        // return canonical irreducible presentation
        // XXX should we distinguish +inf and -inf here?
        if (den == 0) return rat { num > 0 ? +1 : num < 0 ? -1 : 0, 0 };
        auto g = std::gcd(num, den);
        return den < 0 ? rat {-num/g, -den/g} : rat {num/g, den/g};
    }
    friend auto operator + (rat a, rat b) {
        // a/b + c/d = (ad + bc)/bd
        return rat {a.num*b.den + b.num*a.den, a.den*b.den}.reduce();
    }
    friend auto operator - (rat a, rat b) {
        // a/b - c/d = (ad - bc)/bd
        return rat {a.num*b.den - b.num*a.den, a.den*b.den}.reduce();
    }
    friend auto operator * (rat a, rat b) {
        // a/b * c/d = ac/bd
        return rat {a.num*b.num, a.den*b.den}.reduce();
    }
    friend auto operator / (rat a, rat b) {
        // a/b / c/d = ad/bc
        return rat {a.num*b.den, a.den*b.num}.reduce();
    }
    auto operator - () { return rat { -num, den }; }
    auto operator + () { return reduce(); }
    auto &operator += (rat b) { *this = *this + b; return *this; }
    auto &operator -= (rat b) { *this = *this - b; return *this; }
    auto &operator *= (rat b) { *this = *this * b; return *this; }
    auto &operator /= (rat b) { *this = *this / b; return *this; }
    auto &operator ++() {
        num += den;
        return *this;
    }
    auto &operator --() {
        num -= den;
        return *this;
    }
    auto operator ++(int) { auto temp = *this; ++*this; return temp; }
    auto operator --(int) { auto temp = *this; --*this; return temp; }
    friend auto operator << (rat x, int s) {
        while(s > 0 && !(x.den & 1)) {
            --s; x.den >>= 1;
        }
        x.num <<= s;
        return x;
    }
    friend auto operator >> (rat x, int s) {
        while(s > 0 && !(x.num & 1)) {
            --s; x.num >>= 1;
        }
        x.den <<= s;
        return x;
    }
    operator bool () const {
        return den != 0 && num;
    }
    bool isnan() const {
        return den == 0 && num == 0;
    }
    bool isinf() const {
        return den == 0 && num != 0;
    }
    bool isneginf() const {
        return den == 0 && num < 0;
    }
    bool isposinf() const {
        return den == 0 && num > 0;
    }
    friend bool operator < (rat a, rat b) {
        bool inv = (a.den < 0 && b.den > 0) || (a.den > 0 && b.den < 0);
        return inv ? a.num*b.den < b.num*a.den : a.num*b.den > b.num*a.den;
    }
    friend bool operator > (rat a, rat b) = default;
    friend bool operator <= (rat a, rat b) = default;
    friend bool operator >= (rat a, rat b) = default;
    friend bool operator == (rat a, rat b) = default;
    friend bool operator != (rat a, rat b) = default;
    friend auto& operator <<(std::ostream& os, rat x) {
        if (x.den == 1)
            os << x.num;
        else
            os <<'('<< x.num <<'/' << x.den<<')';
        return os;
    }
    friend auto& operator >>(std::istream& is, rat& x) {
        if (is.peek() == '(') {
            if (char c; !(is >> c) || !(is >> x.num) || !(is >> c) || c != '/' || !(is >> x.den) || !(is >> c) || c != ')')
                is.setstate(std::ios_base::failbit);
        } else {
            x.den = 0;
            is >> x.num;
        }
        return is;
    }
};
#endif
