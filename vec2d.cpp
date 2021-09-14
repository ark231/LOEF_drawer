#include "vec2d.hpp"

#include <cmath>
namespace LOEF {
vec2d::vec2d(double x, double y) {
    this->x_ = x;
    this->y_ = y;
}
vec2d::vec2d(QPoint &point) {
    this->x_ = point.x();
    this->y_ = point.y();
}
vec2d::vec2d(QPoint &&point) {
    this->x_ = point.x();
    this->y_ = point.y();
}
double vec2d::length() const { return std::sqrt(x_ * x_ + y_ * y_); }
double vec2d::length_square() const { return x_ * x_ + y_ * y_; }
double vec2d::x() const noexcept { return x_; }
double vec2d::y() const noexcept { return y_; }
vec2d operator+(const vec2d &a, const vec2d &b) { return vec2d(a.x() + b.x(), a.y() + b.y()); }
vec2d operator-(const vec2d &a, const vec2d &b) { return vec2d(a.x() - b.x(), a.y() - b.y()); }
vec2d operator*(double k, const vec2d &a) { return vec2d(k * a.x(), k * a.y()); }
vec2d operator*(const vec2d &a, double k) { return k * a; }
vec2d operator/(const vec2d &a, double k) { return vec2d(a.x() / k, a.y() / k); }
vec2d operator-(const vec2d &a) { return -1 * a; }
void vec2d::operator+=(const vec2d &that) {
    this->x_ += that.x();
    this->y_ += that.y();
}
void vec2d::operator-=(const vec2d &that) {
    this->x_ -= that.x();
    this->y_ -= that.y();
}
void vec2d::operator*=(double k) {
    this->x_ *= k;
    this->y_ *= k;
}
void vec2d::operator/=(double k) {
    this->x_ /= k;
    this->y_ /= k;
}
vec2d::operator QPoint() { return QPoint(x_, y_); }
vec2d normalize(const vec2d &a) { return a / a.length(); }
}  // namespace LOEF
