#include "vect2.hpp"

vect2::vect2()
{
  _v[0] = 0;
  _v[1] = 0;
}

vect2::vect2(int x, int y)
{
  _v[0] = x;
  _v[1] = y;
}

vect2::vect2(const vect2 &src)
{
  _v[0] = src._v[0];
  _v[1] = src._v[1];
}

vect2::~vect2()
{
}

vect2 &vect2::operator=(const vect2 &rhs)
{
  if (this != &rhs) {
    _v[0] = rhs._v[0];
    _v[1] = rhs._v[1];
  }
  return *this;
}

int &vect2::operator[](int i)
{
  return _v[i];
}

int vect2::operator[](int i) const
{
  return _v[i];
}

vect2 vect2::operator+(const vect2 &rhs) const
{
  return vect2(_v[0] + rhs._v[0], _v[1] + rhs._v[1]);
}

vect2 vect2::operator-(const vect2 &rhs) const
{
  return vect2(_v[0] - rhs._v[0], _v[1] - rhs._v[1]);
}

vect2 vect2::operator-() const
{
  return vect2(-_v[0], -_v[1]);
}

vect2 vect2::operator*(int k) const
{
  return vect2(_v[0] * k, _v[1] * k);
}

vect2 operator*(int k, const vect2 &v)
{
  return v * k;
}

vect2 &vect2::operator+=(const vect2 &rhs)
{
  _v[0] += rhs._v[0];
  _v[1] += rhs._v[1];
  return *this;
}

vect2 &vect2::operator-=(const vect2 &rhs)
{
  _v[0] -= rhs._v[0];
  _v[1] -= rhs._v[1];
  return *this;
}

vect2 &vect2::operator*=(int k)
{
  _v[0] *= k;
  _v[1] *= k;
  return *this;
}

bool vect2::operator==(const vect2 &rhs) const
{
  return _v[0] == rhs._v[0] && _v[1] == rhs._v[1];
}

bool vect2::operator!=(const vect2 &rhs) const
{
  return !(*this == rhs);
}

vect2 &vect2::operator++()
{
  _v[0]++;
  _v[1]++;
  return *this;
}

vect2 &vect2::operator--()
{
  _v[0]--;
  _v[1]--;
  return *this;
}

vect2 vect2::operator++(int)
{
  ++(*this);
  return *this;
}

vect2 vect2::operator--(int)
{
  --(*this);
  return *this;
}

std::ostream &operator<<(std::ostream &o, const vect2 &v)
{
  o << "{" << v[0] << ", " << v[1] << "}";
  return o;
}
