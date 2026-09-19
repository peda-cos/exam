#pragma once
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

class bigint {
    std::string s;
    void trim() {
        size_t p = s.find_first_not_of('0');
        s = (p == std::string::npos) ? "0" : s.substr(p);
    }
public:
    bigint() : s("0") {}
    bigint(unsigned int v) { std::ostringstream o; o << v; s = o.str(); }
    bigint(const std::string& str) : s(str) { trim(); }
    bigint(const bigint& o) : s(o.s) {}
    bigint& operator=(const bigint& o) { s = o.s; return *this; }
    ~bigint() {}

    const std::string& getDigits() const { return s; }

    bigint operator+(const bigint& o) const {
        std::string r;
        int i = s.size() - 1, j = o.s.size() - 1, c = 0;
        while (i >= 0 || j >= 0 || c) {
            if (i >= 0) c += s[i--] - '0';
            if (j >= 0) c += o.s[j--] - '0';
            r.push_back((c % 10) + '0');
            c /= 10;
        }
        std::reverse(r.begin(), r.end());
        return bigint(r);
    }
    bigint& operator+=(const bigint& o) { return *this = *this + o; }
    bigint& operator++() { return *this += 1; }
    bigint operator++(int) { bigint t(*this); *this += 1; return t; }

    bool operator==(const bigint& o) const { return s == o.s; }
    bool operator!=(const bigint& o) const { return !(*this == o); }
    bool operator<(const bigint& o) const {
        return s.size() == o.s.size() ? s < o.s : s.size() < o.s.size();
    }
    bool operator>(const bigint& o) const { return o < *this; }
    bool operator<=(const bigint& o) const { return !(o < *this); }
    bool operator>=(const bigint& o) const { return !(*this < o); }

    bigint& operator<<=(int n) { if (s != "0") s.append(n, '0'); return *this; }
    bigint& operator>>=(int n) {
        s = ((size_t)n >= s.size()) ? "0" : s.substr(0, s.size() - n);
        return *this;
    }
    bigint operator<<(int n) const { bigint t(*this); return t <<= n; }
    bigint operator>>(int n) const { bigint t(*this); return t >>= n; }

    bigint& operator<<=(const bigint& o) { std::istringstream iss(o.s); int n; iss >> n; return *this <<= n; }
    bigint& operator>>=(const bigint& o) { std::istringstream iss(o.s); int n; iss >> n; return *this >>= n; }
    bigint operator<<(const bigint& o) const { bigint t(*this); return t <<= o; }
    bigint operator>>(const bigint& o) const { bigint t(*this); return t >>= o; }
};

inline std::ostream& operator<<(std::ostream& os, const bigint& b) { return os << b.getDigits(); }
