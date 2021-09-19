#include "vec2d.hpp"

#include <cmath>
namespace LOEF {
vec2d::vec2d(millimetre_value x, millimetre_value y) {
    this->x_ = x;
    this->y_ = y;
}
vec2d::vec2d(const QPoint &point, dot_per_millimetre_value dpmm) {
    this->x_ = static_cast<double>(point.x()) / dpmm;
    this->y_ = static_cast<double>(point.y()) / dpmm;
}
vec2d::vec2d(const QPointF &point, dot_per_millimetre_value dpmm) {
    this->x_ = static_cast<double>(point.x()) / dpmm;
    this->y_ = static_cast<double>(point.y()) / dpmm;
}
millimetre_value vec2d::length() const {
    auto val_x = this->x_ / millimetre;  //無次元の値を取り出す
    auto val_y = this->y_ / millimetre;
    return std::sqrt(val_x * val_x + val_y * val_y) * millimetre;
}
decltype(vec2d::x_ * vec2d::x_) vec2d::length_square() const { return (x_ * x_ + y_ * y_); }
millimetre_value vec2d::x() const noexcept { return x_; }
millimetre_value vec2d::y() const noexcept { return y_; }
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

QPoint vec2d::to_QPoint(dot_per_millimetre_value dpmm) { return QPoint(x_ * dpmm, y_ * dpmm); }
QPointF vec2d::to_QPointF(dot_per_millimetre_value dpmm) { return QPointF(x_ * dpmm, y_ * dpmm); }

vec2d normalize(const vec2d &a) { return a / (a.length() / millimetre); }
}  // namespace LOEF
