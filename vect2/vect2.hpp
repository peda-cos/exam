#ifndef VECT2_HPP
#define VECT2_HPP

#include <iostream>

class vect2
{
    public:
        vect2();
        vect2(int x, int y);
        vect2(const vect2 &src);
        ~vect2();

        vect2 &operator=(const vect2 &rhs);

        int        &operator[](int i);
        int         operator[](int i) const;

        vect2 operator+(const vect2 &rhs) const;
        vect2 operator-(const vect2 &rhs) const;
        vect2 operator-() const;
        vect2 operator*(int k) const;

        vect2 &operator+=(const vect2 &rhs);
        vect2 &operator-=(const vect2 &rhs);
        vect2 &operator*=(int k);

        bool operator==(const vect2 &rhs) const;
        bool operator!=(const vect2 &rhs) const;

        vect2 &operator++();
        vect2  operator++(int);
        vect2 &operator--();
        vect2  operator--(int);

    private:
        int _v[2];
};

vect2 operator*(int k, const vect2 &v);
std::ostream &operator<<(std::ostream &o, const vect2 &v);

#endif
