#ifndef LOEF_DRAWER_POINT
#define LOEF_DRAWER_POINT
#include <QPoint>
namespace LOEF {
class vec2d {
   protected:
    double x_ = 0;
    double y_ = 0;

   public:
    vec2d(double x, double y);
    vec2d(QPoint &point);
    vec2d(QPoint &&point);
    double length() const;
    double length_square() const;
    double x() const noexcept;
    double y() const noexcept;
    vec2d &operator+=(vec2d &that);
    vec2d &operator-=(vec2d &that);
    void operator*=(double k);
};
vec2d operator+(const vec2d &a, const vec2d &b);
vec2d operator-(const vec2d &a, const vec2d &b);
vec2d operator-(const vec2d &a);
vec2d operator*(double k, const vec2d &a);
vec2d operator*(const vec2d &a, double k);
}  // namespace LOEF
#endif
