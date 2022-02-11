#ifndef LOEF_VEC2D
#define LOEF_VEC2D
#include <QDebug>
#include <QPoint>
#include <QPointF>
#include <array>

#include "units.hpp"
namespace LOEF {
/**
 * @brief 2次元ベクトル
 *
 * 成分はテンプレート引数単位
 */
template <class QUANTITY>
class basic_vec2d_ {
   protected:
    QUANTITY x_;
    QUANTITY y_;

   public:
    basic_vec2d_(QUANTITY x, QUANTITY y);
    /*
    basic_vec2d_(const QPoint &point, dot_per_millimetre_quantity dpmm);
    basic_vec2d_(const QPointF &point, dot_per_millimetre_quantity dpmm);
    */
    basic_vec2d_() {}
    QUANTITY length() const;
    QUANTITY x() const noexcept;
    QUANTITY y() const noexcept;
    basic_vec2d_<dimensionless_quantity> to_dimentionless();
    void operator+=(const basic_vec2d_ &that);
    void operator-=(const basic_vec2d_ &that);
    void operator*=(double k);
    void operator/=(double k);
};

template <class QUANTITY>
basic_vec2d_<QUANTITY> normalize(const LOEF::basic_vec2d_<QUANTITY> &a);

template <class A_CLASS_WHICH_IS_NOT_BASIC_VEC2D>
struct is_basic_vec2d_ {
    static constexpr bool value = false;
};

template <class QUANTITY>
struct is_basic_vec2d_<basic_vec2d_<QUANTITY>> {
    static constexpr bool value = true;
};

template <class T>
inline constexpr bool is_basic_vec2d_v = is_basic_vec2d_<T>::value;

namespace {
template <class QUANTITY, typename FACTOR_TYPE>
using multiplied = decltype(std::declval<QUANTITY>() * std::declval<FACTOR_TYPE>());
// using multiplied = boost::units::multiply_typeof_helper<QUANTITY, FACTOR_TYPE>;
}  // namespace
}  // namespace LOEF
#ifdef LOEF_VEC2D_LIBRARY_BUILD
template <class QUANTITY>
LOEF::basic_vec2d_<QUANTITY> operator+(const LOEF::basic_vec2d_<QUANTITY> &a, const LOEF::basic_vec2d_<QUANTITY> &b);
template <class QUANTITY>
LOEF::basic_vec2d_<QUANTITY> operator-(const LOEF::basic_vec2d_<QUANTITY> &a, const LOEF::basic_vec2d_<QUANTITY> &b);
template <class QUANTITY>
LOEF::basic_vec2d_<QUANTITY> operator-(const LOEF::basic_vec2d_<QUANTITY> &a);
template <class QUANTITY, typename FACTOR_TYPE>
LOEF::basic_vec2d_<LOEF::multiplied<QUANTITY, FACTOR_TYPE>> operator*(const FACTOR_TYPE k,
                                                                      const LOEF::basic_vec2d_<QUANTITY> &a);

template <class QUANTITY, typename FACTOR_TYPE>
LOEF::basic_vec2d_<LOEF::multiplied<QUANTITY, FACTOR_TYPE>> operator*(const LOEF::basic_vec2d_<QUANTITY> &a,
                                                                      const FACTOR_TYPE k);
template <class QUANTITY, typename FACTOR_TYPE>
LOEF::basic_vec2d_<QUANTITY> operator/(const LOEF::basic_vec2d_<QUANTITY> &a, const FACTOR_TYPE k);
// LOEF::basic_vec2d_<QUANTITY> operator/(const FACTOR_TYPE k, const LOEF::basic_vec2d_<QUANTITY> &a);

/*
template <class QUANTITY>
QDebug operator<<(QDebug, const LOEF::basic_vec2d_<QUANTITY> &);
*/
template <class QUANTITY>
bool fuzzy_compare(const LOEF::basic_vec2d_<QUANTITY> &a, const LOEF::basic_vec2d_<QUANTITY> &b);
#endif
/*
template <class QUANTITY>
LOEF::basic_vec2d_<QUANTITY> operator/(const LOEF::basic_vec2d_<QUANTITY> &a, double k);
*/

/*------------------------------------------
 *              vec2d_position
 *------------------------------------------*/
namespace LOEF {
class vec2d_position : public basic_vec2d_<millimetre_quantity> {
   public:
    using super = basic_vec2d_<millimetre_quantity>;
    using basic_vec2d_<millimetre_quantity>::basic_vec2d_;
    vec2d_position(const basic_vec2d_<millimetre_quantity> &point);  // downcast OK!
    vec2d_position(const QPoint &point, dot_per_millimetre_quantity dpmm);
    vec2d_position(const QPointF &point, dot_per_millimetre_quantity dpmm);
    // lazy
    vec2d_position(std::array<double, 2>);
    // end lazy
    decltype(x_ * x_) length_square() const;
    QPoint to_QPoint(dot_per_millimetre_quantity dpmm);
    QPointF to_QPointF(dot_per_millimetre_quantity dpmm);
};
radian_quantity argument(vec2d_position);
using vec2d = vec2d_position;  //!既存コードをできるだけ壊さないため
using electric_field = basic_vec2d_<electric_field_strength_quantity>;
}  // namespace LOEF

#ifndef LOEF_VEC2D_LIBRARY_BUILD
#    include "vec2d.cpp"
#endif

#endif
