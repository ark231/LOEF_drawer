#include "vec2d.hpp"

#include <QtGlobal>
#include <cmath>
#include <sstream>
namespace LOEF {
#ifdef LOEF_VEC2D_LIBRARY_BUILD
template <class QUANTITY>
basic_vec2d_<QUANTITY>::basic_vec2d_(QUANTITY x, QUANTITY y) {
    this->x_ = x;
    this->y_ = y;
}

template <>
millimetre_quantity basic_vec2d_<millimetre_quantity>::length() const {
    auto val_x = this->x_ / millimetre;  //無次元の値を取り出す
    auto val_y = this->y_ / millimetre;
    return std::sqrt(val_x * val_x + val_y * val_y) * millimetre;
}
template <>
electric_field_strength_quantity basic_vec2d_<electric_field_strength_quantity>::length() const {
    auto val_x = this->x_ / (boostunits::coulomb / (millimetre * millimetre));  //無次元の値を取り出す
    auto val_y = this->y_ / (boostunits::coulomb / (millimetre * millimetre));
    return std::sqrt(val_x * val_x + val_y * val_y) * (boostunits::coulomb / (millimetre * millimetre));
}
template <>
basic_vec2d_<dimensionless_quantity> basic_vec2d_<millimetre_quantity>::to_dimentionless() {
    return basic_vec2d_<dimensionless_quantity>(this->x_ / millimetre, this->y_ / millimetre);
}
template <class QUANTITY>
QUANTITY basic_vec2d_<QUANTITY>::x() const noexcept {
    return x_;
}
template <class QUANTITY>
QUANTITY basic_vec2d_<QUANTITY>::y() const noexcept {
    return y_;
}
template <class QUANTITY>
void basic_vec2d_<QUANTITY>::operator+=(const basic_vec2d_<QUANTITY> &that) {
    this->x_ = this->x_ + that.x();
    this->y_ = this->y_ + that.y();
}
template <class QUANTITY>
void basic_vec2d_<QUANTITY>::operator-=(const basic_vec2d_<QUANTITY> &that) {
    this->x_ = this->x_ - that.x();
    this->y_ = this->y_ - that.y();
}
template <class QUANTITY>
void basic_vec2d_<QUANTITY>::operator*=(const double k) {
    this->x_ = this->x_ * k;
    this->y_ = this->y_ * k;
}
template <class QUANTITY>
void basic_vec2d_<QUANTITY>::operator/=(const double k) {
    this->x_ = this->x_ / k;
    this->y_ = this->y_ / k;
}

template class basic_vec2d_<millimetre_quantity>;
template class basic_vec2d_<electric_field_strength_quantity>;
template class basic_vec2d_<dimensionless_quantity>;
#endif
template <class QUANTITY>
basic_vec2d_<QUANTITY> normalize(const basic_vec2d_<QUANTITY> &a) {
    Q_ASSERT_X(!qFuzzyIsNull(a.length().value()), __func__, "dividion by zero (given vector's length is 0)");
    return a / a.length().value();
}
}  // namespace LOEF

template <class QUANTITY>
LOEF::basic_vec2d_<QUANTITY> operator+(const LOEF::basic_vec2d_<QUANTITY> &a, const LOEF::basic_vec2d_<QUANTITY> &b) {
    return LOEF::basic_vec2d_<QUANTITY>(a.x() + b.x(), a.y() + b.y());
}
template <class QUANTITY>
LOEF::basic_vec2d_<QUANTITY> operator-(const LOEF::basic_vec2d_<QUANTITY> &a, const LOEF::basic_vec2d_<QUANTITY> &b) {
    return LOEF::basic_vec2d_<QUANTITY>(a.x() - b.x(), a.y() - b.y());
}
template <class QUANTITY, typename FACTOR_TYPE>
LOEF::basic_vec2d_<QUANTITY> operator*(const FACTOR_TYPE k, const LOEF::basic_vec2d_<QUANTITY> &a) {
    return LOEF::basic_vec2d_<QUANTITY>(static_cast<double>(k) * a.x(), static_cast<double>(k) * a.y());
}
template <class QUANTITY, typename FACTOR_TYPE>
LOEF::basic_vec2d_<QUANTITY> operator*(const LOEF::basic_vec2d_<QUANTITY> &a, const FACTOR_TYPE k) {
    return static_cast<double>(k) * a;
}
template <class QUANTITY, typename FACTOR_TYPE>
LOEF::basic_vec2d_<QUANTITY> operator/(const LOEF::basic_vec2d_<QUANTITY> &a, const FACTOR_TYPE k) {
    return LOEF::basic_vec2d_<QUANTITY>(a.x() / static_cast<double>(k), a.y() / static_cast<double>(k));
}
template <class QUANTITY>
LOEF::basic_vec2d_<QUANTITY> operator-(const LOEF::basic_vec2d_<QUANTITY> &a) {
    return -1 * a;
}
template <class QUANTITY>
QDebug &operator<<(QDebug &debug, const LOEF::basic_vec2d_<QUANTITY> &data) {
    auto data_ = static_cast<LOEF::basic_vec2d_<QUANTITY>>(data);
    std::stringstream output;
    output << "(" << data_.x() << "," << data_.y() << ")";
    debug << output.str().c_str();
    return debug;
}
template <class QUANTITY>
bool fuzzy_compare(const LOEF::basic_vec2d_<QUANTITY> &a, const LOEF::basic_vec2d_<QUANTITY> &b) {
    return qFuzzyCompare(a.x().value(), b.x().value()) && qFuzzyCompare(a.y().value(), b.y().value());
}
#ifdef LOEF_VEC2D_LIBRARY_BUILD

/*------------------------------------------
 *              vec2d_position
 *------------------------------------------*/
namespace LOEF {
vec2d_position::vec2d_position(const QPoint &point, dot_per_millimetre_quantity dpmm) {
    this->x_ = static_cast<double>(point.x()) / dpmm;
    this->y_ = static_cast<double>(point.y()) / dpmm;
}
vec2d_position::vec2d_position(const QPointF &point, dot_per_millimetre_quantity dpmm) {
    this->x_ = static_cast<double>(point.x()) / dpmm;
    this->y_ = static_cast<double>(point.y()) / dpmm;
}
vec2d_position::vec2d_position(const basic_vec2d_<millimetre_quantity> &point) {
    this->x_ = point.x();
    this->y_ = point.y();
}
decltype(vec2d_position::x_ * vec2d_position::x_) vec2d_position::length_square() const { return (x_ * x_ + y_ * y_); }

QPoint vec2d_position::to_QPoint(dot_per_millimetre_quantity dpmm) { return QPoint(x_ * dpmm, y_ * dpmm); }
QPointF vec2d_position::to_QPointF(dot_per_millimetre_quantity dpmm) { return QPointF(x_ * dpmm, y_ * dpmm); }
}  // namespace LOEF
#endif
