#ifndef LOEF_VEC2D
#define LOEF_VEC2D
#include <QPoint>
#include <QPointF>

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

}  // namespace LOEF
template <class QUANTITY>
LOEF::basic_vec2d_<QUANTITY> operator+(const LOEF::basic_vec2d_<QUANTITY> &a, const LOEF::basic_vec2d_<QUANTITY> &b);
template <class QUANTITY>
LOEF::basic_vec2d_<QUANTITY> operator-(const LOEF::basic_vec2d_<QUANTITY> &a, const LOEF::basic_vec2d_<QUANTITY> &b);
template <class QUANTITY>
LOEF::basic_vec2d_<QUANTITY> operator-(const LOEF::basic_vec2d_<QUANTITY> &a);
template <class QUANTITY, typename FACTOR_TYPE>
LOEF::basic_vec2d_<QUANTITY> operator*(const FACTOR_TYPE k, const LOEF::basic_vec2d_<QUANTITY> &a);
template <class QUANTITY, typename FACTOR_TYPE>
LOEF::basic_vec2d_<QUANTITY> operator*(const LOEF::basic_vec2d_<QUANTITY> &a, const FACTOR_TYPE k);
template <class QUANTITY, typename FACTOR_TYPE>
LOEF::basic_vec2d_<QUANTITY> operator/(const FACTOR_TYPE k, const LOEF::basic_vec2d_<QUANTITY> &a);
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
    using basic_vec2d_<millimetre_quantity>::basic_vec2d_;
    vec2d_position(const basic_vec2d_<millimetre_quantity> &point);  // downcast OK!
    vec2d_position(const QPoint &point, dot_per_millimetre_quantity dpmm);
    vec2d_position(const QPointF &point, dot_per_millimetre_quantity dpmm);
    decltype(x_ * x_) length_square() const;
    QPoint to_QPoint(dot_per_millimetre_quantity dpmm);
    QPointF to_QPointF(dot_per_millimetre_quantity dpmm);
};
using vec2d = vec2d_position;  //!既存コードをできるだけ壊さないため
using electric_field = basic_vec2d_<electric_field_strength_quantity>;

}  // namespace LOEF
/*とりあえずの策*/
LOEF::vec2d_position operator+(const LOEF::vec2d_position &a, const LOEF::vec2d_position &b);
LOEF::vec2d_position operator-(const LOEF::vec2d_position &a, const LOEF::vec2d_position &b);
LOEF::vec2d_position operator-(const LOEF::vec2d_position &a);
LOEF::vec2d_position operator*(double k, const LOEF::vec2d_position &a);
LOEF::vec2d_position operator*(const LOEF::vec2d_position &a, double k);
// LOEF::vec2d_position operator/(double k, const LOEF::vec2d_position &a);
LOEF::vec2d_position operator/(const LOEF::vec2d_position &a, double k);
LOEF::vec2d_position normalize(const LOEF::vec2d_position &a);

#ifndef LOEF_VEC2D_LIBRARY_BUILD
#include "vec2d.cpp"
#endif

#endif
