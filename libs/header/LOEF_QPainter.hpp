#ifndef LOEF_DRAWER_LOEF_QPAINTER
#define LOEF_DRAWER_LOEF_QPAINTER
#include <QPainter>

#include "charges.hpp"
namespace LOEF {
class painter : public QPainter {
    using QPainter::QPainter;
    dot_per_millimetre_quantity dpmm_;

   public:
    void set_resolution(dot_per_millimetre_quantity dpmm);
    void draw_fixed_charge(const fixed_charge&);  // use different naming conventions to avoid confliction
    void draw_LOEF_path(const LOEF_path&);
};
}  // namespace LOEF
#endif
