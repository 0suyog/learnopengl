#pragma once
#include <cmath>
#include <iostream>
double infinity = INFINITY;
class interval {
public:
  double min, max;
  interval() : min(+infinity), max(-infinity) {};
  interval(double min, double max) : min(min), max(max) {};

  interval(const interval &int1, const interval &int2) {
    min = int1.min <= int2.min ? int1.min : int2.min;
    max = int1.max >= int2.max ? int1.max : int2.max;
  }

  double size() const { return max - min; }

  bool contains(double x) const { return min <= x && x <= max; }

  bool surrounds(double x) const { return min < x && x < max; }

  double clamp(double x) const {
    if (x < min)
      return min;
    if (x > max)
      return max;
    return x;
  }

  interval expand(double x) const {
    auto delta = x / 2;
    return interval(min - delta, max + delta);
  }

  static const interval empty, universe;
};

inline std::ostream &operator<<(std::ostream &out, const interval &i) {
  out << "Interval: min: " << i.min << " max: " << i.max;
  return out;
}

inline const interval interval::empty = interval(infinity, -infinity);
inline const interval interval::universe = interval(-infinity, infinity);

inline interval operator+(const interval &i, double offset) {
  return interval(i.min + offset, i.max + offset);
}
inline interval operator+(double offset, const interval &i) {
  return interval(i.min + offset, i.max + offset);
}
