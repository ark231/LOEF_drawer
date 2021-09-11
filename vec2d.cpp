#include "vec2d.hpp"

#include <cmath>
namespace LOEF {
vec2d::vec2d(double x, double y) {
    this->x_ = x;
    this->y_ = y;
}
double vec2d::length() { return std::sqrt(x_ * x_ + y_ * y_); }
double vec2d::x() { return x_; }
double vec2d::y() { return y_; }
vec2d operator+(vec2d &a, vec2d &b) { return vec2d(a.x() + b.x(), a.y() + b.y()); }
vec2d operator*(vec2d &a, vec2d &b) { return vec2d(a.x() * b.x(), a.y() * b.y()); }
vec2d &vec2d::operator+=(vec2d &that) {
    this->x_ += that.x();
    this->y_ += that.y();
    return that;
}
vec2d &vec2d::operator*=(vec2d &that) {
    this->x_ *= that.x();
    this->y_ *= that.y();
    return that;
}
}  // namespace LOEF
