#include "vec2d.hpp"

#include <cmath>
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

template <>
basic_vec2d_<millimetre_quantity> normalize(const basic_vec2d_<millimetre_quantity> &a) {
    return a / (a.length() / millimetre);
}
/*
template <>
basic_vec2d_<electric_field_strength_quantity> normalize(const basic_vec2d_<electric_field_strength_quantity> &a) {
    return a / (a.length() / (boostunits::coulomb / (millimetre * millimetre)));
}
*/

template class basic_vec2d_<millimetre_quantity>;
template class basic_vec2d_<electric_field_strength_quantity>;
template class basic_vec2d_<dimensionless_quantity>;
}  // namespace LOEF
#endif

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
#ifdef LOEF_VEC2D_LIBRARY_BUILD
namespace LOEF {
template <>
basic_vec2d_<electric_field_strength_quantity> normalize(const basic_vec2d_<electric_field_strength_quantity> &a) {
    return a / (a.length() / (boostunits::coulomb / (millimetre * millimetre)));
}
}  // namespace LOEF

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
#endif
}  // namespace LOEF
#ifdef LOEF_VEC2D_LIBRARY_BUILD
LOEF::vec2d_position operator+(const LOEF::vec2d_position &a, const LOEF::vec2d_position &b) {
    return static_cast<LOEF::vec2d_position>(static_cast<LOEF::basic_vec2d_<LOEF::millimetre_quantity>>(a) +
                                             static_cast<LOEF::basic_vec2d_<LOEF::millimetre_quantity>>(b));
}
LOEF::vec2d_position operator-(const LOEF::vec2d_position &a, const LOEF::vec2d_position &b) {
    return static_cast<LOEF::vec2d_position>(static_cast<LOEF::basic_vec2d_<LOEF::millimetre_quantity>>(a) -
                                             static_cast<LOEF::basic_vec2d_<LOEF::millimetre_quantity>>(b));
}
LOEF::vec2d_position operator-(const LOEF::vec2d_position &a) {
    return static_cast<LOEF::vec2d_position>(-static_cast<LOEF::basic_vec2d_<LOEF::millimetre_quantity>>(a));
}
LOEF::vec2d_position operator*(const double k, const LOEF::vec2d_position &a) {
    return static_cast<LOEF::vec2d_position>(k * static_cast<LOEF::basic_vec2d_<LOEF::millimetre_quantity>>(a));
}
LOEF::vec2d_position operator*(const LOEF::vec2d_position &a, const double k) {
    return static_cast<LOEF::vec2d_position>(static_cast<LOEF::basic_vec2d_<LOEF::millimetre_quantity>>(a) * k);
}
/*
LOEF::vec2d_position operator/(const double k, const LOEF::vec2d_position &a) {
    return static_cast<LOEF::vec2d_position>(k / static_cast<LOEF::basic_vec2d_<LOEF::millimetre_quantity>>(a));
}
*/
LOEF::vec2d_position operator/(const LOEF::vec2d_position &a, const double k) {
    return static_cast<LOEF::vec2d_position>(static_cast<LOEF::basic_vec2d_<LOEF::millimetre_quantity>>(a) / k);
}
LOEF::vec2d_position normalize(const LOEF::vec2d_position &a) {
    return LOEF::normalize(static_cast<LOEF::basic_vec2d_<LOEF::millimetre_quantity>>(a));
}
#endif
