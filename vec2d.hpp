#ifndef LOEF_DRAWER_POINT
#define LOEF_DRAWER_POINT
namespace LOEF {
class vec2d {
   protected:
    double x_ = 0;
    double y_ = 0;

   public:
    vec2d(double x, double y);
    double length();
    double x();
    double y();
    vec2d &operator+=(vec2d &that);
    vec2d &operator*=(vec2d &that);
};
vec2d operator+(vec2d &a, vec2d &b);
vec2d operator*(vec2d &a, vec2d &b);
}  // namespace LOEF
#endif
