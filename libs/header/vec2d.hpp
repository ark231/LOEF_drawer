#ifndef LOEF_DRAWER_POINT
#define LOEF_DRAWER_POINT
#include <QPoint>
#include <QPointF>

#include "units.hpp"
namespace LOEF {
/**
 * @brief 2次元ベクトル
 *
 * 成分はミリメートル単位!
 */
class vec2d {
   protected:
    millimetre_quantity x_;
    millimetre_quantity y_;

   public:
    vec2d(millimetre_quantity x, millimetre_quantity y);
    vec2d(const QPoint &point, dot_per_millimetre_quantity dpmm);
    vec2d(const QPointF &point, dot_per_millimetre_quantity dpmm);
    vec2d() {}
    millimetre_quantity length() const;
    decltype(x_ * x_) length_square() const;
    millimetre_quantity x() const noexcept;
    millimetre_quantity y() const noexcept;
    void operator+=(const vec2d &that);
    void operator-=(const vec2d &that);
    void operator*=(double k);
    void operator/=(double k);
    QPoint to_QPoint(dot_per_millimetre_quantity dpmm);
    QPointF to_QPointF(dot_per_millimetre_quantity dpmm);
};
vec2d operator+(const vec2d &a, const vec2d &b);
vec2d operator-(const vec2d &a, const vec2d &b);
vec2d operator-(const vec2d &a);
vec2d operator*(double k, const vec2d &a);
vec2d operator*(const vec2d &a, double k);
vec2d operator/(double k, const vec2d &a);
vec2d operator/(const vec2d &a, double k);
vec2d normalize(const vec2d &a);
}  // namespace LOEF
#endif
