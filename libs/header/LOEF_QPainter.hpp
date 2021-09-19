#ifndef LOEF_DRAWER_LOEF_QPAINTER
#define LOEF_DRAWER_LOEF_QPAINTER
#include <QPainter>

#include "charges.hpp"
namespace LOEF {
class painter : public QPainter {
    using QPainter::QPainter;
    double dpmm_ = 0;  //! dot per millimetre

   public:
    void set_resolution(double dpmm);
    void draw_fixed_charge(const fixed_charge&);  // use different naming conventions to avoid confliction
    void draw_LOEF_path(const LOEF_path&);
};
}  // namespace LOEF
#endif
